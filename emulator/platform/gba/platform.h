#ifndef _WEBGBA_EMULATOR_PLATFORM_GBA_PLATFORM_
#define _WEBGBA_EMULATOR_PLATFORM_GBA_PLATFORM_

#include <stdint.h>

#include "emulator/memory/memory.h"
#include "emulator/platform/interrupt_line.h"

typedef struct _GbaPlatform GbaPlatform;

bool GbaPlatformAllocate(GbaPlatform **platform, Memory **registers,
                         InterruptLine **rst_line, InterruptLine **fiq_line,
                         InterruptLine **irq_line);

// Interrupt Controls
void GbaPlatformRaiseVBlankInterrupt(GbaPlatform *platform);
void GbaPlatformRaiseHBlankInterrupt(GbaPlatform *platform);
void GbaPlatformRaiseVBlankCountInterrupt(GbaPlatform *platform);
void GbaPlatformRaiseTimer0Interrupt(GbaPlatform *platform);
void GbaPlatformRaiseTimer1Interrupt(GbaPlatform *platform);
void GbaPlatformRaiseTimer2Interrupt(GbaPlatform *platform);
void GbaPlatformRaiseTimer3Interrupt(GbaPlatform *platform);
void GbaPlatformRaiseSerialInterrupt(GbaPlatform *platform);
void GbaPlatformRaiseDma0Interrupt(GbaPlatform *platform);
void GbaPlatformRaiseDma1Interrupt(GbaPlatform *platform);
void GbaPlatformRaiseDma2Interrupt(GbaPlatform *platform);
void GbaPlatformRaiseDma3Interrupt(GbaPlatform *platform);
void GbaPlatformRaiseKeypadInterrupt(GbaPlatform *platform);
void GbaPlatformRaiseCartridgeInterrupt(GbaPlatform *platform);

void GbaPlatformRetain(GbaPlatform *platform);
void GbaPlatformRelease(GbaPlatform *platform);

#endif  // _WEBGBA_EMULATOR_PLATFORM_GBA_PLATFORM_