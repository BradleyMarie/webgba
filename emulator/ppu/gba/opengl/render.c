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
  OpenGlPalette palette;
  OpenGlTilemap tilemap;
  OpenGlObjectAttributes obj_attributes;
  OpenGlTiles tiles;
  OpenGlWindow window;
  uint8_t flush_start;
  bool flush_required;
  bool next_frame_flush_required;
  uint8_t next_render_scale;
  uint8_t render_scale;
  Screen* screen;
  bool initialized;
};

static bool GbaPpuOpenGlRendererLoad(GbaPpuOpenGlRenderer* renderer,
                                     const GbaPpuMemory* memory,
                                     const GbaPpuRegisters* registers,
                                     GbaPpuDirtyBits* dirty_bits) {
  if (registers->dispcnt.forced_blank || registers->dispcnt.mode > 5u) {
    return false;
  }

  bool result = OpenGlBgAffineLoad(&renderer->affine, registers, dirty_bits);
  result |=
      OpenGlBgScrollingLoad(&renderer->bg_scrolling, registers, dirty_bits);
  result |= OpenGlBlendLoad(&renderer->blend, registers, dirty_bits);
  result |= OpenGlWindowLoad(&renderer->window, registers, dirty_bits);
  return result;
}

static bool GbaPpuOpenGlRendererStage(GbaPpuOpenGlRenderer* renderer,
                                      const GbaPpuMemory* memory,
                                      const GbaPpuRegisters* registers,
                                      GbaPpuDirtyBits* dirty_bits) {
  if (registers->dispcnt.forced_blank || registers->dispcnt.mode > 5u) {
    return false;
  }

  bool result = OpenGlProgramsStage(&renderer->programs, registers, dirty_bits);
  result |= OpenGlBgBitmapMode3Stage(&renderer->bg_bitmap_mode3, memory,
                                     registers, dirty_bits);
  result |= OpenGlBgBitmapMode4Stage(&renderer->bg_bitmap_mode4, memory,
                                     registers, dirty_bits);
  result |= OpenGlBgBitmapMode5Stage(&renderer->bg_bitmap_mode5, memory,
                                     registers, dirty_bits);
  result |= OpenGlBgControlStage(&renderer->bg_control, registers, dirty_bits);
  result |= OpenGlPaletteStage(&renderer->palette, dirty_bits);
  result |=
      OpenGlTilemapStage(&renderer->tilemap, memory, registers, dirty_bits);
  result |= OpenGlObjectAttributesStage(&renderer->obj_attributes, memory,
                                        registers, dirty_bits);
  result |= OpenGlTilesStage(&renderer->tiles, memory, registers, dirty_bits);

  return result;
}

static void GbaPpuOpenGlRendererRender(GbaPpuOpenGlRenderer* renderer,
                                       GLuint framebuffer, GLint start,
                                       GLint end) {
  assert(start != end);

  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

  glEnable(GL_SCISSOR_TEST);

  GLint gl_start_row = GBA_SCREEN_HEIGHT - end;
  GLint num_rows = end - start;
  glScissor(0u, gl_start_row * renderer->render_scale,
            GBA_SCREEN_WIDTH * renderer->render_scale,
            num_rows * renderer->render_scale);

  glViewport(0u, 0u, GBA_SCREEN_WIDTH * renderer->render_scale,
             GBA_SCREEN_HEIGHT * renderer->render_scale);

  if (renderer->programs.blank) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
  } else {
    GLuint program = OpenGlProgramsGet(&renderer->programs);
    glUseProgram(program);

    OpenGlBgAffineBind(&renderer->affine, start, end, program);
    OpenGlBgScrollingBind(&renderer->bg_scrolling, start, end, program);
    OpenGlBlendBind(&renderer->blend, start, end, program);
    OpenGlWindowBind(&renderer->window, start, end, program);

    OpenGlBgBitmapMode3Bind(&renderer->bg_bitmap_mode3, program);
    OpenGlBgBitmapMode4Bind(&renderer->bg_bitmap_mode4, program);
    OpenGlBgBitmapMode5Bind(&renderer->bg_bitmap_mode5, program);
    OpenGlBgControlBind(&renderer->bg_control, program);
    OpenGlPaletteBind(&renderer->palette, program);
    OpenGlTilemapBind(&renderer->tilemap, program);
    OpenGlObjectAttributesBind(&renderer->obj_attributes, program);
    OpenGlTilesBind(&renderer->tiles, program);

    GLint render_scale = glGetUniformLocation(program, "render_scale");
    glUniform1f(render_scale, renderer->render_scale);

    glDrawArrays(GL_TRIANGLES, 0, 3u);
  }

  glDisable(GL_SCISSOR_TEST);
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
  renderer->flush_required = true;
  renderer->next_frame_flush_required = true;
}

void GbaPpuOpenGlRendererSetScale(GbaPpuOpenGlRenderer* renderer,
                                  uint8_t render_scale) {
  renderer->next_render_scale = render_scale;
  renderer->flush_required = true;
  renderer->next_frame_flush_required = true;
}

void GbaPpuOpenGlRendererDrawRow(GbaPpuOpenGlRenderer* renderer,
                                 const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuDirtyBits* dirty_bits) {
  if (!renderer->initialized) {
    return;
  }

  bool loaded_data =
      GbaPpuOpenGlRendererLoad(renderer, memory, registers, dirty_bits);
  renderer->flush_required |= loaded_data;
  renderer->next_frame_flush_required |= loaded_data;

  bool staged_data =
      GbaPpuOpenGlRendererStage(renderer, memory, registers, dirty_bits);
  renderer->flush_required |= staged_data;
  renderer->next_frame_flush_required |= staged_data;

  if (registers->vcount != 0u && staged_data) {
    GLuint framebuffer = ScreenGetRenderBuffer(
        renderer->screen, GBA_SCREEN_WIDTH * renderer->render_scale,
        GBA_SCREEN_HEIGHT * renderer->render_scale);

    GbaPpuOpenGlRendererRender(renderer, framebuffer, renderer->flush_start,
                               registers->vcount);
    renderer->flush_start = registers->vcount;
  } else if (registers->vcount == 0u) {
    renderer->flush_start = 0u;
  }

  if (staged_data) {
    OpenGlProgramsReload(&renderer->programs);
    OpenGlBgBitmapMode3Reload(&renderer->bg_bitmap_mode3);
    OpenGlBgBitmapMode4Reload(&renderer->bg_bitmap_mode4);
    OpenGlBgBitmapMode5Reload(&renderer->bg_bitmap_mode5);
    OpenGlBgControlReload(&renderer->bg_control);
    OpenGlPaletteReload(&renderer->palette, memory);
    OpenGlTilemapReload(&renderer->tilemap, memory);
    OpenGlObjectAttributesReload(&renderer->obj_attributes);
    OpenGlTilesReload(&renderer->tiles);
  }

  if (registers->vcount == GBA_SCREEN_HEIGHT - 1) {
    GLuint framebuffer = ScreenGetRenderBuffer(
        renderer->screen, GBA_SCREEN_WIDTH * renderer->render_scale,
        GBA_SCREEN_HEIGHT * renderer->render_scale);

    if (renderer->flush_required || framebuffer == 0u) {
      GbaPpuOpenGlRendererRender(renderer, framebuffer, renderer->flush_start,
                                 GBA_SCREEN_HEIGHT);
    }

    renderer->flush_required = renderer->next_frame_flush_required;
    renderer->next_frame_flush_required = false;
  }
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
  OpenGlTilemapReloadContext(&renderer->tilemap);
  OpenGlTilesReloadContext(&renderer->tiles);
  OpenGlPaletteReloadContext(&renderer->palette);
  OpenGlObjectAttributesReloadContext(&renderer->obj_attributes);
  OpenGlBlendReloadContext(&renderer->blend);

  renderer->initialized = true;
  renderer->flush_required = true;
  renderer->next_frame_flush_required = true;
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
    OpenGlTilemapDestroy(&renderer->tilemap);
    OpenGlTilesDestroy(&renderer->tiles);
    OpenGlPaletteDestroy(&renderer->palette);
    OpenGlObjectAttributesDestroy(&renderer->obj_attributes);
    OpenGlBlendDestroy(&renderer->blend);
  }

  free(renderer);
}