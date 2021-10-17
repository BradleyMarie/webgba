#ifndef _WEBGBA_EMULATOR_SOUND_GBA_SOUND_
#define _WEBGBA_EMULATOR_SOUND_GBA_SOUND_

#include "emulator/dma/gba/dma.h"
#include "emulator/memory/memory.h"

typedef struct _GbaSpu GbaSpu;

bool GbaSpuAllocate(GbaDmaUnit* dma_unit, GbaSpu **spu, Memory **registers);

void GbaSpuStep(GbaSpu *spu);

void GbaSpuTimerTick(GbaSpu *spu, bool timer_index);

typedef void (*GbaSpuRenderAudioSampleRoutine)(int16_t left, int16_t right);
void GbaSpuSetRenderAudioSampleRoutine(
    GbaSpu *spu, GbaSpuRenderAudioSampleRoutine render_routine);

void GbaSpuRetain(GbaSpu *spu);

void GbaSpuRelease(GbaSpu *spu);

#endif  // _WEBGBA_EMULATOR_SOUND_GBA_SOUND_