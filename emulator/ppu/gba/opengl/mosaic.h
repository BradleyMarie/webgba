#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_MOSAIC_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_MOSAIC_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint identity;
  GLuint bg;
  GLuint obj;
} GbaPpuOpenGlMosaicContext;

GLuint GbaPpuOpenGlMosaicBG(GbaPpuOpenGlMosaicContext* context,
                            const GbaPpuRegisters* registers,
                            GbaPpuDirtyBits* dirty_bits, uint8_t index);

GLuint GbaPpuOpenGlMosaicOBJ(GbaPpuOpenGlMosaicContext* context,
                             const GbaPpuMemory* memory,
                             const GbaPpuRegisters* registers,
                             GbaPpuDirtyBits* dirty_bits, uint8_t index);

void GbaPpuOpenGlMosaicReloadContext(GbaPpuOpenGlMosaicContext* context);

void GbaPpuOpenGlMosaicDestroy(GbaPpuOpenGlMosaicContext* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_MOSAIC_