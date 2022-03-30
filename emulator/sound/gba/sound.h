#ifndef _WEBGBA_EMULATOR_SOUND_GBA_SOUND_
#define _WEBGBA_EMULATOR_SOUND_GBA_SOUND_

#include "emulator/dma/gba/dma.h"
#include "emulator/memory/memory.h"

typedef struct _GbaSpu GbaSpu;

bool GbaSpuAllocate(GbaDmaUnit* dma_unit, GbaSpu **spu, Memory **registers);

// Callback type for one sample's worth of audio data
typedef void (*GbaSpuRenderAudioSample)(int16_t left, int16_t right);

void GbaSpuStep(GbaSpu *spu, GbaSpuRenderAudioSample audio_sample_callback);

void GbaSpuTimerTick(GbaSpu *spu, bool timer_index);

void GbaSpuRetain(GbaSpu *spu);

void GbaSpuRelease(GbaSpu *spu);

#endif  // _WEBGBA_EMULATOR_SOUND_GBA_SOUND_