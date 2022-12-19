/**
 * @file barrier.h
 * @author Lizbing (lizbing@relight.com)
 * @brief The load value barrier(AKA read barrier).
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#ifndef UBOA_GC_BARRIER_
#define UBOA_GC_BARRIER_

#include "object.h"

Object* loadValueBarrier(uboa_reference);

#endif