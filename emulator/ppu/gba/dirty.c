#include "emulator/ppu/gba/dirty.h"

void GbaPpuDirtyBitsAllDirty(GbaPpuDirtyBits *bits) {
  bits->palette.bg_palette = true;
  bits->palette.obj_palette = true;

  bits->vram.tile_mode.overall = true;
  bits->vram.mode_3.overall = true;
  bits->vram.mode_4.pages[0u] = true;
  bits->vram.mode_4.pages[1u] = true;
  bits->vram.mode_5.pages[0u] = true;
  bits->vram.mode_5.pages[1u] = true;

  for (uint8_t i = 0; i < OAM_NUM_OBJECTS; i++) {
    GbaPpuSetAdd(&bits->oam.objects, i);
  }

  for (uint8_t i = 0; i < OAM_NUM_ROTATE_SCALE_GROUPS; i++) {
    GbaPpuSetAdd(&bits->oam.rotations, i);
  }

  bits->composite.bg_affine[0u] = true;
  bits->composite.bg_affine[1u] = true;
  bits->composite.bg_mosaic = true;
}