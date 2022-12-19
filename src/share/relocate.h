/**
 * @file relocate.h
 * @author Lizbing (lizbing@relight.com)
 * @brief 
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#ifndef UBOA_GC_RELOCATE_
#define UBOA_GC_RELOCATE_

#include "object.h"

Object* relocate(Object*);

void prepareRelocater();
void worker_relocate();

#endif