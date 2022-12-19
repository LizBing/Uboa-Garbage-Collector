/**
 * @file stqck.c
 * @author Lizbing (lizbing@relight.com)
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#include "stack.h"

inline void Stack_init(Stack* this) {
    *this = NULL;
}

inline bool Stack_empty(Stack* this) {
    return !*this;
}

inline void Stack_push(Stack* this, ListNode* n, intptr_t value) {
    n->value = value;

    ListNode* next = *this;
    do 
        n->next = next;
    while(!atomic_compare_exchange_strong(this, &next, n));
}

inline intptr_t Stack_pop(Stack* this) {
    ListNode* n = *this, *newTop = NULL;
    
    do {
        if(!n)
            return 0;
        newTop = n->next;
    } while(!atomic_compare_exchange_strong(this, &n, newTop));

    return n->value;
}