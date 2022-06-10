#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_OPENGL_RENDER_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_OPENGL_RENDER_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/screen.h"

typedef struct _GbaPpuOpenGlRenderer GbaPpuOpenGlRenderer;

GbaPpuOpenGlRenderer* GbaPpuOpenGlRendererAllocate();

void GbaPpuOpenGlRendererSetScreen(GbaPpuOpenGlRenderer* renderer,
                                   Screen* screen);

void GbaPpuOpenGlRendererDrawRow(GbaPpuOpenGlRenderer* renderer,
                                 const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuDirtyBits* dirty_bits);

void GbaPpuOpenGlRendererReloadContext(GbaPpuOpenGlRenderer* renderer);
void GbaPpuOpenGlRendererSetScale(GbaPpuOpenGlRenderer* renderer,
                                  uint8_t render_scale);

void GbaPpuOpenGlRendererFree(GbaPpuOpenGlRenderer* renderer);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_OPENGL_RENDER_