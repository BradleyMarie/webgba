#ifndef _WEBGBA_EMULATOR_TIMERS_GBA_TIMERS_
#define _WEBGBA_EMULATOR_TIMERS_GBA_TIMERS_

#include "emulator/memory/memory.h"

typedef struct _GbaTimers GbaTimers;

bool GbaTimersAllocate(GbaTimers **timers, Memory **registers);

void GbaTimersStep(GbaTimers *timers);

void GbaTimersFree(GbaTimers *timers);

#endif  // _WEBGBA_EMULATOR_TIMERS_GBA_TIMERS_