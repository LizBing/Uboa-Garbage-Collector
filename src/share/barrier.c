/**
 * @file barrier.c
 * @author Lizbing (lizbing@relight.com)
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#include "barrier.h"
#include "runtime.h"
#include "address.h"
#include "mark.h"
#include "forwarding.h"

inline Object* loadValueBarrier(uboa_reference r) {
    intptr_t n = *r;
    if(!n || rt.isCleaningUp)
        return n;

    PtrView pv = getView(n);
    if(rt.isMarking) {
        if(PV_PREVIOUS == pv) 
            Following_push(n);
        else if(PV_NEXT == pv) {
            n = *r = Forwarding_get(n, false);
            Following_push(n);
        }
    } 
    else if(PV_PREVIOUS == pv) 
        n = *r = Forwarding_get(n, false);

    return n;
}