#include "emulator/ppu/gba/opengl/uniform_locations.h"

void FillUniformLocations(GLuint program, UniformLocations* locations) {
  locations->render_scale = glGetUniformLocation(program, "render_scale");
  locations->row_start = glGetUniformLocation(program, "row_start");
  locations->num_rows = glGetUniformLocation(program, "num_rows");
  locations->background_control =
      glGetUniformLocation(program, "background_control");
  locations->background_palette =
      glGetUniformLocation(program, "background_palette");
  locations->object_palette = glGetUniformLocation(program, "object_palette");
  locations->background_tiles =
      glGetUniformLocation(program, "background_tiles");
  locations->object_tiles = glGetUniformLocation(program, "object_tiles");
  locations->affine_tilemap = glGetUniformLocation(program, "affine_tilemap");
  locations->scrolling_tilemap =
      glGetUniformLocation(program, "scrolling_tilemap");
  locations->bitmap = glGetUniformLocation(program, "bitmap");
  locations->palette_bitmap = glGetUniformLocation(program, "palette_bitmap");
  locations->object_transformations =
      glGetUniformLocation(program, "object_transformations");
  locations->object_attributes =
      glGetUniformLocation(program, "object_attributes");
  locations->object_rows = glGetUniformLocation(program, "object_rows");
  locations->object_columns = glGetUniformLocation(program, "object_columns");
  locations->object_window = glGetUniformLocation(program, "object_window");
  locations->object_drawn = glGetUniformLocation(program, "object_drawn");
  locations->scrolling_coordinates =
      glGetUniformLocation(program, "scrolling_coordinates");
  locations->affine_base_row = glGetUniformLocation(program, "affine_base_row");
  locations->affine_base[0u] = glGetUniformLocation(program, "affine_base[0]");
  locations->affine_base[1u] = glGetUniformLocation(program, "affine_base[1]");
  locations->affine_delta[0u] =
      glGetUniformLocation(program, "affine_delta[0]");
  locations->affine_delta[1u] =
      glGetUniformLocation(program, "affine_delta[1]");
  locations->window_and_bounds =
      glGetUniformLocation(program, "window_and_bounds");
  locations->blend_control_ev =
      glGetUniformLocation(program, "blend_control_ev");
}