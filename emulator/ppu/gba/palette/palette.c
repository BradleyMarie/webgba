#include "emulator/ppu/gba/palette/palette.h"

#include <assert.h>
#include <stdlib.h>

#define PALETTE_ADDRESS_MASK 0x3FFu
#define PALETTE_BYTE_ADDRESS_MASK 0x3FEu
#define PALETTE_DIRTY_SHIFT 9u

typedef struct {
  GbaPpuPaletteMemory *memory;
  GbaPpuPaletteDirtyBits *dirty;
  MemoryContextFree free_routine;
  void *free_address;
} GbaPpuPalette;

static inline uint16_t RotateLeft(uint16_t value, uint_fast8_t amount) {
  return (value << amount) | (value >> (16u - amount));
}

static inline uint16_t RotateRight(uint16_t value, uint_fast8_t amount) {
  return (value >> amount) | (value << (16u - amount));
}

static bool PaletteLoad16LE(const void *context, uint32_t address,
                            uint16_t *value) {
  assert((address & 0x1u) == 0u);

  const GbaPpuPalette *palette = (const GbaPpuPalette *)context;

  address &= PALETTE_ADDRESS_MASK;
  *value = RotateRight(palette->memory->half_words[address >> 1u], 1u);

  return true;
}

static bool PaletteLoad32LE(const void *context, uint32_t address,
                            uint32_t *value) {
  assert((address & 0x3u) == 0u);

  uint16_t low;
  PaletteLoad16LE(context, address, &low);

  uint16_t high;
  PaletteLoad16LE(context, address + 2u, &high);

  *value = low | (((uint32_t)high) << 16u);

  return true;
}

static bool PaletteLoad8(const void *context, uint32_t address,
                         uint8_t *value) {
  uint16_t temp;
  PaletteLoad16LE(context, address & PALETTE_BYTE_ADDRESS_MASK, &temp);
  *value = temp >> (8u * (address & 1u));

  return true;
}

static bool PaletteStore16LE(void *context, uint32_t address, uint16_t value) {
  assert((address & 0x1u) == 0u);

  GbaPpuPalette *palette = (GbaPpuPalette *)context;

  address &= PALETTE_ADDRESS_MASK;

  uint16_t new_value = RotateLeft(value, 1u);
  bool dirty = palette->memory->half_words[address >> 1u] != new_value;
  palette->memory->half_words[address >> 1u] = new_value;

  palette->dirty->palette[address >> PALETTE_DIRTY_SHIFT] |= dirty;

  return true;
}

static bool PaletteStore32LE(void *context, uint32_t address, uint32_t value) {
  assert((address & 0x3u) == 0u);

  PaletteStore16LE(context, address, value);
  PaletteStore16LE(context, address + 2u, value >> 16u);

  return true;
}

static bool PaletteStore8(void *context, uint32_t address, uint8_t value) {
  address &= PALETTE_BYTE_ADDRESS_MASK;
  uint16_t value16 = ((uint16_t)value << 8u) | value;
  PaletteStore16LE(context, address, value16);

  return true;
}

static void PaletteFree(void *context) {
  GbaPpuPalette *palette = (GbaPpuPalette *)context;
  palette->free_routine(palette->free_address);
  free(palette);
}

Memory *PaletteAllocate(GbaPpuPaletteMemory *palette_memory,
                        GbaPpuPaletteDirtyBits *dirty,
                        MemoryContextFree free_routine, void *free_address) {
  GbaPpuPalette *palette = (GbaPpuPalette *)malloc(sizeof(GbaPpuPalette));
  if (palette == NULL) {
    return NULL;
  }

  palette->memory = palette_memory;
  palette->dirty = dirty;
  palette->free_routine = free_routine;
  palette->free_address = free_address;

  Memory *result = MemoryAllocate(palette, PaletteLoad32LE, PaletteLoad16LE,
                                  PaletteLoad8, PaletteStore32LE,
                                  PaletteStore16LE, PaletteStore8, PaletteFree);
  if (result == NULL) {
    free(palette);
    return NULL;
  }

  return result;
}