#ifndef UBOA_GC_WORKSTACK_
#define UBOA_GC_WORKSTACK_

#include "object.h"

void WorkStack_init();

void WorkStack_backUp();

void PageStack_push(Page*);
Page* PageStack_pop();
void PageStack_pushSelected(int, Page*);
Page* PageStack_popSelected(int);

void LargeObject_push(Object*);
Object* LargeObject_pop();
void LargeObject_pushSelected(Object*);
intptr_t LargeObject_popSelected();

#endif