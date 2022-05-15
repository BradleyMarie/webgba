#include "emulator/ppu/gba/opengl/mosaic.h"

#include <assert.h>

void OpenGlBgMosaicReload(OpenGlBgMosaic* context,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits) {
  if (!dirty_bits->composite.bg_mosaic) {
    return;
  }

  if (registers->bgcnt[0u].mosaic) {
    context->bg0[0u] = registers->mosaic.bg_horiz + 1;
    context->bg0[1u] = registers->mosaic.bg_vert + 1;
  } else {
    context->bg0[0u] = 1.0;
    context->bg0[1u] = 1.0;
  }

  if (registers->bgcnt[1u].mosaic) {
    context->bg1[0u] = registers->mosaic.bg_horiz + 1;
    context->bg1[1u] = registers->mosaic.bg_vert + 1;
  } else {
    context->bg1[0u] = 1.0;
    context->bg1[1u] = 1.0;
  }

  if (registers->bgcnt[2u].mosaic) {
    context->bg2[0u] = registers->mosaic.bg_horiz + 1;
    context->bg2[1u] = registers->mosaic.bg_vert + 1;
  } else {
    context->bg2[0u] = 1.0;
    context->bg2[1u] = 1.0;
  }

  if (registers->bgcnt[3u].mosaic) {
    context->bg3[0u] = registers->mosaic.bg_horiz + 1;
    context->bg3[1u] = registers->mosaic.bg_vert + 1;
  } else {
    context->bg3[0u] = 1.0;
    context->bg3[1u] = 1.0;
  }

  dirty_bits->composite.bg_mosaic = false;
}

void OpenGlBgMosaicBind(const OpenGlBgMosaic* context, GLuint program) {
  GLint bg0_mosaic = glGetUniformLocation(program, "bg0_mosaic");
  glUniform2f(bg0_mosaic, context->bg0[0u], context->bg0[1u]);

  GLint bg1_mosaic = glGetUniformLocation(program, "bg1_mosaic");
  glUniform2f(bg1_mosaic, context->bg1[0u], context->bg1[1u]);

  GLint bg2_mosaic = glGetUniformLocation(program, "bg2_mosaic");
  glUniform2f(bg2_mosaic, context->bg2[0u], context->bg2[1u]);

  GLint bg3_mosaic = glGetUniformLocation(program, "bg3_mosaic");
  glUniform2f(bg3_mosaic, context->bg3[0u], context->bg3[1u]);
}