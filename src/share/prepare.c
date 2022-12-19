/**
 * @file prepare.c
 * @author Lizbing (lizbing@relight.com)
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#include "prepare.h"
#include "workStack.h"
#include "forwarding.h"
#include "runtime.h"
#include "vm.h"

#include "../os/mman.h"

static atomic_int allocator = 0;
inline void prepareCleanUp() {
    allocator = 0;
}

infile_inline(void selectPages(int n)) {
    Page* p = NULL;
    while((p = PageStack_pop())) 
        if(p->counter) {
            createPageForwarding(p);
            PageStack_pushSelected(n, p);
        } else 
            satisfy(delete_Page(p));
}

infile_inline(void selectLargeObjs()) {
    Object* n = NULL;
    while((n = LargeObject_pop())) {
        if(atomic_flag_test_and_set(&n->flag)) {
            createLargeObjForwarding(n);
            LargeObject_pushSelected(n);
        } else {
            size_t size = n->size;
            vfree(n, (void*)n + size, size);
            satisfy(size);
        }
    }
} 

void worker_select() {
    int n = atomic_fetch_add(&allocator, 1);
    selectPages(n);
    selectLargeObjs();
}
