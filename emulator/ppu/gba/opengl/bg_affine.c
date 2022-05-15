#include "emulator/ppu/gba/opengl/bg_affine.h"

#include <string.h>

static GLfloat FixedToFloat(int32_t value) { return (double)value / 256.0; }

void OpenGlBgAffineInitialize(OpenGlBgAffine* context) {
  context->reload_scanlines[0u] = UINT16_MAX;
  context->reload_scanlines[1u] = UINT16_MAX;
}

void OpenGlBgAffineReload(OpenGlBgAffine* context,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits, uint8_t index) {
  assert(index == 2u || index == 3u);
  index -= 2u;

  if (!dirty_bits->composite.bg_affine[index] &&
      context->reload_scanlines[index] <= registers->vcount) {
    return;
  }

  context->reload_scanlines[index] = registers->vcount;

  int32_t top_left_x = registers->affine[index].x -
                       registers->vcount * registers->affine[index].pb;
  int32_t top_left_y = registers->affine[index].y -
                       registers->vcount * registers->affine[index].pd;

  GLfloat array[8u];

  // Bottom Left
  array[0] = FixedToFloat(top_left_x +
                          GBA_SCREEN_HEIGHT * registers->affine[index].pb);
  array[1] = FixedToFloat(top_left_y +
                          GBA_SCREEN_HEIGHT * registers->affine[index].pd);

  // Top Left
  array[2] = FixedToFloat(top_left_x);
  array[3] = FixedToFloat(top_left_y);

  // Top Right
  array[4] =
      FixedToFloat(top_left_x + GBA_SCREEN_WIDTH * registers->affine[index].pa);
  array[5] =
      FixedToFloat(top_left_y + GBA_SCREEN_WIDTH * registers->affine[index].pc);

  // Bottom Right
  array[6] = FixedToFloat(top_left_x +
                          GBA_SCREEN_HEIGHT * registers->affine[index].pb +
                          GBA_SCREEN_WIDTH * registers->affine[index].pa);
  array[7] = FixedToFloat(top_left_y +
                          GBA_SCREEN_HEIGHT * registers->affine[index].pd +
                          GBA_SCREEN_WIDTH * registers->affine[index].pc);

  glBindBuffer(GL_ARRAY_BUFFER, context->buffers[index]);
  glBufferSubData(GL_ARRAY_BUFFER, /*offset=*/0, sizeof(GLfloat) * 8, array);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  dirty_bits->composite.bg_affine[index] = false;
}

void OpenGlBgAffineBind(OpenGlBgAffine* context,
                        const GbaPpuRegisters* registers, GLuint program) {
  GLuint bg2_affine = glGetAttribLocation(program, "bg2_affine");
  glBindBuffer(GL_ARRAY_BUFFER, context->buffers[0u]);
  glVertexAttribPointer(bg2_affine, /*size=*/2, /*type=*/GL_FLOAT,
                        /*normalized=*/false, /*stride=*/0, /*pointer=*/NULL);
  glEnableVertexAttribArray(bg2_affine);

  GLuint bg3_affine = glGetAttribLocation(program, "bg3_affine");
  glBindBuffer(GL_ARRAY_BUFFER, context->buffers[1u]);
  glVertexAttribPointer(bg3_affine, /*size=*/2, /*type=*/GL_FLOAT,
                        /*normalized=*/false, /*stride=*/0, /*pointer=*/NULL);
  glEnableVertexAttribArray(bg3_affine);
}

void OpenGlBgAffineReloadContext(OpenGlBgAffine* context) {
  glGenBuffers(GBA_PPU_NUM_AFFINE_BACKGROUNDS, context->buffers);
  for (uint8_t i = 0; i < GBA_PPU_NUM_AFFINE_BACKGROUNDS; i++) {
    glBindBuffer(GL_ARRAY_BUFFER, context->buffers[i]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, NULL, GL_STATIC_DRAW);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGlBgAffineDestroy(OpenGlBgAffine* context) {
  glDeleteBuffers(GBA_PPU_NUM_AFFINE_BACKGROUNDS, context->buffers);
}