#include "emulator/ppu/gba/software/obj_visibility.h"

void GbaPpuObjectVisibilityHidden(GbaPpuObjectVisibility* visibility,
                                  const GbaPpuObjectAttributeMemory* oam,
                                  uint_fast8_t object) {
  if (visibility->object_coordinates[object].pixel_x_size == 0u) {
    return;
  }

  for (uint8_t x = visibility->object_coordinates[object].pixel_x_start;
       x < visibility->object_coordinates[object].pixel_x_end; x++) {
    GbaPpuSetRemove(visibility->x_sets + x, object);
  }

  for (uint8_t y = visibility->object_coordinates[object].pixel_y_start;
       y < visibility->object_coordinates[object].pixel_y_end; y++) {
    GbaPpuSetRemove(visibility->y_sets + y, object);
  }

  visibility->object_coordinates[object].pixel_x_size = 0u;
}

void GbaPpuObjectVisibilityDrawn(GbaPpuObjectVisibility* visibility,
                                 const GbaPpuObjectAttributeMemory* oam,
                                 uint_fast8_t object) {
  assert(visibility->object_coordinates[object].pixel_x_size == 0u);

  static const int_fast16_t shape_size_to_x_size_pixels[4][4] = {
      {8u, 16u, 32u, 64u},
      {16u, 32u, 32u, 64u},
      {8u, 8u, 16u, 32u},
      {0u, 0u, 0u, 0u}};

  static const int_fast16_t shape_size_to_y_size_pixels[4][4] = {
      {8u, 16u, 32u, 64u},
      {8u, 8u, 16u, 32u},
      {16u, 32u, 32u, 64u},
      {0u, 0u, 0u, 0u}};

  if (!oam->object_attributes[object].affine &&
      oam->object_attributes[object].flex_param_0) {
    visibility->object_coordinates[object].pixel_x_size = 0u;
    return;
  }

  int_fast16_t x_start = oam->object_attributes[object].x_coordinate;
  int_fast16_t x_texture_size =
      shape_size_to_x_size_pixels[oam->object_attributes[object].obj_shape]
                                 [oam->object_attributes[object].obj_size];
  int_fast16_t x_render_size = x_texture_size
                               << oam->object_attributes[object].flex_param_0;
  int_fast16_t x_end = x_start + x_render_size;

  visibility->object_coordinates[object].true_x_start = x_start;
  visibility->object_coordinates[object].true_x_center =
      x_start + (x_render_size >> 1u);

  if (x_start < 0) {
    x_start = 0;
  }

  if (x_end > GBA_SCREEN_WIDTH) {
    x_end = GBA_SCREEN_WIDTH;
  }

  for (int_fast16_t x = x_start; x < x_end; x++) {
    GbaPpuSetAdd(visibility->x_sets + x, object);
  }

  int_fast16_t y_start = oam->object_attributes[object].y_coordinate;
  int_fast16_t y_texture_size =
      shape_size_to_y_size_pixels[oam->object_attributes[object].obj_shape]
                                 [oam->object_attributes[object].obj_size];
  int_fast16_t y_render_size = y_texture_size
                               << oam->object_attributes[object].flex_param_0;
  int_fast16_t y_end = y_start + y_render_size;

  // Since there are only 8 bits for storing an object's y coordinate in OAM,
  // there are not enough bits to fully express all of its possible locations
  // on screen as a signed value. To address this, if an object is wholly below
  // the screen, the GBA instead treats the value as an unsigned integer to
  // allow drawing at line 128 and above.
  if (y_end < 0) {
    y_start = oam->object_attributes[object].y_coordinate_u;
    y_end = y_start + y_render_size;
  }

  visibility->object_coordinates[object].true_y_start = y_start;
  visibility->object_coordinates[object].true_y_center =
      y_start + (y_render_size >> 1u);

  if (y_start < 0) {
    y_start = 0;
  }

  if (y_end > GBA_SCREEN_HEIGHT) {
    y_end = GBA_SCREEN_HEIGHT;
  }

  for (int_fast16_t y = y_start; y < y_end; y++) {
    GbaPpuSetAdd(visibility->y_sets + y, object);
  }

  visibility->object_coordinates[object].pixel_x_start = x_start;
  visibility->object_coordinates[object].pixel_x_end = x_end;
  visibility->object_coordinates[object].pixel_x_size = x_texture_size;
  visibility->object_coordinates[object].pixel_y_start = y_start;
  visibility->object_coordinates[object].pixel_y_end = y_end;
  visibility->object_coordinates[object].pixel_y_size = y_texture_size;
}