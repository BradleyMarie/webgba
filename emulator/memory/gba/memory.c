#include "emulator/memory/gba/memory.h"

#include <stdlib.h>

#include "emulator/memory/gba/bad/bad.h"
#include "emulator/memory/gba/bios/bios.h"
#include "emulator/memory/gba/io/io.h"
#include "emulator/memory/gba/iram/iram.h"
#include "emulator/memory/gba/wram/wram.h"

typedef struct {
  Memory* banks[16];
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

  if (bank >= 16u) {
    return memory->bad;
  }

  static const uint32_t address_mask[16] = {
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu,
      0x01FFFFFFu, 0x01FFFFFFu, 0x01FFFFFFu, 0x01FFFFFFu,
      0x01FFFFFFu, 0x01FFFFFFu, 0x00FFFFFFu, 0x00FFFFFFu};
  *address &= address_mask[bank];

  return memory->banks[bank];
}

static bool GbaMemoryLoad32LE(const void* context, uint32_t address,
                              uint32_t* value) {
  const GbaMemory* gba_memory = (const GbaMemory*)context;
  Memory* memory = GbaMemorySelectBank(gba_memory, &address);
  if (Load32LE(memory, address, value)) {
    return true;
  }

  // TODO: Implement Open Bus
  *value = 0u;

  return true;
}

static bool GbaMemoryLoad16LE(const void* context, uint32_t address,
                              uint16_t* value) {
  const GbaMemory* gba_memory = (const GbaMemory*)context;
  Memory* memory = GbaMemorySelectBank(gba_memory, &address);
  if (Load16LE(memory, address, value)) {
    return true;
  }

  // TODO: Implement Open Bus
  *value = 0u;

  return true;
}

static bool GbaMemoryLoad8(const void* context, uint32_t address,
                           uint8_t* value) {
  const GbaMemory* gba_memory = (const GbaMemory*)context;
  Memory* memory = GbaMemorySelectBank(gba_memory, &address);
  if (Load8(memory, address, value)) {
    return true;
  }

  // TODO: Implement Open Bus
  *value = 0u;

  return true;
}

static bool GbaMemoryStore32LE(void* context, uint32_t address,
                               uint32_t value) {
  GbaMemory* gba_memory = (GbaMemory*)context;
  Memory* memory = GbaMemorySelectBank(gba_memory, &address);
  Store32LE(memory, address, value);
  return true;
}

static bool GbaMemoryStore16LE(void* context, uint32_t address,
                               uint16_t value) {
  GbaMemory* gba_memory = (GbaMemory*)context;
  Memory* memory = GbaMemorySelectBank(gba_memory, &address);
  Store16LE(memory, address, value);
  return true;
}

static bool GbaMemoryStore8(void* context, uint32_t address, uint8_t value) {
  GbaMemory* gba_memory = (GbaMemory*)context;
  Memory* memory = GbaMemorySelectBank(gba_memory, &address);
  Store8(memory, address, value);
  return true;
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

  Memory* bad = BadMemoryAllocate();
  if (bad == NULL) {
    free(gba_memory);
    return false;
  }

  Memory* bios = GBABiosAllocate();
  if (bios == NULL) {
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

  gba_memory->banks[0x0u] = bios;
  gba_memory->banks[0x1u] = bad;
  gba_memory->banks[0x2u] = ewram;
  gba_memory->banks[0x3u] = iwram;
  gba_memory->banks[0x4u] = NULL;
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

  gba_memory->bios = bios;
  gba_memory->ewram = ewram;
  gba_memory->iwram = iwram;
  gba_memory->io = NULL;
  gba_memory->palette = NULL;
  gba_memory->vram = NULL;
  gba_memory->oam = NULL;
  gba_memory->game = NULL;
  gba_memory->sram = NULL;
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

  // This is a bit of a hack in my opinion. It'd be a little cleaner if
  // gba_memory was not modified by this function after allocating the result.
  // That said, this is easier than the alternative mechanisms for *not* freeing
  // the inputs to this function in the presence of a failed allocation.
  Memory* io = IoMemoryAllocate(ppu_registers, sound_registers, dma_registers,
                                timer_registers, peripheral_registers,
                                platform_registers);
  if (io == NULL) {
    MemoryFree(result);
    return NULL;
  }

  gba_memory->banks[0x4u] = io;

  gba_memory->io = io;
  gba_memory->palette = palette;
  gba_memory->vram = vram;
  gba_memory->oam = oam;
  gba_memory->game = game;
  gba_memory->sram = sram;

  return result;
}