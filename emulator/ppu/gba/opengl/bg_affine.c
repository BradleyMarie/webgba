#include "emulator/ppu/gba/opengl/bg_affine.h"

#include <string.h>

static GLfloat FixedToFloat(int32_t value) { return (double)value / 256.0; }

void OpenGlBgAffineReload(OpenGlBgAffine* context,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits) {
  for (uint8_t i = 0; i < GBA_PPU_NUM_AFFINE_BACKGROUNDS; i++) {
    if (!dirty_bits->io.bg_affine[i]) {
      continue;
    }

    int32_t top_left_x =
        registers->affine[i].x - registers->vcount * registers->affine[i].pb;
    int32_t top_left_y =
        registers->affine[i].y - registers->vcount * registers->affine[i].pd;

    GLfloat array[6u];

    // Bottom Left
    array[0] =
        FixedToFloat(top_left_x + GBA_SCREEN_HEIGHT * registers->affine[i].pb);
    array[1] =
        FixedToFloat(top_left_y + GBA_SCREEN_HEIGHT * registers->affine[i].pd);

    // Bottom Right
    array[2] =
        FixedToFloat(top_left_x + GBA_SCREEN_HEIGHT * registers->affine[i].pb +
                     2u * GBA_SCREEN_WIDTH * registers->affine[i].pa);
    array[3] =
        FixedToFloat(top_left_y + GBA_SCREEN_HEIGHT * registers->affine[i].pd +
                     2u * GBA_SCREEN_WIDTH * registers->affine[i].pc);

    // Top Left
    array[4] =
        FixedToFloat(top_left_x - GBA_SCREEN_HEIGHT * registers->affine[i].pb);
    array[5] =
        FixedToFloat(top_left_y - GBA_SCREEN_HEIGHT * registers->affine[i].pd);

    glBindBuffer(GL_ARRAY_BUFFER, context->buffers[i]);
    glBufferSubData(GL_ARRAY_BUFFER, /*offset=*/0, sizeof(GLfloat) * 6, array);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    dirty_bits->io.bg_affine[i] = false;
  }
}

void OpenGlBgAffineBind(const OpenGlBgAffine* context, GLuint program) {
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

void OpenGlBgAffineDestroy(OpenGlBgAffine* context) {
  glDeleteBuffers(GBA_PPU_NUM_AFFINE_BACKGROUNDS, context->buffers);
}