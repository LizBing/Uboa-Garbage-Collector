#ifndef ROSA_UTIL_ASSERT_
#define ROSA_UTIL_ASSERT_

#include "stdafx.h"

#define uboa_assert(expr, ...) \
    do if(!(expr)) { \
        fprintf(stderr, __VA_ARGS__); \
        putchar('\n'); \
        exit(EXIT_FAILURE); \
    } while(false)

#endif