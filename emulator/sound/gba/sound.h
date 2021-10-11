#ifndef _WEBGBA_EMULATOR_SOUND_GBA_SOUND_
#define _WEBGBA_EMULATOR_SOUND_GBA_SOUND_

#include "emulator/memory/memory.h"

typedef struct _GbaSpu GbaSpu;

bool GbaSpuAllocate(GbaSpu **spu, Memory **registers);

void GbaSpuStep(GbaSpu *spu);

typedef void (*GbaSpuRenderAudioSampleRoutine)(int16_t left, int16_t right);
void GbaSpuSetRenderAudioSampleRoutine(
    GbaSpu *spu, GbaSpuRenderAudioSampleRoutine render_routine);

void GbaSpuFree(GbaSpu *spu);

#endif  // _WEBGBA_EMULATOR_SOUND_GBA_SOUND_