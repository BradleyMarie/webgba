#include "emulator/timers/gba/timers.h"

#include <assert.h>
#include <stdlib.h>

#define GBA_TIMER_REGISTERS_SIZE 0x10u
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
  uint16_t half_words[8];
  uint32_t words[4];
  uint8_t bytes[16];
} GbaTimerRegisters;

struct _GbaTimers {
  uint_fast8_t num_active_timers;
  bool timer_cascade[GBA_NUM_TIMERS];
  uint16_t timer_ticks[GBA_NUM_TIMERS];
  uint16_t timer_tick_rate[GBA_NUM_TIMERS];
  uint_fast8_t timer_index[GBA_NUM_TIMERS];
  uint16_t all_timer_ticks[GBA_NUM_TIMERS];
  GbaTimerRegisters read;
  GbaTimerRegisters write;
  GbaPlatform *platform;
  uint16_t reference_count;
};

static void UpdateTimersAfterWrite(GbaTimers *timers) {
  static const uint16_t tick_rates[4] = {1u, 64u, 256u, 1024u};

  // Copy out tick counts
  for (uint_fast8_t i = 0; i < timers->num_active_timers; i++) {
    timers->all_timer_ticks[timers->timer_index[i]] = timers->timer_ticks[i];
  }

  // Setup active timers
  timers->num_active_timers = 0u;
  for (uint_fast8_t i = 0u; i < GBA_NUM_TIMERS; i++) {
    bool timer_started = timers->write.registers[i].tmcnt_h.started &&
                         !timers->read.registers[i].tmcnt_h.started;
    timers->read.registers[i].tmcnt_h = timers->write.registers[i].tmcnt_h;

    if (!timers->read.registers[i].tmcnt_h.started) {
      continue;
    }

    if (timers->read.registers[i].tmcnt_h.cascade && i != 0 &&
        !timers->read.registers[i - 1u].tmcnt_h.started) {
      continue;
    }

    timers->timer_cascade[timers->num_active_timers] =
        timers->read.registers[i].tmcnt_h.cascade;
    timers->timer_tick_rate[timers->num_active_timers] =
        tick_rates[timers->read.registers[i].tmcnt_h.prescalar];
    timers->timer_index[timers->num_active_timers] = i;

    if (timer_started) {
      timers->read.registers[i].tmcnt_l = timers->write.registers[i].tmcnt_l;
      timers->timer_ticks[timers->num_active_timers] = 0;
    } else {
      timers->timer_ticks[timers->num_active_timers] =
          timers->all_timer_ticks[i];
    }

    timers->num_active_timers += 1u;
  }
}

static bool GbaTimersRegistersLoad32LE(const void *context, uint32_t address,
                                       uint32_t *value) {
  assert((address & 0x3u) == 0u);

  if (address >= GBA_TIMER_REGISTERS_SIZE) {
    return false;
  }

  const GbaTimers *timers = (const GbaTimers *)context;

  *value = timers->read.words[address >> 2u];

  return true;
}

static bool GbaTimersRegistersLoad16LE(const void *context, uint32_t address,
                                       uint16_t *value) {
  assert((address & 0x1u) == 0u);

  if (address >= GBA_TIMER_REGISTERS_SIZE) {
    return false;
  }

  const GbaTimers *timers = (const GbaTimers *)context;

  *value = timers->read.half_words[address >> 1u];

  return true;
}

static bool GbaTimersRegistersLoad8(const void *context, uint32_t address,
                                    uint8_t *value) {
  if (address >= GBA_TIMER_REGISTERS_SIZE) {
    return false;
  }

  const GbaTimers *timers = (const GbaTimers *)context;

  *value = timers->read.bytes[address];

  return true;
}

static bool GbaTimersRegistersStore32LE(void *context, uint32_t address,
                                        uint32_t value) {
  assert((address & 0x3u) == 0u);

  if (address >= GBA_TIMER_REGISTERS_SIZE) {
    return true;
  }

  GbaTimers *timers = (GbaTimers *)context;

  timers->write.words[address >> 2u] = value;

  // Update read registers
  UpdateTimersAfterWrite(timers);

  return true;
}

static bool GbaTimersRegistersStore16LE(void *context, uint32_t address,
                                        uint16_t value) {
  assert((address & 0x1u) == 0u);

  if (address >= GBA_TIMER_REGISTERS_SIZE) {
    return true;
  }

  GbaTimers *timers = (GbaTimers *)context;

  timers->write.half_words[address >> 1u] = value;

  // Update read registers
  UpdateTimersAfterWrite(timers);

  return true;
}

static bool GbaTimersRegistersStore8(void *context, uint32_t address,
                                     uint8_t value) {
  if (address >= GBA_TIMER_REGISTERS_SIZE) {
    return true;
  }

  GbaTimers *timers = (GbaTimers *)context;

  timers->write.bytes[address] = value;

  // Update read registers
  UpdateTimersAfterWrite(timers);

  return true;
}

void GbaTimersMemoryFree(void *context) {
  GbaTimers *timers = (GbaTimers *)context;
  GbaTimersFree(timers);
}

bool GbaTimersAllocate(GbaPlatform *platform, GbaTimers **timers,
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

  (*timers)->platform = platform;
  (*timers)->reference_count = 2u;

  GbaPlatformRetain(platform);

  return true;
}

void GbaTimersStep(GbaTimers *timers) {
  bool overflowed = false;
  for (uint_fast8_t i = 0u; i < timers->num_active_timers; i++) {
    if (timers->timer_cascade[i]) {
      if (!overflowed) {
        continue;
      }
    } else {
      timers->timer_ticks[i] += 1;
      if (timers->timer_ticks[i] != timers->timer_tick_rate[i]) {
        overflowed = false;
        continue;
      }

      timers->timer_ticks[i] = 0u;
    }

    uint_fast8_t timer_index = timers->timer_index[i];
    timers->read.registers[timer_index].tmcnt_l += 1;
    if (timers->read.registers[timer_index].tmcnt_l == 0u) {
      timers->read.registers[timer_index].tmcnt_l =
          timers->write.registers[timer_index].tmcnt_l;
      if (timers->read.registers[timer_index].tmcnt_h.irq_enable) {
        GbaPlatformRaiseTimerInterrupt(timers->platform, timer_index);
      }

      overflowed = true;
    } else {
      overflowed = false;
    }
  }
}

void GbaTimersFree(GbaTimers *timers) {
  assert(timers->reference_count != 0u);
  timers->reference_count -= 1u;
  if (timers->reference_count == 0u) {
    free(timers);
  }
}

static_assert(sizeof(GbaTimerRegisters) == GBA_TIMER_REGISTERS_SIZE,
              "sizeof(GbaTimerRegisters) != GBA_TIMER_REGISTERS_SIZE");