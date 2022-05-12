#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_SOFTWARE_RENDER_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_SOFTWARE_RENDER_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

typedef struct _GbaPpuSoftwareRenderer GbaPpuSoftwareRenderer;

GbaPpuSoftwareRenderer* GbaPpuSoftwareRendererAllocate();

void GbaPpuSoftwareRendererDrawRow(
    GbaPpuSoftwareRenderer* renderer, const GbaPpuMemory* memory,
    const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuDirtyBits* dirty_bits);

void GbaPpuSoftwareRendererDrawPixel(
    GbaPpuSoftwareRenderer* renderer, const GbaPpuMemory* memory,
    const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuDirtyBits* dirty_bits, uint8_t x);

void GbaPpuSoftwareRendererPresent(GbaPpuSoftwareRenderer* renderer, GLuint fbo,
                                   GLsizei width, GLsizei height,
                                   uint8_t* fbo_contents);

void GbaPpuSoftwareRendererReloadContext(GbaPpuSoftwareRenderer* renderer);

void GbaPpuSoftwareRendererFree(GbaPpuSoftwareRenderer* renderer);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_SOFTWARE_RENDER_