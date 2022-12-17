#ifndef UBOA_GC_VM_
#define UBOA_GC_VM_

#include "stdafx.h"
#include "../os/mman.h"

void vmInit();

void resetView();

void satisfy(size_t);
void verifyGCResult();

void enableRequest();
void disableRequest();

void* vmalloc(size_t, bool tagRemapped);
void vfree(void* begin, void* end, size_t);

#endif