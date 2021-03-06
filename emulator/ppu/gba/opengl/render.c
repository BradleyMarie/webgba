#include "emulator/ppu/gba/opengl/render.h"

#include <stdlib.h>
#include <string.h>

#include "emulator/ppu/gba/opengl/bg_affine.h"
#include "emulator/ppu/gba/opengl/bg_bitmap_mode3.h"
#include "emulator/ppu/gba/opengl/bg_bitmap_mode4.h"
#include "emulator/ppu/gba/opengl/bg_bitmap_mode5.h"
#include "emulator/ppu/gba/opengl/bg_control.h"
#include "emulator/ppu/gba/opengl/bg_scrolling.h"
#include "emulator/ppu/gba/opengl/blend.h"
#include "emulator/ppu/gba/opengl/objects.h"
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
  OpenGlObjects objects;
  OpenGlTiles tiles;
  OpenGlWindow window;
  uint8_t flush_start;
  bool flush_required;
  bool next_frame_flush_required;
  bool frame_flipped;
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
  result |= OpenGlWindowStage(&renderer->window, registers, dirty_bits);
  result |= OpenGlBgControlStage(&renderer->bg_control, registers, dirty_bits);
  result |= OpenGlBlendStage(&renderer->blend, registers, dirty_bits);
  result |= OpenGlBgAffineStage(&renderer->affine, registers, dirty_bits);
  result |=
      OpenGlObjectsStage(&renderer->objects, memory, registers, dirty_bits);
  result |=
      OpenGlBgScrollingStage(&renderer->bg_scrolling, registers, dirty_bits);
  result |= OpenGlBgBitmapMode3Stage(&renderer->bg_bitmap_mode3, memory,
                                     registers, dirty_bits);
  result |= OpenGlBgBitmapMode4Stage(&renderer->bg_bitmap_mode4, memory,
                                     registers, dirty_bits);
  result |= OpenGlBgBitmapMode5Stage(&renderer->bg_bitmap_mode5, memory,
                                     registers, dirty_bits);
  result |= OpenGlPaletteStage(&renderer->palette, dirty_bits);
  result |=
      OpenGlTilemapStage(&renderer->tilemap, memory, registers, dirty_bits);
  result |= OpenGlTilesStage(&renderer->tiles, memory, registers, dirty_bits);

  return result;
}

static void GbaPpuOpenGlRendererRender(GbaPpuOpenGlRenderer* renderer,
                                       GLuint framebuffer, GLint start,
                                       GLint end) {
  assert(start != end);

  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

  glViewport(0u, 0u, GBA_SCREEN_WIDTH * renderer->render_scale,
             GBA_SCREEN_HEIGHT * renderer->render_scale);

  GLuint program;
  const UniformLocations* locations;
  OpenGlProgramsGet(&renderer->programs, &program, &locations);
  if (!program) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
  } else {
    glUseProgram(program);

    OpenGlWindowBind(&renderer->window, locations);
    OpenGlBgControlBind(&renderer->bg_control, locations);
    OpenGlBlendBind(&renderer->blend, locations);
    OpenGlBgAffineBind(&renderer->affine, locations);
    OpenGlBgScrollingBind(&renderer->bg_scrolling, locations);
    OpenGlObjectsBind(&renderer->objects, locations);
    OpenGlBgBitmapMode3Bind(&renderer->bg_bitmap_mode3, locations);
    OpenGlBgBitmapMode4Bind(&renderer->bg_bitmap_mode4, locations);
    OpenGlBgBitmapMode5Bind(&renderer->bg_bitmap_mode5, locations);
    OpenGlPaletteBind(&renderer->palette, locations);
    OpenGlTilemapBind(&renderer->tilemap, locations);
    OpenGlTilesBind(&renderer->tiles, locations);

    glUniform1ui(locations->render_scale, renderer->render_scale);
    glUniform1i(locations->row_start, 80 - start);
    glUniform1i(locations->num_rows, end - start);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4u);
  }
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
  if (renderer->flush_start == 0u) {
    if (renderer->render_scale != render_scale) {
      renderer->flush_required = true;
    }

    renderer->render_scale = render_scale;
  } else {
    renderer->next_render_scale = render_scale;

    if (renderer->render_scale != renderer->next_render_scale) {
      renderer->flush_required = true;
      renderer->next_frame_flush_required = true;
    }
  }
}

void GbaPpuOpenGlRendererDrawRow(GbaPpuOpenGlRenderer* renderer,
                                 const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuDirtyBits* dirty_bits) {
  if (!renderer->initialized) {
    return;
  }

  bool staged_data =
      GbaPpuOpenGlRendererStage(renderer, memory, registers, dirty_bits);
  renderer->flush_required |= staged_data;
  renderer->next_frame_flush_required |= staged_data;

  if (registers->vcount != 0u && staged_data) {
    GLuint framebuffer = ScreenGetFrameBuffer(
        renderer->screen, GBA_SCREEN_WIDTH * renderer->render_scale,
        GBA_SCREEN_HEIGHT * renderer->render_scale, !renderer->frame_flipped);
    renderer->frame_flipped = true;

    GbaPpuOpenGlRendererRender(renderer, framebuffer, renderer->flush_start,
                               registers->vcount);
    renderer->flush_start = registers->vcount;
  } else if (registers->vcount == 0u) {
    renderer->flush_start = 0u;
  }

  if (staged_data) {
    OpenGlProgramsReload(&renderer->programs);
    OpenGlWindowReload(&renderer->window);
    OpenGlBgControlReload(&renderer->bg_control);
    OpenGlBlendReload(&renderer->blend);
    OpenGlBgAffineReload(&renderer->affine);
    OpenGlBgScrollingReload(&renderer->bg_scrolling);
    OpenGlObjectsReload(&renderer->objects);
    OpenGlBgBitmapMode3Reload(&renderer->bg_bitmap_mode3);
    OpenGlBgBitmapMode4Reload(&renderer->bg_bitmap_mode4, memory);
    OpenGlBgBitmapMode5Reload(&renderer->bg_bitmap_mode5);
    OpenGlPaletteReload(&renderer->palette, memory);
    OpenGlTilemapReload(&renderer->tilemap, memory);
    OpenGlTilesReload(&renderer->tiles, memory);
  }

  if (registers->vcount == GBA_SCREEN_HEIGHT - 1) {
    GLuint framebuffer = ScreenGetFrameBuffer(
        renderer->screen, GBA_SCREEN_WIDTH * renderer->render_scale,
        GBA_SCREEN_HEIGHT * renderer->render_scale,
        !renderer->frame_flipped && renderer->flush_required);

    if (renderer->flush_required || framebuffer == 0u) {
      GbaPpuOpenGlRendererRender(renderer, framebuffer, renderer->flush_start,
                                 GBA_SCREEN_HEIGHT);
    }

    renderer->render_scale = renderer->next_render_scale;
    renderer->flush_required = renderer->next_frame_flush_required;
    renderer->next_frame_flush_required = false;
    renderer->frame_flipped = false;
  }
}

void GbaPpuOpenGlRendererReloadContext(GbaPpuOpenGlRenderer* renderer) {
  OpenGlProgramsReloadContext(&renderer->programs);
  OpenGlBgBitmapMode3ReloadContext(&renderer->bg_bitmap_mode3);
  OpenGlBgBitmapMode4ReloadContext(&renderer->bg_bitmap_mode4);
  OpenGlBgBitmapMode5ReloadContext(&renderer->bg_bitmap_mode5);
  OpenGlTilemapReloadContext(&renderer->tilemap);
  OpenGlTilesReloadContext(&renderer->tiles);
  OpenGlPaletteReloadContext(&renderer->palette);
  OpenGlObjectsReloadContext(&renderer->objects);

  renderer->initialized = true;
  renderer->flush_required = true;
  renderer->next_frame_flush_required = true;
}

void GbaPpuOpenGlRendererFree(GbaPpuOpenGlRenderer* renderer) {
  if (renderer->initialized) {
    OpenGlProgramsDestroy(&renderer->programs);
    OpenGlBgBitmapMode3Destroy(&renderer->bg_bitmap_mode3);
    OpenGlBgBitmapMode4Destroy(&renderer->bg_bitmap_mode4);
    OpenGlBgBitmapMode5Destroy(&renderer->bg_bitmap_mode5);
    OpenGlTilemapDestroy(&renderer->tilemap);
    OpenGlTilesDestroy(&renderer->tiles);
    OpenGlPaletteDestroy(&renderer->palette);
    OpenGlObjectsDestroy(&renderer->objects);
  }

  free(renderer);
}