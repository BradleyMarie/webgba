#include "emulator/ppu/gba/opengl/palette.h"

#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

static void ExtractComponents(uint16_t value, GLfloat output[4u]) {
  uint_fast32_t r = (value & 0x001Fu) >> 0u;
  uint_fast32_t g = (value & 0x03E0u) >> 5u;
  uint_fast32_t b = (value & 0x7C00u) >> 10u;

  output[0u] = (GLfloat)b / 32.0;
  output[1u] = (GLfloat)g / 32.0;
  output[2u] = (GLfloat)r / 32.0;
  output[3u] = 1.0;
}

void OpenGlBgPaletteReload(OpenGlBgPalette* context, const GbaPpuMemory* memory,
                           GbaPpuDirtyBits* dirty_bits) {
  if (dirty_bits->palette.bg_palette) {
    for (uint16_t i = 0; i < GBA_LARGE_PALETTE_SIZE; i++) {
      ExtractComponents(memory->palette.bg.large_palette[i],
                        &context->staging[i][0]);
    }

    glBindBuffer(GL_UNIFORM_BUFFER, context->bg_palette);
    glBufferSubData(GL_UNIFORM_BUFFER, /*offset=*/0,
                    /*size=*/sizeof(context->staging),
                    /*data=*/context->staging);

    dirty_bits->palette.bg_palette = false;
  }

  if (dirty_bits->palette.obj_palette) {
    for (uint16_t i = 0; i < GBA_LARGE_PALETTE_SIZE; i++) {
      ExtractComponents(memory->palette.obj.large_palette[i],
                        &context->staging[i][0]);
    }

    glBindBuffer(GL_UNIFORM_BUFFER, context->obj_palette);
    glBufferSubData(GL_UNIFORM_BUFFER, /*offset=*/0,
                    /*size=*/sizeof(context->staging),
                    /*data=*/context->staging);

    dirty_bits->palette.obj_palette = false;
  }

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGlBgPaletteBind(const OpenGlBgPalette* context, GLuint program) {
  GLint bg_palette = glGetUniformBlockIndex(program, "BackgroundPalette");
  glUniformBlockBinding(program, bg_palette, BG_PALETTE_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->bg_palette);
  glBindBufferBase(GL_UNIFORM_BUFFER, BG_PALETTE_BUFFER, context->bg_palette);

  GLint obj_palette = glGetUniformBlockIndex(program, "ObjectPalette");
  glUniformBlockBinding(program, obj_palette, OBJ_PALETTE_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->obj_palette);
  glBindBufferBase(GL_UNIFORM_BUFFER, OBJ_PALETTE_BUFFER, context->obj_palette);
}

void OpenGlBgPaletteReloadContext(OpenGlBgPalette* context) {
  glGenBuffers(1, &context->bg_palette);
  glBindBuffer(GL_UNIFORM_BUFFER, context->bg_palette);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->staging), context->staging,
               GL_DYNAMIC_DRAW);

  glGenBuffers(1, &context->obj_palette);
  glBindBuffer(GL_UNIFORM_BUFFER, context->obj_palette);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->staging), context->staging,
               GL_DYNAMIC_DRAW);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGlBgPaletteDestroy(OpenGlBgPalette* context) {
  glDeleteBuffers(1u, &context->bg_palette);
  glDeleteBuffers(1u, &context->obj_palette);
}