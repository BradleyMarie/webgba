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
  bool affine_tilemap;
  bool scrolling_tilemap;
  bool bg_tiles;
  bool obj_tiles;
} GbaPpuVramDirtyBits;

typedef struct {
  bool transformations;
  bool attributes;
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