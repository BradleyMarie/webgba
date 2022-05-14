#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PROGRAM_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PROGRAM_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint blank;
  GLuint mode3;
  GLuint mode4;
  GLuint mode5;
} GbaOpenGlProgram;

void GbaOpenGlProgramSelect(GbaOpenGlProgram* context,
                            const GbaPpuRegisters* registers);

GLuint GbaOpenGlProgramGet(const GbaOpenGlProgram* context);

void GbaOpenGlProgramReloadContet(GbaOpenGlProgram* context);

void GbaOpenGlProgramDestroy(GbaOpenGlProgram* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PROGRAM_