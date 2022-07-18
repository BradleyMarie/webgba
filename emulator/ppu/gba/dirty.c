#include "emulator/ppu/gba/dirty.h"

void GbaPpuDirtyBitsAllDirty(GbaPpuDirtyBits *bits) {
  bits->palette.palette[0u] = true;
  bits->palette.palette[1u] = true;
  bits->vram.bitmap_mode_3 = true;
  bits->vram.bitmap_mode_4[0u] = true;
  bits->vram.bitmap_mode_4[1u] = true;
  bits->vram.bitmap_mode_5[0u] = true;
  bits->vram.bitmap_mode_5[1u] = true;
  bits->vram.affine_tilemap = true;
  bits->vram.scrolling_tilemap = true;
  bits->vram.bg_tiles = true;
  bits->vram.obj_tiles = true;
  bits->oam.transformations = true;
  bits->oam.attributes = true;
  bits->io.obj_mosaic = true;
}