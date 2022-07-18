#include "emulator/ppu/gba/opengl/bg_affine.h"

static GLfloat LoadFixed(int32_t value) { return (double)value / 256.0; }

bool OpenGlBgAffineStage(OpenGlBgAffine* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits) {
  context->final_row = false;

  context->base_row = context->next_row;
  for (uint8_t i = 0; i < GBA_PPU_NUM_AFFINE_BACKGROUNDS; i++) {
    context->bases[i][0u] = context->next_bases[i][0u];
    context->bases[i][1u] = context->next_bases[i][1u];
    context->base_dx[i][0u] = context->next_dx[i][0u];
    context->base_dx[i][1u] = context->next_dx[i][1u];
    context->base_dy[i][0u] = context->next_dy[i][0u];
    context->base_dy[i][1u] = context->next_dy[i][1u];
  }

  context->next_row = registers->vcount;
  for (uint8_t i = 0; i < GBA_PPU_NUM_AFFINE_BACKGROUNDS; i++) {
    context->next_bases[i][0u] = registers->internal.affine[i].current[0u];
    context->next_bases[i][1u] = registers->internal.affine[i].current[1u];
    context->next_dx[i][0u] = registers->affine[i].pa;
    context->next_dx[i][1u] = registers->affine[i].pc;
    context->next_dy[i][0u] = registers->affine[i].pb;
    context->next_dy[i][1u] = registers->affine[i].pd;
  }

  if (registers->dispcnt.mode == 0u || registers->vcount == 0u) {
    return false;
  }

  bool next_row_dirty = false;
  for (uint8_t i = 0; i < GBA_PPU_NUM_AFFINE_BACKGROUNDS; i++) {
    if (context->bases[i][0u] + context->base_dy[i][0u] !=
        context->next_bases[i][0u]) {
      next_row_dirty = true;
      break;
    }

    if (context->bases[i][1u] + context->base_dy[i][1u] !=
        context->next_bases[i][1u]) {
      next_row_dirty = true;
      break;
    }

    if (context->base_dx[i][0u] != context->next_dx[i][0u] ||
        context->base_dx[i][1u] != context->next_dx[i][1u] ||
        context->base_dy[i][0u] != context->next_dy[i][0u] ||
        context->base_dy[i][1u] != context->next_dy[i][1u]) {
      next_row_dirty = true;
      break;
    }
  }

  if (registers->vcount != GBA_SCREEN_HEIGHT - 1) {
    return next_row_dirty;
  }

  for (uint8_t i = 0; i < GBA_PPU_NUM_AFFINE_BACKGROUNDS; i++) {
    context->final_bases[i][0u] =
        context->next_bases[i][0u] +
        (context->next_bases[i][0u] - context->bases[i][0u]);
    context->final_bases[i][1u] =
        context->next_bases[i][1u] +
        (context->next_bases[i][1u] - context->bases[i][1u]);
    context->final_dx[i][0u] =
        context->next_dx[i][0u] +
        (context->next_dx[i][0u] - context->base_dx[i][0u]);
    context->final_dx[i][1u] =
        context->next_dx[i][1u] +
        (context->next_dx[i][1u] - context->base_dx[i][1u]);
  }

  context->final_row = true;

  return next_row_dirty;
}

void OpenGlBgAffineBind(const OpenGlBgAffine* context,
                        const UniformLocations* locations) {
  if (locations->affine_base_row < 0) {
    return;
  }

  glUniform1f(locations->affine_base_row, context->base_row);

  glUniform4f(locations->affine_base[0u], LoadFixed(context->bases[0u][0u]),
              LoadFixed(context->bases[0u][1u]),
              LoadFixed(context->base_dx[0u][0u]),
              LoadFixed(context->base_dx[0u][1u]));
  glUniform4f(locations->affine_delta[0u],
              LoadFixed(context->next_bases[0u][0u] - context->bases[0u][0u]),
              LoadFixed(context->next_bases[0u][1u] - context->bases[0u][1u]),
              LoadFixed(context->next_dx[0u][0u] - context->base_dx[0u][0u]),
              LoadFixed(context->next_dx[0u][1u] - context->base_dx[0u][1u]));

  if (locations->affine_base[1u] >= 0) {
    glUniform4f(locations->affine_base[1u], LoadFixed(context->bases[1u][0u]),
                LoadFixed(context->bases[1u][1u]),
                LoadFixed(context->base_dx[1u][0u]),
                LoadFixed(context->base_dx[1u][1u]));
    glUniform4f(locations->affine_delta[1u],
                LoadFixed(context->next_bases[1u][0u] - context->bases[1u][0u]),
                LoadFixed(context->next_bases[1u][1u] - context->bases[1u][1u]),
                LoadFixed(context->next_dx[1u][0u] - context->base_dx[1u][0u]),
                LoadFixed(context->next_dx[1u][1u] - context->base_dx[1u][1u]));
  }
}

void OpenGlBgAffineReload(OpenGlBgAffine* context) {
  if (context->final_row) {
    context->base_row = context->next_row;
    for (uint8_t i = 0; i < GBA_PPU_NUM_AFFINE_BACKGROUNDS; i++) {
      context->bases[i][0u] = context->next_bases[i][0u];
      context->bases[i][1u] = context->next_bases[i][1u];
      context->base_dx[i][0u] = context->next_dx[i][0u];
      context->base_dx[i][1u] = context->next_dx[i][1u];
      context->base_dy[i][0u] = context->next_dy[i][0u];
      context->base_dy[i][1u] = context->next_dy[i][1u];
    }
  }
}