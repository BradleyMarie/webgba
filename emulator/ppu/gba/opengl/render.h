#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_OPENGL_RENDER_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_OPENGL_RENDER_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

typedef struct _GbaPpuOpenGlRenderer GbaPpuOpenGlRenderer;

GbaPpuOpenGlRenderer* GbaPpuOpenGlRendererAllocate();

void GbaPpuOpenGlRendererDrawRow(GbaPpuOpenGlRenderer* renderer,
                                 const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuDirtyBits* dirty_bits);

void GbaPpuOpenGlRendererPresent(GbaPpuOpenGlRenderer* renderer, GLuint fbo,
                                 GLsizei width, GLsizei height,
                                 uint8_t* fbo_contents);

void GbaPpuOpenGlRendererReloadContext(GbaPpuOpenGlRenderer* renderer);
void GbaPpuOpenGlRendererSetScale(GbaPpuOpenGlRenderer* renderer,
                                  uint8_t render_scale);

void GbaPpuOpenGlRendererFree(GbaPpuOpenGlRenderer* renderer);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_OPENGL_RENDER_