#ifndef UBOA_RUNTIME_
#define UBOA_RUNTIME_

#include "stdafx.h"

#include "../os/thread.h"

typedef struct Runtime {
    long viewCurrent, viewPrevious, viewNext;

    long gcCount;
    volatile bool isMarking;
    volatile bool isCleaningUp;

    atomic_size_t remaining;
} Runtime;
extern Runtime rt;

#endif