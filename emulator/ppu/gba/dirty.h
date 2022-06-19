#ifndef _WEBGBA_EMULATOR_PPU_GBA_DIRTY_
#define _WEBGBA_EMULATOR_PPU_GBA_DIRTY_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/set.h"

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
  GbaPpuSet rotations;
  bool objects;
} GbaPpuOamDirtyBits;

typedef struct {
  bool dispcnt;
  bool blend;
  bool bg_affine[GBA_PPU_NUM_AFFINE_BACKGROUNDS];
  bool bg_offset[GBA_PPU_NUM_BACKGROUNDS];
  bool bg_control[GBA_PPU_NUM_BACKGROUNDS];
  bool bg_mosaic;
  bool obj_mosaic;
  bool window;
} GbaPpuIoDirtyBits;

typedef struct {
  GbaPpuPaletteDirtyBits palette;
  GbaPpuVramDirtyBits vram;
  GbaPpuOamDirtyBits oam;
  GbaPpuIoDirtyBits io;
} GbaPpuDirtyBits;

void GbaPpuDirtyBitsAllDirty(GbaPpuDirtyBits *bits);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_DIRTY_