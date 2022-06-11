#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_SOFTWARE_RENDER_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_SOFTWARE_RENDER_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/screen.h"

typedef struct _GbaPpuSoftwareRenderer GbaPpuSoftwareRenderer;

GbaPpuSoftwareRenderer* GbaPpuSoftwareRendererAllocate();

bool GbaPpuSoftwareRendererSetScreen(GbaPpuSoftwareRenderer* renderer,
                                     Screen* screen);

void GbaPpuSoftwareRendererDrawRow(GbaPpuSoftwareRenderer* renderer,
                                   const GbaPpuMemory* memory,
                                   const GbaPpuRegisters* registers,
                                   GbaPpuDirtyBits* dirty_bits);

void GbaPpuSoftwareRendererDrawPixel(GbaPpuSoftwareRenderer* renderer,
                                     const GbaPpuMemory* memory,
                                     const GbaPpuRegisters* registers,
                                     GbaPpuDirtyBits* dirty_bits, uint8_t x);

void GbaPpuSoftwareRendererFree(GbaPpuSoftwareRenderer* renderer);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_SOFTWARE_RENDER_