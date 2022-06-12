#include "emulator/ppu/gba/opengl/bg_affine.h"

#include <string.h>

static GLfloat FixedToFloat(int32_t value) { return (double)value / 256.0; }

bool OpenGlBgAffineStage(OpenGlBgAffine* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits) {
  if (registers->dispcnt.mode == 0u) {
    return false;
  }

  bool result = false;
  for (uint8_t i = 0; i < GBA_PPU_NUM_AFFINE_BACKGROUNDS; i++) {
    if (i == 0 && !registers->dispcnt.bg2_enable) {
      return false;
    }

    if (i == 1 &&
        (!registers->dispcnt.bg3_enable || registers->dispcnt.mode != 2u)) {
      return false;
    }

    if (!dirty_bits->io.bg_affine[i]) {
      continue;
    }

    int32_t top_left_x =
        registers->affine[i].x - registers->vcount * registers->affine[i].pb;
    int32_t top_left_y =
        registers->affine[i].y - registers->vcount * registers->affine[i].pd;

    // Bottom Left
    context->staging[i][0] =
        FixedToFloat(top_left_x + GBA_SCREEN_HEIGHT * registers->affine[i].pb);
    context->staging[i][1] =
        FixedToFloat(top_left_y + GBA_SCREEN_HEIGHT * registers->affine[i].pd);

    // Bottom Right
    context->staging[i][2] =
        FixedToFloat(top_left_x + GBA_SCREEN_HEIGHT * registers->affine[i].pb +
                     2u * GBA_SCREEN_WIDTH * registers->affine[i].pa);
    context->staging[i][3] =
        FixedToFloat(top_left_y + GBA_SCREEN_HEIGHT * registers->affine[i].pd +
                     2u * GBA_SCREEN_WIDTH * registers->affine[i].pc);

    // Top Left
    context->staging[i][4] =
        FixedToFloat(top_left_x - GBA_SCREEN_HEIGHT * registers->affine[i].pb);
    context->staging[i][5] =
        FixedToFloat(top_left_y - GBA_SCREEN_HEIGHT * registers->affine[i].pd);

    dirty_bits->io.bg_affine[i] = false;
    context->dirty[i] = true;
    result = true;
  }

  return result;
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

void OpenGlBgAffineReload(OpenGlBgAffine* context) {
  if (context->dirty[0u]) {
    glBindBuffer(GL_ARRAY_BUFFER, context->buffers[0u]);
    glBufferSubData(GL_ARRAY_BUFFER, /*offset=*/0, sizeof(context->staging[0u]),
                    context->staging[0u]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    context->dirty[0u] = false;
  }

  if (context->dirty[1u]) {
    glBindBuffer(GL_ARRAY_BUFFER, context->buffers[1u]);
    glBufferSubData(GL_ARRAY_BUFFER, /*offset=*/0, sizeof(context->staging[1u]),
                    context->staging[1u]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    context->dirty[1u] = false;
  }
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