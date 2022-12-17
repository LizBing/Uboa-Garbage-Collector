#ifndef UBOA_GC_ALLOCATOR_
#define UBOA_GC_ALLOCATOR_

#include "object.h"

void allocatorInit();

void retire();

void* memAlloc(size_t);
void* survAllocSmall(size_t, Page** src);
void* survAllocMedium(size_t, Page** src);
void* survAllocLarge(size_t);

#endif