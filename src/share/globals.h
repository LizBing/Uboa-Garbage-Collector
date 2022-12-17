#ifndef UBOA_GC_GLOBALS_
#define UBOA_GC_GLOBALS_

#include "stdafx.h"

extern int nGCWorkers;
extern size_t XMX;

#define regionSizeShift 21

#define pageSizeShiftSmall regionSizeShift
#define pageSizeShiftMedium (regionSizeShift + 4)

#define pageSizeSmall (1l << pageSizeShiftSmall)
#define pageSizeMedium (1l << pageSizeShiftMedium)

#define limitShiftSmall 18

#define limitSmall (1l << limitShiftSmall)
#define limitMedium (1l << 24)

#define alignmentShiftSmall 7
#define alignmentShiftMedium 12
#define alignmentShiftLarge regionSizeShift

#define alignmentSmall (1l << alignmentShiftSmall)
#define alignmentMedium (1l << alignmentShiftMedium)
#define alignmentLarge (1l << alignmentShiftLarge)

#define VIEW0 0b01l
#define VIEW1 0b10l
#define VIEW2 0b11l
#define MASK_ADDRESS ((1l << 44) - 1)

#endif