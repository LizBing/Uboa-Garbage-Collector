#ifndef ROSA_UTIL_THREADPOOL_
#define ROSA_UTIL_THREADPOOL_

#include "stdafx.h"

typedef struct ThrdPool ThrdPool;

ThrdPool* new_ThrdPool(size_t fixedSize);
void ThrdPool_run(ThrdPool*, void(*)());
bool ThrdPool_isRunning(ThrdPool*);
void ThrdPool_wait(ThrdPool*);

#endif