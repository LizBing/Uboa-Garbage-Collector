#include "forwarding.h"
#include "address.h"
#include "page.h"
#include "allocator.h"
#include "relocate.h"

#include "globals.h"

#define FORW_MAPSIZESMALL (((1l << (pageSizeShiftSmall - alignmentShiftSmall))) * sizeof(atomic_short))
#define FORW_MAPSIZEMEDIUM ((1l << (pageSizeShiftMedium - alignmentShiftMedium)) * sizeof(atomic_short))

#define NREGIONSINMEDIUMPAGE (1 << (pageSizeShiftMedium - regionSizeShift))

typedef struct {
    PageType type;
    atomic_short* map;

    atomic_long count;
    intptr_t* forwarding;

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

    size_t mapSize = PAGETYPE_SMALL == t->type ? FORW_MAPSIZESMALL : FORW_MAPSIZEMEDIUM;
    void* mem = memAlloc(mapSize + sizeof(intptr_t) * p->counter);
    
    t->map = mem;
    memset(t->map, -1, mapSize);

    t->forwarding = mem + mapSize;
    t->count = 0;

    if(PAGETYPE_MEDIUM == t->type) {
        for(int i = 1; i < NREGIONSINMEDIUMPAGE; ++i) 
            t[i] = *t;
    }
}

infile_inline(bool isLarge(ForwardingTable* t)) {
    return !t->map;
}

inline void createLargeObjForwarding(intptr_t addr) {
    ForwardingTable* t = getTable(addr);
    t->map = NULL;
    t->large = NULL;
}

infile_inline(atomic_short* getForwarding(ForwardingTable* t, intptr_t addr)) {
    if(PAGETYPE_SMALL == t->type) 
        return t->map + inPageOffsSmall(addr);
    return t->map + inPageOffsMedium(addr);
}

#define WAITING_VALUE (-2)

infile_inline(intptr_t Forwarding_getInPage(ForwardingTable* t, intptr_t addr, bool imm)) {
    atomic_short* p = getForwarding(t, addr);
    short exp = -1, des = 0;
    if(atomic_compare_exchange_strong(p, &exp, WAITING_VALUE)) {
        intptr_t ret = relocate(addr);
        des = t->count++;
        t->forwarding[des] = ret;
        atomic_store(p, des);

        return ret;
    } 

    if(!imm) {
        do des = atomic_load(p);
        while(WAITING_VALUE == des);
    }

    return t->forwarding[des];
}

infile_inline(intptr_t Forwarding_getLarge(ForwardingTable* t, intptr_t addr, bool imm)) {
    intptr_t exp = 0;
    if(atomic_compare_exchange_strong(&t->large, &exp, WAITING_VALUE)) {
        intptr_t ret = relocate(addr);
        atomic_store(&t->large, ret);

        return ret;
    }

    if(!imm) {
        do exp = atomic_load(&t->large);
        while(WAITING_VALUE == exp);
    }

    return exp;
}

intptr_t Forwarding_get(intptr_t addr, bool imm) {
    ForwardingTable* t = getTable(addr);
    if(isLarge(t)) 
        return Forwarding_getLarge(t, addr, imm);
    return Forwarding_getInPage(t, addr, imm);
}