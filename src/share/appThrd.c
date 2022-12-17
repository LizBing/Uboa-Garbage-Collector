#include "appThrd.h"

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

uboa_appThrdHandle uboa_createAppThrdHandle(void* stack) {
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

void uboa_destroyAppThrdHandle(uboa_appThrdHandle hdl) {
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

uboa_reference uboa_pushReference(uboa_appThrdHandle hdl) {
    uboa_reference ref = hdl->stack + hdl->size++;
    *ref = NULL;
    return ref;
}

void uboa_popReferences(uboa_appThrdHandle hdl, size_t n) {
    hdl->size -= n;
}