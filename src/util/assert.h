/**
 * @file assert.h
 * @author Lizbing (lizbing@relight.com)
 * @brief assertion
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#ifndef UBOA_UTIL_ASSERT_
#define UBOA_UTIL_ASSERT_

#include "stdafx.h"

#define uboa_assert(expr, ...) \
    do if(!(expr)) { \
        fprintf(stderr, __VA_ARGS__); \
        putchar('\n'); \
        exit(EXIT_FAILURE); \
    } while(false)

#endif