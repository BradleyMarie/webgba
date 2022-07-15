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

void OpenGlBgAffineBind(const OpenGlBgAffine* context,
                        const UniformLocations* locations) {
  if (locations->affine_start_coordinates[0u] >= 0) {
    glUniform4f(
        locations->affine_start_coordinates[0u],
        context->start_base_scale[0u][0u], context->start_base_scale[0u][1u],
        context->start_base_scale[0u][2u], context->start_base_scale[0u][3u]);
    glUniform4f(
        locations->affine_end_coordinates[0u], context->end_base_scale[0u][0u],
        context->end_base_scale[0u][1u], context->end_base_scale[0u][2u],
        context->end_base_scale[0u][3u]);
  }

  if (locations->affine_start_coordinates[1u] >= 0) {
    glUniform4f(
        locations->affine_start_coordinates[1u],
        context->start_base_scale[1u][0u], context->start_base_scale[1u][1u],
        context->start_base_scale[1u][2u], context->start_base_scale[1u][3u]);
    glUniform4f(
        locations->affine_end_coordinates[1u], context->end_base_scale[1u][0u],
        context->end_base_scale[1u][1u], context->end_base_scale[1u][2u],
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