#ifndef UBOA_GC_RELOCATE_
#define UBOA_GC_RELOCATE_

#include "object.h"

Object* relocate(Object*);

void prepareRelocater();
void worker_relocate();

#endif