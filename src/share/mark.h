#ifndef UBOA_GC_MARKPHASE_
#define UBOA_GC_MARKPHASE_

#include "appThrd.h"
#include "object.h"

void markerInit();

void Following_push(Object*);

void prepareRoots(AppThrd*);
void worker_markRoots();

void prepareFollowingMark();
bool verifyMarkingResult();
void worker_markFollowings();


#endif