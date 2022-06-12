#include "emulator/ppu/gba/opengl/bg_scrolling.h"

#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

void OpenGlBgScrollingReload(OpenGlBgScrolling* context,
                             const GbaPpuRegisters* registers,
                             GbaPpuDirtyBits* dirty_bits) {
  bool do_copy;
  for (uint8_t i = 0; i < GBA_PPU_NUM_BACKGROUNDS; i++) {
    if (!dirty_bits->io.bg_offset[i]) {
      continue;
    }

    context->staging.origins[i][0u] = registers->bg_offsets[i].x;
    context->staging.origins[i][1u] = registers->bg_offsets[i].y;

    dirty_bits->io.bg_offset[i] = false;
    do_copy = true;
  }

  if (do_copy) {
    glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, /*offset=*/0,
                    /*size=*/sizeof(context->staging),
                    /*data=*/&context->staging);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }
}

void OpenGlBgScrollingBind(const OpenGlBgScrolling* context, GLuint program) {
  GLint scrolling_backgrounds =
      glGetUniformBlockIndex(program, "ScrollingBackgrounds");
  glUniformBlockBinding(program, scrolling_backgrounds, SCROLLING_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, SCROLLING_BUFFER, context->buffer);
}

void OpenGlBgScrollingReloadContext(OpenGlBgScrolling* context) {
  glGenBuffers(1, &context->buffer);
  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->staging), &context->staging,
               GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGlBgScrollingDestroy(OpenGlBgScrolling* context) {
  glDeleteBuffers(1, &context->buffer);
}