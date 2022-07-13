#include "emulator/ppu/gba/opengl/bg_affine.h"

static GLfloat LoadFixed(int32_t value) { return (double)value / 256.0; }

bool OpenGlBgAffineStage(OpenGlBgAffine* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits) {
  if (registers->dispcnt.mode == 0u) {
    return false;
  }

  for (uint8_t i = 0; i < GBA_PPU_NUM_AFFINE_BACKGROUNDS; i++) {
    context->end_base_scale[i][0u] =
        LoadFixed(registers->internal.affine[i].current[0u]);
    context->end_base_scale[i][1u] =
        LoadFixed(registers->internal.affine[i].current[1u]);
    context->end_base_scale[i][2u] = LoadFixed(registers->affine[i].pa);
    context->end_base_scale[i][3u] = LoadFixed(registers->affine[i].pc);
  }

  if (registers->vcount != GBA_SCREEN_HEIGHT - 1) {
    return true;
  }

  for (uint8_t i = 0; i < GBA_PPU_NUM_AFFINE_BACKGROUNDS; i++) {
    context->final_base_scale[i][0u] =
        context->end_base_scale[i][0u] +
        (context->end_base_scale[i][0u] - context->start_base_scale[i][0u]);
    context->final_base_scale[i][1u] =
        context->end_base_scale[i][1u] +
        (context->end_base_scale[i][1u] - context->start_base_scale[i][1u]);
    context->final_base_scale[i][2u] =
        context->end_base_scale[i][2u] +
        (context->end_base_scale[i][2u] - context->start_base_scale[i][2u]);
    context->final_base_scale[i][3u] =
        context->end_base_scale[i][3u] +
        (context->end_base_scale[i][3u] - context->start_base_scale[i][3u]);
  }

  return true;
}

void OpenGlBgAffineBind(const OpenGlBgAffine* context, GLuint program) {
  GLint affine_start_coordinates0 =
      glGetUniformLocation(program, "affine_start_coordinates[0]");
  if (affine_start_coordinates0 >= 0) {
    glUniform4f(affine_start_coordinates0, context->start_base_scale[0u][0u],
                context->start_base_scale[0u][1u],
                context->start_base_scale[0u][2u],
                context->start_base_scale[0u][3u]);
    GLint affine_end_coordinates0 =
        glGetUniformLocation(program, "affine_end_coordinates[0]");
    glUniform4f(affine_end_coordinates0, context->end_base_scale[0u][0u],
                context->end_base_scale[0u][1u],
                context->end_base_scale[0u][2u],
                context->end_base_scale[0u][3u]);
  }

  GLint affine_start_coordinates1 =
      glGetUniformLocation(program, "affine_start_coordinates[1]");
  if (affine_start_coordinates1 >= 0) {
    glUniform4f(affine_start_coordinates1, context->start_base_scale[1u][0u],
                context->start_base_scale[1u][1u],
                context->start_base_scale[1u][2u],
                context->start_base_scale[1u][3u]);
    GLint affine_end_coordinates1 =
        glGetUniformLocation(program, "affine_end_coordinates[1]");
    glUniform4f(affine_end_coordinates1, context->end_base_scale[1u][0u],
                context->end_base_scale[1u][1u],
                context->end_base_scale[1u][2u],
                context->end_base_scale[1u][3u]);
  }
}

void OpenGlBgAffineReload(OpenGlBgAffine* context) {
  for (uint8_t i = 0; i < GBA_PPU_NUM_AFFINE_BACKGROUNDS; i++) {
    for (uint8_t j = 0u; j < 4u; j++) {
      context->start_base_scale[i][j] = context->end_base_scale[i][j];
      context->end_base_scale[i][j] = context->final_base_scale[i][j];
    }
  }
}