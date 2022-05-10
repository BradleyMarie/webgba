#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/opengl/affine.h"
#include "emulator/ppu/gba/opengl/mosaic.h"
#include "emulator/ppu/gba/opengl/palette_large.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint vertices;
  GLuint mode3_program;
  GLuint mode3_texture;
  GLuint mode4_program;
  GLuint mode4_textures[2u];
  GLuint mode5_program;
  GLuint mode5_textures[2u];
  union {
    uint16_t colors[GBA_SCREEN_HEIGHT * GBA_SCREEN_WIDTH];
    uint8_t indices[GBA_SCREEN_HEIGHT * GBA_SCREEN_WIDTH];
  } staging;
} GbaPpuOpenGlBgBitmap;

void GbaPpuOpenGlBgBitmapMode3(GbaPpuOpenGlBgBitmap* context,
                               const GbaPpuMemory* memory,
                               const GbaPpuRegisters* registers,
                               GbaPpuOpenGlAffine* affine,
                               GbaPpuOpenGlMosaic* mosaic,
                               GbaPpuDirtyBits* dirty_bits, GLuint fbo);

void GbaPpuOpenGlBgBitmapMode4(GbaPpuOpenGlBgBitmap* context,
                               const GbaPpuMemory* memory,
                               const GbaPpuRegisters* registers,
                               GbaPpuOpenGlAffine* affine,
                               GbaPpuOpenGlMosaic* mosaic,
                               GbaPpuOpenGlLargePalette* palette,
                               GbaPpuDirtyBits* dirty_bits, GLuint fbo);

void GbaPpuOpenGlBgBitmapMode5(GbaPpuOpenGlBgBitmap* context,
                               const GbaPpuMemory* memory,
                               const GbaPpuRegisters* registers,
                               GbaPpuOpenGlAffine* affine,
                               GbaPpuOpenGlMosaic* mosaic,
                               GbaPpuDirtyBits* dirty_bits, GLuint fbo);

void GbaPpuOpenGlBgBitmapReloadContext(GbaPpuOpenGlBgBitmap* context);

void GbaPpuOpenGlBgBitmapDestroy(GbaPpuOpenGlBgBitmap* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_