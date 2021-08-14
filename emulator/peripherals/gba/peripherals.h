#ifndef _WEBGBA_EMULATOR_PERIPHERALS_GBA_PERIPHERALS_
#define _WEBGBA_EMULATOR_PERIPHERALS_GBA_PERIPHERALS_

#include "emulator/memory/memory.h"
#include "emulator/peripherals/gamepad.h"
#include "emulator/platform/gba/platform.h"

typedef struct _GbaPeripherals GbaPeripherals;

bool GbaPeripheralsAllocate(GbaPlatform *platform, GbaPeripherals **peripherals,
                            GamePad **gamepad, Memory **registers);

void GbaPeripheralsFree(GbaPeripherals *peripherals);

#endif  // _WEBGBA_EMULATOR_PERIPHERALS_GBA_PERIPHERALS_