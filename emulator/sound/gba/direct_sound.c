#include "emulator/sound/gba/direct_sound.h"

#define LOW_WATER_MARK 16

void DirectSoundChannelPush(DirectSoundChannel* channel, uint8_t sample) {
  if (channel->num_samples == DIRECT_SOUND_CHANNEL_BUFFER_SIZE) {
    channel->samples[channel->front_index % DIRECT_SOUND_CHANNEL_BUFFER_SIZE] =
        (int8_t)sample;
    channel->front_index += 1u;
  } else {
    uint_fast8_t insertion_index =
        (channel->front_index + channel->num_samples) %
        DIRECT_SOUND_CHANNEL_BUFFER_SIZE;
    channel->samples[insertion_index] = (int8_t)sample;
    channel->num_samples += 1u;
  }
}

void DirectSoundChannelPushTwo(DirectSoundChannel* channel,
                               uint16_t packed_samples) {
  DirectSoundChannelPush(channel, (uint8_t)packed_samples);
  DirectSoundChannelPush(channel, (uint8_t)(packed_samples >> 8u));
}

void DirectSoundChannelPushFour(DirectSoundChannel* channel,
                                uint32_t packed_samples) {
  DirectSoundChannelPushTwo(channel, (uint16_t)packed_samples);
  DirectSoundChannelPushTwo(channel, (uint16_t)(packed_samples >> 16u));
}

bool DirectSoundChannelPop(DirectSoundChannel* channel, int8_t* value) {
  if (channel->num_samples == 0) {
    *value = 0;
    return true;
  }

  *value =
      channel->samples[channel->front_index % DIRECT_SOUND_CHANNEL_BUFFER_SIZE];

  channel->front_index += 1u;
  channel->num_samples -= 1u;

  return channel->num_samples <= LOW_WATER_MARK;
}

void DirectSoundChannelClear(DirectSoundChannel* channel) {
  channel->front_index = 0u;
  channel->num_samples = 0u;
}