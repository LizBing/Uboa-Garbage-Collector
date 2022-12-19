/**
 * @file list.h
 * @author Lizbing (lizbing@relight.com)
 * @brief linked-list node type definition
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#ifndef UBOA_GC_UTIL_LIST_
#define UBOA_GC_UTIL_LIST_

#include "stdafx.h"

typedef struct ListNode ListNode;
struct ListNode {
    intptr_t value;
    ListNode* next;
};

#endif