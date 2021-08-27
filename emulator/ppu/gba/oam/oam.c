#include "emulator/ppu/gba/oam/oam.h"

#include <assert.h>
#include <stdlib.h>

#define OAM_ADDRESS_MASK 0x3FFu

typedef struct {
  GbaPpuObjectAttributeMemory *memory;
  void *free_address;
  uint16_t *reference_count;
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

static bool OamStore32LE(void *context, uint32_t address, uint32_t value) {
  assert((address & 0x3u) == 0u);

  GbaPpuOam *oam = (GbaPpuOam *)context;

  address &= OAM_ADDRESS_MASK;
  oam->memory->words[address >> 2u] = value;

  return true;
}

static bool OamStore16LE(void *context, uint32_t address, uint16_t value) {
  assert((address & 0x1u) == 0u);

  GbaPpuOam *oam = (GbaPpuOam *)context;

  address &= OAM_ADDRESS_MASK;
  oam->memory->half_words[address >> 1u] = value;

  return true;
}

static bool OamStore8(void *context, uint32_t address, uint8_t value) {
  // Byte writes to OAM are ignored
  return true;
}

static void OamFree(void *context) {
  GbaPpuOam *oam = (GbaPpuOam *)context;
  assert(*oam->reference_count != 0u);
  *oam->reference_count -= 1u;
  if (*oam->reference_count == 0u) {
    free(oam->free_address);
    free(oam);
  }
}

Memory *OamAllocate(GbaPpuObjectAttributeMemory *oam_memory, void *free_address,
                    uint16_t *reference_count) {
  GbaPpuOam *oam = (GbaPpuOam *)malloc(sizeof(GbaPpuOam));
  if (oam == NULL) {
    return NULL;
  }

  oam->memory = oam_memory;
  oam->free_address = free_address;
  oam->reference_count = reference_count;

  Memory *result =
      MemoryAllocate(oam, OamLoad32LE, OamLoad16LE, OamLoad8, OamStore32LE,
                     OamStore16LE, OamStore8, OamFree);
  if (result == NULL) {
    free(oam);
    return NULL;
  }

  *reference_count += 1u;

  return result;
}