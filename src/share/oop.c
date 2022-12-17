#include "oop.h"
#include "object.h"

void uboa_new_Oop(AppThrdHandle hdl, uboa_reference r, size_t s, off_t* m) {
    size_t size = sizeof(off_t) * s;
    uboa_new(hdl, r, sizeof(OopDec) + size, uboa_null());
    
    Oop oop = ((Object*)*r)->data;
    oop->size = s;
    memcpy(oop->map, m, size);
}