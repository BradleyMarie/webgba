#include "emulator/ppu/gba/dirty.h"

void GbaPpuDirtyBitsAllDirty(GbaPpuDirtyBits *bits) {
  bits->palette.palette[0u] = true;
  bits->palette.palette[1u] = true;

  bits->vram.bitmap_mode_3 = true;
  bits->vram.bitmap_mode_4[0u] = true;
  bits->vram.bitmap_mode_4[1u] = true;
  bits->vram.bitmap_mode_5[0u] = true;
  bits->vram.bitmap_mode_5[1u] = true;

  for (uint8_t i = 0u; i < GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS; i++) {
    bits->vram.affine_tilemap[i] = true;
    bits->vram.scrolling_tilemap[i] = true;
  }

  bits->vram.tiles[0u] = true;
  bits->vram.tiles[1u] = true;
  bits->vram.tiles[2u] = true;
  bits->vram.tiles[3u] = true;
  bits->vram.tiles[4u] = true;
  bits->vram.tiles[5u] = true;

  for (uint8_t i = 0u; i < OAM_NUM_ROTATE_SCALE_GROUPS; i++) {
    GbaPpuSetAdd(&bits->oam.rotations, i);
  }

  bits->oam.objects = true;

  bits->io.dispcnt = true;
  bits->io.blend = true;
  bits->io.bg_affine[0u] = true;
  bits->io.bg_affine[1u] = true;
  bits->io.bg_offset[0u] = true;
  bits->io.bg_offset[1u] = true;
  bits->io.bg_offset[2u] = true;
  bits->io.bg_offset[3u] = true;
  bits->io.bg_control[0u] = true;
  bits->io.bg_control[1u] = true;
  bits->io.bg_control[2u] = true;
  bits->io.bg_control[3u] = true;
  bits->io.bg_mosaic = true;
  bits->io.obj_mosaic = true;
  bits->io.window = true;
}