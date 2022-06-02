#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_OBJ_ATTRIBUTES_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_OBJ_ATTRIBUTES_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/set.h"

typedef struct {
  struct {
    GLint mosaic[2u];
    GLint tile_base;
    GLint palette;
    bool affine;
    bool flip_x;
    bool flip_y;
    bool large_palette;
    bool semi_transparent;
    bool window;
    GLint priority;
  } attributes[OAM_NUM_OBJECTS];
  GLfloat center_and_half_size[OAM_NUM_OBJECTS][4u];
  GLfloat transformations[OAM_NUM_OBJECTS][4u];
  uint32_t visibility_staging[2u][GBA_SCREEN_WIDTH][4u];
  GLuint object_center_and_half_size;
  GLuint object_transformations;
  GLuint object_visibility;
  GbaPpuSet rows[GBA_SCREEN_HEIGHT];
  GbaPpuSet columns[GBA_SCREEN_WIDTH];
  GbaPpuSet rotations[OAM_NUM_ROTATE_SCALE_GROUPS];
} OpenGlObjectAttributes;

void OpenGlObjectAttributesReload(OpenGlObjectAttributes* context,
                                  const GbaPpuMemory* memory,
                                  const GbaPpuRegisters* registers,
                                  GbaPpuDirtyBits* dirty_bits);

void OpenGlObjectAttributesReloadContext(OpenGlObjectAttributes* context);

void OpenGlBgObjectAttributesBind(const OpenGlObjectAttributes* context,
                                  GLuint program);

void OpenGlObjectAttributesDestroy(OpenGlObjectAttributes* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_OBJ_ATTRIBUTES_