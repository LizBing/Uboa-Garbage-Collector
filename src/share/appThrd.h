#ifndef UBOA_GC_APPLICATIONTHREAD_
#define UBOA_GC_APPLICATIONTHREAD_

#include "page.h"
#include "address.h"

#include "../os/thread.h"

typedef struct AppThrd AppThrd, *AppThrdHandle;
struct AppThrd {
    Page* TLAB;

    atomic_flag flag;
    AppThrd* prev, *next;

    atomic_size_t size;
    atomic_intptr_t stack[0];
};

void forEachRoot(void(*func)(AppThrd*));

#endif