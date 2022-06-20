#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_OBJECTS_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_OBJECTS_

#include <GLES3/gl3.h>
#include <stdalign.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/set.h"

typedef struct {
  struct {
    GLfloat object_transformations[OAM_NUM_ROTATE_SCALE_GROUPS + 1u][2u][4u];
    GLuint object_attributes[OAM_NUM_OBJECTS][4u];
    GLuint object_columns[GBA_SCREEN_WIDTH][4u];
    GLuint object_rows[GBA_SCREEN_HEIGHT][4u];
    GLuint object_window[4u];
    GLuint object_drawn[4u];
  } staging;
  GbaPpuSet rows[GBA_SCREEN_HEIGHT];
  GbaPpuSet columns[GBA_SCREEN_WIDTH];
  GLuint buffer;
  bool dirty;
} OpenGlObjects;

bool OpenGlObjectsStage(OpenGlObjects* context, const GbaPpuMemory* memory,
                        const GbaPpuRegisters* registers,
                        GbaPpuDirtyBits* dirty_bits);

void OpenGlObjectsBind(const OpenGlObjects* context, GLuint program);

void OpenGlObjectsReload(OpenGlObjects* context);

void OpenGlObjectsReloadContext(OpenGlObjects* context);

void OpenGlObjectsDestroy(OpenGlObjects* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_OBJECTS_