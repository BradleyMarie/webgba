#ifndef _WEBGBA_EMULATOR_SOUND_GBA_DIRECT_SOUND_
#define _WEBGBA_EMULATOR_SOUND_GBA_DIRECT_SOUND_

#include <stdbool.h>
#include <stdint.h>

typedef struct _DirectSoundChannel DirectSoundChannel;

DirectSoundChannel* DirectSoundChannelAllocate();

void DirectSoundChannelPush8BitSamples(DirectSoundChannel* channel,
                                       uint32_t packed_samples);

void DirectSoundChannelPush16BitSamples(DirectSoundChannel* channel,
                                        uint32_t packed_samples);

uint32_t DirectSoundChannelPeekBack(DirectSoundChannel* channel);

bool DirectSoundChannelPop(DirectSoundChannel* channel, int16_t* value);

void DirectSoundChannelClear(DirectSoundChannel* channel);

void DirectSoundChannelFree(DirectSoundChannel* channel);

#endif  // _WEBGBA_EMULATOR_SOUND_GBA_DIRECT_SOUND_