#include "emulator/ppu/gba/obj/draw.h"

static inline bool GbaPpuObjectMosaic(const GbaPpuRegisters* registers,
                                      const GbaPpuMemory* memory,
                                      uint_fast8_t x, uint_fast8_t y,
                                      uint_fast8_t object,
                                      int32_t* mosaic_lookup_x,
                                      int32_t* mosaic_lookup_y) {
  if (!memory->oam.object_attributes[object].obj_mosaic) {
    return false;
  }

  uint_fast16_t mosaic_size_x = registers->mosaic.bg_horiz + 1u;
  uint_fast16_t mosaic_size_y = registers->mosaic.bg_vert + 1u;

  uint_fast16_t mosaic_offset_x = x % mosaic_size_x;
  uint_fast16_t mosaic_offset_y = y % mosaic_size_y;
  if (mosaic_offset_x == 0u && mosaic_offset_y == 0u) {
    return false;
  }

  *mosaic_lookup_x = x / mosaic_size_x;
  *mosaic_lookup_y = y / mosaic_size_y;

  return true;
}

void GbaPpuObjectPixel(const GbaPpuMemory* memory,
                       const GbaPpuRegisters* registers,
                       const GbaPpuObjectVisibility* visibility,
                       const uint_fast8_t x, uint_fast8_t y,
                       GbaPpuScreen* screen) {
  static const int_fast16_t shape_size_to_x_size_pixels[4][4] = {
      {8u, 16u, 32u, 64u},
      {16u, 32u, 32u, 64u},
      {8u, 8u, 16u, 32u},
      {0u, 0u, 0u, 0u}};

  static const int_fast16_t shape_size_to_x_size_tiles[4][4] = {
      {1u, 2u, 4u, 8u}, {2u, 4u, 4u, 8u}, {1u, 1u, 2u, 4u}, {0u, 0u, 0u, 0u}};

  static const int_fast16_t shape_size_to_y_size_pixels[4][4] = {
      {8u, 16u, 32u, 64u},
      {8u, 8u, 16u, 32u},
      {16u, 32u, 32u, 64u},
      {0u, 0u, 0u, 0u}};

  GbaPpuObjectSet objects = GbaPpuObjectVisibilityGet(visibility, x, y);
  while (!GbaPpuObjectSetEmpty(&objects)) {
    uint_fast8_t object = GbaPpuObjectSetPop(&objects);
    assert(memory->oam.object_attributes[object].affine ||
           !memory->oam.object_attributes[object].flex_param_0);

    int32_t lookup_x, lookup_y;
    if (!GbaPpuObjectMosaic(registers, memory, x, y, object, &lookup_x,
                            &lookup_y)) {
      lookup_x = x;
      lookup_y = y;
    }

    lookup_x -= memory->oam.object_attributes[object].x_coordinate;
    lookup_y -= memory->oam.object_attributes[object].y_coordinate;

    if (memory->oam.object_attributes[object].affine) {
      unsigned char group = memory->oam.object_attributes[object].flex_param_1;

      int_fast16_t obj_size_x = shape_size_to_x_size_pixels
          [memory->oam.object_attributes[object].obj_shape]
          [memory->oam.object_attributes[object].obj_size];
      int_fast16_t obj_size_y = shape_size_to_y_size_pixels
          [memory->oam.object_attributes[object].obj_shape]
          [memory->oam.object_attributes[object].obj_size];

      if (memory->oam.object_attributes[object].flex_param_0) {
        int_fast16_t center_x = obj_size_x;
        int_fast16_t center_y = obj_size_y;

        int_fast16_t from_center_x = lookup_x - center_x;
        int_fast16_t from_center_y = lookup_y - center_y;

        int_fast32_t x_rotation =
            memory->oam.rotate_scale[group].pa * from_center_x +
            memory->oam.rotate_scale[group].pb * from_center_y;

        int_fast32_t y_rotation =
            memory->oam.rotate_scale[group].pc * from_center_x +
            memory->oam.rotate_scale[group].pd * from_center_y;

        lookup_x = center_x + (x_rotation >> 8u) - (obj_size_x >> 1u);
        lookup_y = center_y + (y_rotation >> 8u) - (obj_size_y >> 1u);
      } else {
        int_fast16_t center_x = obj_size_x >> 1u;
        int_fast16_t center_y = obj_size_y >> 1u;

        int_fast16_t from_center_x = lookup_x - center_x;
        int_fast16_t from_center_y = lookup_y - center_y;

        int_fast32_t x_rotation =
            memory->oam.rotate_scale[group].pa * from_center_x +
            memory->oam.rotate_scale[group].pb * from_center_y;

        int_fast32_t y_rotation =
            memory->oam.rotate_scale[group].pc * from_center_x +
            memory->oam.rotate_scale[group].pd * from_center_y;

        lookup_x = center_x + (x_rotation >> 8u);
        lookup_y = center_y + (y_rotation >> 8u);
      }

      if (lookup_x < 0 || lookup_x >= obj_size_x) {
        continue;
      }

      if (lookup_y < 0 || lookup_y >= obj_size_y) {
        continue;
      }
    } else {
      // Horizontal Flip
      if (memory->oam.object_attributes[object].flex_param_1 & 0x8u) {
        int_fast16_t x_size = shape_size_to_x_size_pixels
            [memory->oam.object_attributes[object].obj_shape]
            [memory->oam.object_attributes[object].obj_size];
        lookup_x = x_size - lookup_x - 1;
      }

      // Vertical Flip
      if (memory->oam.object_attributes[object].flex_param_1 & 0x10u) {
        int_fast16_t y_size = shape_size_to_y_size_pixels
            [memory->oam.object_attributes[object].obj_shape]
            [memory->oam.object_attributes[object].obj_size];
        lookup_y = y_size - lookup_y - 1;
      }
    }

    unsigned short character_name =
        memory->oam.object_attributes[object].character_name;
    if (registers->dispcnt.mode >= 3u) {
      static const uint_least16_t minimum_index[2] = {
          GBA_BITMAP_MODE_NUM_OBJECT_D_TILES,
          GBA_BITMAP_MODE_NUM_OBJECT_D_TILES};
      if (character_name <
          minimum_index[memory->oam.object_attributes[object].palette_mode]) {
        continue;
      }
    }

    unsigned short x_tile = lookup_x / 8u;
    unsigned short y_tile = lookup_y / 8u;

    unsigned short tile_index;
    if (registers->dispcnt.object_mode) {
      // One Dimensional Lookup
      int_fast16_t x_size = shape_size_to_x_size_tiles
          [memory->oam.object_attributes[object].obj_shape]
          [memory->oam.object_attributes[object].obj_size];
      tile_index = character_name + y_tile * x_size + x_tile;
    } else {
      // Two Dimensional Lookup
      tile_index = character_name + y_tile * 32u + x_tile;
    }

    uint_fast8_t x_tile_pixel = lookup_x & 0x7u;
    uint_fast8_t y_tile_pixel = lookup_y & 0x7u;

    uint16_t color;
    if (memory->oam.object_attributes[object].palette_mode) {
      uint8_t color_index = memory->vram.mode_012.obj.d_tiles[tile_index >> 1u]
                                .pixels[y_tile_pixel][x_tile_pixel];
      if (color_index == 0u) {
        continue;
      }

      color = memory->palette.obj.large_palette[color_index];
    } else {
      uint8_t color_index_pair = memory->vram.mode_012.obj.s_tiles[tile_index]
                                     .pixels[y_tile_pixel][x_tile_pixel >> 1u]
                                     .value;

      // Select lower 4 bits if lookup_x_tile_pixel is even and upper 4 bits
      // if lookup_x_tile_pixel is odd.
      uint_fast8_t color_index =
          (color_index_pair >> ((x_tile_pixel & 1u) << 2u)) & 0xFu;
      if (color_index == 0u) {
        continue;
      }

      color = memory->palette.obj
                  .small_palettes[memory->oam.object_attributes[object].palette]
                                 [color_index];
    }

    // TODO: Transparency and Windowing

    GbaPpuScreenDrawObjectPixel(screen, x, y, color,
                                memory->oam.object_attributes[object].priority);
  }
}