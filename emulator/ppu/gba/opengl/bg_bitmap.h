#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint src_texture_mode34;
  GLuint src_texture_mode5;
  GLuint program;
  GLuint vertices;
  uint16_t colors[GBA_SCREEN_HEIGHT * GBA_SCREEN_WIDTH];
} GbaPpuOpenGlBgBitmap;

void GbaPpuOpenGlBgBitmapMode3(
    GbaPpuOpenGlBgBitmap* context, const GbaPpuMemory* memory,
    const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuDirtyBits* dirty_bits, GLuint fbo, GLsizei width, GLsizei height,
    uint8_t y);

void GbaPpuOpenGlBgBitmapMode4(
    GbaPpuOpenGlBgBitmap* context, const GbaPpuMemory* memory,
    const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuDirtyBits* dirty_bits, GLuint fbo, GLsizei width, GLsizei height,
    uint8_t y);

void GbaPpuOpenGlBgBitmapMode5(
    GbaPpuOpenGlBgBitmap* context, const GbaPpuMemory* memory,
    const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuDirtyBits* dirty_bits, GLuint fbo, GLsizei width, GLsizei height,
    uint8_t y);

void GbaPpuOpenGlBgBitmapReloadContext(GbaPpuOpenGlBgBitmap* context);

void GbaPpuOpenGlBgBitmapDestroy(GbaPpuOpenGlBgBitmap* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_