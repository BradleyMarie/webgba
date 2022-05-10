#include "emulator/ppu/gba/opengl/mosaic.h"

#include <assert.h>
#include <string.h>

static void GbaPpuOpenGlMosaicReload(GLfloat buffer[2], unsigned char x,
                                     unsigned char y) {
  buffer[0] = x + 1;
  buffer[1] = y + 1;
}

void GbaPpuOpenGlMosaicBG(GbaPpuOpenGlMosaic* context,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits, uint8_t index,
                          GLfloat mosaic[2]) {
  assert(index < GBA_PPU_NUM_BACKGROUNDS);

  if (!registers->bgcnt[index].mosaic) {
    mosaic[0u] = 1.0;
    mosaic[1u] = 1.0;
    return;
  }

  if (dirty_bits->io.bg_mosaic) {
    GbaPpuOpenGlMosaicReload(context->bg, registers->mosaic.bg_horiz,
                             registers->mosaic.bg_vert);
    dirty_bits->io.bg_mosaic = false;
  }

  mosaic[0u] = context->bg[0u];
  mosaic[1u] = context->bg[1u];
}

void GbaPpuOpenGlMosaicOBJ(GbaPpuOpenGlMosaic* context,
                           const GbaPpuMemory* memory,
                           const GbaPpuRegisters* registers,
                           GbaPpuDirtyBits* dirty_bits, uint8_t index,
                           GLfloat mosaic[2]) {
  assert(index < OAM_NUM_OBJECTS);

  if (!memory->oam.object_attributes[index].obj_mosaic) {
    mosaic[0u] = 1.0;
    mosaic[1u] = 1.0;
    return;
  }

  if (dirty_bits->io.obj_mosaic) {
    GbaPpuOpenGlMosaicReload(context->obj, registers->mosaic.obj_horiz,
                             registers->mosaic.obj_vert);
    dirty_bits->io.obj_mosaic = false;
  }

  mosaic[0u] = context->obj[0u];
  mosaic[1u] = context->obj[1u];
}