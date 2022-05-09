#include "emulator/ppu/gba/opengl/mosaic.h"

#include <assert.h>
#include <string.h>

static void GbaPpuOpenGlMosaicReload(GLuint buffer, unsigned char x,
                                     unsigned char y) {
  GLfloat array[2u] = {x + 1, y + 1};
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2, array, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint GbaPpuOpenGlMosaicBG(GbaPpuOpenGlMosaicContext* context,
                            const GbaPpuRegisters* registers,
                            GbaPpuDirtyBits* dirty_bits, uint8_t index) {
  assert(index < GBA_PPU_NUM_BACKGROUNDS);

  if (!registers->bgcnt->mosaic) {
    return context->identity;
  }

  if (dirty_bits->io.bg_mosaic) {
    GbaPpuOpenGlMosaicReload(context->bg, registers->mosaic.bg_horiz,
                             registers->mosaic.bg_vert);
    dirty_bits->io.bg_mosaic = false;
  }

  return context->bg;
}

GLuint GbaPpuOpenGlMosaicOBJ(GbaPpuOpenGlMosaicContext* context,
                             const GbaPpuMemory* memory,
                             const GbaPpuRegisters* registers,
                             GbaPpuDirtyBits* dirty_bits, uint8_t index) {
  assert(index < OAM_NUM_OBJECTS);

  if (!memory->oam.object_attributes->obj_mosaic) {
    return context->identity;
  }

  if (dirty_bits->io.obj_mosaic) {
    GbaPpuOpenGlMosaicReload(context->obj, registers->mosaic.obj_horiz,
                             registers->mosaic.obj_vert);
    dirty_bits->io.obj_mosaic = false;
  }

  return context->obj;
}

void GbaPpuOpenGlMosaicReloadContext(GbaPpuOpenGlMosaicContext* context) {
  static const GLfloat identity[2u] = {1.0, 1.0};
  glGenBuffers(1, &context->identity);
  glBindBuffer(GL_ARRAY_BUFFER, context->identity);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2, identity, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &context->bg);
  glBindBuffer(GL_ARRAY_BUFFER, context->bg);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2, NULL, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &context->obj);
  glBindBuffer(GL_ARRAY_BUFFER, context->obj);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2, NULL, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GbaPpuOpenGlMosaicDestroy(GbaPpuOpenGlMosaicContext* context) {
  glDeleteBuffers(1u, &context->identity);
  glDeleteBuffers(1u, &context->bg);
  glDeleteBuffers(1u, &context->obj);
}