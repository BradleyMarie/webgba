#include "emulator/ppu/gba/dirty.h"

void GbaPpuDirtyBitsAllDirty(GbaPpuDirtyBits *bits) {
  bits->palette.bg_palette = true;
  bits->palette.obj_palette = true;

  for (uint8_t i = 0u; i < GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS; i++) {
    bits->vram.tile_mode.affine_tilemap[i] = true;
    bits->vram.tile_mode.scrolling_tilemap[i] = true;
  }

  for (uint8_t i = 0u; i < GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS; i++) {
    bits->vram.tile_mode.tiles[i] = true;
  }

  bits->vram.mode_3.overall = true;
  bits->vram.mode_4.pages[0u] = true;
  bits->vram.mode_4.pages[1u] = true;
  bits->vram.mode_5.pages[0u] = true;
  bits->vram.mode_5.pages[1u] = true;
  bits->vram.obj_tiles[0u] = true;
  bits->vram.obj_tiles[1u] = true;

  for (uint8_t i = 0u; i < OAM_NUM_OBJECTS; i++) {
    GbaPpuSetAdd(&bits->oam.objects, i);
  }

  for (uint8_t i = 0u; i < OAM_NUM_ROTATE_SCALE_GROUPS; i++) {
    GbaPpuSetAdd(&bits->oam.rotations, i);
  }

  bits->io.dispcnt = true;
  bits->io.blend = true;
  bits->io.bg_affine_x[0u] = true;
  bits->io.bg_affine_x[1u] = true;
  bits->io.bg_affine_y[0u] = true;
  bits->io.bg_affine_y[1u] = true;
  bits->io.bg_affine_params[0u] = true;
  bits->io.bg_affine_params[1u] = true;
  bits->io.bg_offset[0u] = true;
  bits->io.bg_offset[1u] = true;
  bits->io.bg_offset[2u] = true;
  bits->io.bg_offset[3u] = true;
  bits->io.bg_control[0u] = true;
  bits->io.bg_control[1u] = true;
  bits->io.bg_control[2u] = true;
  bits->io.bg_control[3u] = true;

  bits->composite.bg_mosaic = true;
  bits->composite.window = true;
}