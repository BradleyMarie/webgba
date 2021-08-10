#ifndef _WEBGBA_EMULATOR_TIMERS_GBA_TIMERS_
#define _WEBGBA_EMULATOR_TIMERS_GBA_TIMERS_

#include "emulator/memory/memory.h"
#include "emulator/peripherals/gamepad.h"
#include "emulator/platform/gba/platform.h"

typedef struct _GbaPeripherals GbaPeripherals;

bool GbaPeripheralsAllocate(GbaPlatform *platform, GbaPeripherals **peripherals,
                            GamePad **gamepad, Memory **registers);

void GbaPeripheralsStep(GbaPeripherals *peripherals);

void GbaPeripheralsFree(GbaPeripherals *peripherals);

#endif  // _WEBGBA_EMULATOR_TIMERS_GBA_TIMERS_