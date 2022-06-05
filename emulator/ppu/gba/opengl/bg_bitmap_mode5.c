#include "emulator/ppu/gba/opengl/bg_bitmap_mode5.h"

#include <assert.h>
#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

void OpenGlBgBitmapMode5Reload(OpenGlBgBitmapMode5* context,
                               const GbaPpuMemory* memory,
                               const GbaPpuRegisters* registers,
                               GbaPpuDirtyBits* dirty_bits) {
  if (!registers->dispcnt.bg2_enable || registers->dispcnt.mode != 5) {
    context->enabled = false;
    return;
  }

  context->enabled = true;
  context->page = registers->dispcnt.page_select;

  if (!dirty_bits->vram.mode_5.pages[registers->dispcnt.page_select]) {
    return;
  }

  for (uint_fast8_t y = 0; y < GBA_REDUCED_FRAME_HEIGHT; y++) {
    for (uint_fast8_t x = 0; x < GBA_REDUCED_FRAME_WIDTH; x++) {
      uint16_t color =
          memory->vram.mode_5.bg.pages[registers->dispcnt.page_select]
              .pixels[y][x];
      context->staging[y * GBA_REDUCED_FRAME_WIDTH + x] = (color << 1u) | 1u;
    }
  }

  glBindTexture(GL_TEXTURE_2D,
                context->textures[registers->dispcnt.page_select]);
  glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                  /*width=*/GBA_REDUCED_FRAME_WIDTH,
                  /*height=*/GBA_REDUCED_FRAME_HEIGHT,
                  /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                  /*pixels=*/context->staging);
  glBindTexture(GL_TEXTURE_2D, 0);

  dirty_bits->vram.mode_5.pages[registers->dispcnt.page_select] = false;
}

void OpenGlBgBitmapMode5Bind(const OpenGlBgBitmapMode5* context,
                             GLuint program) {
  if (!context->enabled) {
    return;
  }

  GLint bg_mode5 = glGetUniformLocation(program, "bitmap");
  glUniform1i(bg_mode5, BITMAP_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BITMAP_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->textures[context->page]);
}

void OpenGlBgBitmapMode5ReloadContext(OpenGlBgBitmapMode5* context) {
  glGenTextures(2u, context->textures);
  for (uint8_t i = 0u; i < 2u; i++) {
    glBindTexture(GL_TEXTURE_2D, context->textures[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
                 /*width=*/GBA_REDUCED_FRAME_WIDTH,
                 /*height=*/GBA_REDUCED_FRAME_HEIGHT,
                 /*border=*/0, /*format=*/GL_RGBA,
                 /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                 /*pixels=*/context->staging);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlBgBitmapMode5Destroy(OpenGlBgBitmapMode5* context) {
  glDeleteTextures(2u, context->textures);
}