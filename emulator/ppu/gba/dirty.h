#ifndef _WEBGBA_EMULATOR_PPU_GBA_DIRTY_
#define _WEBGBA_EMULATOR_PPU_GBA_DIRTY_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/set.h"

typedef struct {
  bool bg_palette;
  bool obj_palette;
} GbaPpuPaletteDirtyBits;

typedef struct {
  bool affine_tilemap[GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS];
  bool scrolling_tilemap[GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS];
  bool tiles[GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS];
} GbaPpuTileModeDirtyBits;

typedef struct {
  bool overall;
} GbaPpuMode3DirtyBits;

typedef struct {
  bool pages[2u];
} GbaPpuMode4DirtyBits;

typedef struct {
  bool pages[2u];
} GbaPpuMode5DirtyBits;

typedef struct {
  GbaPpuTileModeDirtyBits tile_mode;
  GbaPpuMode3DirtyBits mode_3;
  GbaPpuMode4DirtyBits mode_4;
  GbaPpuMode5DirtyBits mode_5;
  bool obj_tilemode_tiles[2u];
} GbaPpuVramDirtyBits;

typedef struct {
  GbaPpuSet objects;
  GbaPpuSet rotations;
} GbaPpuOamDirtyBits;

typedef struct {
  bool dispcnt;
  bool blend;
  bool bg_affine_x[GBA_PPU_NUM_AFFINE_BACKGROUNDS];
  bool bg_affine_y[GBA_PPU_NUM_AFFINE_BACKGROUNDS];
  bool bg_affine_params[GBA_PPU_NUM_AFFINE_BACKGROUNDS];
  bool bg_offset[GBA_PPU_NUM_BACKGROUNDS];
  bool bg_control[GBA_PPU_NUM_BACKGROUNDS];
} GbaPpuIoDirtyBits;

typedef struct {
  bool bg_mosaic;
  bool window;
} GbaPpuCompositeDirtyBits;

typedef struct {
  GbaPpuPaletteDirtyBits palette;
  GbaPpuVramDirtyBits vram;
  GbaPpuOamDirtyBits oam;
  GbaPpuIoDirtyBits io;
  GbaPpuCompositeDirtyBits composite;
} GbaPpuDirtyBits;

void GbaPpuDirtyBitsAllDirty(GbaPpuDirtyBits *bits);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_DIRTY_