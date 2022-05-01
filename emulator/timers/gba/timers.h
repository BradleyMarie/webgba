#ifndef _WEBGBA_EMULATOR_TIMERS_GBA_TIMERS_
#define _WEBGBA_EMULATOR_TIMERS_GBA_TIMERS_

#include "emulator/memory/memory.h"
#include "emulator/platform/gba/platform.h"
#include "emulator/sound/gba/sound.h"

typedef struct _GbaTimers GbaTimers;

bool GbaTimersAllocate(GbaPlatform *platform, GbaSpu *spu, GbaTimers **timers,
                       Memory **registers);

uint32_t GbaTimersCyclesUntilNextWake(const GbaTimers *timers);

void GbaTimersStep(GbaTimers *timers, uint32_t num_cycles);

void GbaTimersFree(GbaTimers *timers);

#endif  // _WEBGBA_EMULATOR_TIMERS_GBA_TIMERS_