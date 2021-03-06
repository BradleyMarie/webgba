#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_OPENGL_UNIFORM_LOCATIONS_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_OPENGL_UNIFORM_LOCATIONS_

#include <GLES3/gl3.h>

typedef struct {
  GLint render_scale;
  GLint row_start;
  GLint num_rows;
  GLint background_control;
  GLint background_palette;
  GLint object_palette;
  GLint background_tiles;
  GLint object_tiles;
  GLint affine_tilemap;
  GLint scrolling_tilemap;
  GLint bitmap;
  GLint palette_bitmap;
  GLint object_transformations;
  GLint object_attributes;
  GLint object_window;
  GLint object_drawn;
  GLint scrolling_coordinates;
  GLint affine_base_row;
  GLint affine_base[2u];
  GLint affine_delta[2u];
  GLint window_and_bounds;
  GLint blend_control_ev;
} UniformLocations;

void FillUniformLocations(GLuint program, UniformLocations* locations);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_OPENGL_UNIFORM_LOCATIONS_