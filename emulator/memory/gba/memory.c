#include "emulator/memory/gba/memory.h"

#include <assert.h>
#include <stdlib.h>

#include "emulator/memory/gba/bad/bad.h"
#include "emulator/memory/gba/bios/bios.h"
#include "emulator/memory/gba/io/io.h"
#include "emulator/memory/gba/iram/iram.h"
#include "emulator/memory/gba/open_bus/open_bus.h"
#include "emulator/memory/gba/wram/wram.h"

#define NUMBER_OF_MEMORY_BANKS 256u

typedef struct {
  Memory* banks[NUMBER_OF_MEMORY_BANKS];
  Memory* bios;
  Memory* ewram;
  Memory* iwram;
  Memory* io;
  Memory* palette;
  Memory* vram;
  Memory* oam;
  Memory* game;
  Memory* sram;
  Memory* bad;
} GbaMemory;

static Memory* GbaMemorySelectBank(const GbaMemory* memory, uint32_t* address) {
  uint32_t bank = *address >> 24u;

  static const uint32_t address_mask[NUMBER_OF_MEMORY_BANKS] = {
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x01FFFFFFu, 0x01FFFFFFu,
      0x01FFFFFFu, 0x01FFFFFFu, 0x01FFFFFFu, 0x01FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu,  // Everything after this is for an unused bank
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu};
  *address &= address_mask[bank];

  return memory->banks[bank];
}

static bool GbaMemoryLoad32LE(const void* context, uint32_t address,
                              uint32_t* value) {
  const GbaMemory* gba_memory = (const GbaMemory*)context;
  Memory* memory = GbaMemorySelectBank(gba_memory, &address);
  bool result = Load32LE(memory, address, value);
  assert(result);
  return result;
}

static bool GbaMemoryLoad16LE(const void* context, uint32_t address,
                              uint16_t* value) {
  const GbaMemory* gba_memory = (const GbaMemory*)context;
  Memory* memory = GbaMemorySelectBank(gba_memory, &address);
  bool result = Load16LE(memory, address, value);
  assert(result);
  return result;
}

static bool GbaMemoryLoad8(const void* context, uint32_t address,
                           uint8_t* value) {
  const GbaMemory* gba_memory = (const GbaMemory*)context;
  Memory* memory = GbaMemorySelectBank(gba_memory, &address);
  bool result = Load8(memory, address, value);
  assert(result);
  return result;
}

static bool GbaMemoryStore32LE(void* context, uint32_t address,
                               uint32_t value) {
  GbaMemory* gba_memory = (GbaMemory*)context;
  Memory* memory = GbaMemorySelectBank(gba_memory, &address);
  bool result = Store32LE(memory, address, value);
  assert(result);
  return result;
}

static bool GbaMemoryStore16LE(void* context, uint32_t address,
                               uint16_t value) {
  GbaMemory* gba_memory = (GbaMemory*)context;
  Memory* memory = GbaMemorySelectBank(gba_memory, &address);
  bool result = Store16LE(memory, address, value);
  assert(result);
  return result;
}

static bool GbaMemoryStore8(void* context, uint32_t address, uint8_t value) {
  GbaMemory* gba_memory = (GbaMemory*)context;
  Memory* memory = GbaMemorySelectBank(gba_memory, &address);
  bool result = Store8(memory, address, value);
  assert(result);
  return result;
}

static void GbaMemoryFree(void* context) {
  GbaMemory* gba_memory = (GbaMemory*)context;
  MemoryFree(gba_memory->bios);
  MemoryFree(gba_memory->ewram);
  MemoryFree(gba_memory->iwram);
  MemoryFree(gba_memory->io);
  MemoryFree(gba_memory->palette);
  MemoryFree(gba_memory->vram);
  MemoryFree(gba_memory->oam);
  MemoryFree(gba_memory->game);
  MemoryFree(gba_memory->sram);
  MemoryFree(gba_memory->bad);
  free(gba_memory);
}

Memory* GbaMemoryAllocate(Memory* ppu_registers, Memory* sound_registers,
                          Memory* dma_registers, Memory* timer_registers,
                          Memory* peripheral_registers,
                          Memory* platform_registers, Memory* palette,
                          Memory* vram, Memory* oam, Memory* game,
                          Memory* sram) {
  GbaMemory* gba_memory = (GbaMemory*)malloc(sizeof(GbaMemory));
  if (gba_memory == NULL) {
    return NULL;
  }

  Memory* bad_internal = BadMemoryAllocate();
  if (bad_internal == NULL) {
    free(gba_memory);
    return false;
  }

  Memory* bad = OpenBusAllocate(bad_internal);
  if (bad == NULL) {
    MemoryFree(bad_internal);
    free(gba_memory);
    return NULL;
  }

  Memory* bios_internal = GBABiosAllocate();
  if (bios_internal == NULL) {
    MemoryFree(bad);
    free(gba_memory);
    return NULL;
  }

  Memory* bios = OpenBusAllocate(bios_internal);
  if (bios == NULL) {
    MemoryFree(bios_internal);
    MemoryFree(bad);
    free(gba_memory);
    return NULL;
  }

  Memory* iwram = IRamAllocate();
  if (iwram == NULL) {
    MemoryFree(bios);
    MemoryFree(bad);
    free(gba_memory);
    return NULL;
  }

  Memory* ewram = WRamAllocate();
  if (ewram == NULL) {
    MemoryFree(iwram);
    MemoryFree(bios);
    MemoryFree(bad);
    free(gba_memory);
    return NULL;
  }

  // TODO: Make memory reference counted
  Memory* io_internal = IoMemoryAllocate(
      ppu_registers, sound_registers, dma_registers, timer_registers,
      peripheral_registers, platform_registers);
  if (io_internal == NULL) {
    MemoryFree(ewram);
    MemoryFree(iwram);
    MemoryFree(bios);
    MemoryFree(bad);
    free(gba_memory);
    return NULL;
  }

  Memory* io = OpenBusAllocate(io_internal);
  if (io == NULL) {
    MemoryFree(io_internal);
    MemoryFree(ewram);
    MemoryFree(iwram);
    MemoryFree(bios);
    MemoryFree(bad);
    free(gba_memory);
    return NULL;
  }

  gba_memory->banks[0x0u] = bios;
  gba_memory->banks[0x1u] = bad;
  gba_memory->banks[0x2u] = ewram;
  gba_memory->banks[0x3u] = iwram;
  gba_memory->banks[0x4u] = io;
  gba_memory->banks[0x5u] = palette;
  gba_memory->banks[0x6u] = vram;
  gba_memory->banks[0x7u] = oam;
  gba_memory->banks[0x8u] = game;
  gba_memory->banks[0x9u] = game;
  gba_memory->banks[0xAu] = game;
  gba_memory->banks[0xBu] = game;
  gba_memory->banks[0xCu] = game;
  gba_memory->banks[0xDu] = game;
  gba_memory->banks[0xEu] = sram;
  gba_memory->banks[0xFu] = bad;

  for (size_t i = 16u; i < NUMBER_OF_MEMORY_BANKS; i++) {
    gba_memory->banks[i] = bad;
  }

  gba_memory->bios = bios;
  gba_memory->ewram = ewram;
  gba_memory->iwram = iwram;
  gba_memory->io = io;
  gba_memory->palette = palette;
  gba_memory->vram = vram;
  gba_memory->oam = oam;
  gba_memory->game = game;
  gba_memory->sram = sram;
  gba_memory->bad = bad;

  Memory* result = MemoryAllocate(
      gba_memory, GbaMemoryLoad32LE, GbaMemoryLoad16LE, GbaMemoryLoad8,
      GbaMemoryStore32LE, GbaMemoryStore16LE, GbaMemoryStore8, GbaMemoryFree);
  if (result == NULL) {
    MemoryFree(ewram);
    MemoryFree(iwram);
    MemoryFree(bios);
    MemoryFree(bad);
    free(gba_memory);
    return NULL;
  }

  MemorySetIgnoreWrites(game);
  MemorySetIgnoreWrites(sram);

  return result;
}