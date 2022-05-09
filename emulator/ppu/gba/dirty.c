#include "emulator/ppu/gba/dirty.h"

void GbaPpuDirtyBitsAllDirty(GbaPpuDirtyBits *bits) {
  bits->palette.bg_large_palette = true;
  bits->palette.obj_large_palette = true;
  for (uint8_t i = 0; i < GBA_NUM_SMALL_PALETTES; i++) {
    bits->palette.bg_small_palettes[i] = true;
    bits->palette.obj_small_palettes[i] = true;
  }

  bits->vram.tile_mode.overall = true;
  bits->vram.mode_3.overall = true;
  bits->vram.mode_4.pages[0] = true;
  bits->vram.mode_4.pages[1] = true;
  bits->vram.mode_5.pages[0] = true;
  bits->vram.mode_5.pages[1] = true;

  for (uint8_t i = 0; i < OAM_NUM_OBJECTS; i++) {
    GbaPpuSetAdd(&bits->oam.objects, i);
  }

  for (uint8_t i = 0; i < OAM_NUM_ROTATE_SCALE_GROUPS; i++) {
    GbaPpuSetAdd(&bits->oam.rotations, i);
  }
}