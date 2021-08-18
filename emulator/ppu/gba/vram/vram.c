#include "emulator/ppu/gba/vram/vram.h"

#include <assert.h>
#include <stdlib.h>

#define VRAM_ADDRESS_MASK 0x1FFFFu

static inline uint32_t VRamComputeAddress(uint32_t address) {
  address &= VRAM_ADDRESS_MASK;
  static const uint32_t mask[2u] = {0xFFFFu, 0x17FFFu};
  return address & mask[address >> 16u];
}

static inline uint32_t VRamComputeByteStoreAddress(uint32_t address) {
  address &= VRAM_ADDRESS_MASK;
  static const uint32_t mask[2u] = {0xFFFEu, 0x17FFEu};
  return address & mask[address >> 16u];
}

static bool VRamLoad32LE(const void *context, uint32_t address,
                         uint32_t *value) {
  assert((address & 0x3u) == 0u);

  const GbaPpuMemory *ppu_memory = (const GbaPpuMemory *)context;

  address = VRamComputeAddress(address);
  *value = ppu_memory->vram.words[address >> 2u];

  return true;
}

static bool VRamLoad16LE(const void *context, uint32_t address,
                         uint16_t *value) {
  assert((address & 0x1u) == 0u);

  const GbaPpuMemory *ppu_memory = (const GbaPpuMemory *)context;

  address = VRamComputeAddress(address);
  *value = ppu_memory->vram.half_words[address >> 1u];

  return true;
}

static bool VRamLoad8(const void *context, uint32_t address, uint8_t *value) {
  const GbaPpuMemory *ppu_memory = (const GbaPpuMemory *)context;

  address = VRamComputeAddress(address);
  *value = ppu_memory->vram.bytes[address];

  return true;
}

static bool VRamStore32LE(void *context, uint32_t address, uint32_t value) {
  assert((address & 0x3u) == 0u);

  GbaPpuMemory *ppu_memory = (GbaPpuMemory *)context;

  address = VRamComputeAddress(address);
  ppu_memory->vram.words[address >> 2u] = value;

  return true;
}

static bool VRamStore16LE(void *context, uint32_t address, uint16_t value) {
  assert((address & 0x1u) == 0u);

  GbaPpuMemory *ppu_memory = (GbaPpuMemory *)context;

  address = VRamComputeAddress(address);
  ppu_memory->vram.half_words[address >> 1u] = value;

  return true;
}

static bool VRamStore8(void *context, uint32_t address, uint8_t value) {
  GbaPpuMemory *ppu_memory = (GbaPpuMemory *)context;

  address = VRamComputeByteStoreAddress(address);
  if (address >= VRAM_BG_SIZE) {
    return true;
  }

  uint16_t value16 = ((uint16_t)value << 8u) | value;
  ppu_memory->vram.half_words[address >> 1u] = value16;

  return true;
}

static void VRamFree(void *context) {
  GbaPpuMemory *ppu_memory = (GbaPpuMemory *)context;
  assert(ppu_memory->reference_count != 0u);
  ppu_memory->reference_count -= 1u;
  if (ppu_memory->reference_count == 0u) {
    free(ppu_memory->free_address);
  }
}

Memory *VRamAllocate(GbaPpuMemory *ppu_memory) {
  Memory *result =
      MemoryAllocate(ppu_memory, VRamLoad32LE, VRamLoad16LE, VRamLoad8,
                     VRamStore32LE, VRamStore16LE, VRamStore8, VRamFree);
  if (result == NULL) {
    return NULL;
  }

  ppu_memory->reference_count += 1u;

  return result;
}