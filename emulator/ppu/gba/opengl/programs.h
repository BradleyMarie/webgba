#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PROGRAMS_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PROGRAMS_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/opengl/uniform_locations.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  UniformLocations all_uniform_locations[6u][2u][2u][2u][2u][2u];
  const UniformLocations* uniform_locations;
  const UniformLocations* staging_uniform_locations;
  GLuint programs[6u][2u][2u][2u][2u][2u];
  GLuint mode0[2u][2u][2u][2u][2u];
  GLuint mode1[2u][2u][2u][2u];
  GLuint mode2[2u][2u][2u];
  GLuint mode3[2u][2u];
  GLuint mode4[2u][2u];
  GLuint mode5[2u][2u];
  GLuint program;
  GLuint staging;
} OpenGlPrograms;

bool OpenGlProgramsStage(OpenGlPrograms* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits);

void OpenGlProgramsGet(const OpenGlPrograms* context, GLuint* program,
                       const UniformLocations** locations);

void OpenGlProgramsReload(OpenGlPrograms* context);

void OpenGlProgramsReloadContext(OpenGlPrograms* context);

void OpenGlProgramsDestroy(OpenGlPrograms* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PROGRAMS_