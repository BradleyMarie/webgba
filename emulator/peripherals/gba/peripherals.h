#ifndef _WEBGBA_EMULATOR_TIMERS_GBA_TIMERS_
#define _WEBGBA_EMULATOR_TIMERS_GBA_TIMERS_

#include "emulator/memory/memory.h"

typedef struct _GbaPeripherals GbaPeripherals;

bool GbaPeripheralsAllocate(GbaPeripherals **peripherals, Memory **registers);

void GbaPeripheralsStep(GbaPeripherals *peripherals);

void GbaPeripheralsFree(GbaPeripherals *peripherals);

#endif  // _WEBGBA_EMULATOR_TIMERS_GBA_TIMERS_