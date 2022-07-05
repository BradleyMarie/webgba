#include "emulator/memory/gba/memory.h"

#include <assert.h>
#include <stdlib.h>

#include "emulator/memory/gba/bad/bad.h"
#include "emulator/memory/gba/bios/bios.h"
#include "emulator/memory/gba/io/io.h"
#include "emulator/memory/gba/open_bus/open_bus.h"

#define IWRAM_SIZE (32u * 1024u)
#define EWRAM_SIZE (256u * 1024u)
#define NUMBER_OF_MEMORY_BANKS 256u

typedef struct {
  Memory* banks[NUMBER_OF_MEMORY_BANKS];
  Memory* bios;
  Memory* io;
  Memory* palette;
  Memory* vram;
  Memory* oam;
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
  MemoryFree(gba_memory->io);
  MemoryFree(gba_memory->palette);
  MemoryFree(gba_memory->vram);
  MemoryFree(gba_memory->oam);
  MemoryFree(gba_memory->bad);
  free(gba_memory);
}

Memory* GbaMemoryAllocate(Memory* ppu_registers, Memory* sound_registers,
                          Memory* dma_registers, Memory* timer_registers,
                          Memory* peripheral_registers,
                          Memory* platform_registers, Memory* palette,
                          Memory* vram, Memory* oam, MemoryBank* game) {
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

  MemoryBank* iwram = MemoryBankAllocate(IWRAM_SIZE, 1u, NULL);
  if (iwram == NULL) {
    MemoryFree(bios);
    MemoryFree(bad);
    free(gba_memory);
    return NULL;
  }

  MemoryBank* ewram = MemoryBankAllocate(EWRAM_SIZE, 1u, NULL);
  if (ewram == NULL) {
    MemoryBankFree(iwram);
    MemoryFree(bios);
    MemoryFree(bad);
    free(gba_memory);
    return NULL;
  }

  MemoryBank* sram = MemoryBankAllocate(1u, 1u, NULL);
  if (sram == NULL) {
    MemoryBankFree(ewram);
    MemoryBankFree(iwram);
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
    MemoryBankFree(sram);
    MemoryBankFree(ewram);
    MemoryBankFree(iwram);
    MemoryFree(bios);
    MemoryFree(bad);
    free(gba_memory);
    return NULL;
  }

  Memory* io = OpenBusAllocate(io_internal);
  if (io == NULL) {
    MemoryFree(io_internal);
    MemoryBankFree(sram);
    MemoryBankFree(ewram);
    MemoryBankFree(iwram);
    MemoryFree(bios);
    MemoryFree(bad);
    free(gba_memory);
    return NULL;
  }

  gba_memory->banks[0x0u] = bios;
  gba_memory->banks[0x1u] = bad;
  gba_memory->banks[0x2u] = NULL;
  gba_memory->banks[0x3u] = NULL;
  gba_memory->banks[0x4u] = io;
  gba_memory->banks[0x5u] = palette;
  gba_memory->banks[0x6u] = vram;
  gba_memory->banks[0x7u] = oam;
  gba_memory->banks[0x8u] = NULL;
  gba_memory->banks[0x9u] = NULL;
  gba_memory->banks[0xAu] = NULL;
  gba_memory->banks[0xBu] = NULL;
  gba_memory->banks[0xCu] = NULL;
  gba_memory->banks[0xDu] = NULL;
  gba_memory->banks[0xEu] = NULL;
  gba_memory->banks[0xFu] = bad;

  for (size_t i = 16u; i < NUMBER_OF_MEMORY_BANKS; i++) {
    gba_memory->banks[i] = bad;
  }

  gba_memory->bios = bios;
  gba_memory->io = io;
  gba_memory->palette = palette;
  gba_memory->vram = vram;
  gba_memory->oam = oam;
  gba_memory->bad = bad;

  MemoryBank** memory_banks =
      calloc(NUMBER_OF_MEMORY_BANKS, sizeof(MemoryBank*));
  if (memory_banks == NULL) {
    MemoryFree(io);
    MemoryBankFree(sram);
    MemoryBankFree(ewram);
    MemoryBankFree(iwram);
    MemoryFree(bios);
    MemoryFree(bad);
    free(gba_memory);
    return NULL;
  }

  memory_banks[0x2u] = ewram;
  memory_banks[0x3u] = iwram;
  memory_banks[0x8u] = game;
  memory_banks[0x9u] = game;
  memory_banks[0xAu] = game;
  memory_banks[0xBu] = game;
  memory_banks[0xCu] = game;
  memory_banks[0xDu] = game;
  memory_banks[0xEu] = game;

  Memory* result = MemoryAllocateWithBanks(
      gba_memory, memory_banks, NUMBER_OF_MEMORY_BANKS, GbaMemoryLoad32LE,
      GbaMemoryLoad16LE, GbaMemoryLoad8, GbaMemoryStore32LE, GbaMemoryStore16LE,
      GbaMemoryStore8, GbaMemoryFree);

  free(memory_banks);

  if (result == NULL) {
    MemoryBankFree(sram);
    MemoryBankFree(ewram);
    MemoryBankFree(iwram);
    MemoryFree(bios);
    MemoryFree(bad);
    free(gba_memory);
    return NULL;
  }

  return result;
}