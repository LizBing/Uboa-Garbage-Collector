#define _GNU_SOURCE
#include <unistd.h>
#include <sys/mman.h>

#include "../mman.h"

inline void uboa_map(void* addr, size_t s) {
    uboa_assert(MAP_FAILED != mmap(addr, s, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0),
        "failed to call 'mmap'(address = %p, size = %lu)",
        addr, s
    );
}

inline void uboa_remap(void* dst, void* src, size_t s) {
    uboa_assert(
        MAP_FAILED != mremap(src, s, s, MREMAP_FIXED | MREMAP_MAYMOVE, dst), 
        "failed to call 'mremap'(destination address = %p, source address = %p, size = %lu).",
        dst, src, s
    );
}

inline void uboa_unmap(void* addr, size_t s) {
    uboa_assert(!munmap(addr, s), "failed to call 'munmap'(address = %p, size = %lu).", addr, s);
}