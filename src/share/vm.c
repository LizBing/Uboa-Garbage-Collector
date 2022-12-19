/**
 * @file vm.c
 * @author Lizbing (lizbing@relight.com)
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#include "vm.h"
#include "runtime.h"
#include "globals.h"
#include "gclog.h"

#include "driver.h"

#include "../util/stack.h"

typedef struct {
    size_t value;
    ListNode node;
} Request;

static Stack reqStack = NULL;
static mutex_t mtx = NULL;
static cond_t cond = NULL;

static _Atomic(intptr_t) top = 0;

static atomic_long req = 0;

inline void vmInit() {
    mtx = new_mutex();
    cond = new_cond();

    rt.viewCurrent = VIEW0;
    rt.viewPrevious = VIEW1;
    rt.viewNext = VIEW2;

    top = VIEW0 << 44;
}

inline void resetView() {
    long tmp = rt.viewCurrent;
    rt.viewCurrent = rt.viewNext;
    rt.viewNext = rt.viewPrevious;
    rt.viewPrevious = tmp;

    top = rt.viewCurrent << 44;
}

inline void satisfy(size_t n) {
    if(!n)
        return;
        
    long tmp = atomic_fetch_sub(&req, n);
    while(tmp <= 0) {
        cond_notifyAll(cond);
        Request* r = Stack_pop(&reqStack);
        if(!r)
            return;
        tmp = atomic_fetch_add(&req, r->value);
        free(r);
    }
}

inline void verifyGCResult() {
    uboa_assert(Stack_empty(&reqStack) && req <= 0, "Out of memory(GC).");
    req = 0;
}

static volatile bool pushable = true;
inline void enableRequest() {
    pushable = true;
    cond_notifyAll(cond);
}

inline void disableRequest() {
    pushable = false;
}

infile_inline(void pushRequest(size_t s)) {
    Request* request = malloc(sizeof(Request));
    uboa_assert(request, "Out of memory.");
    request->value = s;

    while(!pushable)
        cond_wait(cond, mtx);
    Stack_push(&reqStack, &request->node, request);

    uboa_exitSafeRegion();

    gcGuard(false, "OOM");
    while(rt.remaining < s)
        cond_wait(cond, mtx);

    uboa_enterSafeRegion();
}

inline void* vmalloc(size_t s, bool tagRemapped) {
    size_t tmp = 0;

    if(!tagRemapped) {
        mutex_lock(mtx);
        if(rt.remaining < s) 
            pushRequest(s);

        tmp = atomic_fetch_sub(&rt.remaining, s);
        mutex_unlock(mtx);
    }
    
    return atomic_fetch_add(&top, s);
}

inline void vfree(void* begin, void* end, size_t s) {
    uboa_unmap(begin, end - begin);
    size_t tmp = atomic_fetch_add(&rt.remaining, s);
}