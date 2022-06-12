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
    struct {
      GLfloat transformation[2u][4u];
      GLfloat center[2u];
      GLint half_size[2u];
      GLint mosaic[2u];
      GLint flip[2u];
      GLint tile_base;
      GLuint palette;
      GLuint priority;
      GLuint semi_transparent;
      GLuint large_palette;
      GLuint padding[3u];
    } objects[OAM_NUM_OBJECTS];
    GLuint linear_tiles;
    GLuint padding[3u];
  } object_staging;
  struct {
    GLuint object_columns[GBA_SCREEN_WIDTH][4u];
    GLuint object_rows[GBA_SCREEN_HEIGHT][4u];
    GLuint object_indices[OAM_NUM_OBJECTS][4];
    GLuint object_window[4u];
    GLuint object_drawn[4u];
  } visibility_staging;
  GLuint buffers[2u];
  uint8_t begin[OAM_NUM_OBJECTS][2u];
  uint8_t end[OAM_NUM_OBJECTS][2u];
  GbaPpuSet rows[GBA_SCREEN_HEIGHT];
  GbaPpuSet columns[GBA_SCREEN_WIDTH];
  GbaPpuSet rotations[OAM_NUM_ROTATE_SCALE_GROUPS];
  GbaPpuSet layers[4u];
  GbaPpuSet window;
  bool objects_dirty;
  bool visibility_dirty;
} OpenGlObjectAttributes;

bool OpenGlObjectAttributesStage(OpenGlObjectAttributes* context,
                                 const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuDirtyBits* dirty_bits);

void OpenGlObjectAttributesBind(const OpenGlObjectAttributes* context,
                                GLuint program);

void OpenGlObjectAttributesReload(OpenGlObjectAttributes* context);

void OpenGlObjectAttributesReloadContext(OpenGlObjectAttributes* context);

void OpenGlObjectAttributesDestroy(OpenGlObjectAttributes* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_OBJ_ATTRIBUTES_