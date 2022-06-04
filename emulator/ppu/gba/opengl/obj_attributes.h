#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_OBJ_ATTRIBUTES_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_OBJ_ATTRIBUTES_

#include <GLES3/gl3.h>
#include <stdalign.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/set.h"

typedef struct {
  struct {
    GLuint object_columns[GBA_SCREEN_WIDTH][4u];
    GLuint object_rows[GBA_SCREEN_HEIGHT][4u];
    struct {
      GLfloat transformation[2u][4u];
      GLfloat center[2u];
      GLint half_size[2];
      GLint mosaic[2];
      GLint flip[2];
      GLint tile_base;
      GLuint palette;
      GLuint priority;
      GLuint semi_transparent;
      GLuint large_palette;
      GLuint window;
      GLuint padding[2];
    } objects[OAM_NUM_OBJECTS];
    GLuint linear_tiles;
  } staging;
  GLuint buffer;
  GbaPpuSet rows[GBA_SCREEN_HEIGHT];
  GbaPpuSet columns[GBA_SCREEN_WIDTH];
  GbaPpuSet rotations[OAM_NUM_ROTATE_SCALE_GROUPS];
} OpenGlObjectAttributes;

void OpenGlObjectAttributesReload(OpenGlObjectAttributes* context,
                                  const GbaPpuMemory* memory,
                                  const GbaPpuRegisters* registers,
                                  GbaPpuDirtyBits* dirty_bits);

void OpenGlBgObjectAttributesBind(const OpenGlObjectAttributes* context,
                                  GLuint program);

void OpenGlObjectAttributesReloadContext(OpenGlObjectAttributes* context);

void OpenGlObjectAttributesDestroy(OpenGlObjectAttributes* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_OBJ_ATTRIBUTES_