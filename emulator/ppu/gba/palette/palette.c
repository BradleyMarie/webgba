#include "emulator/ppu/gba/palette/palette.h"

#include <assert.h>
#include <stdlib.h>

#define PALETTE_ADDRESS_MASK 0x3FFu
#define PALETTE_BYTE_ADDRESS_MASK 0x3FEu

static bool PaletteLoad32LE(const void *context, uint32_t address,
                            uint32_t *value) {
  assert((address & 0x3u) == 0u);

  const GbaPpuMemory *ppu_memory = (const GbaPpuMemory *)context;

  address &= PALETTE_ADDRESS_MASK;
  *value = ppu_memory->palette.words[address >> 2u];

  return true;
}

static bool PaletteLoad16LE(const void *context, uint32_t address,
                            uint16_t *value) {
  assert((address & 0x1u) == 0u);

  const GbaPpuMemory *ppu_memory = (const GbaPpuMemory *)context;

  address &= PALETTE_ADDRESS_MASK;
  *value = ppu_memory->palette.half_words[address >> 1u];

  return true;
}

static bool PaletteLoad8(const void *context, uint32_t address,
                         uint8_t *value) {
  const GbaPpuMemory *ppu_memory = (const GbaPpuMemory *)context;

  address &= PALETTE_ADDRESS_MASK;
  *value = ppu_memory->palette.bytes[address];

  return true;
}

static bool PaletteStore32LE(void *context, uint32_t address, uint32_t value) {
  assert((address & 0x3u) == 0u);

  GbaPpuMemory *ppu_memory = (GbaPpuMemory *)context;

  address &= PALETTE_ADDRESS_MASK;
  ppu_memory->palette.words[address >> 2u] = value;

  return true;
}

static bool PaletteStore16LE(void *context, uint32_t address, uint16_t value) {
  assert((address & 0x1u) == 0u);

  GbaPpuMemory *ppu_memory = (GbaPpuMemory *)context;

  address &= PALETTE_ADDRESS_MASK;
  ppu_memory->palette.half_words[address >> 1u] = value;

  return true;
}

static bool PaletteStore8(void *context, uint32_t address, uint8_t value) {
  GbaPpuMemory *ppu_memory = (GbaPpuMemory *)context;

  address &= PALETTE_BYTE_ADDRESS_MASK;
  uint16_t value16 = ((uint16_t)value << 8u) | value;
  ppu_memory->palette.half_words[address >> 1u] = value16;

  return true;
}

static void PaletteFree(void *context) {
  GbaPpuMemory *ppu_memory = (GbaPpuMemory *)context;
  assert(ppu_memory->reference_count != 0u);
  ppu_memory->reference_count -= 1u;
  if (ppu_memory->reference_count == 0u) {
    free(ppu_memory->free_address);
  }
}

Memory *PaletteAllocate(GbaPpuMemory *ppu_memory) {
  Memory *result = MemoryAllocate(ppu_memory, PaletteLoad32LE, PaletteLoad16LE,
                                  PaletteLoad8, PaletteStore32LE,
                                  PaletteStore16LE, PaletteStore8, PaletteFree);
  if (result == NULL) {
    return NULL;
  }

  ppu_memory->reference_count += 1u;

  return result;
}