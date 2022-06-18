#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PROGRAMS_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PROGRAMS_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
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

GLuint OpenGlProgramsGet(const OpenGlPrograms* context);

void OpenGlProgramsReload(OpenGlPrograms* context);

void OpenGlProgramsReloadContext(OpenGlPrograms* context);

void OpenGlProgramsDestroy(OpenGlPrograms* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PROGRAMS_