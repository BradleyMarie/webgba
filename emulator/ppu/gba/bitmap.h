#ifndef _WEBGBA_EMULATOR_PPU_GBA_BITMAP_
#define _WEBGBA_EMULATOR_PPU_GBA_BITMAP_

#include "emulator/ppu/gba/types.h"

void GbaPpuRenderMode3Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y,
                            GbaPpuFrameBuffer* framebuffer);

void GbaPpuRenderMode4Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y,
                            GbaPpuFrameBuffer* framebuffer);

void GbaPpuRenderMode5Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y,
                            GbaPpuFrameBuffer* framebuffer);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_BITMAP_