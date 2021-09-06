#include "emulator/ppu/gba/bg/scrolling.h"

static inline bool GbaPpuScrollingMosaic(const GbaPpuRegisters* registers,
                                         uint_fast8_t x, uint_fast8_t y,
                                         GbaPpuScrollingBackground background,
                                         uint_fast16_t* mosaic_pixel_x,
                                         uint_fast16_t* mosaic_pixel_y) {
  if (!registers->bgcnt[background].mosaic) {
    return false;
  }

  uint_fast16_t mosaic_size_x = registers->mosaic.bg_horiz + 1u;
  uint_fast16_t mosaic_size_y = registers->mosaic.bg_vert + 1u;

  uint_fast16_t mosaic_offset_x = x % mosaic_size_x;
  uint_fast16_t mosaic_offset_y = y % mosaic_size_y;
  if (mosaic_offset_x == 0u && mosaic_offset_y == 0u) {
    return false;
  }

  *mosaic_pixel_x = x / mosaic_size_x;
  *mosaic_pixel_y = y / mosaic_size_y;

  return true;
}

void GbaPpuScrollingBackgroundPixel(
    const GbaPpuMemory* memory, const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuScrollingBackground background, uint_fast8_t x, uint_fast8_t y,
    GbaPpuScreen* screen) {
  static const uint_fast16_t bg_mask_x[4] = {0xFFu, 0x1FFu, 0xFFu, 0x1FFu};
  static const uint_fast16_t bg_mask_y[4] = {0xFFu, 0xFFu, 0x1FFu, 0x1FFu};
  static const uint_fast8_t bg_tile_block_width[4] = {1u, 2u, 1u, 2u};

  uint_fast16_t lookup_x, lookup_y;
  if (!GbaPpuScrollingMosaic(registers, x, y, background, &lookup_x,
                             &lookup_y)) {
    lookup_x = x;
    lookup_y = y;
  }

  lookup_x += registers->bg_offsets[background].x;
  lookup_x &= bg_mask_x[registers->bgcnt[background].size];

  lookup_y += registers->bg_offsets[background].y;
  lookup_y &= bg_mask_y[registers->bgcnt[background].size];

  uint_fast8_t lookup_x_block = lookup_x / GBA_TILE_MAP_BLOCK_1D_SIZE_PIXELS;
  uint_fast8_t lookup_y_block = lookup_y / GBA_TILE_MAP_BLOCK_1D_SIZE_PIXELS;
  uint_fast8_t tile_map_block =
      registers->bgcnt[background].tile_map_base_block +
      bg_tile_block_width[registers->bgcnt[background].size] * lookup_y_block +
      lookup_x_block;

  uint_fast8_t lookup_x_block_tile =
      (lookup_x % GBA_TILE_MAP_BLOCK_1D_SIZE_PIXELS) / GBA_TILE_1D_SIZE;
  uint_fast8_t lookup_y_block_tile =
      (lookup_y % GBA_TILE_MAP_BLOCK_1D_SIZE_PIXELS) / GBA_TILE_1D_SIZE;

  TileMapEntry entry = memory->vram.mode_012.bg.tile_map.blocks[tile_map_block]
                           .entries[lookup_y_block_tile][lookup_x_block_tile];

  uint_fast8_t lookup_x_tile_pixel = lookup_x % GBA_TILE_1D_SIZE;
  uint_fast8_t lookup_y_tile_pixel = lookup_y % GBA_TILE_1D_SIZE;

  if (entry.h_flip) {
    lookup_x_tile_pixel = GBA_TILE_1D_SIZE - 1u - lookup_x_tile_pixel;
  }

  if (entry.v_flip) {
    lookup_y_tile_pixel = GBA_TILE_1D_SIZE - 1u - lookup_y_tile_pixel;
  }

  // TODO: Handle accesses to obj tiles
  uint16_t color;
  if (registers->bgcnt[background].large_palette) {
    uint8_t color_index =
        memory->vram.mode_012.bg.tiles
            .blocks[registers->bgcnt[background].tile_base_block]
            .d_tiles[entry.index]
            .pixels[lookup_y_tile_pixel][lookup_x_tile_pixel];
    if (color_index == 0u) {
      return;
    }

    color = memory->palette.bg.large_palette[color_index];
  } else {
    uint8_t color_index_pair =
        memory->vram.mode_012.bg.tiles
            .blocks[registers->bgcnt[background].tile_base_block]
            .s_tiles[entry.index]
            .pixels[lookup_y_tile_pixel][lookup_x_tile_pixel >> 1u]
            .value;

    // Select lower 4 bits if lookup_x_tile_pixel is even and upper 4 bits
    // if lookup_x_tile_pixel is odd.
    uint_fast8_t color_index =
        (color_index_pair >> ((lookup_x_tile_pixel & 1u) << 2u)) & 0xFu;
    if (color_index == 0u) {
      return;
    }

    color = memory->palette.bg.small_palettes[entry.palette][color_index];
  }

  GbaPpuScreenDrawPixel(screen, x, y, color,
                        registers->bgcnt[background].priority);
}