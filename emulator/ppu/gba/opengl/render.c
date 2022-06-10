#include "emulator/ppu/gba/opengl/render.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "emulator/ppu/gba/draw_manager.h"
#include "emulator/ppu/gba/opengl/bg_affine.h"
#include "emulator/ppu/gba/opengl/bg_bitmap_mode3.h"
#include "emulator/ppu/gba/opengl/bg_bitmap_mode4.h"
#include "emulator/ppu/gba/opengl/bg_bitmap_mode5.h"
#include "emulator/ppu/gba/opengl/bg_control.h"
#include "emulator/ppu/gba/opengl/bg_scrolling.h"
#include "emulator/ppu/gba/opengl/blend.h"
#include "emulator/ppu/gba/opengl/control.h"
#include "emulator/ppu/gba/opengl/obj_attributes.h"
#include "emulator/ppu/gba/opengl/palette.h"
#include "emulator/ppu/gba/opengl/programs.h"
#include "emulator/ppu/gba/opengl/tilemap.h"
#include "emulator/ppu/gba/opengl/tiles.h"
#include "emulator/ppu/gba/opengl/window.h"

#define NUM_LAYERS (GBA_PPU_NUM_BACKGROUNDS + 1u)

struct _GbaPpuOpenGlRenderer {
  GbaPpuDrawManager draw_manager;
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
  OpenGlControl control;
  OpenGlWindow window;
  uint8_t flush_start_row;
  uint8_t flush_size;
  uint8_t next_render_scale;
  uint8_t render_scale;
  Screen* screen;
  bool initialized;
};

static void GbaPpuOpenGlRendererDraw(const GbaPpuOpenGlRenderer* renderer,
                                     GLuint fbo) {
  if (renderer->control.blank || renderer->control.mode > 5) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }

  GLuint program = OpenGlProgramsGet(
      &renderer->programs, renderer->control.mode,
      renderer->control.obj_enabled, renderer->control.bg_enabled[0u],
      renderer->control.bg_enabled[1u], renderer->control.bg_enabled[2u],
      renderer->control.bg_enabled[3u]);
  glUseProgram(program);

  OpenGlBgPaletteBind(&renderer->bg_palette, program);
  OpenGlWindowBind(&renderer->window, program);
  OpenGlBgAffineBind(&renderer->affine, program);
  OpenGlBgBitmapMode3Bind(&renderer->bg_bitmap_mode3, program);
  OpenGlBgBitmapMode4Bind(&renderer->bg_bitmap_mode4, program);
  OpenGlBgBitmapMode5Bind(&renderer->bg_bitmap_mode5, program);
  OpenGlBgControlBind(&renderer->bg_control, program);
  OpenGlBgScrollingBind(&renderer->bg_scrolling, program);
  OpenGlBgTilemapBind(&renderer->bg_tilemap, program);
  OpenGlBgObjectAttributesBind(&renderer->obj_attributes, program);
  OpenGlTilesBind(&renderer->tiles, program);
  OpenGlBlendBind(&renderer->blend, program);

  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glDrawArrays(GL_TRIANGLES, 0, 3u);
}

static void GbaPpuOpenGlRendererFlush(const GbaPpuOpenGlRenderer* renderer) {
  glEnable(GL_SCISSOR_TEST);
  glScissor(0u, renderer->flush_start_row * renderer->render_scale,
            GBA_SCREEN_WIDTH, renderer->flush_size * renderer->render_scale);
  glViewport(0u, 0u, GBA_SCREEN_WIDTH * renderer->render_scale,
             GBA_SCREEN_HEIGHT * renderer->render_scale);
  GLuint framebuffer = ScreenGetRenderBuffer(
      renderer->screen, GBA_SCREEN_WIDTH * renderer->render_scale,
      GBA_SCREEN_HEIGHT * renderer->render_scale);
  GbaPpuOpenGlRendererDraw(renderer, framebuffer);
  glDisable(GL_SCISSOR_TEST);
}

static void GbaPpuOpenGlRendererReload(GbaPpuOpenGlRenderer* renderer,
                                       const GbaPpuMemory* memory,
                                       const GbaPpuRegisters* registers,
                                       GbaPpuDirtyBits* dirty_bits) {
  OpenGlBgAffineReload(&renderer->affine, registers, dirty_bits);
  OpenGlControlReload(&renderer->control, registers, dirty_bits);

  if (registers->dispcnt.forced_blank) {
    return;
  }

  OpenGlBgControlReload(&renderer->bg_control, registers, dirty_bits);
  OpenGlBgPaletteReload(&renderer->bg_palette, memory, dirty_bits);
  OpenGlWindowReload(&renderer->window, registers, dirty_bits);
  OpenGlBlendReload(&renderer->blend, registers, dirty_bits);
  OpenGlTilesReload(&renderer->tiles, memory, registers, dirty_bits);
  OpenGlObjectAttributesReload(&renderer->obj_attributes, memory, registers,
                               dirty_bits);

  switch (registers->dispcnt.mode) {
    case 0u:
      OpenGlBgScrollingReload(&renderer->bg_scrolling, registers, dirty_bits);
      OpenGlBgTilemapReload(&renderer->bg_tilemap, memory, dirty_bits);
      break;
    case 1u:
      OpenGlBgScrollingReload(&renderer->bg_scrolling, registers, dirty_bits);
      OpenGlBgTilemapReload(&renderer->bg_tilemap, memory, dirty_bits);
      break;
    case 2u:
      OpenGlBgScrollingReload(&renderer->bg_scrolling, registers, dirty_bits);
      OpenGlBgTilemapReload(&renderer->bg_tilemap, memory, dirty_bits);
      break;
    case 3u:
      OpenGlBgBitmapMode3Reload(&renderer->bg_bitmap_mode3, memory, registers,
                                dirty_bits);
      break;
    case 4u:
      OpenGlBgBitmapMode4Reload(&renderer->bg_bitmap_mode4, memory, registers,
                                dirty_bits);
      break;
    case 5u:
      OpenGlBgBitmapMode5Reload(&renderer->bg_bitmap_mode5, memory, registers,
                                dirty_bits);
      break;
  }
}

GbaPpuOpenGlRenderer* GbaPpuOpenGlRendererAllocate() {
  GbaPpuOpenGlRenderer* renderer = calloc(1u, sizeof(GbaPpuOpenGlRenderer));

  if (renderer != NULL) {
    GbaPpuDrawManagerInitialize(&renderer->draw_manager);
    renderer->next_render_scale = 1u;
    renderer->render_scale = 1u;
    renderer->flush_start_row = 0u;
    renderer->flush_size = 0u;
  }

  return renderer;
}

void GbaPpuOpenGlRendererSetScreen(GbaPpuOpenGlRenderer* renderer,
                                   Screen* screen) {
  renderer->screen = screen;
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

  if (registers->vcount == 0u &&
      renderer->render_scale != renderer->next_render_scale) {
    renderer->render_scale = renderer->next_render_scale;
    GbaPpuDrawManagerInvalidatePreviousFrame(&renderer->draw_manager);
  }

  if (registers->vcount == 0u) {
    GbaPpuDrawManagerStartFrame(&renderer->draw_manager, registers, dirty_bits);
    GbaPpuOpenGlRendererReload(renderer, memory, registers, dirty_bits);
    renderer->flush_start_row = 0u;
    renderer->flush_size = 1u;
    return;
  }

  if (registers->vcount == GBA_SCREEN_HEIGHT) {
    GbaPpuDrawManagerEndFrame(&renderer->draw_manager);
    GbaPpuOpenGlRendererFlush(renderer);
    GbaPpuOpenGlRendererReload(renderer, memory, registers, dirty_bits);
    return;
  }

  bool should_flush = GbaPpuDrawManagerShouldFlush(&renderer->draw_manager,
                                                   registers, dirty_bits);

  if (!should_flush) {
    OpenGlBgAffineReload(&renderer->affine, registers, dirty_bits);
    renderer->flush_size += 1u;
    return;
  }

  GbaPpuOpenGlRendererFlush(renderer);
  GbaPpuOpenGlRendererReload(renderer, memory, registers, dirty_bits);
  renderer->flush_start_row = registers->vcount;
  renderer->flush_size = 1u;
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

void GbaPpuOpenGlRendererSetScale(GbaPpuOpenGlRenderer* renderer,
                                  uint8_t render_scale) {
  renderer->next_render_scale = render_scale;
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