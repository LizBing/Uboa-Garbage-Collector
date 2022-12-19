/**
 * @file queue.h
 * @author Lizbing (lizbing@relight.com)
 * @brief lockfree queue
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#ifndef UBOA_GC_UTIL_QUEUE_
#define UBOA_GC_UTIL_QUEUE_

#include "list.h"

typedef struct {
    _Atomic(ListNode*) head, tail;
} Queue;

void Queue_init(Queue*);
void Queue_push(Queue*, ListNode*, intptr_t value);
intptr_t Queue_pop(Queue*);

#endif