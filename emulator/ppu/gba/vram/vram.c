#include "emulator/ppu/gba/vram/vram.h"

#include <assert.h>
#include <stdlib.h>

#define VRAM_ADDRESS_MASK 0x1FFFFu
#define VRAM_BG_SIZE (64u * 1024u)

typedef struct {
  GbaPpuVideoMemory *memory;
  MemoryContextFree free_routine;
  void *free_address;
} GbaPpuVRam;

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

  const GbaPpuVRam *vram = (const GbaPpuVRam *)context;

  address = VRamComputeAddress(address);
  *value = vram->memory->words[address >> 2u];

  return true;
}

static bool VRamLoad16LE(const void *context, uint32_t address,
                         uint16_t *value) {
  assert((address & 0x1u) == 0u);

  const GbaPpuVRam *vram = (const GbaPpuVRam *)context;

  address = VRamComputeAddress(address);
  *value = vram->memory->half_words[address >> 1u];

  return true;
}

static bool VRamLoad8(const void *context, uint32_t address, uint8_t *value) {
  const GbaPpuVRam *vram = (const GbaPpuVRam *)context;

  address = VRamComputeAddress(address);
  *value = vram->memory->bytes[address];

  return true;
}

static bool VRamStore32LE(void *context, uint32_t address, uint32_t value) {
  assert((address & 0x3u) == 0u);

  GbaPpuVRam *vram = (GbaPpuVRam *)context;

  address = VRamComputeAddress(address);
  vram->memory->words[address >> 2u] = value;

  return true;
}

static bool VRamStore16LE(void *context, uint32_t address, uint16_t value) {
  assert((address & 0x1u) == 0u);

  GbaPpuVRam *vram = (GbaPpuVRam *)context;

  address = VRamComputeAddress(address);
  vram->memory->half_words[address >> 1u] = value;

  return true;
}

static bool VRamStore8(void *context, uint32_t address, uint8_t value) {
  GbaPpuVRam *vram = (GbaPpuVRam *)context;

  address = VRamComputeByteStoreAddress(address);
  if (address >= VRAM_BG_SIZE) {
    return true;
  }

  uint16_t value16 = ((uint16_t)value << 8u) | value;
  vram->memory->half_words[address >> 1u] = value16;

  return true;
}

static void VRamFree(void *context) {
  GbaPpuVRam *vram = (GbaPpuVRam *)context;
  vram->free_routine(vram->free_address);
  free(vram);
}

Memory *VRamAllocate(GbaPpuVideoMemory *video_memory,
                     MemoryContextFree free_routine, void *free_address) {
  GbaPpuVRam *vram = (GbaPpuVRam *)malloc(sizeof(GbaPpuVRam));
  if (vram == NULL) {
    return NULL;
  }

  vram->memory = video_memory;
  vram->free_routine = free_routine;
  vram->free_address = free_address;

  Memory *result =
      MemoryAllocate(vram, VRamLoad32LE, VRamLoad16LE, VRamLoad8, VRamStore32LE,
                     VRamStore16LE, VRamStore8, VRamFree);
  if (result == NULL) {
    free(vram);
    return NULL;
  }

  return result;
}