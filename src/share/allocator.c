/**
 * @file allocator.c
 * @author Lizbing (lizbing@relight.com)
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#include "allocator.h"
#include "appThrd.h"
#include "vm.h"
#include "globals.h"
#include "barrier.h"
#include "director.h"
#include "workStack.h"

#include "../os/thread.h"

static mutex_t mtx = NULL;
static volatile Page* eden = NULL;

typedef struct {
    atomic_bool sl;
    PageType type;
    Page* p;
} SurvAllocator;
static SurvAllocator 
    survSmall = { .type = PAGETYPE_SMALL },
    survMedium = { .type = PAGETYPE_MEDIUM };

void allocatorInit() {
    mtx = new_mutex();
}

inline void retire() {
    Page_abandon(eden);
    eden = survSmall.p = survMedium.p = NULL;

    WorkStack_backUp();
}

infile_inline(size_t align(size_t s, int alignment)) {
    int n = s % alignment;
    if(!n)
        return s;
    return s - n + alignment;
}

infile_inline(void* edenAllocSmall(uboa_appThrdHandle hdl, size_t s, Page** src)) {
    Page* p = hdl->TLAB;

    void* ret = Page_alloc(p, s);
    if(!ret) {
        Page_abandon(p);
        hdl->TLAB = p = new_Page(PAGETYPE_SMALL, false);
        ret = Page_alloc(p, s);
    }

    *src = p;
    return ret;
}

infile_inline(void* edenAllocMedium(size_t s, Page** src)) {
    Page* p = eden;
    void* ret = NULL;

    while(!(ret = Page_alloc(p, s))) {
        if(mutex_trylock(mtx)) {
            if(eden == p) {
                Page_abandon(p);
                eden = new_Page(PAGETYPE_MEDIUM, false);
            }
        } else {
            uboa_exitSafeRegion();
            mutex_lock(mtx);
            uboa_enterSafeRegion();
        }
        mutex_unlock(mtx);
        p = eden;
    }

    *src = p;
    return ret;
}

infile_inline(void* edenAllocLarge(size_t s)) {
    void* ret = vmalloc(s, false);
    uboa_map(ret, s);

    return ret;
}

infile_inline(void* survAllocInPage(SurvAllocator* a, size_t s, Page** src)) {
    Page* p = a->p;
    void* ret = NULL;

    while(!(ret = Page_alloc(p, s))) {
        bool exp = false;
        if(atomic_compare_exchange_strong(&a->sl, &exp, true)) {
            if(a->p == p)
                a->p = new_Page(a->type, true);
            a->sl = false;
        } else
            while(atomic_load(&a->sl));
        p = a->p;
    }

    if(src)
        *src = p;
    return ret;
}

inline void* survAllocSmall(size_t s, Page** src) {
    return survAllocInPage(&survSmall, s, src);
}

inline void* survAllocMedium(size_t s, Page** src) {
    return survAllocInPage(&survMedium, s, src);
}

inline void* survAllocLarge(size_t s) {
    return vmalloc(s, true);
}

inline void* memAlloc(size_t s) {
    return survAllocSmall(align(s, alignmentSmall), NULL);
}

void uboa_new(uboa_appThrdHandle hdl, uboa_reference r, size_t s, uboa_reference oopRef) {
    uboa_newArray(hdl, r, s, 1, oopRef);
}

inline void Object_init(Object* this, Page* p) {
    atomic_flag_clear(&this->flag);
    this->page = p;
    if(!p)
        LargeObject_push(this);
}

void uboa_newArray(uboa_appThrdHandle hdl, uboa_reference r, size_t s, size_t count, uboa_reference oopRef) {
    Object* ptr = NULL;
    Page* src = NULL;

    size_t size = s * count + sizeof(Object);
    if(size < limitSmall) {
        size = align(size, alignmentSmall);
        ptr = edenAllocSmall(hdl, size, &src);
    }
    else if (size <= limitMedium) {
        size = align(size, alignmentMedium);
        ptr = edenAllocMedium(size, &src);
    } else {
        size = align(size, alignmentLarge);
        ptr = edenAllocLarge(size);
    }
    recordAllocation(size);

    Object_init(ptr, src);
    uboa_assign(&ptr->oopObj, oopRef);
    ptr->count = count;
    ptr->size = size;
    ptr->unit = s;

    *r = ptr;
}