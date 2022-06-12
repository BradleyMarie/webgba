#include "emulator/ppu/gba/opengl/render.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "emulator/ppu/gba/opengl/bg_affine.h"
#include "emulator/ppu/gba/opengl/bg_bitmap_mode3.h"
#include "emulator/ppu/gba/opengl/bg_bitmap_mode4.h"
#include "emulator/ppu/gba/opengl/bg_bitmap_mode5.h"
#include "emulator/ppu/gba/opengl/bg_control.h"
#include "emulator/ppu/gba/opengl/bg_scrolling.h"
#include "emulator/ppu/gba/opengl/blend.h"
#include "emulator/ppu/gba/opengl/obj_attributes.h"
#include "emulator/ppu/gba/opengl/palette.h"
#include "emulator/ppu/gba/opengl/programs.h"
#include "emulator/ppu/gba/opengl/tilemap.h"
#include "emulator/ppu/gba/opengl/tiles.h"
#include "emulator/ppu/gba/opengl/window.h"

#define NUM_LAYERS (GBA_PPU_NUM_BACKGROUNDS + 1u)

struct _GbaPpuOpenGlRenderer {
  OpenGlPrograms programs;
  OpenGlBgAffine affine;
  OpenGlBgBitmapMode3 bg_bitmap_mode3;
  OpenGlBgBitmapMode4 bg_bitmap_mode4;
  OpenGlBgBitmapMode5 bg_bitmap_mode5;
  OpenGlBgControl bg_control;
  OpenGlBgScrolling bg_scrolling;
  OpenGlBlend blend;
  OpenGlBgPalette bg_palette;
  OpenGlBgTilemap bg_tilemap;
  OpenGlObjectAttributes obj_attributes;
  OpenGlTiles tiles;
  OpenGlWindow window;
  uint8_t flush_start;
  bool flush_required;
  uint8_t next_render_scale;
  uint8_t render_scale;
  Screen* screen;
  bool initialized;
};

static bool GbaPpuOpenGlRendererStage(GbaPpuOpenGlRenderer* renderer,
                                      const GbaPpuMemory* memory,
                                      const GbaPpuRegisters* registers,
                                      GbaPpuDirtyBits* dirty_bits) {
  if (registers->dispcnt.forced_blank || registers->dispcnt.mode > 5u) {
    return false;
  }

  bool result = OpenGlProgramsStage(&renderer->programs, registers, dirty_bits);
  result |= OpenGlBgAffineStage(&renderer->affine, registers, dirty_bits);
  result |= OpenGlBgBitmapMode3Stage(&renderer->bg_bitmap_mode3, memory,
                                     registers, dirty_bits);
  result |= OpenGlBgBitmapMode4Stage(&renderer->bg_bitmap_mode4, memory,
                                     registers, dirty_bits);
  result |= OpenGlBgBitmapMode5Stage(&renderer->bg_bitmap_mode5, memory,
                                     registers, dirty_bits);
  result |= OpenGlBgControlStage(&renderer->bg_control, registers, dirty_bits);
  result |=
      OpenGlBgScrollingStage(&renderer->bg_scrolling, registers, dirty_bits);
  result |= OpenGlBlendStage(&renderer->blend, registers, dirty_bits);
  result |= OpenGlBgPaletteStage(&renderer->bg_palette, memory, dirty_bits);
  result |= OpenGlBgTilemapStage(&renderer->bg_tilemap, memory, registers,
                                 dirty_bits);
  result |= OpenGlObjectAttributesStage(&renderer->obj_attributes, memory,
                                        registers, dirty_bits);
  result |= OpenGlTilesStage(&renderer->tiles, memory, registers, dirty_bits);
  result |= OpenGlWindowStage(&renderer->window, registers, dirty_bits);

  return result;
}

GbaPpuOpenGlRenderer* GbaPpuOpenGlRendererAllocate() {
  GbaPpuOpenGlRenderer* renderer = calloc(1u, sizeof(GbaPpuOpenGlRenderer));

  if (renderer != NULL) {
    renderer->next_render_scale = 1u;
    renderer->render_scale = 1u;
    renderer->flush_start = 0u;
  }

  return renderer;
}

void GbaPpuOpenGlRendererSetScreen(GbaPpuOpenGlRenderer* renderer,
                                   Screen* screen) {
  renderer->screen = screen;
}

void GbaPpuOpenGlRendererSetScale(GbaPpuOpenGlRenderer* renderer,
                                  uint8_t render_scale) {
  renderer->next_render_scale = render_scale;
}

void GbaPpuOpenGlRendererDrawRow(GbaPpuOpenGlRenderer* renderer,
                                 const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuDirtyBits* dirty_bits) {
  if (!renderer->initialized) {
    return;
  }

  // Hack to workaround missing dirty bits
  if (registers->vcount == 0u) {
    GbaPpuDirtyBitsAllDirty(dirty_bits);
  }

  bool staged_data =
      GbaPpuOpenGlRendererStage(renderer, memory, registers, dirty_bits);
  renderer->flush_required |= staged_data;

  if (registers->vcount != 0u) {
    GLuint framebuffer = ScreenGetRenderBuffer(
        renderer->screen, GBA_SCREEN_WIDTH * renderer->render_scale,
        GBA_SCREEN_HEIGHT * renderer->render_scale);

    if (!staged_data && (registers->vcount != GBA_SCREEN_HEIGHT - 1 ||
                         (!renderer->flush_required && framebuffer != 0))) {
      return;
    }

    glEnable(GL_SCISSOR_TEST);

    glScissor(0u, renderer->flush_start * renderer->render_scale,
              GBA_SCREEN_WIDTH * renderer->render_scale,
              (registers->vcount - renderer->flush_start + 1u) *
                  renderer->render_scale);

    glViewport(0u, 0u, GBA_SCREEN_WIDTH * renderer->render_scale,
               GBA_SCREEN_HEIGHT * renderer->render_scale);

    if (renderer->programs.blank) {
      glClearColor(0.0, 0.0, 0.0, 1.0);
      glClear(GL_COLOR_BUFFER_BIT);
    } else {
      GLuint program = OpenGlProgramsGet(&renderer->programs);
      glUseProgram(program);

      OpenGlBgAffineBind(&renderer->affine, program);
      OpenGlBgBitmapMode3Bind(&renderer->bg_bitmap_mode3, program);
      OpenGlBgBitmapMode4Bind(&renderer->bg_bitmap_mode4, program);
      OpenGlBgBitmapMode5Bind(&renderer->bg_bitmap_mode5, program);
      OpenGlBgControlBind(&renderer->bg_control, program);
      OpenGlBgScrollingBind(&renderer->bg_scrolling, program);
      OpenGlBlendBind(&renderer->blend, program);
      OpenGlBgPaletteBind(&renderer->bg_palette, program);
      OpenGlBgTilemapBind(&renderer->bg_tilemap, program);
      OpenGlObjectAttributesBind(&renderer->obj_attributes, program);
      OpenGlTilesBind(&renderer->tiles, program);
      OpenGlWindowBind(&renderer->window, program);

      glDrawArrays(GL_TRIANGLES, 0, 3u);
    }

    glDisable(GL_SCISSOR_TEST);

    if (registers->vcount == GBA_SCREEN_HEIGHT - 1) {
      renderer->flush_required = false;
    }
  }

  OpenGlProgramsReload(&renderer->programs);
  OpenGlBgAffineReload(&renderer->affine);
  OpenGlBgBitmapMode3Reload(&renderer->bg_bitmap_mode3);
  OpenGlBgBitmapMode4Reload(&renderer->bg_bitmap_mode4);
  OpenGlBgBitmapMode5Reload(&renderer->bg_bitmap_mode5);
  OpenGlBgControlReload(&renderer->bg_control);
  OpenGlBgScrollingReload(&renderer->bg_scrolling);
  OpenGlBlendReload(&renderer->blend);
  OpenGlBgPaletteReload(&renderer->bg_palette);
  OpenGlBgTilemapReload(&renderer->bg_tilemap);
  OpenGlObjectAttributesReload(&renderer->obj_attributes);
  OpenGlTilesReload(&renderer->tiles);
  OpenGlWindowReload(&renderer->window);

  renderer->flush_start = registers->vcount;
}

void GbaPpuOpenGlRendererReloadContext(GbaPpuOpenGlRenderer* renderer) {
  OpenGlProgramsReloadContext(&renderer->programs);
  OpenGlBgAffineReloadContext(&renderer->affine);
  OpenGlWindowReloadContext(&renderer->window);
  OpenGlBgControlReloadContext(&renderer->bg_control);
  OpenGlBgBitmapMode3ReloadContext(&renderer->bg_bitmap_mode3);
  OpenGlBgBitmapMode4ReloadContext(&renderer->bg_bitmap_mode4);
  OpenGlBgBitmapMode5ReloadContext(&renderer->bg_bitmap_mode5);
  OpenGlBgScrollingReloadContext(&renderer->bg_scrolling);
  OpenGlBgTilemapReloadContext(&renderer->bg_tilemap);
  OpenGlTilesReloadContext(&renderer->tiles);
  OpenGlBgPaletteReloadContext(&renderer->bg_palette);
  OpenGlObjectAttributesReloadContext(&renderer->obj_attributes);
  OpenGlBlendReloadContext(&renderer->blend);

  renderer->initialized = true;
}

void GbaPpuOpenGlRendererFree(GbaPpuOpenGlRenderer* renderer) {
  if (renderer->initialized) {
    OpenGlProgramsDestroy(&renderer->programs);
    OpenGlBgAffineDestroy(&renderer->affine);
    OpenGlWindowDestroy(&renderer->window);
    OpenGlBgControlDestroy(&renderer->bg_control);
    OpenGlBgBitmapMode3Destroy(&renderer->bg_bitmap_mode3);
    OpenGlBgBitmapMode4Destroy(&renderer->bg_bitmap_mode4);
    OpenGlBgBitmapMode5Destroy(&renderer->bg_bitmap_mode5);
    OpenGlBgScrollingDestroy(&renderer->bg_scrolling);
    OpenGlBgTilemapDestroy(&renderer->bg_tilemap);
    OpenGlTilesDestroy(&renderer->tiles);
    OpenGlBgPaletteDestroy(&renderer->bg_palette);
    OpenGlObjectAttributesDestroy(&renderer->obj_attributes);
    OpenGlBlendDestroy(&renderer->blend);
  }

  free(renderer);
}