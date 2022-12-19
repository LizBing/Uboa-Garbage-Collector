/**
 * @file address.c
 * @author Lizbing (lizbing@relight.com)
 * 
 * @copyright Copyright (c) 2022. All rights reserved.
 * 
 */

#include "runtime.h"
#include "address.h"
#include "globals.h"

inline PtrView getView(intptr_t n) {
    long m = n >> 44;
    if(rt.viewCurrent == m)
        return PV_CURRENT;
    else if(rt.viewPrevious == m)
        return PV_PREVIOUS;
    else if(rt.viewNext == m)
        return PV_NEXT;
    
    return PV_NULL;
}

inline intptr_t address(intptr_t n) {
    return n & MASK_ADDRESS;
}

inline int regionNo(intptr_t n) {
    return address(n) >> regionSizeShift;
}

inline int inPageOffsSmall(intptr_t n) {
    return (n & (pageSizeSmall - 1)) >> alignmentShiftSmall;
}

inline int inPageOffsMedium(intptr_t n) {
    return (n & (pageSizeMedium - 1)) >> limitShiftSmall;
}