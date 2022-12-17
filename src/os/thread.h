#ifndef ROSA_EXT_THREAD_
#define ROSA_EXT_THREAD_

#include "stdafx.h"

typedef void* NativeHandle;
typedef NativeHandle rwlock_t, mutex_t, thread_t, cond_t;

rwlock_t new_rwlock();
void rwlock_wrlock(rwlock_t);
void rwlock_rdlock(rwlock_t);
void rwlock_unlock(rwlock_t);

mutex_t new_mutex();
bool mutex_trylock(mutex_t);
void mutex_lock(mutex_t);
void mutex_unlock(mutex_t);

thread_t new_thread(void(*)(void*), void* arg);
void* thread_join(thread_t);
void thread_detach(thread_t);

cond_t new_cond();
void cond_notify(cond_t);
void cond_notifyAll(cond_t);
void cond_wait(cond_t, mutex_t);

#endif