/**
 * @file oop.h
 * @author Lizbing (lizbing@relight.com)
 * @brief OOP maps for objects
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#ifndef UBOA_GC_OOP_
#define UBOA_GC_OOP_

#include "stdafx.h"

typedef struct {
    size_t size;
    off_t map[0];
} OopDec, *Oop;

#endif