#include "emulator/sound/gba/sound.h"

#include <assert.h>
#include <stdlib.h>

#include "emulator/sound/gba/direct_sound.h"

#define GBA_SPU_CYCLES_PER_SAMPLE 512u

#define SOUND1CNT_L_OFFSET 0x00u
#define SOUND1CNT_H_OFFSET 0x02u
#define SOUND1CNT_X_OFFSET 0x04u
#define SOUND2CNT_L_OFFSET 0x08u
#define SOUND2CNT_H_OFFSET 0x0Cu
#define SOUND3CNT_L_OFFSET 0x10u
#define SOUND3CNT_H_OFFSET 0x12u
#define SOUND3CNT_X_OFFSET 0x14u
#define SOUND4CNT_L_OFFSET 0x18u
#define SOUND4CNT_H_OFFSET 0x1Cu
#define SOUNDCNT_L_OFFSET 0x20u
#define SOUNDCNT_H_OFFSET 0x22u
#define SOUNDCNT_X_OFFSET 0x24u
#define SOUNDBIAS_OFFSET 0x28u
#define WAVE_RAM0_L_OFFSET 0x30u
#define WAVE_RAM0_H_OFFSET 0x32u
#define WAVE_RAM1_L_OFFSET 0x34u
#define WAVE_RAM1_H_OFFSET 0x36u
#define WAVE_RAM2_L_OFFSET 0x38u
#define WAVE_RAM2_H_OFFSET 0x3Au
#define WAVE_RAM3_L_OFFSET 0x3Cu
#define WAVE_RAM3_H_OFFSET 0x3Eu
#define FIFO_A_LL_OFFSET 0x40u
#define FIFO_A_LH_OFFSET 0x41u
#define FIFO_A_HL_OFFSET 0x42u
#define FIFO_A_HH_OFFSET 0x43u
#define FIFO_B_LL_OFFSET 0x44u
#define FIFO_B_LH_OFFSET 0x45u
#define FIFO_B_HL_OFFSET 0x46u
#define FIFO_B_HH_OFFSET 0x47u
#define FIFO_A_16_LL_OFFSET 0x48u
#define FIFO_A_16_LH_OFFSET 0x49u
#define FIFO_A_16_HL_OFFSET 0x4Au
#define FIFO_A_16_HH_OFFSET 0x4Bu
#define FIFO_B_16_LL_OFFSET 0x4Cu
#define FIFO_B_16_LH_OFFSET 0x4Du
#define FIFO_B_16_HL_OFFSET 0x4Eu
#define FIFO_B_16_HH_OFFSET 0x4Fu

#define GBA_SPU_REGISTERS_SIZE 0x50u

typedef union {
  struct {
    unsigned char sweep_number : 3;
    bool sweep_down : 1;
    unsigned char sweep_step_time : 3;
    unsigned short unused : 9;
  };
  uint16_t value;
} ToneSweepRegister;

typedef union {
  struct {
    unsigned char sound_length : 6;
    unsigned char wave_duty_cycle : 2;
    unsigned char envelope_step_time : 3;
    bool envelope_decreases : 1;
    unsigned char envelope_initial_volume : 4;
  };
  uint16_t value;
} ToneDutyLengthEnvelopeRegister;

typedef union {
  struct {
    unsigned short frequency : 11;
    unsigned char unused : 3;
    bool length_flag : 1;
    bool initial : 1;
  };
  uint16_t value;
} ToneFrequencyControlRegister;

typedef union {
  struct {
    unsigned char unused0 : 5;
    bool two_banks : 1;
    bool bank_number : 1;
    bool enabled : 1;
    unsigned char unused1;
  };
  uint16_t value;
} Sound3ControlLowRegister;

typedef union {
  struct {
    unsigned char length;
    unsigned char unused : 5;
    unsigned char volume : 2;
    bool force_volume : 1;
  };
  uint16_t value;
} Sound3ControlHighRegister;

typedef union {
  struct {
    unsigned short sample_rate : 11;
    unsigned char unused : 3;
    bool length_flag : 1;
    bool reset : 1;
  };
  uint16_t value;
} Sound3ControlExtendedRegister;

typedef union {
  struct {
    unsigned char length : 6;
    unsigned char unused0 : 2;
    unsigned char evelope_step_time : 3;
    bool envelope_decreases : 1;
    unsigned char evelope_initial_volume : 4;
  };
  uint16_t value;
} NoiseLengthEnvelopeRegister;

typedef union {
  struct {
    unsigned char dividing_ratio : 3;
    bool counter_step_width : 1;
    unsigned char shift_clock_frequency : 4;
    unsigned char unused : 6;
    bool length_flag : 1;
    bool reset : 1;
  };
  uint16_t value;
} NoiseFrequencyControlRegister;

typedef union {
  struct {
    unsigned char right_volume : 3;
    bool unused0 : 1;
    unsigned char left_volume : 3;
    bool unused1 : 1;
    bool sound0_right_enabled : 1;
    bool sound1_right_enabled : 1;
    bool sound2_right_enabled : 1;
    bool sound3_right_enabled : 1;
    bool sound0_left_enabled : 1;
    bool sound1_left_enabled : 1;
    bool sound2_left_enabled : 1;
    bool sound3_left_enabled : 1;
  };
  uint16_t value;
} SoundControlLowRegister;

typedef union {
  struct {
    unsigned char sound_volume : 2;
    bool dma_sound_a_volume : 1;
    bool dma_sound_b_volume : 1;
    unsigned char unused : 4;
    bool dma_sound_a_right_enabled : 1;
    bool dma_sound_a_left_enabled : 1;
    bool dma_sound_a_timer_select : 1;
    bool dma_sound_a_reset_fifo : 1;
    bool dma_sound_b_right_enabled : 1;
    bool dma_sound_b_left_enabled : 1;
    bool dma_sound_b_timer_select : 1;
    bool dma_sound_b_reset_fifo : 1;
  };
  uint16_t value;
} SoundControlHighRegister;

typedef union {
  struct {
    bool sound0_on : 1;
    bool sound1_on : 1;
    bool sound2_on : 1;
    bool sound3_on : 1;
    unsigned char unused0 : 3;
    bool fifo_master_enable : 1;
    unsigned char unused1;
  };
  uint16_t value;
} SoundControlExtendedRegister;

typedef union {
  struct {
    unsigned short level : 10;
    unsigned char unused : 4;
    unsigned char amplitude_resolution : 2;
  };
  uint16_t value;
} SoundBiasRegister;

typedef struct {
  unsigned char second : 4;
  unsigned char first : 4;
} WaveData;

typedef union {
  struct {
    WaveData banks[4][4];
  };
  uint16_t half_words[8];
} WaveRam;

typedef union {
  struct {
    ToneSweepRegister sound1cnt_l;
    ToneDutyLengthEnvelopeRegister sound1cnt_h;
    ToneFrequencyControlRegister sound1cnt_x;
    uint16_t unused0;
    ToneDutyLengthEnvelopeRegister sound2cnt_l;
    uint16_t unused1;
    ToneFrequencyControlRegister sound2cnt_h;
    uint16_t unused2;
    Sound3ControlLowRegister sound3cnt_l;
    Sound3ControlHighRegister sound3cnt_h;
    Sound3ControlExtendedRegister sound3cnt_x;
    uint16_t unused3;
    NoiseLengthEnvelopeRegister sound4cnt_l;
    uint16_t unused4;
    NoiseFrequencyControlRegister sound4cnt_h;
    uint16_t unused5;
    SoundControlLowRegister soundcnt_l;
    SoundControlHighRegister soundcnt_h;
    SoundControlExtendedRegister soundcnt_x;
    uint16_t unused6;
    SoundBiasRegister soundbias;
    uint16_t unused7;
    uint16_t unused8;
    uint16_t unused9;
    WaveRam wave_ram;
    uint32_t unused10[4u];
  };
  uint16_t half_words[32];
} GbaSpuRegisters;

struct _GbaSpu {
  GbaSpuRenderAudioSampleRoutine render_routine;
  int16_t current_samples[2];
  int_fast16_t next_sample;
  GbaSpuRegisters registers;
  GbaDmaUnit *dma_unit;
  DirectSoundChannel *direct_sound_a;
  DirectSoundChannel *direct_sound_b;
  bool xq_audio_enabled;
  uint16_t reference_count;
};

static bool GbaSpuRegistersLoad16LE(const void *context, uint32_t address,
                                    uint16_t *value) {
  assert((address & 0x1u) == 0u);

  const GbaSpu *spu = (const GbaSpu *)context;

  switch (address) {
    case SOUND1CNT_L_OFFSET:
      *value = spu->registers.sound1cnt_l.value;
      return true;
    case SOUND1CNT_H_OFFSET:
      *value = spu->registers.sound1cnt_h.value;
      return true;
    case SOUND1CNT_X_OFFSET:
      *value = spu->registers.sound1cnt_x.value;
      return true;
    case SOUND2CNT_L_OFFSET:
      *value = spu->registers.sound2cnt_l.value;
      return true;
    case SOUND2CNT_H_OFFSET:
      *value = spu->registers.sound2cnt_h.value;
      return true;
    case SOUND3CNT_L_OFFSET:
      *value = spu->registers.sound3cnt_l.value;
      return true;
    case SOUND3CNT_H_OFFSET:
      *value = spu->registers.sound3cnt_h.value;
      return true;
    case SOUND3CNT_X_OFFSET:
      *value = spu->registers.sound3cnt_x.value;
      return true;
    case SOUND4CNT_L_OFFSET:
      *value = spu->registers.sound4cnt_l.value;
      return true;
    case SOUND4CNT_H_OFFSET:
      *value = spu->registers.sound4cnt_h.value;
      return true;
    case SOUNDCNT_L_OFFSET:
      *value = spu->registers.soundcnt_l.value;
      return true;
    case SOUNDCNT_H_OFFSET:
      *value = spu->registers.soundcnt_h.value;
      return true;
    case SOUNDCNT_X_OFFSET:
      *value = spu->registers.soundcnt_x.value;
      return true;
    case SOUNDBIAS_OFFSET:
      *value = spu->registers.soundbias.value;
      return true;
    case WAVE_RAM0_L_OFFSET:
      *value = spu->registers.wave_ram.half_words[0];
      return true;
    case WAVE_RAM0_H_OFFSET:
      *value = spu->registers.wave_ram.half_words[1];
      return true;
    case WAVE_RAM1_L_OFFSET:
      *value = spu->registers.wave_ram.half_words[2];
      return true;
    case WAVE_RAM1_H_OFFSET:
      *value = spu->registers.wave_ram.half_words[3];
      return true;
    case WAVE_RAM2_L_OFFSET:
      *value = spu->registers.wave_ram.half_words[4];
      return true;
    case WAVE_RAM2_H_OFFSET:
      *value = spu->registers.wave_ram.half_words[5];
      return true;
    case WAVE_RAM3_L_OFFSET:
      *value = spu->registers.wave_ram.half_words[6];
      return true;
    case WAVE_RAM3_H_OFFSET:
      *value = spu->registers.wave_ram.half_words[7];
      return true;
  }

  return false;
}

static bool GbaSpuRegistersLoad32LE(const void *context, uint32_t address,
                                    uint32_t *value) {
  assert((address & 0x3u) == 0u);

  uint16_t low_bits;
  bool low = GbaSpuRegistersLoad16LE(context, address, &low_bits);
  if (!low) {
    return false;
  }

  uint16_t high_bits;
  bool high = GbaSpuRegistersLoad16LE(context, address + 2u, &high_bits);
  if (high) {
    *value = (((uint32_t)high_bits) << 16u) | (uint32_t)low_bits;
  } else {
    *value = 0u;
  }

  return true;
}

static bool GbaSpuRegistersLoad8(const void *context, uint32_t address,
                                 uint8_t *value) {
  uint32_t read_address = address & 0xFFFFFFFEu;

  uint16_t value16;
  bool success = GbaSpuRegistersLoad16LE(context, read_address, &value16);
  if (success) {
    *value = (address == read_address) ? value16 : value16 >> 8u;
  }

  return success;
}

static bool GbaSpuRegistersStore16LE(void *context, uint32_t address,
                                     uint16_t value) {
  assert((address & 0x1u) == 0u);

  if (address > GBA_SPU_REGISTERS_SIZE) {
    return false;
  }

  GbaSpu *spu = (GbaSpu *)context;

  uint32_t samples;
  switch (address) {
    case FIFO_A_LL_OFFSET:
      samples = DirectSoundChannelPeekBack(spu->direct_sound_a);
      samples &= 0xFFFF0000;
      samples |= value;
      DirectSoundChannelPush8BitSamples(spu->direct_sound_a, samples);
      return true;
    case FIFO_A_HL_OFFSET:
      samples = DirectSoundChannelPeekBack(spu->direct_sound_a);
      samples &= 0x0000FFFF;
      samples |= (uint32_t)value << 16u;
      DirectSoundChannelPush8BitSamples(spu->direct_sound_a, samples);
      return true;
    case FIFO_B_LL_OFFSET:
      samples = DirectSoundChannelPeekBack(spu->direct_sound_b);
      samples &= 0xFFFF0000;
      samples |= value;
      DirectSoundChannelPush8BitSamples(spu->direct_sound_b, samples);
      return true;
    case FIFO_B_HL_OFFSET:
      samples = DirectSoundChannelPeekBack(spu->direct_sound_b);
      samples &= 0x0000FFFF;
      samples |= (uint32_t)value << 16u;
      DirectSoundChannelPush8BitSamples(spu->direct_sound_b, samples);
      return true;
    case FIFO_A_16_LL_OFFSET:
      if (!spu->xq_audio_enabled) {
        return false;
      }
      samples = value;
      DirectSoundChannelPush8BitSamples(spu->direct_sound_a, samples);
      return true;
    case FIFO_A_16_HL_OFFSET:
      if (!spu->xq_audio_enabled) {
        return false;
      }
      samples = (uint32_t)value << 16u;
      DirectSoundChannelPush8BitSamples(spu->direct_sound_a, samples);
      return true;
    case FIFO_B_16_LL_OFFSET:
      if (!spu->xq_audio_enabled) {
        return false;
      }
      samples = value;
      DirectSoundChannelPush8BitSamples(spu->direct_sound_b, samples);
      return true;
    case FIFO_B_16_HL_OFFSET:
      if (!spu->xq_audio_enabled) {
        return false;
      }
      samples = (uint32_t)value << 16u;
      DirectSoundChannelPush8BitSamples(spu->direct_sound_b, samples);
      return true;
  }

  spu->registers.half_words[address >> 1u] = value;

  if (!spu->registers.soundcnt_x.fifo_master_enable) {
    DirectSoundChannelClear(spu->direct_sound_a);
    DirectSoundChannelClear(spu->direct_sound_b);

    for (uint_fast8_t index = 0; index < SOUNDCNT_H_OFFSET;
         index += sizeof(uint16_t)) {
      spu->registers.half_words[index >> 2u] = 0u;
    }
  } else if (spu->registers.soundcnt_h.dma_sound_a_reset_fifo) {
    DirectSoundChannelClear(spu->direct_sound_a);
  } else if (spu->registers.soundcnt_h.dma_sound_b_reset_fifo) {
    DirectSoundChannelClear(spu->direct_sound_b);
  }

  return true;
}

static bool GbaSpuRegistersStore32LE(void *context, uint32_t address,
                                     uint32_t value) {
  assert((address & 0x3u) == 0u);

  GbaSpu *spu = (GbaSpu *)context;

  switch (address) {
    case FIFO_A_LL_OFFSET:
      DirectSoundChannelPush8BitSamples(spu->direct_sound_a, value);
      return true;
    case FIFO_B_LL_OFFSET:
      DirectSoundChannelPush8BitSamples(spu->direct_sound_b, value);
      return true;
    case FIFO_A_16_LL_OFFSET:
      if (!spu->xq_audio_enabled) {
        return false;
      }
      DirectSoundChannelPush16BitSamples(spu->direct_sound_a, value);
      return true;
    case FIFO_B_16_LL_OFFSET:
      if (!spu->xq_audio_enabled) {
        return false;
      }
      DirectSoundChannelPush16BitSamples(spu->direct_sound_b, value);
      return true;
  }

  GbaSpuRegistersStore16LE(context, address, value);
  GbaSpuRegistersStore16LE(context, address + 2u, value >> 16u);

  return true;
}

static bool GbaSpuRegistersStore8(void *context, uint32_t address,
                                  uint8_t value) {
  GbaSpu *spu = (GbaSpu *)context;

  uint32_t samples;
  switch (address) {
    case FIFO_A_LL_OFFSET:
      samples = DirectSoundChannelPeekBack(spu->direct_sound_a);
      samples &= 0xFFFFFF00;
      samples |= value;
      DirectSoundChannelPush8BitSamples(spu->direct_sound_a, samples);
      return true;
    case FIFO_A_LH_OFFSET:
      samples = DirectSoundChannelPeekBack(spu->direct_sound_a);
      samples &= 0xFFFF00FF;
      samples |= (uint32_t)value << 8u;
      DirectSoundChannelPush8BitSamples(spu->direct_sound_a, samples);
      return true;
    case FIFO_A_HL_OFFSET:
      samples = DirectSoundChannelPeekBack(spu->direct_sound_a);
      samples &= 0xFF00FFFF;
      samples |= (uint32_t)value << 16u;
      DirectSoundChannelPush8BitSamples(spu->direct_sound_a, samples);
      return true;
    case FIFO_A_HH_OFFSET:
      samples = DirectSoundChannelPeekBack(spu->direct_sound_a);
      samples &= 0x00FFFFFF;
      samples |= (uint32_t)value << 24u;
      DirectSoundChannelPush8BitSamples(spu->direct_sound_a, samples);
      return true;
    case FIFO_B_LL_OFFSET:
      samples = DirectSoundChannelPeekBack(spu->direct_sound_b);
      samples &= 0xFFFFFF00;
      samples |= value;
      DirectSoundChannelPush8BitSamples(spu->direct_sound_b, samples);
      return true;
    case FIFO_B_LH_OFFSET:
      samples = DirectSoundChannelPeekBack(spu->direct_sound_b);
      samples &= 0xFFFF00FF;
      samples |= (uint32_t)value << 8u;
      DirectSoundChannelPush8BitSamples(spu->direct_sound_b, samples);
      return true;
    case FIFO_B_HL_OFFSET:
      samples = DirectSoundChannelPeekBack(spu->direct_sound_b);
      samples &= 0xFF00FFFF;
      samples |= (uint32_t)value << 16u;
      DirectSoundChannelPush8BitSamples(spu->direct_sound_b, samples);
      return true;
    case FIFO_B_HH_OFFSET:
      samples = DirectSoundChannelPeekBack(spu->direct_sound_b);
      samples &= 0x00FFFFFF;
      samples |= (uint32_t)value << 24u;
      DirectSoundChannelPush8BitSamples(spu->direct_sound_b, samples);
      return true;
    case FIFO_A_16_LL_OFFSET:
      if (!spu->xq_audio_enabled) {
        return false;
      }
      samples = value;
      DirectSoundChannelPush16BitSamples(spu->direct_sound_a, samples);
      return true;
    case FIFO_A_16_LH_OFFSET:
      if (!spu->xq_audio_enabled) {
        return false;
      }
      samples = (uint32_t)value << 8u;
      DirectSoundChannelPush16BitSamples(spu->direct_sound_a, samples);
      return true;
    case FIFO_A_16_HL_OFFSET:
      if (!spu->xq_audio_enabled) {
        return false;
      }
      samples = (uint32_t)value << 16u;
      DirectSoundChannelPush16BitSamples(spu->direct_sound_a, samples);
      return true;
    case FIFO_A_16_HH_OFFSET:
      if (!spu->xq_audio_enabled) {
        return false;
      }
      samples = (uint32_t)value << 24u;
      DirectSoundChannelPush16BitSamples(spu->direct_sound_a, samples);
      return true;
    case FIFO_B_16_LL_OFFSET:
      if (!spu->xq_audio_enabled) {
        return false;
      }
      samples = value;
      DirectSoundChannelPush16BitSamples(spu->direct_sound_b, samples);
      return true;
    case FIFO_B_16_LH_OFFSET:
      if (!spu->xq_audio_enabled) {
        return false;
      }
      samples = (uint32_t)value << 8u;
      DirectSoundChannelPush16BitSamples(spu->direct_sound_b, samples);
      return true;
    case FIFO_B_16_HL_OFFSET:
      if (!spu->xq_audio_enabled) {
        return false;
      }
      samples = (uint32_t)value << 16u;
      DirectSoundChannelPush16BitSamples(spu->direct_sound_b, samples);
      return true;
    case FIFO_B_16_HH_OFFSET:
      if (!spu->xq_audio_enabled) {
        return false;
      }
      samples = (uint32_t)value << 24u;
      DirectSoundChannelPush16BitSamples(spu->direct_sound_b, samples);
      return true;
  }

  uint32_t read_address = address & 0xFFFFFFFEu;
  uint16_t value16 = spu->registers.half_words[read_address >> 1u];
  if (address == read_address) {
    value16 &= 0xFF00;
    value16 |= value;
  } else {
    value16 &= 0x00FF;
    value16 |= (uint16_t)value << 8u;
  }

  GbaSpuRegistersStore16LE(context, read_address, value16);

  return true;
}

void GbaSpuMemoryFree(void *context) {
  GbaSpu *spu = (GbaSpu *)context;
  GbaSpuRelease(spu);
}

bool GbaSpuAllocate(GbaDmaUnit *dma_unit, GbaSpu **spu, Memory **registers) {
  *spu = (GbaSpu *)calloc(1, sizeof(GbaSpu));
  if (*spu == NULL) {
    return false;
  }

  *registers = MemoryAllocate(
      *spu, GbaSpuRegistersLoad32LE, GbaSpuRegistersLoad16LE,
      GbaSpuRegistersLoad8, GbaSpuRegistersStore32LE, GbaSpuRegistersStore16LE,
      GbaSpuRegistersStore8, GbaSpuMemoryFree);
  if (*registers == NULL) {
    free(*spu);
    return false;
  }

  (*spu)->direct_sound_a = DirectSoundChannelAllocate();
  if ((*spu)->direct_sound_a == NULL) {
    MemoryFree(*registers);
    free(*spu);
    return false;
  }

  (*spu)->direct_sound_b = DirectSoundChannelAllocate();
  if ((*spu)->direct_sound_b == NULL) {
    DirectSoundChannelFree((*spu)->direct_sound_a);
    MemoryFree(*registers);
    free(*spu);
    return false;
  }

  GbaDmaUnitRetain(dma_unit);

  (*spu)->dma_unit = dma_unit;
  (*spu)->registers.soundbias.level = 0x200u;
  (*spu)->reference_count = 2u;

  return true;
}

void GbaSpuStep(GbaSpu *spu) {
  if (spu->next_sample < 1024) {
    spu->next_sample += 1;
    return;
  }

  spu->next_sample = 0;

  if (spu->render_routine != NULL) {
    spu->render_routine(spu->current_samples[0], spu->current_samples[1]);
  }
}

void GbaSpuTimerTick(GbaSpu *spu, bool timer_index) {
  if (!spu->registers.soundcnt_x.fifo_master_enable) {
    return;
  }

  int32_t left = 0u;
  int32_t right = 0u;
  if (spu->registers.soundcnt_h.dma_sound_a_timer_select == timer_index) {
    int16_t sample;
    bool refill_needed = DirectSoundChannelPop(spu->direct_sound_a, &sample);
    if (refill_needed) {
      GbaDmaUnitSignalFifoRefresh(spu->dma_unit, 0x40000A0u);
      GbaDmaUnitSignalFifoRefresh(spu->dma_unit, 0x40000A8u);
    }

    if (!spu->registers.soundcnt_h.dma_sound_a_volume) {
      sample /= 2u;
    }

    if (spu->registers.soundcnt_h.dma_sound_a_left_enabled) {
      left += sample;
    }

    if (spu->registers.soundcnt_h.dma_sound_a_right_enabled) {
      right += sample;
    }
  }

  if (spu->registers.soundcnt_h.dma_sound_b_timer_select == timer_index) {
    int16_t sample;
    bool refill_needed = DirectSoundChannelPop(spu->direct_sound_b, &sample);
    if (refill_needed) {
      GbaDmaUnitSignalFifoRefresh(spu->dma_unit, 0x40000A4u);
      GbaDmaUnitSignalFifoRefresh(spu->dma_unit, 0x40000ACu);
    }

    if (!spu->registers.soundcnt_h.dma_sound_b_volume) {
      sample /= 2u;
    }

    if (spu->registers.soundcnt_h.dma_sound_b_left_enabled) {
      left += sample;
    }

    if (spu->registers.soundcnt_h.dma_sound_b_right_enabled) {
      right += sample;
    }
  }

  if (left < INT16_MIN) {
    left = INT16_MIN;
  } else if (left > INT16_MAX) {
    left = INT16_MAX;
  }

  if (right < INT16_MIN) {
    right = INT16_MIN;
  } else if (right > INT16_MAX) {
    right = INT16_MAX;
  }

  spu->current_samples[0] = left;
  spu->current_samples[1] = right;
}

void GbaSpuSetRenderAudioSampleRoutine(
    GbaSpu *spu, GbaSpuRenderAudioSampleRoutine render_routine) {
  spu->render_routine = render_routine;
}

void GbaSpuRetain(GbaSpu *spu) {
  assert(spu->reference_count != UINT16_MAX);
  spu->reference_count += 1u;
}

void GbaSpuRelease(GbaSpu *spu) {
  assert(spu->reference_count != 0u);
  spu->reference_count -= 1u;
  if (spu->reference_count == 0u) {
    DirectSoundChannelFree(spu->direct_sound_a);
    DirectSoundChannelFree(spu->direct_sound_b);
    GbaDmaUnitRelease(spu->dma_unit);
    free(spu);
  }
}

static_assert(sizeof(GbaSpuRegisters) == GBA_SPU_REGISTERS_SIZE,
              "sizeof(GbaSpuRegisters) != GBA_SPU_REGISTERS_SIZE");