#ifndef _WEBGBA_EMULATOR_PLATFORM_GBA_PLATFORM_
#define _WEBGBA_EMULATOR_PLATFORM_GBA_PLATFORM_

#include <stdint.h>

#include "emulator/memory/memory.h"
#include "emulator/platform/interrupt_line.h"

typedef struct _GbaPlatform GbaPlatform;

bool GbaPlatformAllocate(GbaPlatform **platform, Memory **registers,
                         InterruptLine **rst_line, InterruptLine **fiq_line,
                         InterruptLine **irq_line);

// Interrupts
void GbaPlatformRaiseVBlankInterrupt(GbaPlatform *platform);
void GbaPlatformRaiseHBlankInterrupt(GbaPlatform *platform);
void GbaPlatformRaiseVBlankCountInterrupt(GbaPlatform *platform);
void GbaPlatformRaiseSerialInterrupt(GbaPlatform *platform);
void GbaPlatformRaiseKeypadInterrupt(GbaPlatform *platform);
void GbaPlatformRaiseCartridgeInterrupt(GbaPlatform *platform);

typedef enum {
  GBA_TIMER_0 = 0u,
  GBA_TIMER_1 = 1u,
  GBA_TIMER_2 = 2u,
  GBA_TIMER_3 = 3u
} GbaTimerId;

void GbaPlatformRaiseTimerInterrupt(GbaPlatform *platform, GbaTimerId timer);

typedef enum {
  GBA_DMA_0 = 0u,
  GBA_DMA_1 = 1u,
  GBA_DMA_2 = 2u,
  GBA_DMA_3 = 3u
} GbaDmaId;

void GbaPlatformRaiseDmaInterrupt(GbaPlatform *platform, GbaDmaId dma);

// Waitstate Cycles
uint_fast8_t GbaPlatformSramWaitStateCycles(const GbaPlatform *platform);
uint_fast8_t GbaPlatformRom0FirstAccessWaitCycles(const GbaPlatform *platform);
uint_fast8_t GbaPlatformRom0SecondAccessWaitCycles(const GbaPlatform *platform);
uint_fast8_t GbaPlatformRom1FirstAccessWaitCycles(const GbaPlatform *platform);
uint_fast8_t GbaPlatformRom1SecondAccessWaitCycles(const GbaPlatform *platform);
uint_fast8_t GbaPlatformRom2FirstAccessWaitCycles(const GbaPlatform *platform);
uint_fast8_t GbaPlatformRom2SecondAccessWaitCycles(const GbaPlatform *platform);

// Rom Instruction Prefetching
bool GbaPlatformRomPrefetch(const GbaPlatform *platform);

// Power State
typedef enum {
  GBA_POWER_STATE_RUN = 0,
  GBA_POWER_STATE_HALT = 1,
  GBA_POWER_STATE_STOP = 2
} GbaPowerState;

GbaPowerState GbaPlatformPowerState(const GbaPlatform *platform);

// Reference Counting
void GbaPlatformRetain(GbaPlatform *platform);
void GbaPlatformRelease(GbaPlatform *platform);

#endif  // _WEBGBA_EMULATOR_PLATFORM_GBA_PLATFORM_