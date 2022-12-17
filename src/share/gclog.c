#include "gclog.h"
#include "../util/queue.h"

#include "../../build/build.h"

typedef struct Log Log;
struct Log {
    ListNode n;

    long time;
    char str[0];
};
static atomic_int s = 0;
static Queue q[2] = { 0 };

FILE* logFile = NULL;

static void logFileFinalize() {
    GCLog_output();
    fprintf(logFile, "\nGC service stopped.\n\n");
    fclose(logFile);
}

void logInit(const char* filePath) {
    logFile = fopen(filePath, "w");
    uboa_assert(logFile, "cannot open file '%s'", filePath);
    atexit(logFileFinalize);
}

inline bool isLoggerAvailable() {
    return logFile;
}

inline void GCLog_push(const char* str, size_t size) {
    Log* l = malloc(sizeof(Log) + size);
    uboa_assert(l, "out of memory");

    l->time = getTime();
    strcpy(l->str, str);
    Queue_push(q + s, &l->n, l);
}

inline void GCLog_output() {
    int t = atomic_exchange(&s, !s);
    Log* l = NULL;
    while((l = Queue_pop(q + t))) {
        fprintf(logFile, "[%ld]: %s\n", l->time, l->str);
        free(l);
    }
}

const char* uboa_copyright() {
    returnString(COPYRIGHT);
}

const char* uboa_name() {
    returnString(GC_PRODUCT_NAME);
}

const char* uboa_feedBackEmail() {
    returnString(FEEDBACK_EMAIL);
}

const char* uboa_version() {
    returnString(GC_VERSION);
}