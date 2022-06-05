#include "emulator/ppu/gba/opengl/bg_scrolling.h"

#include <string.h>

void OpenGlBgScrollingReload(OpenGlBgScrolling* context,
                             const GbaPpuRegisters* registers,
                             GbaPpuDirtyBits* dirty_bits) {
  for (uint8_t i = 0; i < GBA_PPU_NUM_BACKGROUNDS; i++) {
    if (!dirty_bits->io.bg_offset[i]) {
      continue;
    }

    GLfloat array[6u];

    // Bottom Left
    array[0u] = registers->bg_offsets[i].x;
    array[1u] = registers->bg_offsets[i].y + GBA_SCREEN_HEIGHT;

    // Bottom Right
    array[2u] = registers->bg_offsets[i].x + 2u * GBA_SCREEN_WIDTH;
    array[3u] = registers->bg_offsets[i].y + GBA_SCREEN_HEIGHT;

    // Top Left
    array[4u] = registers->bg_offsets[i].x;
    array[5u] = (int)registers->bg_offsets[i].y - GBA_SCREEN_HEIGHT;

    glBindBuffer(GL_ARRAY_BUFFER, context->buffers[i]);
    glBufferSubData(GL_ARRAY_BUFFER, /*offset=*/0, sizeof(GLfloat) * 6, array);
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
    GLfloat array[6u] = {0.0,
                         (GLfloat)GBA_SCREEN_HEIGHT,
                         2.0 * (GLfloat)GBA_SCREEN_WIDTH,
                         (GLfloat)GBA_SCREEN_HEIGHT,
                         0.0,
                         -(GLfloat)GBA_SCREEN_HEIGHT};
    glBindBuffer(GL_ARRAY_BUFFER, context->buffers[i]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6, array, GL_STATIC_DRAW);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGlBgScrollingDestroy(OpenGlBgScrolling* context) {
  glDeleteBuffers(GBA_PPU_NUM_BACKGROUNDS, context->buffers);
}