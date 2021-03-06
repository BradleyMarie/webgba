#include "emulator/ppu/gba/oam/oam.h"

#include <assert.h>
#include <stdlib.h>

#define OAM_ADDRESS_MASK 0x3FFu

typedef struct {
  GbaPpuObjectAttributeMemory *memory;
  GbaPpuOamDirtyBits *dirty;
  MemoryContextFree free_routine;
  void *free_address;
} GbaPpuOam;

static bool OamLoad32LE(const void *context, uint32_t address,
                        uint32_t *value) {
  assert((address & 0x3u) == 0u);

  const GbaPpuOam *oam = (const GbaPpuOam *)context;

  address &= OAM_ADDRESS_MASK;
  *value = oam->memory->words[address >> 2u];

  return true;
}

static bool OamLoad16LE(const void *context, uint32_t address,
                        uint16_t *value) {
  assert((address & 0x1u) == 0u);

  const GbaPpuOam *oam = (const GbaPpuOam *)context;

  address &= OAM_ADDRESS_MASK;
  *value = oam->memory->half_words[address >> 1u];

  return true;
}

static bool OamLoad8(const void *context, uint32_t address, uint8_t *value) {
  const GbaPpuOam *oam = (const GbaPpuOam *)context;

  address &= OAM_ADDRESS_MASK;
  *value = oam->memory->bytes[address];

  return true;
}

static bool OamStore16LE(void *context, uint32_t address, uint16_t value) {
  assert((address & 0x1u) == 0u);

  GbaPpuOam *oam = (GbaPpuOam *)context;

  address &= OAM_ADDRESS_MASK;

  if (oam->memory->half_words[address >> 1u] == value) {
    return true;
  }

  if (address % 8u < 6u) {
    GbaPpuObjectVisibilityHidden(oam->memory, address >> 3u);
  }

  oam->memory->half_words[address >> 1u] = value;

  if (address % 8u < 6u) {
    GbaPpuObjectVisibilityDrawn(oam->memory, address >> 3u);
    oam->dirty->attributes = true;
  } else {
    oam->dirty->transformations = true;
  }

  return true;
}

static bool OamStore32LE(void *context, uint32_t address, uint32_t value) {
  assert((address & 0x3u) == 0u);

  GbaPpuOam *oam = (GbaPpuOam *)context;

  if (address % 8u == 4u) {
    uint16_t low = (uint16_t)value;
    OamStore16LE(context, address, low);

    uint16_t high = (uint16_t)(value >> 16u);
    OamStore16LE(context, address + 2u, high);

    return true;
  }

  address &= OAM_ADDRESS_MASK;

  if (oam->memory->words[address >> 2u] == value) {
    return true;
  }

  GbaPpuObjectVisibilityHidden(oam->memory, address >> 3u);
  oam->memory->words[address >> 2u] = value;
  GbaPpuObjectVisibilityDrawn(oam->memory, address >> 3u);

  oam->dirty->attributes = true;

  return true;
}

static bool OamStore8(void *context, uint32_t address, uint8_t value) {
  // Byte writes to OAM are ignored
  return true;
}

static void OamFree(void *context) {
  GbaPpuOam *oam = (GbaPpuOam *)context;
  oam->free_routine(oam->free_address);
  free(oam);
}

Memory *OamAllocate(GbaPpuObjectAttributeMemory *oam_memory,
                    GbaPpuOamDirtyBits *dirty, MemoryContextFree free_routine,
                    void *free_address) {
  GbaPpuOam *oam = (GbaPpuOam *)malloc(sizeof(GbaPpuOam));
  if (oam == NULL) {
    return NULL;
  }

  oam->memory = oam_memory;
  oam->dirty = dirty;
  oam->free_routine = free_routine;
  oam->free_address = free_address;

  Memory *result =
      MemoryAllocate(oam, OamLoad32LE, OamLoad16LE, OamLoad8, OamStore32LE,
                     OamStore16LE, OamStore8, OamFree);
  if (result == NULL) {
    free(oam);
    return NULL;
  }

  return result;
}