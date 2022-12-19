/**
 * @file director.c
 * @author Lizbing (lizbing@relight.com)
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#include "director.h"
#include "runtime.h"
#include "globals.h"
#include "gclog.h"

#include "driver.h"

#include "../os/time.h"

static long ruleTimerInterval = 0;
static int warmedUp = 1;
static long lastFinished = 0, avgTime = 0, maxTime = 0;
static atomic_size_t durationAlloc = 0;
static bool ruleProactiveEnabled = false;

void directorInit(long _interval, bool enableRuleProactive) {
    ruleTimerInterval = _interval * 60 * 1000 * 1000;
    ruleProactiveEnabled = enableRuleProactive;
}

infile_inline(bool isRuleTimerAvailable()) {
    return ruleTimerInterval;
}

infile_inline(bool isWarmed()) {
    return warmedUp > 3;
}

infile_inline(bool rule_warmUp()) {
    if(!isWarmed()) {
        int n = 10 - warmedUp;
        long m = (double)XMX * 0.1 * n;
        if(rt.remaining <= m) {
            ++warmedUp;
            return true;
        }
    }

    return false;
}

infile_inline(bool rule_timer()) {
    static long lastCall = 0;

    if(!isWarmed() || !ruleTimerInterval) 
        return false;

    long 
        tmp = lastCall,
        now = lastCall = getTime();

    return now - tmp >= ruleTimerInterval;
}

inline long recordGCTime(long t) {
    lastFinished = getTime();
    t = getTime() - t;

    if(!avgTime)
        avgTime = t;
    else
        avgTime = (avgTime * 30 + t * 2) / 32;

    if(maxTime < t)
        maxTime = t;

    return t;
}

inline bool rule_proactive() {
    long past = getTime() - lastFinished;
    if(!isWarmed() || !ruleProactiveEnabled)
        return false;
    return 
        past >= 5 * 60 * 1000 * 1000 &&
        (double)XMX * 0.9 >= rt.remaining &&
        past >= maxTime * 49;
}

inline void recordAllocation(size_t s) {
    durationAlloc += s;
}

infile_inline(bool rule_allocationRate()) {
    static long lastCall = 0;
    static double avgAllocRate = 0;

    long 
        tmp = lastCall,
        now = lastCall = getTime(),
        time = now - tmp;

    double sample = (double)atomic_exchange(&durationAlloc, 0) / time;
    if(avgAllocRate)
        avgAllocRate = (avgAllocRate * 30 + sample * 2) / 32;
    else 
        avgAllocRate = sample;

    if(!isWarmed())
        return false;

    long timeOOM = rt.remaining / avgAllocRate;
    return timeOOM <= avgTime;
}

inline const char* makeGCDecision() {
    if(rule_allocationRate())
        returnString("allocation rate");
    if(rule_timer())
        returnString("timer");
    if(rule_warmUp())
        returnString("warm up");
    if(rule_proactive())
        returnString("proactive");

    return NULL;
}

