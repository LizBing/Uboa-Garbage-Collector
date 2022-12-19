/**
 * @file forwarding.c
 * @author Lizbing (lizbing@relight.com)
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#include "forwarding.h"
#include "address.h"
#include "page.h"
#include "allocator.h"
#include "relocate.h"

#include "globals.h"

#define MAPSIZE (((1l << pageSizeShiftSmall - alignmentShiftSmall)) * sizeof(atomic_short))
#define FORWCOUNTMEDIUM (1l << pageSizeShiftMedium - limitShiftSmall)

#define NREGIONSINMEDIUMPAGE (1 << (pageSizeShiftMedium - regionSizeShift))

#define MAPEXP -1
#define WAITING_VALUE -2l

typedef struct {
    PageType type;
    atomic_short* map;

    atomic_long count;
    atomic_intptr_t* forwarding;

    atomic_intptr_t large;
} ForwardingTable;
static ForwardingTable* set = NULL;

void forwardingInit() {
    set = calloc((XMX >> regionSizeShift) + 32, sizeof(ForwardingTable));
    uboa_assert(set, "out of memory");
}

infile_inline(ForwardingTable* getTable(intptr_t addr)) {
    return set + regionNo(addr);
}

void createPageForwarding(Page* p) {
    ForwardingTable* t = getTable(p->begin);
    t->type = p->type;
    t->count = 0;

    if(PAGETYPE_SMALL == p->type) {
        void* mem = memAlloc(MAPSIZE + sizeof(atomic_intptr_t) * p->counter);
        t->map = mem;
        t->forwarding = mem + MAPSIZE;
        memset(t->map, MAPEXP, MAPSIZE);
    }
    else {
        t->forwarding = memAlloc(sizeof(atomic_intptr_t) * FORWCOUNTMEDIUM);
        for(int i = 0; i < FORWCOUNTMEDIUM; ++i)
            t->forwarding[i] = NULL;

        for(int i = 1; i < NREGIONSINMEDIUMPAGE; ++i)
            t[i] = *t;
    } 
}

infile_inline(bool isLarge(ForwardingTable* t)) {
    return t->forwarding == &t->large;
}

inline void createLargeObjForwarding(intptr_t addr) {
    ForwardingTable* t = getTable(addr);
    t->forwarding = &t->large;
    t->large = NULL;
}

infile_inline(intptr_t getSmall(ForwardingTable* t, intptr_t addr, bool imm)) {
    short exp = MAPEXP, des = 0;
    atomic_short* n = t->map + inPageOffsSmall(addr);

    if(atomic_compare_exchange_strong(n, &exp, WAITING_VALUE)) {
        des = t->count++;
        intptr_t ret = t->forwarding[des] = relocate(addr);
        *n = des;

        return ret;
    }

    if(!imm && WAITING_VALUE == exp) 
        do des = atomic_load(n);
        while(WAITING_VALUE == des);

    return t->forwarding[des];
}

infile_inline(intptr_t getForw(atomic_intptr_t* n, intptr_t addr, bool imm)) {
    intptr_t exp = NULL;
    if(atomic_compare_exchange_strong(n, &exp, WAITING_VALUE)) 
        return *n = relocate(addr);

    intptr_t des = exp;
    if(!imm && WAITING_VALUE == exp) 
        do des = atomic_load(n);
        while(WAITING_VALUE == des);

    return des;
}

infile_inline(intptr_t getMedium(ForwardingTable* t, intptr_t addr, bool imm)) {
    return getForw(t->forwarding + inPageOffsMedium(addr), addr, imm);
}

infile_inline(intptr_t getLarge(ForwardingTable* t, intptr_t addr, bool imm)) {
    return getForw(&t->large, addr, imm);
}

intptr_t Forwarding_get(intptr_t addr, bool imm) {
    ForwardingTable* t = getTable(addr);

    if(isLarge(t)) 
        return getLarge(t, addr, imm);
    if(PAGETYPE_SMALL == t->type) 
        return getSmall(t, addr, imm);
    return getMedium(t, addr, imm);
}