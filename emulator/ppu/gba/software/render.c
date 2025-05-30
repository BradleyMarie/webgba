#include "emulator/ppu/gba/software/render.h"

#include <stdlib.h>
#include <string.h>

#include "emulator/ppu/gba/software/bg_affine.h"
#include "emulator/ppu/gba/software/bg_bitmap.h"
#include "emulator/ppu/gba/software/bg_scrolling.h"
#include "emulator/ppu/gba/software/blend.h"
#include "emulator/ppu/gba/software/obj.h"
#include "emulator/ppu/gba/software/window.h"
#include "emulator/screen.h"

struct _GbaPpuSoftwareRenderer {
  uint8_t* subpixels;
};

static void GbaPpuSoftwareRendererDrawPixelImpl(
    GbaPpuSoftwareRenderer* renderer, const GbaPpuMemory* memory,
    const GbaPpuRegisters* registers, GbaPpuDirtyBits* dirty_bits, uint8_t x,
    const int32_t affine_bg2[2], const int32_t affine_bg3[2], uint8_t rgb[3]) {
  uint16_t obj_color;
  uint8_t obj_priority;
  bool object_on_pixel, obj_semi_transparent, on_obj_mask;
  if (registers->dispcnt.object_enable) {
    object_on_pixel =
        GbaPpuObjectPixel(memory, registers, x, registers->vcount, &obj_color,
                          &obj_priority, &obj_semi_transparent, &on_obj_mask);
  } else {
    object_on_pixel = false;
    on_obj_mask = false;
  }

  bool draw_obj, draw_bg0, draw_bg1, draw_bg2, draw_bg3, enable_blending;
  GbaPpuWindowCheck(registers, x, registers->vcount, on_obj_mask, &draw_obj,
                    &draw_bg0, &draw_bg1, &draw_bg2, &draw_bg3,
                    &enable_blending);

  GbaPpuBlendUnit blend_unit;
  GbaPpuBlendUnitReset(&blend_unit);
  if (object_on_pixel && draw_obj) {
    GbaPpuBlendUnitAddObject(&blend_unit, registers, obj_color, obj_priority,
                             obj_semi_transparent);
  }

  uint16_t color;
  bool success;
  switch (registers->dispcnt.mode) {
    case 0:
      if (draw_bg0 && registers->dispcnt.bg0_enable &&
          blend_unit.priorities[1u] > registers->bgcnt[0u].priority) {
        success = GbaPpuScrollingBackgroundPixel(memory, registers,
                                                 GBA_PPU_SCROLLING_BACKGROUND_0,
                                                 x, registers->vcount, &color);
        if (success) {
          GbaPpuBlendUnitAddBackground0(&blend_unit, registers, color);
        }
      }

      if (draw_bg1 && registers->dispcnt.bg1_enable &&
          blend_unit.priorities[1u] > registers->bgcnt[1u].priority) {
        success = GbaPpuScrollingBackgroundPixel(memory, registers,
                                                 GBA_PPU_SCROLLING_BACKGROUND_1,
                                                 x, registers->vcount, &color);
        if (success) {
          GbaPpuBlendUnitAddBackground1(&blend_unit, registers, color);
        }
      }

      if (draw_bg2 && registers->dispcnt.bg2_enable &&
          blend_unit.priorities[1u] > registers->bgcnt[2u].priority) {
        success = GbaPpuScrollingBackgroundPixel(memory, registers,
                                                 GBA_PPU_SCROLLING_BACKGROUND_2,
                                                 x, registers->vcount, &color);
        if (success) {
          GbaPpuBlendUnitAddBackground2(&blend_unit, registers, color);
        }
      }

      if (draw_bg3 && registers->dispcnt.bg3_enable &&
          blend_unit.priorities[1u] > registers->bgcnt[3u].priority) {
        success = GbaPpuScrollingBackgroundPixel(memory, registers,
                                                 GBA_PPU_SCROLLING_BACKGROUND_3,
                                                 x, registers->vcount, &color);
        if (success) {
          GbaPpuBlendUnitAddBackground3(&blend_unit, registers, color);
        }
      }
      break;
    case 1:
      if (draw_bg0 && registers->dispcnt.bg0_enable &&
          blend_unit.priorities[1u] > registers->bgcnt[0u].priority) {
        success = GbaPpuScrollingBackgroundPixel(memory, registers,
                                                 GBA_PPU_SCROLLING_BACKGROUND_0,
                                                 x, registers->vcount, &color);
        if (success) {
          GbaPpuBlendUnitAddBackground0(&blend_unit, registers, color);
        }
      }

      if (draw_bg1 && registers->dispcnt.bg1_enable &&
          blend_unit.priorities[1u] > registers->bgcnt[1u].priority) {
        success = GbaPpuScrollingBackgroundPixel(memory, registers,
                                                 GBA_PPU_SCROLLING_BACKGROUND_1,
                                                 x, registers->vcount, &color);
        if (success) {
          GbaPpuBlendUnitAddBackground1(&blend_unit, registers, color);
        }
      }

      if (draw_bg2 && registers->dispcnt.bg2_enable &&
          blend_unit.priorities[1u] > registers->bgcnt[2u].priority) {
        success = GbaPpuAffineBackgroundPixel(
            memory, registers, GBA_PPU_AFFINE_BACKGROUND_2, affine_bg2[0u],
            affine_bg2[1u], &color);
        if (success) {
          GbaPpuBlendUnitAddBackground2(&blend_unit, registers, color);
        }
      }
      break;
    case 2:
      if (draw_bg2 && registers->dispcnt.bg2_enable &&
          blend_unit.priorities[1u] > registers->bgcnt[2u].priority) {
        success = GbaPpuAffineBackgroundPixel(
            memory, registers, GBA_PPU_AFFINE_BACKGROUND_2, affine_bg2[0u],
            affine_bg2[1u], &color);
        if (success) {
          GbaPpuBlendUnitAddBackground2(&blend_unit, registers, color);
        }
      }

      if (draw_bg3 && registers->dispcnt.bg3_enable &&
          blend_unit.priorities[1u] > registers->bgcnt[3u].priority) {
        success = GbaPpuAffineBackgroundPixel(
            memory, registers, GBA_PPU_AFFINE_BACKGROUND_3, affine_bg3[0u],
            affine_bg3[1u], &color);
        if (success) {
          GbaPpuBlendUnitAddBackground3(&blend_unit, registers, color);
        }
      }
      break;
    case 3:
      if (draw_bg2 && registers->dispcnt.bg2_enable) {
        success = GbaPpuBitmapMode3Pixel(memory, registers, affine_bg2[0u],
                                         affine_bg2[1u], &color);
        if (success) {
          GbaPpuBlendUnitAddBackground2(&blend_unit, registers, color);
        }
      }
      break;
    case 4:
      if (draw_bg2 && registers->dispcnt.bg2_enable) {
        success = GbaPpuBitmapMode4Pixel(memory, registers, affine_bg2[0u],
                                         affine_bg2[1u], &color);
        if (success) {
          GbaPpuBlendUnitAddBackground2(&blend_unit, registers, color);
        }
      }
      break;
    case 5:
      if (draw_bg2 && registers->dispcnt.bg2_enable) {
        success = GbaPpuBitmapMode5Pixel(memory, registers, affine_bg2[0u],
                                         affine_bg2[1u], &color);
        if (success) {
          GbaPpuBlendUnitAddBackground2(&blend_unit, registers, color);
        }
      }
      break;
  }

  GbaPpuBlendUnitAddBackdrop(&blend_unit, registers,
                             memory->palette.bg.large_palette[0u]);

  if (enable_blending) {
    GbaPpuBlendUnitBlend(&blend_unit, registers, rgb);
  } else {
    GbaPpuBlendUnitNoBlend(&blend_unit, rgb);
  }
}

GbaPpuSoftwareRenderer* GbaPpuSoftwareRendererAllocate() {
  return calloc(1u, sizeof(GbaPpuSoftwareRenderer));
}

bool GbaPpuSoftwareRendererSetScreen(GbaPpuSoftwareRenderer* renderer,
                                     Screen* screen) {
  renderer->subpixels =
      ScreenGetPixelBuffer(screen, GBA_SCREEN_WIDTH, GBA_SCREEN_HEIGHT);
  return renderer->subpixels != NULL;
}

void GbaPpuSoftwareRendererDrawRow(GbaPpuSoftwareRenderer* renderer,
                                   const GbaPpuMemory* memory,
                                   const GbaPpuRegisters* registers,
                                   GbaPpuDirtyBits* dirty_bits) {
  if (renderer->subpixels == NULL) {
    return;
  }

  int32_t affine_bg2[2] = {registers->internal.affine[0u].current[0u],
                           registers->internal.affine[0u].current[1u]};
  int32_t affine_bg3[2] = {registers->internal.affine[1u].current[0u],
                           registers->internal.affine[1u].current[1u]};

  uint8_t row = GBA_SCREEN_HEIGHT - registers->vcount - 1u;
  if (!registers->dispcnt.forced_blank) {
    for (uint8_t i = 0; i < GBA_SCREEN_WIDTH; i++) {
      GbaPpuSoftwareRendererDrawPixelImpl(
          renderer, memory, registers, dirty_bits, i, affine_bg2, affine_bg3,
          &renderer->subpixels[row * 3u * GBA_SCREEN_WIDTH + 3u * i]);
      affine_bg2[0u] += registers->affine[0u].pa;
      affine_bg2[1u] += registers->affine[0u].pc;
      affine_bg3[0u] += registers->affine[1u].pa;
      affine_bg3[1u] += registers->affine[1u].pc;
    }
  } else {
    for (uint8_t i = 0; i < GBA_SCREEN_WIDTH; i++) {
      renderer->subpixels[row * 3u * GBA_SCREEN_WIDTH + 3u * i + 0u] = 0u;
      renderer->subpixels[row * 3u * GBA_SCREEN_WIDTH + 3u * i + 1u] = 0u;
      renderer->subpixels[row * 3u * GBA_SCREEN_WIDTH + 3u * i + 2u] = 0u;
    }
  }
}

void GbaPpuSoftwareRendererDrawPixel(GbaPpuSoftwareRenderer* renderer,
                                     const GbaPpuMemory* memory,
                                     const GbaPpuRegisters* registers,
                                     GbaPpuDirtyBits* dirty_bits, uint8_t x) {
  if (renderer->subpixels == NULL) {
    return;
  }

  uint8_t row = GBA_SCREEN_HEIGHT - registers->vcount - 1u;
  if (!registers->dispcnt.forced_blank) {
    GbaPpuSoftwareRendererDrawPixelImpl(
        renderer, memory, registers, dirty_bits, x,
        registers->internal.affine[0u].current,
        registers->internal.affine[1u].current,
        &renderer->subpixels[row * 3u * GBA_SCREEN_WIDTH + 3u * x]);
  } else {
    renderer->subpixels[row * 3u * GBA_SCREEN_WIDTH + 3u * x + 0u] = 0u;
    renderer->subpixels[row * 3u * GBA_SCREEN_WIDTH + 3u * x + 1u] = 0u;
    renderer->subpixels[row * 3u * GBA_SCREEN_WIDTH + 3u * x + 2u] = 0u;
  }
}

void GbaPpuSoftwareRendererFree(GbaPpuSoftwareRenderer* renderer) {
  free(renderer);
}