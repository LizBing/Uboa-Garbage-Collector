/**
 * @file page.h
 * @author Lizbing (lizbing@relight.com)
 * @brief page management
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#ifndef UBOA_GC_PAGE_
#define UBOA_GC_PAGE_

#include "stdafx.h"
#include "../util/stack.h"

typedef struct Object Object;

typedef enum {
    PAGETYPE_SMALL, 
    PAGETYPE_MEDIUM,
} PageType;

typedef struct Page {
    PageType type;
    ListNode node;

    void* begin, *end;
    _Atomic(void*) top;

    atomic_size_t counter;
    struct {
        Stack small;
        intptr_t medium[0];
    } liveSet;
} Page;

Page* new_Page(PageType, bool surv);

bool Page_available(Page*);
void Page_abandon(Page*);
size_t delete_Page(Page*);

void* Page_alloc(Page*, size_t);

void Page_liveSetPush(Page*, Object*);
intptr_t Page_liveSetPop(Page*);

#endif