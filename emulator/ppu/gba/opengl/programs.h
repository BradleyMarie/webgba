#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PROGRAMS_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PROGRAMS_

#include <GLES3/gl3.h>
#include <stdbool.h>

typedef struct {
  GLuint programs[6u][2u][2u][2u][2u][2u];
  GLuint mode0[2u][2u][2u][2u][2u];
  GLuint mode1[2u][2u][2u][2u];
  GLuint mode2[2u][2u][2u];
  GLuint mode35[2u][2u];
  GLuint mode4[2u][2u];
} OpenGlPrograms;

GLuint OpenGlProgramsGet(const OpenGlPrograms* context, uint8_t mode,
                         bool objects, bool bg0, bool bg1, bool bg2, bool bg3);

void OpenGlProgramsReloadContext(OpenGlPrograms* context);

void OpenGlProgramsDestroy(OpenGlPrograms* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PROGRAMS_