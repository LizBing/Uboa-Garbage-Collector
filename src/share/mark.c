#include "mark.h"
#include "forwarding.h"
#include "globals.h"

static Stack following = NULL;

static mutex_t mtx = NULL;
static cond_t cond = NULL;
static int waiting = 0;

static struct {
    volatile size_t snapshot;
    atomic_long iterator;
    AppThrd* appThrd;
} task = { 0 };

infile_inline(size_t getMinTask()) {
    size_t tmp = task.appThrd->size;
    if(task.snapshot <= tmp)
        return task.snapshot;
    else
        task.snapshot = tmp;
    return tmp;
}

void markerInit() {
    mtx = new_mutex();
    cond = new_cond();
}

inline void Following_push(Object* n) {
    if(!atomic_flag_test_and_set(&n->flag)) 
        Stack_push(&following, &n->nodeFollowing, n);
}

infile_inline(Object* Following_pop()) {
    Object* ret = NULL;
    while(!(ret = (Object*)Stack_pop(&following))) {
        mutex_lock(mtx);

        if(waiting == nGCWorkers) {
            mutex_unlock(mtx);
            return NULL;
        }
        ++waiting;
        if(waiting == nGCWorkers) {
            cond_notifyAll(cond);
            mutex_unlock(mtx);
            return NULL;
        }

        cond_wait(cond, mtx);
        if(waiting == nGCWorkers) {
            mutex_unlock(mtx);
            return NULL;
        }

        --waiting;
        mutex_unlock(mtx);
    }

    return ret;
}

infile_inline(void LiveSet_push(Object* n)) {
    Page* p = n->page;
    if(p) 
        Page_liveSetPush(p, n);
}

inline void prepareRoots(AppThrd* at) {
    Page* p = at->TLAB;
    if(!Page_available(p)) 
        if(atomic_compare_exchange_strong(&at->TLAB, &p, NULL))
            Page_abandon(p);
    
    task.iterator = 0;
    task.appThrd = at;
    task.snapshot = at->size;
}

inline void prepareFollowingMark() {
    waiting = 0;
}

infile_inline(bool mark(uboa_reference r)) {
    intptr_t exp = *r;
    if(!exp)
        return false;

    PtrView pv = getView(exp);
    if(PV_PREVIOUS == pv)
        Following_push(exp);
    else if(PV_NEXT == pv) {
        intptr_t des = Forwarding_get(exp, false);
        if(atomic_compare_exchange_strong(r, &exp, des))
            Following_push(des);
        else
            return false;
    } else
        return false;

    return true;
}

void worker_markRoots() {
    long i = 0;
    while((i = task.iterator++) < getMinTask())
        mark(task.appThrd->stack + i);
}

void worker_markFollowings() {
    Object* n = NULL;
    while((n = Following_pop())) {
        bool trigger = false;
        if(n->oopObj) {
            mark(&n->oopObj);
            Oop oop = ((Object*)n->oopObj)->data;

            for(long i = 0; i < n->count; ++i) {
                void* base = n->data + n->unit * i;

                for(long j = 0; j < oop->size; ++j)
                    if(mark(base + oop->map[j]))
                        trigger = true;
            }
        }

        if(trigger && waiting)
            cond_notifyAll(cond);

        LiveSet_push(n);
    }
}

inline bool verifyMarkingResult() {
    return !following;
}

