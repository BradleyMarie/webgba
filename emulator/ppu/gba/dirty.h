#ifndef _WEBGBA_EMULATOR_PPU_GBA_DIRTY_
#define _WEBGBA_EMULATOR_PPU_GBA_DIRTY_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/set.h"

typedef struct {
  bool bg_small_palettes[GBA_NUM_SMALL_PALETTES];
  bool bg_large_palette;
  bool obj_small_palettes[GBA_NUM_SMALL_PALETTES];
  bool obj_large_palette;
} GbaPpuPaletteDirtyBits;

typedef struct {
  bool overall;
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
} GbaPpuVramDirtyBits;

typedef struct {
  GbaPpuSet objects;
  GbaPpuSet rotations;
} GbaPpuOamDirtyBits;

typedef struct {
  bool bg_mosaic;
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