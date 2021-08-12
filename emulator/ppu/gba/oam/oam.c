#include "emulator/ppu/gba/oam/oam.h"

#include <assert.h>
#include <stdlib.h>

#define OAM_ADDRESS_MASK 0x3FFu

static bool OamLoad32LE(const void *context, uint32_t address,
                        uint32_t *value) {
  assert((address & 0x3u) == 0u);

  const GbaPpuMemory *ppu_memory = (const GbaPpuMemory *)context;

  address &= OAM_ADDRESS_MASK;
  const unsigned char *first_byte = ppu_memory->oam + address;
  *value = *(const uint32_t *)(const void *)first_byte;

  return true;
}

static bool OamLoad16LE(const void *context, uint32_t address,
                        uint16_t *value) {
  assert((address & 0x1u) == 0u);

  const GbaPpuMemory *ppu_memory = (const GbaPpuMemory *)context;

  address &= OAM_ADDRESS_MASK;
  const unsigned char *first_byte = ppu_memory->oam + address;
  *value = *(const uint16_t *)(const void *)first_byte;

  return true;
}

static bool OamLoad8(const void *context, uint32_t address, uint8_t *value) {
  const GbaPpuMemory *ppu_memory = (const GbaPpuMemory *)context;

  address &= OAM_ADDRESS_MASK;
  const unsigned char *first_byte = ppu_memory->oam + address;
  *value = *(const uint8_t *)(const void *)first_byte;

  return true;
}

static bool OamStore32LE(void *context, uint32_t address, uint32_t value) {
  assert((address & 0x3u) == 0u);

  GbaPpuMemory *ppu_memory = (GbaPpuMemory *)context;

  address &= OAM_ADDRESS_MASK;
  unsigned char *first_byte = ppu_memory->oam + address;
  uint32_t *memory_cell = (uint32_t *)(void *)first_byte;
  *memory_cell = value;

  return true;
}

static bool OamStore16LE(void *context, uint32_t address, uint16_t value) {
  assert((address & 0x1u) == 0u);

  GbaPpuMemory *ppu_memory = (GbaPpuMemory *)context;

  address &= OAM_ADDRESS_MASK;
  unsigned char *first_byte = ppu_memory->oam + address;
  uint16_t *memory_cell = (uint16_t *)(void *)first_byte;
  *memory_cell = value;

  return true;
}

static bool OamStore8(void *context, uint32_t address, uint8_t value) {
  // Byte writes to OAM are ignored
  return true;
}

static void OamFree(void *context) {
  GbaPpuMemory *ppu_memory = (GbaPpuMemory *)context;
  assert(ppu_memory->reference_count != 0u);
  ppu_memory->reference_count -= 1u;
  if (ppu_memory->reference_count == 0u) {
    free(ppu_memory->free_address);
  }
}

Memory *OamAllocate(GbaPpuMemory *ppu_memory) {
  Memory *result =
      MemoryAllocate(ppu_memory, OamLoad32LE, OamLoad16LE, OamLoad8,
                     OamStore32LE, OamStore16LE, OamStore8, OamFree);
  if (result == NULL) {
    return NULL;
  }

  ppu_memory->reference_count += 1u;

  return result;
}