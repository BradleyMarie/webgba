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
  PowerState power_state;
  Arm7Tdmi *cpu;
  Memory *memory;
  GbaDmaUnit *dma;
  GbaPpu *ppu;
  GbaSpu *spu;
  GbaTimers *timers;
  GbaPeripherals *peripherals;
  GbaPlatform *platform;
  uint_fast8_t reference_count;
};

static void GbaEmulatorPowerSet(void *context, PowerState power_state) {
  GbaEmulator *emulator = (GbaEmulator *)context;
  emulator->power_state = power_state;
}

static void GbaEmulatorPowerFree(void *context) {
  GbaEmulator *emulator = (GbaEmulator *)context;
  GbaEmulatorFree(emulator);
}

bool GbaEmulatorAllocate(const unsigned char *rom_data, uint32_t rom_size,
                         GbaEmulator **emulator, GamePad **gamepad) {
  *emulator = malloc(sizeof(GbaEmulator));
  if (*emulator == NULL) {
    return false;
  }

  (*emulator)->reference_count = 2;

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

  Memory *dma_unit_registers;
  success = GbaDmaUnitAllocate((*emulator)->platform, &(*emulator)->dma,
                               &dma_unit_registers);
  if (!success) {
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

void GbaEmulatorStep(GbaEmulator *emulator, GLuint fbo, uint8_t scale_factor,
                     GbaEmulatorRenderAudioSample audio_sample_callback) {
  assert(scale_factor != 0);
  assert(audio_sample_callback != NULL);

  bool frame_rendered = false;
  while (!frame_rendered) {
    if (emulator->power_state == POWER_STATE_RUN) {
      if (GbaDmaUnitIsActive(emulator->dma)) {
        GbaDmaUnitStep(emulator->dma, emulator->memory);
      } else {
        Arm7TdmiStep(emulator->cpu, emulator->memory);
      }

      GbaTimersStep(emulator->timers);
      frame_rendered = GbaPpuStep(emulator->ppu, fbo, scale_factor);
      GbaSpuStep(emulator->spu, audio_sample_callback);
    } else if (emulator->power_state == POWER_STATE_HALT) {
      if (GbaDmaUnitIsActive(emulator->dma)) {
        GbaDmaUnitStep(emulator->dma, emulator->memory);
      }
      GbaTimersStep(emulator->timers);
      frame_rendered = GbaPpuStep(emulator->ppu, fbo, scale_factor);
      GbaSpuStep(emulator->spu, audio_sample_callback);
    } else {
      // TODO: Blank screen
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