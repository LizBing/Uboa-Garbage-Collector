/**
 * @file page.c
 * @author Lizbing (lizbing@relight.com)
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#include "page.h"
#include "vm.h"
#include "globals.h"
#include "director.h"
#include "runtime.h"
#include "address.h"
#include "workStack.h"

inline Page* new_Page(PageType t, bool surv) {
    size_t s = 0; Page* this = NULL;
    if(PAGETYPE_SMALL == t) {
        s = pageSizeSmall;
        this = calloc(1, sizeof(Page));
    } else {
        s = pageSizeMedium;
        this = calloc(1, sizeof(Page) + sizeof(intptr_t) * (1l << pageSizeShiftMedium - limitShiftSmall));
    }
    uboa_assert(this, "out of memory");

    this->type = t;
    this->top = this->begin = vmalloc(s, surv);
    this->end = this->begin + s;

    if(PAGETYPE_MEDIUM != t || !surv) 
        uboa_map(this->begin, s);

    PageStack_push(this);
    return this;
}

inline bool Page_available(Page* this) {
    if(this) 
        return PV_CURRENT == getView((intptr_t)this->begin);
    return false;
}

inline void Page_abandon(Page* this) {
    if(this) {
        size_t s = this->end - this->top;
        this->top = this->end;
        recordAllocation(s);
    }
}

inline size_t delete_Page(Page* this) {
    size_t ps = this->top - this->begin;
    vfree(this->begin, this->end, ps);
    free(this);

    return ps;
}

inline void* Page_alloc(Page* this, size_t s) {
    if(!Page_available(this))
        return NULL;
        
    void* ret = this->top, *newTop = NULL;
    do {
        newTop = ret + s;
        if(newTop > this->end)
            return NULL;
    } while(!atomic_compare_exchange_strong(&this->top, &ret, newTop));
    
    return ret;
}

inline void Page_liveSetPush(Page* this, Object* n) {
    int i = this->counter++;
    this->top -= n->size;

    if(PAGETYPE_SMALL == this->type) 
        Stack_push(&this->liveSet.small, &n->nodeLiveSet, n);
    else 
        this->liveSet.medium[i] = n;
}

inline intptr_t Page_liveSetPop(Page* this) {
    if(PAGETYPE_SMALL == this->type)
        return Stack_pop(&this->liveSet.small);

    int i = --this->counter;
    if(i >= 0)
        return this->liveSet.medium[i];
    return 0;
}