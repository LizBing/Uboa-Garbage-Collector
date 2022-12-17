#include "relocate.h"
#include "allocator.h"
#include "globals.h"
#include "forwarding.h"
#include "vm.h"
#include "workStack.h"

#include "../os/mman.h"
#include "../os/thread.h"

#include "gclog.h"

inline Object* relocate(Object* n) {
    Object* new = NULL; Page* p = NULL;

    if(n->size < limitSmall) {
        new = survAllocSmall(n->size, &p);
        memcpy(new, n, n->size);
    } else {
        if(n->size <= limitMedium)
            new = survAllocMedium(n->size, &p);
        else
            new = survAllocLarge(n->size);
        uboa_remap(new, n, n->size);
    }

    Object_init(new, p);
    return new;
}

static atomic_int allocator = 0;
inline void prepareRelocater() {
    allocator = 0;
}

void worker_relocate() {
    int n = atomic_fetch_add(&allocator, 1);

    Page* p = NULL; Object* o = NULL;
    while((p = PageStack_popSelected(n))) {
        while((o = Page_liveSetPop(p)))
            Forwarding_get(o, true);
        satisfy(delete_Page(p));
    }
    
    while((o = LargeObject_popSelected()))
        Forwarding_get(o, true);
    
}