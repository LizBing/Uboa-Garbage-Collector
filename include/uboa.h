#ifndef UBOA_GC_USER_INTERFACE_
#define UBOA_GC_USER_INTERFACE_

#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdio.h>

#ifndef UBOA_GC_SOURCE_
typedef void* AppThrdHandle;
#endif

typedef long off_t;
typedef atomic_intptr_t* uboa_reference;

const char* uboa_copyright();
const char* uboa_name();
const char* uboa_feedBackEmail();
const char* uboa_version();

void uboa_init(int nGCWorkers, size_t xmx, long timerInterval_min, const char* logFilePath);

// following APIs should be used in safe region
void uboa_enterSafeRegion();
void uboa_exitSafeRegion();
#define uboa_safeRegion(...) \
    uboa_enterSafeRegion(); \
    __VA_ARGS__ \
    uboa_exitSafeRegion();

void uboa_gc();

AppThrdHandle uboa_createAppThrdHandle(void* stack);
void uboa_destroyAppThrdHandle(AppThrdHandle);

uboa_reference uboa_pushReference(AppThrdHandle);
void uboa_popReferences(AppThrdHandle, size_t);

void uboa_new_Oop(AppThrdHandle, uboa_reference, size_t, off_t*);
void uboa_new(AppThrdHandle, uboa_reference, size_t, uboa_reference oopRef);
void uboa_newArray(AppThrdHandle, uboa_reference, size_t, size_t count, uboa_reference oopRef);

uboa_reference uboa_null();
void uboa_assign(uboa_reference dst, uboa_reference src);
bool uboa_isNull(uboa_reference);

intptr_t uboa_load(uboa_reference, off_t);
void uboa_store(uboa_reference, off_t, intptr_t);
void uboa_loadReference(uboa_reference dst, uboa_reference src, off_t);
void uboa_storeReference(uboa_reference dst, off_t, uboa_reference src);

#endif