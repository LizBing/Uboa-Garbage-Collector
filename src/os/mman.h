#ifndef UBOA_GC_EXT_MMAN_
#define UBOA_GC_EXT_MMAN_

#include "stdafx.h"

void uboa_map(void*, size_t);
void uboa_remap(void* dst, void* src, size_t);
void uboa_unmap(void*, size_t);

#endif