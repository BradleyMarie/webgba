#ifndef _WEBGBA_EMULATOR_SOUND_GBA_SOUND_
#define _WEBGBA_EMULATOR_SOUND_GBA_SOUND_

#include "emulator/memory/memory.h"

typedef struct _GbaTimers GbaTimers;

bool GbaTimersAllocate(GbaTimers **timers, Memory **registers);

void GbaTimersStep(GbaTimers *timers);

void GbaTimersFree(GbaTimers *timers);

#endif  // _WEBGBA_EMULATOR_SOUND_GBA_SOUND_