/**
 * @file driver.h
 * @author Lizbing (lizbing@relight.com)
 * @brief The driver of GC.
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#ifndef UBOA_GC_DRIVER_
#define UBOA_GC_DRIVER_

#include "stdafx.h"

void driverInit(int _gcThrdCount);
void gcGuard(bool join, const char* reason);

#endif