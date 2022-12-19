/**
 * @file stack.h
 * @author Lizbing (lizbing@relight.com)
 * @brief lockfree stack
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#ifndef UBOA_GC_LOCKFREESTACK_
#define UBOA_GC_LOCKFREESTACK_

#include "list.h"

typedef _Atomic(ListNode*) Stack;

void Stack_init(Stack*);
bool Stack_empty(Stack*);
void Stack_push(Stack*, ListNode*, intptr_t value);
intptr_t Stack_pop(Stack*);

#endif