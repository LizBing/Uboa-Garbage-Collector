#ifndef UBOA_GC_DEBUG_
#define UBOA_GC_DEBUG_

#include "stdafx.h"
#include "../os/time.h"
#include "../util/stack.h"

#include <alloca.h>

bool isLoggerAvailable();
void logInit(const char* filePath);

void GCLog_push(const char*, size_t);
void GCLog_output();

extern FILE* logFile;
#define gclogf(...) \
    if(isLoggerAvailable()) do { \
        size_t len = snprintf(NULL, 0, __VA_ARGS__) + 1; \
        char* str = alloca(len); \
        sprintf(str, __VA_ARGS__); \
        GCLog_push(str, len); \
    } while(false)

#define returnString(n) \
    do { \
        static char text[] = n; \
        return text; \
    } while(false)

#endif