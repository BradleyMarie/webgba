#include "emulator/gba.h"

#include <assert.h>
#include <stdatomic.h>
#include <stdlib.h>

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"
#include "emulator/dma/gba/dma.h"
#include "emulator/game/gba/game.h"
#include "emulator/memory/gba/memory.h"
#include "emulator/peripherals/gba/peripherals.h"
#include "emulator/platform/gba/platform.h"
#include "emulator/ppu/gba/ppu.h"
#include "emulator/sound/gba/sound.h"
#include "emulator/timers/gba/timers.h"

struct _GbaEmulator {
  bool cpu_active;
  bool dma_active;
  Arm7Tdmi *cpu;
  Memory *memory;
  GbaDmaUnit *dma;
  GbaPpu *ppu;
  GbaSpu *spu;
  GbaTimers *timers;
  GbaPeripherals *peripherals;
  GbaPlatform *platform;
  PowerState power_state;
  bool dma_state;
  uint_fast8_t reference_count;
};

static void GbaEmulatorUpdateActiveBits(GbaEmulator *emulator) {
  switch (emulator->power_state) {
    case POWER_STATE_RUN:
      emulator->cpu_active = !emulator->dma_state;
      emulator->dma_active = emulator->dma_state;
      break;
    case POWER_STATE_HALT:
      emulator->cpu_active = false;
      emulator->dma_active = emulator->dma_state;
      break;
    case POWER_STATE_STOP:
      emulator->cpu_active = false;
      emulator->dma_active = false;
      break;
  }
}

static void GbaEmulatorPowerSet(void *context, PowerState power_state) {
  GbaEmulator *emulator = (GbaEmulator *)context;
  emulator->power_state = power_state;

  if (power_state != POWER_STATE_RUN) {
    Arm7TdmiHalt(emulator->cpu);
  }

  GbaEmulatorUpdateActiveBits(emulator);
}

static void GbaEmulatorPowerFree(void *context) {
  GbaEmulator *emulator = (GbaEmulator *)context;
  GbaEmulatorFree(emulator);
}

static void GbaEmulatorDmaStatusSet(void *context, bool active) {
  GbaEmulator *emulator = (GbaEmulator *)context;

  if (active) {
    Arm7TdmiHalt(emulator->cpu);
  }

  emulator->dma_state = active;
  GbaEmulatorUpdateActiveBits(emulator);
}

static void GbaEmulatorDmaStatusFree(void *context) {
  GbaEmulator *emulator = (GbaEmulator *)context;
  GbaEmulatorFree(emulator);
}

bool GbaEmulatorAllocate(const unsigned char *rom_data, uint32_t rom_size,
                         GbaEmulator **emulator, GamePad **gamepad) {
  *emulator = malloc(sizeof(GbaEmulator));
  if (*emulator == NULL) {
    return false;
  }

  (*emulator)->reference_count = 3;

  InterruptLine *rst;
  InterruptLine *fiq;
  InterruptLine *irq;
  bool success = Arm7TdmiAllocate(&(*emulator)->cpu, &rst, &fiq, &irq);
  if ((*emulator)->cpu == NULL) {
    free(*emulator);
    return false;
  }

  InterruptLineFree(rst);
  InterruptLineFree(fiq);

  Power *power =
      PowerAllocate(*emulator, GbaEmulatorPowerSet, GbaEmulatorPowerFree);
  if (power == NULL) {
    Arm7TdmiFree((*emulator)->cpu);
    InterruptLineFree(irq);
    free(*emulator);
    return false;
  }

  Memory *platform_registers;
  success = GbaPlatformAllocate(power, irq, &(*emulator)->platform,
                                &platform_registers);
  if (!success) {
    PowerFree(power);
    Arm7TdmiFree((*emulator)->cpu);
    InterruptLineFree(irq);
    free(*emulator);
    return false;
  }

  DmaStatus *dma_status = DmaStatusAllocate(*emulator, GbaEmulatorDmaStatusSet,
                                            GbaEmulatorDmaStatusFree);
  if (dma_status == NULL) {
    Arm7TdmiFree((*emulator)->cpu);
    GbaPlatformRelease((*emulator)->platform);
    free(*emulator);
    return false;
  }

  Memory *dma_unit_registers;
  success = GbaDmaUnitAllocate(dma_status, (*emulator)->platform,
                               &(*emulator)->dma, &dma_unit_registers);
  if (!success) {
    DmaStatusFree(dma_status);
    Arm7TdmiFree((*emulator)->cpu);
    GbaPlatformRelease((*emulator)->platform);
    free(*emulator);
    return false;
  }

  Memory *sound_registers;
  success =
      GbaSpuAllocate((*emulator)->dma, &(*emulator)->spu, &sound_registers);
  if (!success) {
    GbaDmaUnitRelease((*emulator)->dma);
    Arm7TdmiFree((*emulator)->cpu);
    GbaPlatformRelease((*emulator)->platform);
    free(*emulator);
    return false;
  }

  Memory *timer_registers;
  success = GbaTimersAllocate((*emulator)->platform, (*emulator)->spu,
                              &(*emulator)->timers, &timer_registers);
  if (!success) {
    GbaSpuRelease((*emulator)->spu);
    GbaDmaUnitRelease((*emulator)->dma);
    Arm7TdmiFree((*emulator)->cpu);
    GbaPlatformRelease((*emulator)->platform);
    free(*emulator);
    return false;
  }

  SaveStorageType storage_type;
  Memory *game_rom;
  success = GbaGameLoad(rom_data, rom_size, &storage_type, &game_rom);
  if (!success) {
    GbaTimersFree((*emulator)->timers);
    GbaSpuRelease((*emulator)->spu);
    GbaDmaUnitRelease((*emulator)->dma);
    Arm7TdmiFree((*emulator)->cpu);
    GbaPlatformRelease((*emulator)->platform);
    free(*emulator);
    return false;
  }

  // HACK HACK HACK
  // TODO: Implement SRAM
  Memory *sram;
  success = GbaGameLoad(rom_data, rom_size, &storage_type, &sram);
  if (!success) {
    MemoryFree(game_rom);
    GbaSpuRelease((*emulator)->spu);
    GbaTimersFree((*emulator)->timers);
    GbaDmaUnitRelease((*emulator)->dma);
    Arm7TdmiFree((*emulator)->cpu);
    GbaPlatformRelease((*emulator)->platform);
    free(*emulator);
    return false;
  }

  Memory *peripherals_registers;
  success =
      GbaPeripheralsAllocate((*emulator)->platform, &(*emulator)->peripherals,
                             gamepad, &peripherals_registers);
  if (!success) {
    MemoryFree(sram);
    MemoryFree(game_rom);
    GbaSpuRelease((*emulator)->spu);
    GbaTimersFree((*emulator)->timers);
    GbaDmaUnitRelease((*emulator)->dma);
    Arm7TdmiFree((*emulator)->cpu);
    GbaPlatformRelease((*emulator)->platform);
    free(*emulator);
    return false;
  }

  Memory *palette;
  Memory *vram;
  Memory *oam;
  Memory *ppu_registers;
  success =
      GbaPpuAllocate((*emulator)->dma, (*emulator)->platform, &(*emulator)->ppu,
                     &palette, &vram, &oam, &ppu_registers);
  if (!success) {
    GbaPeripheralsFree((*emulator)->peripherals);
    GamePadFree(*gamepad);
    MemoryFree(sram);
    MemoryFree(game_rom);
    GbaSpuRelease((*emulator)->spu);
    GbaTimersFree((*emulator)->timers);
    GbaDmaUnitRelease((*emulator)->dma);
    Arm7TdmiFree((*emulator)->cpu);
    GbaPlatformRelease((*emulator)->platform);
    free(*emulator);
    return false;
  }

  (*emulator)->memory =
      GbaMemoryAllocate(ppu_registers, sound_registers, dma_unit_registers,
                        timer_registers, peripherals_registers,
                        platform_registers, palette, vram, oam, game_rom, sram);
  if ((*emulator)->memory == NULL) {
    MemoryFree(palette);
    MemoryFree(vram);
    MemoryFree(oam);
    MemoryFree(ppu_registers);
    GbaPpuFree((*emulator)->ppu);
    GbaPeripheralsFree((*emulator)->peripherals);
    GamePadFree(*gamepad);
    MemoryFree(sram);
    MemoryFree(game_rom);
    GbaSpuRelease((*emulator)->spu);
    GbaTimersFree((*emulator)->timers);
    GbaDmaUnitRelease((*emulator)->dma);
    Arm7TdmiFree((*emulator)->cpu);
    GbaPlatformRelease((*emulator)->platform);
    free(*emulator);
    return false;
  }

  return true;
}

void GbaEmulatorStep(GbaEmulator *emulator, GLuint fbo, GLsizei width,
                     GLsizei height,
                     GbaEmulatorRenderAudioSample audio_sample_callback) {
  assert(width != 0u && height != 0u);
  assert(audio_sample_callback != NULL);

  for (;;) {
    uint32_t cycles_elapsed = GbaTimersCyclesUntilNextWake(emulator->timers);

    uint32_t next_ppu_wake = GbaPpuCyclesUntilNextWake(emulator->ppu);
    if (next_ppu_wake < cycles_elapsed) {
      cycles_elapsed = next_ppu_wake;
    }

    uint32_t next_spu_wake = GbaSpuCyclesUntilNextWake(emulator->spu);
    if (next_spu_wake < cycles_elapsed) {
      cycles_elapsed = next_spu_wake;
    }

    if (emulator->cpu_active) {
      assert(!emulator->dma_active);
      cycles_elapsed =
          Arm7TdmiStep(emulator->cpu, emulator->memory, cycles_elapsed);
    } else if (emulator->dma_active) {
      cycles_elapsed =
          GbaDmaUnitStep(emulator->dma, emulator->memory, cycles_elapsed);
    } else if (emulator->power_state == POWER_STATE_STOP) {
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);
      glClearColor(0.0, 0.0, 0.0, 1.0);
      glClear(GL_COLOR_BUFFER_BIT);
      break;
    }

    GbaTimersStep(emulator->timers, cycles_elapsed);
    GbaSpuStep(emulator->spu, cycles_elapsed, audio_sample_callback);
    if (GbaPpuStep(emulator->ppu, cycles_elapsed, fbo, width, height)) {
      break;
    }
  }
}

void GbaEmulatorReloadContext(GbaEmulator *emulator) {
  GbaPpuReloadContext(emulator->ppu);
}

void GbaEmulatorFree(GbaEmulator *emulator) {
  assert(emulator->reference_count != 0);
  emulator->reference_count -= 1u;
  if (emulator->reference_count == 0u) {
    GbaPlatformRelease(emulator->platform);
    Arm7TdmiFree(emulator->cpu);
    MemoryFree(emulator->memory);
    GbaDmaUnitRelease(emulator->dma);
    GbaPpuFree(emulator->ppu);
    GbaSpuRelease(emulator->spu);
    GbaTimersFree(emulator->timers);
    GbaPeripheralsFree(emulator->peripherals);
    free(emulator);
  }
}