#ifndef _WEBGBA_EMULATOR_PPU_GBA_DIRTY_
#define _WEBGBA_EMULATOR_PPU_GBA_DIRTY_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  bool palette[2u];
} GbaPpuPaletteDirtyBits;

typedef struct {
  bool bitmap_mode_3;
  bool bitmap_mode_4[2u];
  bool bitmap_mode_5[2u];
  bool affine_tilemap[GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS];
  bool scrolling_tilemap[GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS];
  bool tiles[6u];
} GbaPpuVramDirtyBits;

typedef struct {
  bool overall;
} GbaPpuOamDirtyBits;

typedef struct {
  bool obj_mosaic;
} GbaPpuIoDirtyBits;

typedef struct {
  GbaPpuPaletteDirtyBits palette;
  GbaPpuVramDirtyBits vram;
  GbaPpuOamDirtyBits oam;
  GbaPpuIoDirtyBits io;
} GbaPpuDirtyBits;

void GbaPpuDirtyBitsAllDirty(GbaPpuDirtyBits *bits);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_DIRTY_