#include "emulator/ppu/gba/opengl/bg_scrolling.h"

#include <string.h>

void OpenGlBgScrollingReload(OpenGlBgScrolling* context,
                             const GbaPpuRegisters* registers,
                             GbaPpuDirtyBits* dirty_bits) {
  for (uint8_t i = 0; i < GBA_PPU_NUM_BACKGROUNDS; i++) {
    if (!dirty_bits->io.bg_offset[i]) {
      continue;
    }

    GLfloat array[8u];

    // Bottom Left
    array[0] = registers->bg_offsets[i].x;
    array[1] = registers->bg_offsets[i].y + GBA_SCREEN_HEIGHT;

    // Top Left
    array[2] = registers->bg_offsets[i].x;
    array[3] = registers->bg_offsets[i].y;

    // Top Right
    array[4] = registers->bg_offsets[i].x + GBA_SCREEN_WIDTH;
    array[5] = registers->bg_offsets[i].y;

    // Bottom Right
    array[6] = registers->bg_offsets[i].x + GBA_SCREEN_WIDTH;
    array[7] = registers->bg_offsets[i].y + GBA_SCREEN_HEIGHT;

    glBindBuffer(GL_ARRAY_BUFFER, context->buffers[i]);
    glBufferSubData(GL_ARRAY_BUFFER, /*offset=*/0, sizeof(GLfloat) * 8, array);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    dirty_bits->io.bg_offset[i] = false;
  }
}

void OpenGlBgScrollingBind(const OpenGlBgScrolling* context, GLuint program) {
  GLuint bg0_scrolling = glGetAttribLocation(program, "bg0_scrolling");
  glBindBuffer(GL_ARRAY_BUFFER, context->buffers[0u]);
  glVertexAttribPointer(bg0_scrolling, /*size=*/2, /*type=*/GL_FLOAT,
                        /*normalized=*/false, /*stride=*/0, /*pointer=*/NULL);
  glEnableVertexAttribArray(bg0_scrolling);

  GLuint bg1_scrolling = glGetAttribLocation(program, "bg1_scrolling");
  glBindBuffer(GL_ARRAY_BUFFER, context->buffers[1u]);
  glVertexAttribPointer(bg1_scrolling, /*size=*/2, /*type=*/GL_FLOAT,
                        /*normalized=*/false, /*stride=*/0, /*pointer=*/NULL);
  glEnableVertexAttribArray(bg1_scrolling);

  GLuint bg2_scrolling = glGetAttribLocation(program, "bg2_scrolling");
  glBindBuffer(GL_ARRAY_BUFFER, context->buffers[2u]);
  glVertexAttribPointer(bg2_scrolling, /*size=*/2, /*type=*/GL_FLOAT,
                        /*normalized=*/false, /*stride=*/0, /*pointer=*/NULL);
  glEnableVertexAttribArray(bg2_scrolling);

  GLuint bg3_scrolling = glGetAttribLocation(program, "bg3_scrolling");
  glBindBuffer(GL_ARRAY_BUFFER, context->buffers[3u]);
  glVertexAttribPointer(bg3_scrolling, /*size=*/2, /*type=*/GL_FLOAT,
                        /*normalized=*/false, /*stride=*/0, /*pointer=*/NULL);
  glEnableVertexAttribArray(bg3_scrolling);
}

void OpenGlBgScrollingReloadContext(OpenGlBgScrolling* context) {
  glGenBuffers(GBA_PPU_NUM_BACKGROUNDS, context->buffers);
  for (uint8_t i = 0; i < GBA_PPU_NUM_BACKGROUNDS; i++) {
    glBindBuffer(GL_ARRAY_BUFFER, context->buffers[i]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, NULL, GL_STATIC_DRAW);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGlBgScrollingDestroy(OpenGlBgScrolling* context) {
  glDeleteBuffers(GBA_PPU_NUM_BACKGROUNDS, context->buffers);
}