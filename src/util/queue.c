/**
 * @file queue.c
 * @author Lizbing (lizbing@relight.com)
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#include "queue.h"

inline void Queue_init(Queue* this) {
    this->head = NULL;
}

inline void Queue_push(Queue* this, ListNode* n, intptr_t value) {
    n->next = NULL;
    n->value = value;

    ListNode* prev = atomic_exchange(&this->tail, n), *exp = NULL;
    if(!atomic_compare_exchange_strong(&this->head, &exp, n)) 
        prev->next = n;
}

inline intptr_t Queue_pop(Queue* this) {
    ListNode* n = this->head, *next = NULL;
    do {
        if(!n)
            return NULL;
        next = n->next;
    } while(!atomic_compare_exchange_strong(&this->head, &n, next));

    return n->value;
}