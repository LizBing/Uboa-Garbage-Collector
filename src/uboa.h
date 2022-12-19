#ifndef UBOA_GC_USER_INTERFACE_
#define UBOA_GC_USER_INTERFACE_

#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

#ifndef UBOA_GC_SOURCE_
typedef void* uboa_appThrdHandle;
#endif

typedef long off_t;
typedef atomic_intptr_t* uboa_reference;

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
#define uboa_safeRegion(...) \
    uboa_enterSafeRegion(); \
    __VA_ARGS__ \
    uboa_exitSafeRegion();

void uboa_gc(const char* reason);

void uboa_new_Oop(uboa_appThrdHandle, uboa_reference, size_t, off_t*);
void uboa_new(uboa_appThrdHandle, uboa_reference, size_t, uboa_reference oopRef);
void uboa_newArray(uboa_appThrdHandle, uboa_reference, size_t, size_t count, uboa_reference oopRef);

uboa_reference uboa_null();
void uboa_assign(uboa_reference dst, uboa_reference src);
bool uboa_isNull(uboa_reference);
bool uboa_equal(uboa_reference, uboa_reference);

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