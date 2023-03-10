/**
 * @file object.h
 * @author Lizbing (lizbing@relight.com)
 * @brief stores the objects' information
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#ifndef UBOA_GC_OBJECT_
#define UBOA_GC_OBJECT_

#include "page.h"

struct Object {
    ListNode nodeFollowing, nodeLiveSet;
    atomic_flag flag;

    Page* page;

    Oop oop;
    size_t size, unit, count;
    
    byte data[0];
};

void Object_init(Object*, Page*);

#endif