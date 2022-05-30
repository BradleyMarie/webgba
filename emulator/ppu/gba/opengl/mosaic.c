#include "emulator/ppu/gba/opengl/mosaic.h"

#include <stdio.h>

void OpenGlBgMosaicReload(OpenGlBgMosaic* context,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits) {
  if (!dirty_bits->composite.bg_mosaic) {
    return;
  }

  for (uint8_t i = 0; i < GBA_PPU_NUM_BACKGROUNDS; i++) {
    if (registers->bgcnt[0u].mosaic) {
      context->mosaic[i][0u] = registers->mosaic.bg_horiz + 1;
      context->mosaic[i][1u] = registers->mosaic.bg_vert + 1;
    } else {
      context->mosaic[i][0u] = 1;
      context->mosaic[i][1u] = 1;
    }
  }

  dirty_bits->composite.bg_mosaic = false;
}

void OpenGlBgMosaicBind(const OpenGlBgMosaic* context, GLuint program) {
  for (uint8_t i = 0; i < GBA_PPU_NUM_BACKGROUNDS; i++) {
    char variable_name[100u];
    sprintf(variable_name, "bg_mosaic[%u]", i);
    GLint location = glGetUniformLocation(program, variable_name);
    glUniform2i(location, context->mosaic[i][0u], context->mosaic[i][1u]);
  }
}