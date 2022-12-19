/**
 * @file prepare.h
 * @author Lizbing (lizbing@relight.com)
 * @brief The prepare phase of GC cycle.
 * @version 0.0.0dev5
 * @date 2022-12-20
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#ifndef UBOA_GC_PHASE_PRERARE_
#define UBOA_GC_PHASE_PRERARE_

#include "object.h"

void prepareCleanUp();
void worker_select();

#endif