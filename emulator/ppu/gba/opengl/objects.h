#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_OBJECTS_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_OBJECTS_

#include <GLES3/gl3.h>
#include <stdalign.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/set.h"

typedef struct {
  GLuint object_indices[GBA_SCREEN_HEIGHT];
  GLfloat object_transformations[OAM_NUM_ROTATE_SCALE_GROUPS + 1u][4u];
  GLuint object_attributes[OAM_NUM_OBJECTS][4u];
  GLuint object_rows[GBA_SCREEN_HEIGHT][4u];
  GLuint object_columns[GBA_SCREEN_WIDTH][4u];
  GLuint object_drawn[4u];
  GLuint object_window[4u];
  GbaPpuSet rows[GBA_SCREEN_HEIGHT];
  GbaPpuSet columns[GBA_SCREEN_WIDTH];
  GLuint textures[7u];
  uint8_t texture_index;
  uint8_t dirty_start;
  uint8_t dirty_end;
  bool dirty;
} OpenGlObjects;

bool OpenGlObjectsLoad(OpenGlObjects* context, const GbaPpuMemory* memory,
                       const GbaPpuRegisters* registers,
                       GbaPpuDirtyBits* dirty_bits);

void OpenGlObjectsBind(OpenGlObjects* context, GLuint program);

void OpenGlObjectsReloadContext(OpenGlObjects* context);

void OpenGlObjectsDestroy(OpenGlObjects* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_OBJECTS_