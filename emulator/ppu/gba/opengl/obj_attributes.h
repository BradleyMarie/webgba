#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_OBJ_ATTRIBUTES_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_OBJ_ATTRIBUTES_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/set.h"

typedef struct {
  struct {
    bool enabled;
    GLfloat affine[2u][2u];
    GLfloat origin[2u];
    GLfloat sprite_size[2u];
    GLfloat render_size[2u];
    GLfloat mosaic[2u];
    GLfloat flip[2u];
    GLfloat tile_base;
    GLfloat palette;
    GLint large_palette;
    GLint rendered;
    GLint blended;
    GLint priority;
  } attributes[OAM_NUM_OBJECTS];
  GbaPpuSet rotations[OAM_NUM_ROTATE_SCALE_GROUPS];
} OpenGlObjectAttributes;

void OpenGlObjectAttributesReload(OpenGlObjectAttributes* context,
                                  const GbaPpuMemory* memory,
                                  const GbaPpuRegisters* registers,
                                  GbaPpuDirtyBits* dirty_bits);

void OpenGlBgObjectAttributesBind(const OpenGlObjectAttributes* context,
                                  GLuint program);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_OBJ_ATTRIBUTES_