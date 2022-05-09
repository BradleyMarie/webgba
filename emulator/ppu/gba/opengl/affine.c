#include "emulator/ppu/gba/opengl/affine.h"

#include <assert.h>
#include <string.h>

static GLfloat FixedToFloat(int32_t value) { return (double)value / 256.0; }

static void GbaPpuOpenGlAffineReload(GLuint buffer, uint8_t row, int32_t pa,
                                     int32_t pb, int32_t pc, int32_t pd,
                                     int32_t x, int32_t y) {
  int32_t top_left_x = x - row * pb;
  int32_t top_left_y = y - row * pd;

  GLfloat array[8u];

  // Bottom Left
  array[0] = FixedToFloat(top_left_x + GBA_SCREEN_HEIGHT * pb);
  array[1] = FixedToFloat(top_left_y + GBA_SCREEN_HEIGHT * pd);

  // Top Left
  array[2] = FixedToFloat(top_left_x);
  array[3] = FixedToFloat(top_left_y);

  // Top Right
  array[4] = FixedToFloat(top_left_x + GBA_SCREEN_WIDTH * pa);
  array[5] = FixedToFloat(top_left_y + GBA_SCREEN_WIDTH * pc);

  // Bottom Right
  array[6] =
      FixedToFloat(top_left_x + GBA_SCREEN_HEIGHT * pb + GBA_SCREEN_WIDTH * pa);
  array[7] =
      FixedToFloat(top_left_y + GBA_SCREEN_HEIGHT * pd + GBA_SCREEN_WIDTH * pc);

  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2, array, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint GbaPpuOpenGlAffine(GbaPpuOpenGlAffineContext* context,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits, uint8_t index) {
  index -= 2u;
  assert(index < GBA_PPU_NUM_AFFINE_BACKGROUNDS);

  if (dirty_bits->composite.bg_affine[index]) {
    GbaPpuOpenGlAffineReload(
        context->bg_affine[index], registers->vcount,
        registers->affine[index].pa, registers->affine[index].pb,
        registers->affine[index].pc, registers->affine[index].pd,
        registers->affine[index].x, registers->affine[index].y);
    dirty_bits->composite.bg_affine[index] = false;
  }

  return context->bg_affine[index];
}

void GbaPpuOpenGlAffineReloadContext(GbaPpuOpenGlAffineContext* context) {
  glGenBuffers(GBA_PPU_NUM_AFFINE_BACKGROUNDS, context->bg_affine);
  for (uint8_t i = 0; i < GBA_PPU_NUM_AFFINE_BACKGROUNDS; i++) {
    glBindBuffer(GL_ARRAY_BUFFER, context->bg_affine[i]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

void GbaPpuOpenGlAffineDestroy(GbaPpuOpenGlAffineContext* context) {
  glDeleteBuffers(GBA_PPU_NUM_AFFINE_BACKGROUNDS, context->bg_affine);
}