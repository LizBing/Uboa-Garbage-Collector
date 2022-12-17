#ifndef UBOA_GC_DIRECTOR_
#define UBOA_GC_DIRECTOR_

#include "stdafx.h"

void directorInit(long interval, bool enableRuleProactive);

bool rule_proactive();
const char* makeGCDecision();

void recordAllocation(size_t);
long recordGCTime(long);

#endif