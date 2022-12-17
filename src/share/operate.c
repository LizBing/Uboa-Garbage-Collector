#include "barrier.h"

static const intptr_t null = 0;

uboa_reference uboa_null() {
    return &null;
}

void uboa_assign(uboa_reference dst, uboa_reference src) {
    *dst = loadValueBarrier(src);
}

bool uboa_isNull(uboa_reference r) {
    return !*r;
}

void uboa_loadReference(uboa_reference dst, uboa_reference src, off_t off) {
    uboa_assign(dst, ((Object*)loadValueBarrier(src))->data + off);
}

void uboa_storeReference(uboa_reference dst, off_t off, uboa_reference src) {
    *(uboa_reference)(((Object*)loadValueBarrier(dst))->data + off) = *src;
}

int8_t uboa_loadInt8(uboa_reference r, off_t off) {
    return *(int8_t*)(((Object*)loadValueBarrier(r))->data + off);
}

int16_t uboa_loadInt16(uboa_reference r, off_t off) {
    return *(int16_t*)(((Object*)loadValueBarrier(r))->data + off);
}

int32_t uboa_loadInt32(uboa_reference r, off_t off) {
    return *(int32_t*)(((Object*)loadValueBarrier(r))->data + off);
}

int64_t uboa_loadInt64(uboa_reference r, off_t off) {
    return *(int64_t*)(((Object*)loadValueBarrier(r))->data + off);
}

float uboa_loadFloat(uboa_reference r, off_t off) {
    return *(float*)(((Object*)loadValueBarrier(r))->data + off);
}

double uboa_loadDouble(uboa_reference r, off_t off) {
    return *(double*)(((Object*)loadValueBarrier(r))->data + off);
}

void uboa_load(void* dst, uboa_reference src, off_t off, size_t n) {
    memcpy(dst, ((Object*)loadValueBarrier(src))->data + off, n);
}

void uboa_storeInt8(uboa_reference r, off_t off, int8_t v) {
    *(int8_t*)(((Object*)loadValueBarrier(r))->data + off) = v;
}

void uboa_storeInt16(uboa_reference r, off_t off, int16_t v) {
    *(int16_t*)(((Object*)loadValueBarrier(r))->data + off) = v;
}

void uboa_storeInt32(uboa_reference r, off_t off, int32_t v) {
    *(int32_t*)(((Object*)loadValueBarrier(r))->data + off) = v;
}

void uboa_storeInt64(uboa_reference r, off_t off, int64_t v) {
    *(int64_t*)(((Object*)loadValueBarrier(r))->data + off) = v;
}

void uboa_storeFloat(uboa_reference r, off_t off, float v) {
    *(float*)(((Object*)loadValueBarrier(r))->data + off) = v;
}

void uboa_storeDouble(uboa_reference r, off_t off, double v) {
    *(double*)(((Object*)loadValueBarrier(r))->data + off) = v;
}

void uboa_store(uboa_reference dst, off_t off, void* src, size_t n) {
    memcpy(((Object*)loadValueBarrier(dst))->data + off, src, n);
}

void uboa_memset(uboa_reference r, off_t off, int8_t c, size_t n) {
    memset(((Object*)loadValueBarrier(r))->data + off, c, n);
}

void uboa_memcpy(uboa_reference dst, off_t dstOff, uboa_reference src, off_t srcOff, size_t n) {
    memcpy(((Object*)loadValueBarrier(dst))->data + dstOff, ((Object*)loadValueBarrier(src))->data + srcOff, n);
}

void uboa_memmove(uboa_reference dst, off_t dstOff, uboa_reference src, off_t srcOff, size_t n) {
    memmove(((Object*)loadValueBarrier(dst))->data + dstOff, ((Object*)loadValueBarrier(src))->data + srcOff, n);
}

int uboa_memcmp(uboa_reference r1, off_t off1, uboa_reference r2, off_t off2, size_t n) {
    return memcmp(((Object*)loadValueBarrier(r1))->data + off1, ((Object*)loadValueBarrier(r2))->data + off2, n);
}