/**
 * @file appThrd.h
 * @author Lizbing (lizbing@relight.com)
 * @brief The application thread management
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#ifndef UBOA_GC_APPLICATIONTHREAD_
#define UBOA_GC_APPLICATIONTHREAD_

#include "page.h"
#include "address.h"

#include "../os/thread.h"

typedef struct AppThrd AppThrd, *uboa_appThrdHandle;
struct AppThrd {
    _Atomic(Page*) TLAB;

    bool free;
    atomic_flag lock;
    AppThrd* prev, *next;

    atomic_size_t size;
    uboa_pointer stack[0];
};

void forEachRoot(void(*func)(AppThrd*));

#endif