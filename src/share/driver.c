/**
 * @file driver.c
 * @author Lizbing (lizbing@relight.com)
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#include "driver.h"
#include "globals.h"
#include "runtime.h"

#include "allocator.h"
#include "vm.h"
#include "gclog.h"

#include "mark.h"
#include "prepare.h"
#include "relocate.h"

#include "director.h"

#include "../util/thrdPool.h"

static rwlock_t monitor = NULL;
static ThrdPool* workers = NULL;

static mutex_t mtx = NULL;
static cond_t cond = NULL;

#define TIME_100MS (100 * 1000)

static void manage() {
    gclogf("GC service start.");

    for(;;) {
        long c = getTime();

        char* reason = makeGCDecision();
        if(reason)
            gcGuard(false, reason);
        GCLog_output();

        c = getTime() - c;
        if(c < TIME_100MS)
            sleep_us(TIME_100MS - c);
    }
}

void driverInit(int _gcThrdCount) {
    nGCWorkers = _gcThrdCount;
    workers = new_ThrdPool(nGCWorkers);

    monitor = new_rwlock();
    mtx = new_mutex();
    cond = new_cond();

    thread_detach(new_thread(manage, NULL));
}

void uboa_enterSafeRegion() {
    rwlock_rdlock(monitor);
}

void uboa_exitSafeRegion() {
    rwlock_unlock(monitor);
}

infile_inline(void runAndWait(void(*func)())) {
    ThrdPool_run(workers, func);
    ThrdPool_wait(workers);
}

static void lambda(AppThrd* at) {
    prepareRoots(at);
    runAndWait(worker_markRoots);
}

infile_inline(void markRoots()) {
    forEachRoot(lambda);
}

infile_inline(void markFollowings()) {
    prepareFollowingMark();
    runAndWait(worker_markFollowings);
}

infile_inline(void cleanUp()) {
    prepareCleanUp();
    runAndWait(worker_select);
}

infile_inline(void relocationPhase()) {
    prepareRelocater();
    ThrdPool_run(workers, worker_relocate);
    ThrdPool_wait(workers);
}

static long stopTheWorld(const char* s) {
    gclogf("synchronization point(reason: %s).", s);

    long ret = getTime();
    rwlock_wrlock(monitor);

    return ret;
}

static void resumeTheWorld(long t) {
    rwlock_unlock(monitor);

    gclogf("The world had been stopped for %ld us.", getTime() - t);
}

static volatile bool working = false;
static void gc(const char* s) {
    ++rt.gcCount;
    size_t tmpRemaining = rt.remaining;
    gclogf("A new GC cycle(%ld) is occurred for the rule '%s'(remaining: %zu).", rt.gcCount, s, tmpRemaining);

    long timer = getTime();
    long stwt = stopTheWorld("mark phase start");

    resetView();
    retire();
    rt.isMarking = true;

    resumeTheWorld(stwt);

    markRoots();
    markFollowings();

    stwt = stopTheWorld("clean-up phase start");

    if(!verifyMarkingResult())
        markFollowings();

    rt.isMarking = false;
    rt.isCleaningUp = true;

    resumeTheWorld(stwt);

    cleanUp();

    stwt = stopTheWorld("relocation phase start");

    rt.isCleaningUp = false;
    disableRequest();

    resumeTheWorld(stwt);

    relocationPhase();

    verifyGCResult();
    gclogf("The GC cycle finished in %ld us(remaining: %zu -> %zu).", recordGCTime(timer), tmpRemaining, rt.remaining);
    enableRequest();

    working = false;
    cond_notifyAll(cond);
}

inline void gcGuard(bool join, const char* s) {
    mutex_lock(mtx);

    if(join) {
        while(working)
            cond_wait(cond, mtx);

        working = true;
        gc(s);
    }

    else if(!working) {
        working = true;
        thread_detach(new_thread(gc, s));
    }

    mutex_unlock(mtx);
}

void uboa_gc(const char* reason) {
    gcGuard(true, reason);
}