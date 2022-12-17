#ifndef UBOA_GC_DRIVER_
#define UBOA_GC_DRIVER_

#include "stdafx.h"

void driverInit(int _gcThrdCount);
void gcGuard(bool join, const char* reason);

#endif