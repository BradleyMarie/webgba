#include "emulator/timers/gba/timers.h"

#include <assert.h>
#include <stdlib.h>

#define TM0CNT_L_OFFSET 0x00u
#define TM0CNT_H_OFFSET 0x02u
#define TM1CNT_L_OFFSET 0x04u
#define TM1CNT_H_OFFSET 0x06u
#define TM2CNT_L_OFFSET 0x08u
#define TM2CNT_H_OFFSET 0x0Au
#define TM3CNT_L_OFFSET 0x0Cu
#define TM3CNT_H_OFFSET 0x0Eu

#define GBA_TIMER_REGISTERS_SIZE 0x10u
#define GBA_SPU_TIMER_MAX_INDEX 1u
#define GBA_NUM_TIMERS 4u

typedef union {
  struct {
    unsigned char prescalar : 2;
    bool cascade : 1;
    unsigned char unused0 : 3;
    bool irq_enable : 1;
    bool started : 1;
    unsigned char unused1;
  };
  uint16_t value;
} TimerHighRegister;

typedef struct {
  uint16_t tmcnt_l;
  TimerHighRegister tmcnt_h;
} TimerRegisters;

typedef union {
  TimerRegisters registers[GBA_NUM_TIMERS];
  uint16_t half_words[8u];
  uint32_t words[4u];
  uint8_t bytes[16u];
} GbaTimerRegisters;

struct _GbaTimers {
  uint32_t next_overflow_cycle;
  uint32_t current_cycle;
  uint32_t overflow_cycle[GBA_NUM_TIMERS];
  uint32_t write_mask[GBA_NUM_TIMERS];
  bool cascades[GBA_NUM_TIMERS];
  uint_fast8_t start_timer;
  uint_fast8_t end_timer;
  GbaTimerRegisters read;
  GbaTimerRegisters write;
  GbaPlatform *platform;
  GbaSpu *spu;
  uint16_t reference_count;
};

static inline uint32_t TimerTicksRemaining(GbaTimers *timers, uint_fast8_t i) {
  assert(i < GBA_NUM_TIMERS);
  return UINT16_MAX + 1u - timers->read.registers[i].tmcnt_l;
}

static inline uint32_t CyclesPerTick(const GbaTimers *timers, uint_fast8_t i) {
  assert(i < GBA_NUM_TIMERS);
  static const uint16_t tick_rates[4] = {1u, 64u, 256u, 1024u};
  return tick_rates[timers->read.registers[i].tmcnt_h.prescalar];
}

static void UpdateTimersBeforeWrite(GbaTimers *timers) {
  for (uint_fast8_t i = timers->start_timer; i < timers->end_timer; i++) {
    timers->overflow_cycle[i] -= timers->current_cycle;
    timers->overflow_cycle[i] |= timers->write_mask[i];
  }

  timers->current_cycle = 0u;
}

static void UpdateTimersAfterWrite(GbaTimers *timers) {
  timers->start_timer = GBA_NUM_TIMERS;
  timers->end_timer = GBA_NUM_TIMERS;

  timers->next_overflow_cycle = UINT32_MAX;
  for (uint_fast8_t i = 0u; i < GBA_NUM_TIMERS; i++) {
    bool timer_just_started = timers->write.registers[i].tmcnt_h.started &&
                              !timers->read.registers[i].tmcnt_h.started;
    timers->read.registers[i].tmcnt_h = timers->write.registers[i].tmcnt_h;

    if (!timers->write.registers[i].tmcnt_h.started) {
      timers->overflow_cycle[i] = UINT32_MAX;
      timers->write_mask[i] = UINT32_MAX;
      if (i != 0u) {
        timers->cascades[i - 1u] = false;
      }
      continue;
    }

    if (timers->write.registers[i].tmcnt_h.cascade) {
      timers->overflow_cycle[i] |= UINT32_MAX;
      timers->write_mask[i] = UINT32_MAX;
      if (i != 0u) {
        timers->cascades[i - 1u] = true;
      }
    } else {
      if (timers->overflow_cycle[i] == UINT32_MAX) {
        timers->overflow_cycle[i] =
            CyclesPerTick(timers, i) * TimerTicksRemaining(timers, i);
      }
      timers->write_mask[i] = 0u;
      if (i != 0u) {
        timers->cascades[i - 1u] = false;
      }
    }

    if (timer_just_started) {
      timers->read.registers[i].tmcnt_l = timers->write.registers[i].tmcnt_l;
      timers->overflow_cycle[i] =
          CyclesPerTick(timers, i) * TimerTicksRemaining(timers, i);
      timers->overflow_cycle[i] |= timers->write_mask[i];
    }

    if (timers->overflow_cycle[i] < timers->next_overflow_cycle) {
      timers->next_overflow_cycle = timers->overflow_cycle[i];
    }

    if (timers->start_timer == GBA_NUM_TIMERS) {
      timers->start_timer = i;
    }

    timers->end_timer = i + 1u;
  }
}

static uint16_t ReadTimerTicks(const GbaTimers *timers, uint_fast8_t i) {
  assert(i < GBA_NUM_TIMERS);

  if (!timers->read.registers[i].tmcnt_h.started ||
      (i != 0u && timers->read.registers[i].tmcnt_h.cascade)) {
    return timers->read.registers[i].tmcnt_l;
  }

  uint32_t cycles_remaining = timers->overflow_cycle[i] - timers->current_cycle;
  assert(cycles_remaining != 0u);

  uint32_t cycles_per_tick = CyclesPerTick(timers, i);
  uint32_t ticks_remaining = (cycles_remaining - 1u) / cycles_per_tick;

  return UINT16_MAX - ticks_remaining;
}

static bool GbaTimersRegistersLoad16LE(const void *context, uint32_t address,
                                       uint16_t *value) {
  assert((address & 0x1u) == 0u);

  const GbaTimers *timers = (const GbaTimers *)context;

  switch (address) {
    case TM0CNT_L_OFFSET:
      *value = ReadTimerTicks(timers, 0u);
      break;
    case TM0CNT_H_OFFSET:
      *value = timers->read.half_words[address >> 1u];
      break;
    case TM1CNT_L_OFFSET:
      *value = ReadTimerTicks(timers, 1u);
      break;
    case TM1CNT_H_OFFSET:
      *value = timers->read.half_words[address >> 1u];
      break;
    case TM2CNT_L_OFFSET:
      *value = ReadTimerTicks(timers, 2u);
      break;
    case TM2CNT_H_OFFSET:
      *value = timers->read.half_words[address >> 1u];
      break;
    case TM3CNT_L_OFFSET:
      *value = ReadTimerTicks(timers, 3u);
      break;
    case TM3CNT_H_OFFSET:
      *value = timers->read.half_words[address >> 1u];
      break;
    default:
      return false;
  }

  return true;
}

static bool GbaTimersRegistersLoad32LE(const void *context, uint32_t address,
                                       uint32_t *value) {
  assert((address & 0x3u) == 0u);

  uint16_t high_bits;
  if (!GbaTimersRegistersLoad16LE(context, address + 2u, &high_bits)) {
    return false;
  }

  uint16_t low_bits;
  bool result = GbaTimersRegistersLoad16LE(context, address, &low_bits);
  assert(result);

  *value = ((uint32_t)high_bits << 16u) | low_bits;

  return result;
}

static bool GbaTimersRegistersLoad8(const void *context, uint32_t address,
                                    uint8_t *value) {
  uint32_t read_address = address & 0xFFFFFFFEu;

  uint16_t value16;
  bool read_success =
      GbaTimersRegistersLoad16LE(context, read_address, &value16);
  if (!read_success) {
    return false;
  }

  if (address == read_address) {
    *value = value16;
  } else {
    *value = value16 >> 8u;
  }

  return true;
}

static bool GbaTimersRegistersStore16LE(void *context, uint32_t address,
                                        uint16_t value) {
  assert((address & 0x1u) == 0u);

  if (address + 2u > GBA_TIMER_REGISTERS_SIZE) {
    return false;
  }

  GbaTimers *timers = (GbaTimers *)context;

  UpdateTimersBeforeWrite(timers);
  timers->write.half_words[address >> 1u] = value;
  UpdateTimersAfterWrite(timers);

  return true;
}

static bool GbaTimersRegistersStore32LE(void *context, uint32_t address,
                                        uint32_t value) {
  assert((address & 0x3u) == 0u);

  if (address + 4u > GBA_TIMER_REGISTERS_SIZE) {
    return false;
  }

  GbaTimers *timers = (GbaTimers *)context;

  UpdateTimersBeforeWrite(timers);
  timers->write.words[address >> 2u] = value;
  UpdateTimersAfterWrite(timers);

  return true;
}

static bool GbaTimersRegistersStore8(void *context, uint32_t address,
                                     uint8_t value) {
  GbaTimers *timers = (GbaTimers *)context;

  if (address + 1u > GBA_TIMER_REGISTERS_SIZE) {
    return false;
  }

  UpdateTimersBeforeWrite(timers);
  timers->write.bytes[address] = value;
  UpdateTimersAfterWrite(timers);

  return true;
}

void GbaTimersMemoryFree(void *context) {
  GbaTimers *timers = (GbaTimers *)context;
  GbaTimersFree(timers);
}

bool GbaTimersAllocate(GbaPlatform *platform, GbaSpu *spu, GbaTimers **timers,
                       Memory **registers) {
  *timers = (GbaTimers *)calloc(1, sizeof(GbaTimers));
  if (*timers == NULL) {
    return false;
  }

  *registers =
      MemoryAllocate(*timers, GbaTimersRegistersLoad32LE,
                     GbaTimersRegistersLoad16LE, GbaTimersRegistersLoad8,
                     GbaTimersRegistersStore32LE, GbaTimersRegistersStore16LE,
                     GbaTimersRegistersStore8, GbaTimersMemoryFree);
  if (*registers == NULL) {
    free(*timers);
    return false;
  }

  (*timers)->next_overflow_cycle = UINT32_MAX;
  (*timers)->platform = platform;
  (*timers)->spu = spu;
  (*timers)->reference_count = 2u;

  GbaPlatformRetain(platform);
  GbaSpuRetain(spu);

  return true;
}

uint32_t GbaTimersCyclesUntilNextWake(const GbaTimers *timers) {
  return timers->next_overflow_cycle - timers->current_cycle;
}

void GbaTimersStep(GbaTimers *timers, uint32_t num_cycles) {
  timers->current_cycle += num_cycles;
  assert(timers->current_cycle <= timers->next_overflow_cycle);

  if (timers->current_cycle != timers->next_overflow_cycle) {
    return;
  }

  uint32_t overflow_mask = UINT32_MAX;
  timers->next_overflow_cycle = UINT32_MAX;
  for (uint_fast8_t i = timers->start_timer; i < timers->end_timer; i++) {
    timers->overflow_cycle[i] -= timers->current_cycle;
    timers->overflow_cycle[i] |= timers->write_mask[i];

    if ((timers->overflow_cycle[i] & overflow_mask) == 0u) {
      timers->read.registers[i].tmcnt_l = timers->write.registers[i].tmcnt_l;

      timers->overflow_cycle[i] =
          CyclesPerTick(timers, i) * TimerTicksRemaining(timers, i);
      timers->overflow_cycle[i] |= timers->write_mask[i];

      if (timers->overflow_cycle[i] < timers->next_overflow_cycle) {
        timers->next_overflow_cycle = timers->overflow_cycle[i];
      }

      if (timers->read.registers[i].tmcnt_h.irq_enable) {
        GbaPlatformRaiseTimerInterrupt(timers->platform, i);
      }

      if (i <= GBA_SPU_TIMER_MAX_INDEX) {
        GbaSpuTimerTick(timers->spu, i);
      }

      overflow_mask = UINT32_MAX;
      if (timers->cascades[i]) {
        timers->read.registers[i + 1u].tmcnt_l += 1u;
        if (timers->read.registers[i + 1u].tmcnt_l == 0u) {
          overflow_mask = 0u;
        }
      }
    }

    if (timers->overflow_cycle[i] < timers->next_overflow_cycle) {
      timers->next_overflow_cycle = timers->overflow_cycle[i];
    }
  }

  timers->current_cycle = 0u;
}

void GbaTimersFree(GbaTimers *timers) {
  assert(timers->reference_count != 0u);
  timers->reference_count -= 1u;
  if (timers->reference_count == 0u) {
    GbaPlatformRelease(timers->platform);
    GbaSpuRelease(timers->spu);
    free(timers);
  }
}

static_assert(sizeof(GbaTimerRegisters) == GBA_TIMER_REGISTERS_SIZE,
              "sizeof(GbaTimerRegisters) != GBA_TIMER_REGISTERS_SIZE");