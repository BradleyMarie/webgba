#include "emulator/ppu/gba/software/render.h"

#include <stdlib.h>
#include <string.h>

#include "emulator/ppu/gba/software/bg_affine.h"
#include "emulator/ppu/gba/software/bg_bitmap.h"
#include "emulator/ppu/gba/software/bg_scrolling.h"
#include "emulator/ppu/gba/software/blend.h"
#include "emulator/ppu/gba/software/obj.h"
#include "emulator/ppu/gba/software/obj_visibility.h"
#include "emulator/ppu/gba/software/screen.h"
#include "emulator/ppu/gba/software/window.h"

struct _GbaPpuSoftwareRenderer {
  GbaPpuObjectVisibility object_visibility;
  GbaPpuScreen screen;
};

GbaPpuSoftwareRenderer* GbaPpuSoftwareRendererAllocate(
    const GbaPpuObjectAttributeMemory* memory) {
  GbaPpuSoftwareRenderer* result = calloc(1u, sizeof(GbaPpuSoftwareRenderer));

  if (result != NULL) {
    for (uint8_t i = 0; i < OAM_NUM_OBJECTS; i++) {
      GbaPpuObjectVisibilityDrawn(&result->object_visibility, memory, i);
    }
  }

  return result;
}

void GbaPpuSoftwareRendererDrawPixel(
    GbaPpuSoftwareRenderer* renderer, const GbaPpuMemory* memory,
    const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuDirtyBits* dirty_bits, uint8_t x, uint8_t y) {
  while (!GbaPpuSetEmpty(&dirty_bits->oam.objects)) {
    uint_fast8_t index = GbaPpuSetPop(&dirty_bits->oam.objects);
    GbaPpuObjectVisibilityHidden(&renderer->object_visibility, &memory->oam,
                                 index);
    GbaPpuObjectVisibilityDrawn(&renderer->object_visibility, &memory->oam,
                                index);
  }

  uint16_t color = 0u;
  if (!registers->dispcnt.forced_blank) {
    uint16_t obj_color;
    uint8_t obj_priority;
    bool object_on_pixel, obj_semi_transparent, on_obj_mask;
    if (registers->dispcnt.object_enable) {
      object_on_pixel = GbaPpuObjectPixel(
          memory, registers, &renderer->object_visibility, x, y, &obj_color,
          &obj_priority, &obj_semi_transparent, &on_obj_mask);
    } else {
      object_on_pixel = false;
      on_obj_mask = false;
    }

    bool draw_obj, draw_bg0, draw_bg1, draw_bg2, draw_bg3, enable_blending;
    GbaPpuWindowCheck(registers, x, y, on_obj_mask, &draw_obj, &draw_bg0,
                      &draw_bg1, &draw_bg2, &draw_bg3, &enable_blending);

    GbaPpuBlendUnit blend_unit;
    GbaPpuBlendUnitReset(&blend_unit);
    if (object_on_pixel && draw_obj) {
      GbaPpuBlendUnitAddObject(&blend_unit, registers, obj_color, obj_priority,
                               obj_semi_transparent);
    }

    bool success;
    switch (registers->dispcnt.mode) {
      case 0:
        if (draw_bg0 && registers->dispcnt.bg0_enable) {
          success = GbaPpuScrollingBackgroundPixel(
              memory, registers, GBA_PPU_SCROLLING_BACKGROUND_0, x, y, &color);
          if (success) {
            GbaPpuBlendUnitAddBackground0(&blend_unit, registers, color);
          }
        }

        if (draw_bg1 && registers->dispcnt.bg1_enable) {
          success = GbaPpuScrollingBackgroundPixel(
              memory, registers, GBA_PPU_SCROLLING_BACKGROUND_1, x, y, &color);
          if (success) {
            GbaPpuBlendUnitAddBackground1(&blend_unit, registers, color);
          }
        }

        if (draw_bg2 && registers->dispcnt.bg2_enable) {
          success = GbaPpuScrollingBackgroundPixel(
              memory, registers, GBA_PPU_SCROLLING_BACKGROUND_2, x, y, &color);
          if (success) {
            GbaPpuBlendUnitAddBackground2(&blend_unit, registers, color);
          }
        }

        if (draw_bg3 && registers->dispcnt.bg3_enable) {
          success = GbaPpuScrollingBackgroundPixel(
              memory, registers, GBA_PPU_SCROLLING_BACKGROUND_3, x, y, &color);
          if (success) {
            GbaPpuBlendUnitAddBackground3(&blend_unit, registers, color);
          }
        }
        break;
      case 1:
        if (draw_bg0 && registers->dispcnt.bg0_enable) {
          success = GbaPpuScrollingBackgroundPixel(
              memory, registers, GBA_PPU_SCROLLING_BACKGROUND_0, x, y, &color);
          if (success) {
            GbaPpuBlendUnitAddBackground0(&blend_unit, registers, color);
          }
        }

        if (draw_bg1 && registers->dispcnt.bg1_enable) {
          success = GbaPpuScrollingBackgroundPixel(
              memory, registers, GBA_PPU_SCROLLING_BACKGROUND_1, x, y, &color);
          if (success) {
            GbaPpuBlendUnitAddBackground1(&blend_unit, registers, color);
          }
        }

        if (draw_bg2 && registers->dispcnt.bg2_enable) {
          success = GbaPpuAffineBackgroundPixel(
              memory, registers, internal_registers,
              GBA_PPU_AFFINE_BACKGROUND_2, x, y, &color);
          if (success) {
            GbaPpuBlendUnitAddBackground2(&blend_unit, registers, color);
          }
        }
        break;
      case 2:
        if (draw_bg2 && registers->dispcnt.bg2_enable) {
          success = GbaPpuAffineBackgroundPixel(
              memory, registers, internal_registers,
              GBA_PPU_AFFINE_BACKGROUND_2, x, y, &color);
          if (success) {
            GbaPpuBlendUnitAddBackground2(&blend_unit, registers, color);
          }
        }

        if (draw_bg3 && registers->dispcnt.bg3_enable) {
          success = GbaPpuAffineBackgroundPixel(
              memory, registers, internal_registers,
              GBA_PPU_AFFINE_BACKGROUND_3, x, y, &color);
          if (success) {
            GbaPpuBlendUnitAddBackground3(&blend_unit, registers, color);
          }
        }
        break;
      case 3:
        if (draw_bg2 && registers->dispcnt.bg2_enable) {
          success = GbaPpuBitmapMode3Pixel(memory, registers,
                                           internal_registers, x, y, &color);
          if (success) {
            GbaPpuBlendUnitAddBackground2(&blend_unit, registers, color);
          }
        }
        break;
      case 4:
        if (draw_bg2 && registers->dispcnt.bg2_enable) {
          success = GbaPpuBitmapMode4Pixel(memory, registers,
                                           internal_registers, x, y, &color);
          if (success) {
            GbaPpuBlendUnitAddBackground2(&blend_unit, registers, color);
          }
        }
        break;
      case 5:
        if (draw_bg2 && registers->dispcnt.bg2_enable) {
          success = GbaPpuBitmapMode5Pixel(memory, registers,
                                           internal_registers, x, y, &color);
          if (success) {
            GbaPpuBlendUnitAddBackground2(&blend_unit, registers, color);
          }
        }
        break;
    }

    GbaPpuBlendUnitAddBackdrop(&blend_unit, registers,
                               memory->palette.bg.large_palette[0u]);

    if (enable_blending) {
      color = GbaPpuBlendUnitBlend(&blend_unit, registers);
    } else {
      color = GbaPpuBlendUnitNoBlend(&blend_unit);
    }
  }

  GbaPpuScreenSet(&renderer->screen, x, y, color);
}

void GbaPpuSoftwareRendererPresent(GbaPpuSoftwareRenderer* renderer, GLuint fbo,
                                   GLsizei width, GLsizei height) {
  GbaPpuScreenRenderToFbo(&renderer->screen, fbo, width, height);
}

void GbaPpuSoftwareRendererReloadContext(GbaPpuSoftwareRenderer* renderer) {
  GbaPpuScreenReloadContext(&renderer->screen);
}

void GbaPpuSoftwareRendererFree(GbaPpuSoftwareRenderer* renderer) {
  GbaPpuScreenDestroy(&renderer->screen);
  free(renderer);
}