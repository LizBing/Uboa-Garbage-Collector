#include "thrdPool.h"
#include "semaphore.h"
#include "assert.h"

#include "../os/thread.h"

struct ThrdPool {
    size_t nworkers;

    int taskNo;
    void(*func)();

    cond_t workerCond, driverCond;
    mutex_t mtx;
    atomic_int count;
};

void workerManager(ThrdPool* driver) {
    int taskNo = 0;
    ++driver->count;

    for(;;) {
        mutex_lock(driver->mtx);

        while(driver->taskNo == taskNo)
            cond_wait(driver->workerCond, driver->mtx);

        mutex_unlock(driver->mtx);

        driver->func();
        ++taskNo;

        if(++driver->count == driver->nworkers) 
            cond_notify(driver->driverCond);
    }
}

ThrdPool* new_ThrdPool(size_t nworkers) {
    ThrdPool* this = calloc(1, sizeof(ThrdPool));
    uboa_assert(this, "out of memory");

    this->nworkers = nworkers;
    this->workerCond = new_cond();
    this->driverCond = new_cond();
    this->mtx = new_mutex();

    for(int i = 0; i < nworkers; ++i) 
        thread_detach(new_thread(workerManager, this));
    while(this->count != nworkers);

    return this;
}

void ThrdPool_run(ThrdPool* this, void(*func)()) {
    ThrdPool_wait(this);

    this->func = func;
    this->count = 0;

    ++this->taskNo;
    cond_notifyAll(this->workerCond);
}

void ThrdPool_wait(ThrdPool* this) {
    mutex_lock(this->mtx);

    while(this->count != this->nworkers)
        cond_wait(this->driverCond, this->mtx);

    mutex_unlock(this->mtx);
}

inline bool ThrdPool_isRunning(ThrdPool* this) {
    return this->count != this->nworkers;
}