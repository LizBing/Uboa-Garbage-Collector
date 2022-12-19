/**
 * @file address.h
 * @author Lizbing (lizbing@relight.com)
 * @brief Decoding the pointers.
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#ifndef UBOA_GC_POINTER_
#define UBOA_GC_POINTER_

#include "stdafx.h"

typedef enum {
    PV_CURRENT,
    PV_PREVIOUS,
    PV_NEXT,
    PV_NULL
} PtrView;

PtrView getView(intptr_t);

intptr_t address(intptr_t);
int regionNo(intptr_t);
int inPageOffsSmall(intptr_t);
int inPageOffsMedium(intptr_t);

#endif