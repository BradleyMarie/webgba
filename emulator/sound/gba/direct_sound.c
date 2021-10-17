#include "emulator/sound/gba/direct_sound.h"

#include <stdlib.h>

#define FIFO_BUFFER_SIZE_PACKED_SAMPLES 7u
#define FIFO_BUFFER_DMA_THRESHOLD 3u

struct _DirectSoundChannel {
  uint32_t packed_samples_high[FIFO_BUFFER_SIZE_PACKED_SAMPLES];
  uint32_t packed_samples_low[FIFO_BUFFER_SIZE_PACKED_SAMPLES];
  uint_fast8_t sample_index;
  uint_fast8_t front;
  uint_fast8_t size;
  bool write_upper_half;
};

DirectSoundChannel* DirectSoundChannelAllocate() {
  DirectSoundChannel* result = calloc(1u, sizeof(DirectSoundChannel));
  return result;
}

void DirectSoundChannelPush8BitSamples(DirectSoundChannel* channel,
                                       uint32_t packed_samples) {
  uint_fast8_t next =
      (channel->front + channel->size) % FIFO_BUFFER_SIZE_PACKED_SAMPLES;

  channel->packed_samples_high[next] = packed_samples;
  channel->packed_samples_low[next] = 0u;

  if (channel->size < FIFO_BUFFER_SIZE_PACKED_SAMPLES) {
    channel->size += 1u;
  }

  channel->write_upper_half = false;
}

void DirectSoundChannelPush16BitSamples(DirectSoundChannel* channel,
                                        uint32_t packed_samples) {
  uint_fast8_t next =
      (channel->front + channel->size) % FIFO_BUFFER_SIZE_PACKED_SAMPLES;

  uint32_t high_value_to_write = channel->packed_samples_high[next];
  uint32_t low_value_to_write = channel->packed_samples_low[next];

  uint16_t first_sample = packed_samples;
  uint16_t second_sample = packed_samples >> 16u;

  uint16_t high_samples = (first_sample >> 8u) | (second_sample & 0xFF00u);
  uint16_t low_samples = (first_sample & 0xFFu) | (second_sample << 8u);

  if (channel->write_upper_half) {
    high_value_to_write &= 0xFFFF0000u;
    high_value_to_write |= high_samples;

    low_value_to_write &= 0xFFFF0000u;
    low_value_to_write |= low_samples;

    channel->write_upper_half = false;
  } else {
    high_value_to_write &= 0x0000FFFFu;
    high_value_to_write |= (uint32_t)high_samples << 16u;

    low_value_to_write &= 0x0000FFFFu;
    low_value_to_write |= (uint32_t)low_samples << 16u;

    channel->write_upper_half = true;
  }

  channel->packed_samples_high[next] = high_value_to_write;
  channel->packed_samples_low[next] = low_value_to_write;

  if (channel->write_upper_half &&
      channel->size < FIFO_BUFFER_SIZE_PACKED_SAMPLES) {
    channel->size += 1u;
  }
}

bool DirectSoundChannelPop(DirectSoundChannel* channel, int16_t* value) {
  if (channel->size == 0u) {
    *value = 0;
    return true;
  }

  uint_fast8_t bit_shift = 8u * channel->sample_index;
  uint8_t low_bits = channel->packed_samples_low[channel->front] >> bit_shift;
  uint8_t high_bits = channel->packed_samples_high[channel->front] >> bit_shift;

  *value = (uint16_t)low_bits | (((uint16_t)high_bits) << 8u);

  channel->sample_index += 1u;
  if (channel->sample_index == sizeof(uint32_t)) {
    channel->sample_index = 0u;
    channel->front += 1u;
    channel->size -= 1u;

    if (channel->front == FIFO_BUFFER_SIZE_PACKED_SAMPLES) {
      channel->front = 0u;
    }
  }

  return channel->size <= FIFO_BUFFER_DMA_THRESHOLD;
}

uint32_t DirectSoundChannelPeekBack(DirectSoundChannel* channel) {
  uint_fast8_t next =
      (channel->front + channel->size) % FIFO_BUFFER_SIZE_PACKED_SAMPLES;

  return channel->packed_samples_high[next];
}

void DirectSoundChannelClear(DirectSoundChannel* channel) {
  channel->size = 0u;
  channel->sample_index = 0u;
}

void DirectSoundChannelFree(DirectSoundChannel* channel) { free(channel); }