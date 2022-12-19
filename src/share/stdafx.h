/**
 * @file stdafx.h
 * @author Lizbing (lizbing@relight.com)
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#ifndef UBOA_GC_STDAFX_
#define UBOA_GC_STDAFX_

typedef struct AppThrd AppThrd, *uboa_appThrdHandle;

#define UBOA_GC_SOURCE_
#include "../uboa.h"
#include "../util/assert.h"

#include <string.h>

typedef char byte;

#endif