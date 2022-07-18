#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/opengl/uniform_locations.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  float base_row;
  int32_t bases[GBA_PPU_NUM_AFFINE_BACKGROUNDS][2u];
  int32_t base_dx[GBA_PPU_NUM_AFFINE_BACKGROUNDS][2u];
  int32_t base_dy[GBA_PPU_NUM_AFFINE_BACKGROUNDS][2u];
  float next_row;
  int32_t next_bases[GBA_PPU_NUM_AFFINE_BACKGROUNDS][2u];
  int32_t next_dx[GBA_PPU_NUM_AFFINE_BACKGROUNDS][2u];
  int32_t next_dy[GBA_PPU_NUM_AFFINE_BACKGROUNDS][2u];
  int32_t final_bases[GBA_PPU_NUM_AFFINE_BACKGROUNDS][2u];
  int32_t final_dx[GBA_PPU_NUM_AFFINE_BACKGROUNDS][2u];
  int32_t final_dy[GBA_PPU_NUM_AFFINE_BACKGROUNDS][2u];
  bool final_row;
} OpenGlBgAffine;

bool OpenGlBgAffineStage(OpenGlBgAffine* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits);

void OpenGlBgAffineBind(const OpenGlBgAffine* context,
                        const UniformLocations* locations);

void OpenGlBgAffineReload(OpenGlBgAffine* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_