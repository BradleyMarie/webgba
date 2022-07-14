#include "emulator/ppu/gba/vram/vram.h"

#include <assert.h>
#include <stdlib.h>

#define BITMAP_MODE_3_SIZE_BYTES \
  (GBA_SCREEN_WIDTH * GBA_SCREEN_HEIGHT * sizeof(uint16_t))
#define BITMAP_MODE_4_SIZE_BYTES \
  (GBA_SCREEN_WIDTH * GBA_SCREEN_HEIGHT * sizeof(uint8_t))
#define BITMAP_MODE_4_PAGE_SIZE_BYTES sizeof(BitmapMode4BackgroundPage)
#define BITMAP_MODE_5_SIZE_BYTES \
  (GBA_REDUCED_FRAME_WIDTH * GBA_REDUCED_FRAME_HEIGHT * sizeof(uint16_t))

#define TILEMAP_BLOCK_SIZE_BYTES GBA_AFFINE_TILE_MAP_INDICES_PER_BLOCK
#define TILEBLOCK_SIZE_BYTES \
  (GBA_TILE_MODE_TILE_BLOCK_NUM_D_TILES * GBA_TILE_1D_SIZE * GBA_TILE_1D_SIZE)

#define VRAM_ADDRESS_MASK 0x1FFFFu
#define VRAM_BG_SIZE (64u * 1024u)

typedef struct {
  GbaPpuVideoMemory *memory;
  GbaPpuVramDirtyBits *dirty;
  MemoryContextFree free_routine;
  void *free_address;
} GbaPpuVRam;

static void VRamUpdateDirtyBits(GbaPpuVRam *vram, uint32_t address) {
  if (address < BITMAP_MODE_3_SIZE_BYTES) {
    vram->dirty->bitmap_mode_3 = true;
  }

  uint8_t mode4_page = address / BITMAP_MODE_4_PAGE_SIZE_BYTES;
  if (mode4_page < 2u &&
      address % BITMAP_MODE_4_PAGE_SIZE_BYTES < BITMAP_MODE_4_SIZE_BYTES) {
    vram->dirty->bitmap_mode_4[mode4_page] = true;
  }

  uint8_t mode5_page = address / BITMAP_MODE_5_SIZE_BYTES;
  if (mode5_page < 2u) {
    vram->dirty->bitmap_mode_5[mode5_page] = true;
  }

  uint8_t tilemap_block = address / TILEMAP_BLOCK_SIZE_BYTES;
  if (tilemap_block < GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS) {
    vram->dirty->affine_tilemap = true;
    vram->dirty->scrolling_tilemap = true;
  }

  uint8_t tile_block = address / TILEBLOCK_SIZE_BYTES;
  if (tile_block < GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS) {
    vram->dirty->bg_tiles = true;
  } else {
    vram->dirty->obj_tiles = true;
  }
}

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
  if (vram->memory->words[address >> 2u] != value) {
    vram->memory->words[address >> 2u] = value;
    VRamUpdateDirtyBits(vram, address);
  }

  return true;
}

static bool VRamStore16LE(void *context, uint32_t address, uint16_t value) {
  assert((address & 0x1u) == 0u);

  GbaPpuVRam *vram = (GbaPpuVRam *)context;

  address = VRamComputeAddress(address);
  if (vram->memory->half_words[address >> 1u] != value) {
    vram->memory->half_words[address >> 1u] = value;
    VRamUpdateDirtyBits(vram, address);
  }

  return true;
}

static bool VRamStore8(void *context, uint32_t address, uint8_t value) {
  GbaPpuVRam *vram = (GbaPpuVRam *)context;

  address = VRamComputeByteStoreAddress(address);
  if (address >= VRAM_BG_SIZE) {
    return true;
  }

  uint16_t value16 = ((uint16_t)value << 8u) | value;
  if (vram->memory->half_words[address >> 1u] != value16) {
    vram->memory->half_words[address >> 1u] = value16;
    VRamUpdateDirtyBits(vram, address);
  }

  return true;
}

static void VRamFree(void *context) {
  GbaPpuVRam *vram = (GbaPpuVRam *)context;
  vram->free_routine(vram->free_address);
  free(vram);
}

Memory *VRamAllocate(GbaPpuVideoMemory *video_memory,
                     GbaPpuVramDirtyBits *dirty, MemoryContextFree free_routine,
                     void *free_address) {
  GbaPpuVRam *vram = (GbaPpuVRam *)malloc(sizeof(GbaPpuVRam));
  if (vram == NULL) {
    return NULL;
  }

  vram->memory = video_memory;
  vram->dirty = dirty;
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