#include "emulator/ppu/gba/opengl/mosaic.h"

#include <assert.h>

void GbaPpuOpenGlMosaicReload(GbaPpuOpenGlMosaic* context,
                              const GbaPpuRegisters* registers,
                              GbaPpuDirtyBits* dirty_bits) {
  if (!dirty_bits->io.mosaic) {
    return;
  }

  context->bg[0u] = registers->mosaic.bg_horiz + 1;
  context->bg[1u] = registers->mosaic.bg_vert + 1;
  context->obj[0u] = registers->mosaic.obj_horiz + 1;
  context->obj[1u] = registers->mosaic.obj_vert + 1;

  dirty_bits->io.mosaic = false;
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

  mosaic[0u] = context->obj[0u];
  mosaic[1u] = context->obj[1u];
}