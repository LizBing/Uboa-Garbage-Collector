/**
 * @file forwarding.h
 * @author Lizbing (lizbing@relight.com)
 * @brief Records the forwarding information.
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#ifndef UBOA_GC_FORWARDING_
#define UBOA_GC_FORWARDING_

#include "page.h"

void forwardingInit();

void createPageForwarding(Page*);
void createLargeObjForwarding(intptr_t);

intptr_t Forwarding_get(intptr_t, bool imm);

#endif