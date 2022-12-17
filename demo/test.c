#include "../src/uboa.h"
#include <alloca.h>
#include <pthread.h>
#include <assert.h>

#include <unistd.h>

uboa_appThrdHandle statics = NULL;
uboa_reference oop = NULL;

const size_t length = 15;
const off_t numOff = sizeof(off_t) * length;
off_t* map = NULL;

void foo(long num) {
    uboa_appThrdHandle hdl = uboa_createAppThrdHandle(alloca(1024));
    uboa_reference ref[length];
    for(int i = 0; i < length; ++i) 
        ref[i] = uboa_pushReference(hdl);

    uboa_safeRegion(
        for(int i = 0; i < length; ++i) {
            if(num < 8)
                uboa_new(hdl, ref[i], 4096, oop);
            else if(num < 10)
                uboa_new(hdl, ref[i], 1 << 19, oop);
            else
                uboa_new(hdl, ref[i], 1l << 24, oop);

            uboa_storeInt32(ref[i], numOff, 114514);
        }
    )
    
    for(int a = 0; a < 1008600; ++a) {
        uboa_safeRegion(
            for(int i = 0; i < length; ++i) {
                assert(114514 == uboa_loadInt32(ref[i], numOff));

                for(int j = 0; j < length; ++j) {
                    uboa_reference tmp = uboa_pushReference(hdl);

                    uboa_new(hdl, tmp, 128, uboa_null());
                    uboa_storeInt32(tmp, numOff, 114514);

                    uboa_popReferences(hdl, 1);
                }
            }
        )
    }

    uboa_safeRegion(
        for(int i = 0; i < length; ++i) {
            uboa_loadReference(ref[i], ref[i], map[i]);
            assert(114514 == uboa_loadInt32(ref[i], numOff));
        }
    )

    uboa_destroyAppThrdHandle(hdl);
}

int main() {
    uboa_init(12, (1l << 36) + (1l << 35), 0, false, "./log.txt");

    map = alloca(sizeof(off_t) * length);
    for(int i = 0; i < length; ++i) 
        map[i] = i * 8;
    
    statics = uboa_createAppThrdHandle(alloca(1024));
    oop = uboa_pushReference(statics);

    uboa_safeRegion(
        uboa_new_Oop(statics, oop, length, map);
    )

    for(int i = 0; i < 12; ++i) {
        pthread_t t = { 0 };
        pthread_create(&t, NULL, foo, (long)i);
        pthread_detach(t);
    }

    sleep(120);
    printf("Done\n");

    return 0;
}