/**
 * @file uboa.h
 * @author Lizbing (lizbing@relight.team)
 * @version 0.0.0dev5
 * @date 2022-12-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef UBOA_GC_USER_INTERFACE_
#define UBOA_GC_USER_INTERFACE_

#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdio.h>

#ifndef UBOA_GC_SOURCE_
typedef void* uboa_appThrdHandle;
#endif

typedef long off_t;
typedef atomic_intptr_t* uboa_reference, uboa_pointer;
#define uboa_weakReference(ptr) (&(ptr))

typedef struct {
    size_t size;
    off_t* map;
} OopDec, *Oop;

const char* uboa_copyright();
const char* uboa_name();
const char* uboa_feedBackEmail();
const char* uboa_version();

void uboa_init(int nGCWorkers, size_t xmx, long timerInterval_min, bool enableRuleProactive, const char* logFilePath);

uboa_appThrdHandle uboa_createAppThrdHandle(void* stack);
void uboa_destroyAppThrdHandle(uboa_appThrdHandle);

uboa_reference uboa_pushReference(uboa_appThrdHandle);
void uboa_popReferences(uboa_appThrdHandle, size_t);

void uboa_enterSafeRegion();
void uboa_exitSafeRegion();
void uboa_enterRiskRegion(uboa_appThrdHandle);
void uboa_exitRiskRegion(uboa_appThrdHandle);

void uboa_gc(const char* reason);

void uboa_new(uboa_appThrdHandle, uboa_reference, size_t, Oop oop);
void uboa_newArray(uboa_appThrdHandle, uboa_reference, size_t, size_t count, Oop oop);

#define uboa_nullptr ((uboa_pointer)0)
uboa_reference uboa_nullref();

void uboa_assign(uboa_reference dst, uboa_reference src);
bool uboa_isNull(uboa_reference);
bool uboa_equal(uboa_reference, uboa_reference);

void* uboa_castToObject(uboa_appThrdHandle, uboa_reference);
uboa_pointer uboa_castToPointer(void*);

int8_t uboa_loadInt8(uboa_reference, off_t);
int16_t uboa_loadInt16(uboa_reference, off_t);
int32_t uboa_loadInt32(uboa_reference, off_t);
int64_t uboa_loadInt64(uboa_reference, off_t);
float uboa_loadFloat(uboa_reference, off_t);
double uboa_loadDouble(uboa_reference, off_t);
void uboa_loadReference(uboa_reference dst, uboa_reference src, off_t);
void uboa_load(void*, uboa_reference, off_t, size_t);

void uboa_storeInt8(uboa_reference, off_t, int8_t);
void uboa_storeInt16(uboa_reference, off_t, int16_t);
void uboa_storeInt32(uboa_reference, off_t, int32_t);
void uboa_storeInt64(uboa_reference, off_t, int64_t);
void uboa_storeFloat(uboa_reference, off_t, float);
void uboa_storeDouble(uboa_reference, off_t, double);
void uboa_storeReference(uboa_reference dst, off_t, uboa_reference src);
void uboa_store(uboa_reference, off_t, void*, size_t);

void uboa_memset(uboa_reference, off_t, int8_t, size_t);
void uboa_memcpy(uboa_reference dst, off_t dstOff, uboa_reference src, off_t srcOff, size_t);
void uboa_memmove(uboa_reference dst, off_t dstOff, uboa_reference src, off_t srcOff, size_t);
int uboa_memcmp(uboa_reference r1, off_t off1, uboa_reference r2, off_t off2, size_t);

#endif