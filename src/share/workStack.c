#include "workStack.h"
#include "globals.h"

static struct {
    Stack current, previous;
} inPage = { 0 }, large = { 0 };

static Stack* selectedPages = NULL;

static atomic_long top = 0;
static intptr_t* selectedLarge = NULL;

void WorkStack_init() {
    selectedLarge = malloc(sizeof(intptr_t) * (XMX >> 24));
    selectedPages = calloc(nGCWorkers, sizeof(Stack));
    uboa_assert(selectedLarge && selectedPages, "out of memory.");
}

void WorkStack_backUp() {
    inPage.previous = inPage.current;
    inPage.current = NULL;

    large.previous = large.current;
    large.current = NULL;

    top = 0;
}

inline void PageStack_push(Page* p) {
    Stack_push(&inPage.current, &p->node, p);
}

inline Page* PageStack_pop() {
    return Stack_pop(&inPage.previous);
}

inline void PageStack_pushSelected(int id, Page* p) {
    Stack_push(selectedPages + id, &p->node, p);
}

inline Page* PageStack_popSelected(int id) {
    return Stack_pop(selectedPages + id);
}

inline void LargeObject_push(Object* n) {
    Stack_push(&large.current, &n->nodeLiveSet, n);
}

inline Object* LargeObject_pop() {
    return Stack_pop(&large.previous);
}

inline void LargeObject_pushSelected(Object* n) {
    selectedLarge[top++] = n;
}

inline intptr_t LargeObject_popSelected() {
    long n = --top;
    if(n >= 0)
        return selectedLarge[n];
    return 0;
}