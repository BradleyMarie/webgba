#ifndef _WEBGBA_EMULATOR_SOUND_GBA_DIRECT_SOUND_
#define _WEBGBA_EMULATOR_SOUND_GBA_DIRECT_SOUND_

#include <stdbool.h>
#include <stdint.h>

// TODO: Reduce this buffer size to 32 samples. This is a workaround for timing
// issues in webgba that can cause the FIFO buffer to be refreshed too early,
// causing audio issues when samples are dropped if the available buffer space
// is too small to contain them. Setting this to 64 allows the buffer to contain
// two extra frames of audio data without dropping.
#define DIRECT_SOUND_CHANNEL_BUFFER_SIZE 64

typedef struct _DirectSoundChannel {
  int8_t samples[DIRECT_SOUND_CHANNEL_BUFFER_SIZE];
  uint_fast8_t front_index;
  uint_fast8_t num_samples;
} DirectSoundChannel;

void DirectSoundChannelPush(DirectSoundChannel* channel, uint8_t sample);

void DirectSoundChannelPushTwo(DirectSoundChannel* channel,
                               uint16_t packed_samples);

void DirectSoundChannelPushFour(DirectSoundChannel* channel,
                                uint32_t packed_samples);

bool DirectSoundChannelPop(DirectSoundChannel* channel, int8_t* value);

void DirectSoundChannelClear(DirectSoundChannel* channel);

#endif  // _WEBGBA_EMULATOR_SOUND_GBA_DIRECT_SOUND_