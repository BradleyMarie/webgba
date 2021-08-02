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
#define GBA_NUM_TIMERS 4u

typedef union {
  struct {
    unsigned char prescalar : 2;
    bool count_up_timing : 1;
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
  GbaTimerRegisters read;
  GbaTimerRegisters write;
  uint16_t timer_ticks[GBA_NUM_TIMERS];
  GbaPlatform *platform;
  uint16_t reference_count;
};

static void UpdateTimersAfterWrite(GbaTimers *timers) {
  for (uint_fast8_t i = 0u; i < GBA_NUM_TIMERS; i++) {
    bool timer_started = timers->write.registers[i].tmcnt_h.started &&
                         !timers->read.registers[i].tmcnt_h.started;

    timers->read.registers[i].tmcnt_h = timers->write.registers[i].tmcnt_h;

    if (timer_started) {
      timers->read.registers[i].tmcnt_l = timers->write.registers[i].tmcnt_l;
      timers->timer_ticks[i] = 0;
    }
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

void GbaTimersStep(GbaTimers *timers) {}

void GbaTimersFree(GbaTimers *timers) {
  assert(timers->reference_count != 0u);
  timers->reference_count -= 1u;
  if (timers->reference_count == 0u) {
    free(timers);
  }
}

static_assert(sizeof(GbaTimerRegisters) == GBA_TIMER_REGISTERS_SIZE,
              "sizeof(GbaTimerRegisters) != GBA_TIMER_REGISTERS_SIZE");