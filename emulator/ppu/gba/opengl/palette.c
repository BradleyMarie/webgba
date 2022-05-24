#include "emulator/ppu/gba/opengl/palette.h"

#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

void OpenGlBgPaletteReload(OpenGlBgPalette* context, const GbaPpuMemory* memory,
                           GbaPpuDirtyBits* dirty_bits) {
  if (dirty_bits->palette.bg_palette) {
    for (uint16_t i = 0; i < GBA_LARGE_PALETTE_SIZE; i++) {
      context->colors[i] = (memory->palette.bg.large_palette[i] << 1u) | 1u;
    }

    glBindTexture(GL_TEXTURE_2D, context->bg_palette);
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                    /*width=*/GBA_LARGE_PALETTE_SIZE, /*height=*/1u,
                    /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                    /*pixels=*/context->colors);

    dirty_bits->palette.bg_palette = false;
  }

  if (dirty_bits->palette.obj_palette) {
    for (uint16_t i = 0; i < GBA_LARGE_PALETTE_SIZE; i++) {
      context->colors[i] = (memory->palette.obj.large_palette[i] << 1u) | 1u;
    }

    glBindTexture(GL_TEXTURE_2D, context->obj_large_palette);
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                    /*width=*/GBA_LARGE_PALETTE_SIZE, /*height=*/1u,
                    /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                    /*pixels=*/context->colors);
    glBindTexture(GL_TEXTURE_2D, context->obj_small_palette);
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                    /*width=*/GBA_SMALL_PALETTE_SIZE,
                    /*height=*/GBA_NUM_SMALL_PALETTES,
                    /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                    /*pixels=*/context->colors);
    glBindTexture(GL_TEXTURE_2D, 0);

    dirty_bits->palette.obj_palette = false;
  }
}

void OpenGlBgPaletteBind(const OpenGlBgPalette* context, GLuint program) {
  GLint bg_palette = glGetUniformLocation(program, "bg_palette");
  glUniform1i(bg_palette, BG_PALETTE_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BG_PALETTE_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->bg_palette);

  GLint obj_large_palette = glGetUniformLocation(program, "obj_large_palette");
  glUniform1i(obj_large_palette, OBJ_LARGE_PALETTE_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + OBJ_LARGE_PALETTE_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->obj_large_palette);

  GLint obj_small_palette = glGetUniformLocation(program, "obj_small_palette");
  glUniform1i(obj_small_palette, OBJ_SMALL_PALETTE_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + OBJ_SMALL_PALETTE_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->obj_small_palette);
}

void OpenGlBgPaletteReloadContext(OpenGlBgPalette* context) {
  glGenTextures(1u, &context->bg_palette);
  glBindTexture(GL_TEXTURE_2D, context->bg_palette);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
               /*width=*/GBA_LARGE_PALETTE_SIZE,
               /*height=*/1u,
               /*border=*/0, /*format=*/GL_RGBA,
               /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
               /*pixels=*/NULL);

  glGenTextures(1u, &context->obj_large_palette);
  glBindTexture(GL_TEXTURE_2D, context->obj_large_palette);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
               /*width=*/GBA_LARGE_PALETTE_SIZE,
               /*height=*/1u,
               /*border=*/0, /*format=*/GL_RGBA,
               /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
               /*pixels=*/NULL);

  glGenTextures(1u, &context->obj_small_palette);
  glBindTexture(GL_TEXTURE_2D, context->obj_small_palette);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
               /*width=*/GBA_SMALL_PALETTE_SIZE,
               /*height=*/GBA_NUM_SMALL_PALETTES,
               /*border=*/0, /*format=*/GL_RGBA,
               /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
               /*pixels=*/NULL);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlBgPaletteDestroy(OpenGlBgPalette* context) {
  glDeleteTextures(1u, &context->bg_palette);
  glDeleteTextures(1u, &context->obj_small_palette);
  glDeleteTextures(1u, &context->obj_large_palette);
}