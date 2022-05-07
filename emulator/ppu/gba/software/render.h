#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_SOFTWARE_RENDER_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_SOFTWARE_RENDER_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/set.h"

typedef struct _GbaPpuSoftwareRenderer GbaPpuSoftwareRenderer;

GbaPpuSoftwareRenderer* GbaPpuSoftwareRendererAllocate(
    const GbaPpuObjectAttributeMemory* memory);

void GbaPpuSoftwareRendererDrawPixel(
    GbaPpuSoftwareRenderer* renderer, const GbaPpuMemory* memory,
    const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers, GbaPpuSet dirty_objects,
    GbaPpuSet dirty_rotations, uint8_t x, uint8_t y);

void GbaPpuSoftwareRendererPresent(GbaPpuSoftwareRenderer* renderer, GLuint fbo,
                                   GLsizei width, GLsizei height);

void GbaPpuSoftwareRendererReloadContext(GbaPpuSoftwareRenderer* renderer);

void GbaPpuSoftwareRendererFree(GbaPpuSoftwareRenderer* renderer);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_SOFTWARE_RENDER_