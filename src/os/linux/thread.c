#include "../thread.h"

#define __USE_UNIX98
#include <pthread.h>
#include <stdlib.h>

typedef struct {
    pthread_mutex_t mtx;
    pthread_cond_t cond;

    bool blocked;
    int count;
} RWLock;

inline rwlock_t new_rwlock() {
    RWLock* ret = calloc(1, sizeof(RWLock));
    uboa_assert(ret, "out of memory.");

    pthread_mutex_init(&ret->mtx, NULL);
    pthread_cond_init(&ret->cond, NULL);
    
    return ret;
}

inline void rwlock_wrlock(rwlock_t _this) {
    RWLock* this = _this;
    pthread_mutex_lock(&this->mtx);

    this->blocked = true;

    while(this->count)
        pthread_cond_wait(&this->cond, &this->mtx);

    this->blocked = false;
    this->count = -1;

    pthread_mutex_unlock(&this->mtx);
}

inline void rwlock_rdlock(rwlock_t _this) {
    RWLock* this = _this;
    pthread_mutex_lock(&this->mtx);

    while(this->blocked || this->count < 0)
        pthread_cond_wait(&this->cond, &this->mtx);
    ++this->count;

    pthread_mutex_unlock(&this->mtx);
}

inline void rwlock_unlock(rwlock_t _this) {
    RWLock* this = _this;
    pthread_mutex_lock(&this->mtx);

    if(this->count < 0) 
        this->count = 0;
    else
        --this->count;
    pthread_cond_broadcast(&this->cond);

    pthread_mutex_unlock(&this->mtx);
}

inline mutex_t new_mutex() {
    mutex_t ret = malloc(sizeof(pthread_mutex_t));
    uboa_assert(ret, "out of memory");

    pthread_mutex_init(ret, NULL);
    return ret;
}

inline bool mutex_trylock(mutex_t this) {
    return !pthread_mutex_trylock(this);
}

inline void mutex_lock(mutex_t this) {
    pthread_mutex_lock(this);
}

inline void mutex_unlock(mutex_t this) {
    pthread_mutex_unlock(this);
}

inline thread_t new_thread(void(*f)(void*), void* arg) {
    thread_t ret = malloc(sizeof(pthread_t));
    uboa_assert(ret, "out of memory");

    pthread_create(ret, NULL, f, arg);
    return ret;
}

inline void* thread_join(thread_t thrd) {
    void* ret = NULL;

    pthread_join(*(pthread_t*)thrd, &ret);
    free(thrd);

    return ret;
}

inline void thread_detach(thread_t thrd) {
    pthread_detach(*(pthread_t*)thrd);
    free(thrd);
}

inline cond_t new_cond() {
    cond_t ret = malloc(sizeof(pthread_cond_t));
    uboa_assert(ret, "out of memory");
    
    pthread_cond_init(ret, NULL);
    return ret;
}

inline void cond_notify(cond_t this) {
    pthread_cond_signal(this);
}

inline void cond_notifyAll(cond_t this) {
    pthread_cond_broadcast(this);
}

inline void cond_wait(cond_t this, mutex_t mtx) {
    pthread_cond_wait(this, mtx);
}