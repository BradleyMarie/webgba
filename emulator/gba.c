#include "emulator/gba.h"

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
  GbaPlatform *platform;
  Arm7Tdmi *cpu;
  Memory *memory;
  GbaDmaUnit *dma;
  GbaPpu *ppu;
  GbaSpu *spu;
  GbaTimers *timers;
  GbaPeripherals *peripherals;
};

bool GbaEmulatorAllocate(const char *rom_data, uint32_t rom_size,
                         GbaEmulator **emulator, GamePad **gamepad) {
  *emulator = malloc(sizeof(GbaEmulator));
  if (*emulator == NULL) {
    return false;
  }

  Memory *platform_registers;
  InterruptLine *rst;
  InterruptLine *fiq;
  InterruptLine *irq;
  bool success = GbaPlatformAllocate(&(*emulator)->platform,
                                     &platform_registers, &rst, &fiq, &irq);
  if (!success) {
    free(*emulator);
    return false;
  }

  (*emulator)->cpu = Arm7TdmiAllocate(rst, fiq, irq);
  if ((*emulator)->cpu == NULL) {
    GbaPlatformRelease((*emulator)->platform);
    InterruptLineFree(rst);
    InterruptLineFree(fiq);
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

  Memory *timer_registers;
  success = GbaTimersAllocate((*emulator)->platform, &(*emulator)->timers,
                              &timer_registers);
  if (!success) {
    GbaDmaUnitRelease((*emulator)->dma);
    Arm7TdmiFree((*emulator)->cpu);
    GbaPlatformRelease((*emulator)->platform);
    free(*emulator);
    return false;
  }

  Memory *sound_registers;
  success = GbaSpuAllocate(&(*emulator)->spu, &sound_registers);
  if (!success) {
    GbaTimersFree((*emulator)->timers);
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
    GbaSpuFree((*emulator)->spu);
    GbaTimersFree((*emulator)->timers);
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
    GbaSpuFree((*emulator)->spu);
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
    GbaSpuFree((*emulator)->spu);
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
    GbaSpuFree((*emulator)->spu);
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
    GbaSpuFree((*emulator)->spu);
    GbaTimersFree((*emulator)->timers);
    GbaDmaUnitRelease((*emulator)->dma);
    Arm7TdmiFree((*emulator)->cpu);
    GbaPlatformRelease((*emulator)->platform);
    free(*emulator);
    return false;
  }

  return true;
}

void GbaEmulatorFree(GbaEmulator *emulator) {
  GbaPlatformRelease(emulator->platform);
  Arm7TdmiFree(emulator->cpu);
  MemoryFree(emulator->memory);
  GbaDmaUnitRelease(emulator->dma);
  GbaPpuFree(emulator->ppu);
  GbaSpuFree(emulator->spu);
  GbaTimersFree(emulator->timers);
  GbaPeripheralsFree(emulator->peripherals);
  free(emulator);
}

void GbaEmulatorStep(GbaEmulator *emulator) {
  GbaPowerState power_state = GbaPlatformPowerState(emulator->platform);
  if (power_state == GBA_POWER_STATE_STOP) {
    return;
  }

  if (power_state == GBA_POWER_STATE_RUN) {
    if (GbaDmaUnitIsActive(emulator->dma)) {
      GbaDmaUnitStep(emulator->dma, emulator->memory);
    } else {
      Arm7TdmiStep(emulator->cpu, emulator->memory);
    }
  }

  GbaPpuStep(emulator->ppu);
  GbaSpuStep(emulator->spu);
  GbaTimersStep(emulator->timers);
}

void GbaEmulatorSetRenderOutput(GbaEmulator *emulator, GLuint framebuffer) {
  GbaPpuSetRenderOutput(emulator->ppu, framebuffer);
}

void GbaEmulatorSetRenderScale(GbaEmulator *emulator, uint8_t scale_factor) {
  GbaPpuSetRenderScale(emulator->ppu, scale_factor);
}

void GbaEmulatorSetRenderDoneCallback(
    GbaEmulator *emulator, GbaEmulatorRenderDoneFunction frame_done) {
  GbaPpuSetRenderDoneCallback(emulator->ppu, frame_done);
}

void GbaEmulatorReloadContext(GbaEmulator *emulator) {
  GbaPpuReloadContext(emulator->ppu);
}