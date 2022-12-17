#include "appThrd.h"
#include "barrier.h"

static const intptr_t null = 0;
static atomic_flag sl = ATOMIC_FLAG_INIT;
static AppThrd* head = NULL;

void forEachRoot(void(*func)(AppThrd*)) {
    AppThrd* iter = NULL, *n = NULL;
    
    bool tag = false;
    for(;;) {
        while(atomic_flag_test_and_set(&sl));

        if(!iter) {
            if(tag) {
                atomic_flag_clear(&sl);
                return;
            }
            
            iter = head;
            tag = true;
        }
        
        n = iter;
        iter = n->next;

        atomic_flag_test_and_set(&n->flag);

        atomic_flag_clear(&sl);
        
        func(n);
        atomic_flag_clear(&n->flag);
    }
}

AppThrdHandle uboa_createAppThrdHandle(void* stack) {
    AppThrd* at = stack;
    memset(at, 0, sizeof(AppThrd));

    while(atomic_flag_test_and_set(&sl));

    if(head) {
        at->next = head;
        head->prev = at;
    }
    head = at;

    atomic_flag_clear(&sl);

    return at;
}

void uboa_destroyAppThrdHandle(AppThrdHandle hdl) {
    while(atomic_flag_test_and_set(&sl));

    AppThrd* prev = hdl->prev, *next = hdl->next;
    if(prev)
        prev->next = next;
    else 
        head = next;

    if(next) 
        next->prev = prev;

    atomic_flag_clear(&sl);

    while(atomic_flag_test_and_set(&hdl->flag));
}

uboa_reference uboa_null() {
    return &null;
}

uboa_reference uboa_pushReference(AppThrdHandle hdl) {
    uboa_reference ref = hdl->stack + hdl->size++;
    *ref = NULL;
    return ref;
}

void uboa_popReferences(AppThrdHandle hdl, size_t n) {
    hdl->size -= n;
}

void uboa_assign(uboa_reference dst, uboa_reference src) {
    *dst = loadValueBarrier(src);
}

bool uboa_isNull(uboa_reference r) {
    return !*r;
}

intptr_t uboa_load(uboa_reference r, off_t off) {
    return *(intptr_t*)(((Object*)loadValueBarrier(r))->data + off);
}

void uboa_store(uboa_reference r, off_t off, intptr_t v) {
    *(intptr_t*)(((Object*)loadValueBarrier(r))->data + off) = v;
}

void uboa_loadReference(uboa_reference dst, uboa_reference src, off_t off) {
    uboa_assign(dst, ((Object*)loadValueBarrier(src))->data + off);
}

void uboa_storeReference(uboa_reference dst, off_t off, uboa_reference src) {
    *(uboa_reference)(((Object*)loadValueBarrier(dst))->data + off) = *src;
}
