#include "runtime.h"
#include "globals.h"

#include "allocator.h"
#include "director.h"
#include "driver.h"
#include "forwarding.h"
#include "gclog.h"
#include "mark.h"
#include "relocate.h"
#include "vm.h"
#include "workStack.h"

Runtime rt = { 0 };
int nGCWorkers = 0;
size_t XMX = 0;

void uboa_init(int nGCWorkers, size_t xmx, long timerInterval_min, bool enableRuleProactive, const char* logFilePath) {
    rt.remaining = XMX = xmx;

    allocatorInit();
    directorInit(timerInterval_min, enableRuleProactive);
    forwardingInit();
    markerInit();
    vmInit();

    if(logFilePath) {
        logInit(logFilePath);

        fprintf(logFile,
            "%s\n\n"    // copyright
            "GC product name: %s\n"
            "Version: %s, built on %s\n"
            "Memory: %zu bytes\n"
            "GC worker: %d thread(s)\n\n"
            "Warning: This is still a developing version which means it's unstable and may cause some unexpected errors during runtime. "
            "If you have any question, please send feedback to '%s'. Thank you. Have a nice day!\n\n",

            uboa_copyright(),
            uboa_name(),
            uboa_version(), BUILD_DATE,
            XMX,
            nGCWorkers,
            uboa_feedBackEmail()
        );
    }

    // logFile = stderr;

    driverInit(nGCWorkers);
    WorkStack_init();
}

