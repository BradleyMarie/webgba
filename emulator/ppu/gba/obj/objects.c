#include "emulator/ppu/gba/obj/objects.h"

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

void GbaPpuObjectsPixel(const GbaPpuMemory* memory,
                        const GbaPpuRegisters* registers,
                        const GbaPpuObjectState* object_state,
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

  GbaPpuObjectSet objects = GbaPpuObjectStateGetObjects(object_state, x, y);
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

      lookup_x <<= 8u;
      lookup_x += memory->oam.rotate_scale[group].pa * lookup_x +
                  memory->oam.rotate_scale[group].pb * lookup_y;
      lookup_x >>= 8u;

      int_fast16_t x_size =
          shape_size_to_x_size_pixels
              [memory->oam.object_attributes[object].obj_shape]
              [memory->oam.object_attributes[object].obj_size]
          << memory->oam.object_attributes[object].flex_param_0;
      if (lookup_x < 0 || lookup_x >= x_size) {
        continue;
      }

      lookup_y <<= 8u;
      lookup_y += memory->oam.rotate_scale[group].pc * lookup_x +
                  memory->oam.rotate_scale[group].pd * lookup_y;
      lookup_y >>= 8u;

      int_fast16_t y_size =
          shape_size_to_y_size_pixels
              [memory->oam.object_attributes[object].obj_shape]
              [memory->oam.object_attributes[object].obj_size]
          << memory->oam.object_attributes[object].flex_param_0;

      if (lookup_y < 0 || lookup_y >= y_size) {
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
        return;
      }

      color = memory->palette.obj.large_palette[color_index];
    } else {
      uint8_t color_index_pair = memory->vram.mode_012.obj.s_tiles[tile_index]
                                     .pixels[y_tile_pixel][x_tile_pixel]
                                     .value;

      // Select lower 4 bits if lookup_x_tile_pixel is even and upper 4 bits
      // if lookup_x_tile_pixel is odd.
      uint_fast8_t color_index =
          (color_index_pair >> ((x_tile_pixel & 1u) << 2u)) & 0xFu;
      if (color_index == 0u) {
        return;
      }

      color = memory->palette.obj
                  .small_palettes[memory->oam.object_attributes[object].palette]
                                 [color_index];
    }

    // TODO: Transparency and Windowing

    // TODO: Objects should have higher priorities than background layers
    GbaPpuScreenDrawPixel(screen, x, y, color,
                          memory->oam.object_attributes[object].priority);
  }
}