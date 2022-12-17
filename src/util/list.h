#ifndef UBOA_GC_UTIL_LIST_
#define UBOA_GC_UTIL_LIST_

#include "stdafx.h"

typedef struct ListNode ListNode;
struct ListNode {
    intptr_t value;
    ListNode* next;
};

#endif