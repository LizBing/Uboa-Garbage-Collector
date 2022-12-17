#include "../time.h"

#define __USE_POSIX199309
#define _POSIX_C_SOURCE 199309L
#include <time.h>

#define __USE_BSD
#include <unistd.h>

inline long getTime() {
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
    

    return ts.tv_sec * 1000 * 1000 + ts.tv_nsec / 1000;
}

inline void sleep_us(long t) {
    usleep(t);
}