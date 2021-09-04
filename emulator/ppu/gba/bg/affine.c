#include "emulator/ppu/gba/bg/affine.h"

typedef enum {
  GBA_PPU_AFFINE_BG2 = 0,
  GBA_PPU_AFFINE_BG3 = 1,
} GbaPpuAffineBackground;

static bool GbaPpuAffineMosaic(const GbaPpuRegisters* registers, uint_fast8_t x,
                               uint_fast8_t y, GbaPpuAffineBackground bg_index,
                               int32_t* mosaic_pixel_x,
                               int32_t* mosaic_pixel_y) {
  if (!registers->bgcnt[2u + bg_index].mosaic) {
    return false;
  }

  uint_fast16_t mosaic_size_x = registers->mosaic.bg_horiz + 1u;
  uint_fast16_t mosaic_size_y = registers->mosaic.bg_vert + 1u;

  uint_fast16_t mosaic_offset_x = x % mosaic_size_x;
  uint_fast16_t mosaic_offset_y = y % mosaic_size_y;
  if (mosaic_offset_x == 0u && mosaic_offset_y == 0u) {
    return false;
  }

  *mosaic_pixel_x = (x / mosaic_size_x) << 8u;
  *mosaic_pixel_y = (y / mosaic_size_y) << 8u;

  return true;
}

static void GbaPpuAffineBackgroundPixelColor(
    const GbaPpuMemory* memory, const GbaPpuRegisters* registers,
    GbaPpuInternalRegisters* internal_registers, uint_fast8_t x, uint_fast8_t y,
    GbaPpuAffineBackground bg_index, GbaPpuScreen* screen) {
  if (x == 0) {
    if (y == 0) {
      internal_registers->affine[bg_index].x_row_start =
          registers->affine[bg_index].x;
      internal_registers->affine[bg_index].y_row_start =
          registers->affine[bg_index].y;
    }
    internal_registers->affine[bg_index].x =
        internal_registers->affine[bg_index].x_row_start;
    internal_registers->affine[bg_index].y =
        internal_registers->affine[bg_index].y_row_start;
  }

  int32_t lookup_x, lookup_y;
  if (!GbaPpuAffineMosaic(registers, x, y, bg_index, &lookup_x, &lookup_y)) {
    lookup_x = internal_registers->affine[bg_index].x;
    lookup_y = internal_registers->affine[bg_index].y;
  }

  lookup_x += registers->affine[bg_index].x;
  lookup_y += registers->affine[bg_index].y;

  uint32_t pixel_x = lookup_x >> 8u;
  uint32_t pixel_y = lookup_y >> 8u;

  internal_registers->affine[bg_index].x += registers->affine[bg_index].pa;
  internal_registers->affine[bg_index].y += registers->affine[bg_index].pc;

  if (x == GBA_SCREEN_WIDTH - 1u) {
    internal_registers->affine[bg_index].x_row_start +=
        registers->affine[bg_index].pb;
    internal_registers->affine[bg_index].y_row_start +=
        registers->affine[bg_index].pd;
  }

  static const uint_fast16_t screen_sizes_pixels[4] = {128u, 256u, 512u, 1024u};
  static const uint_fast16_t screen_sizes_tiles[4] = {16u, 32u, 64u, 128u};
  uint_fast16_t screen_size =
      screen_sizes_pixels[registers->bgcnt[2u + bg_index].size];

  if (registers->bgcnt[2u + bg_index].wraparound) {
    static const uint_fast16_t screen_size_masks[4] = {0x07Fu, 0x0FFu, 0x1FFu,
                                                       0x3FFu};
    pixel_x &= screen_size_masks[registers->bgcnt[2u + bg_index].size];
    pixel_y &= screen_size_masks[registers->bgcnt[2u + bg_index].size];
  } else if (pixel_x >= screen_size || pixel_y >= screen_size) {
    return;
  }

  uint_fast16_t screen_size_tiles =
      screen_sizes_tiles[registers->bgcnt[2u + bg_index].size];

  uint_fast8_t tile_x = pixel_x / GBA_TILE_1D_SIZE;
  uint_fast8_t tile_y = pixel_y / GBA_TILE_1D_SIZE;

  uint8_t tile_index =
      memory->vram.mode_012.bg.tile_map
          .blocks[registers->bgcnt[2u + bg_index].tile_map_base_block]
          .indices[tile_y * screen_size_tiles + tile_x];

  uint_fast8_t tile_pixel_x = pixel_x % GBA_TILE_1D_SIZE;
  uint_fast8_t tile_pixel_y = pixel_y % GBA_TILE_1D_SIZE;

  // TODO: Handle accesses to obj tiles
  uint8_t color_index =
      memory->vram.mode_012.bg.tiles
          .blocks[registers->bgcnt[2u + bg_index].tile_base_block]
          .d_tiles[tile_index]
          .pixels[tile_pixel_y][tile_pixel_x];
  if (color_index == 0u) {
    return;
  }

  uint16_t color = memory->palette.bg.large_palette[color_index];

  GbaPpuScreenDrawPixel(screen, x, y, color,
                        registers->bgcnt[2u + bg_index].priority);
}

void GbaPpuBackground2Mode1Pixel(const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuInternalRegisters* internal_registers,
                                 uint_fast8_t x, uint_fast8_t y,
                                 GbaPpuScreen* screen) {
  if (!registers->dispcnt.bg2_enable) {
    return;
  }

  GbaPpuAffineBackgroundPixelColor(memory, registers, internal_registers, x, y,
                                   GBA_PPU_AFFINE_BG2, screen);
}

void GbaPpuBackground2Mode2Pixel(const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuInternalRegisters* internal_registers,
                                 uint_fast8_t x, uint_fast8_t y,
                                 GbaPpuScreen* screen) {
  if (!registers->dispcnt.bg2_enable) {
    return;
  }

  GbaPpuAffineBackgroundPixelColor(memory, registers, internal_registers, x, y,
                                   GBA_PPU_AFFINE_BG2, screen);
}

void GbaPpuBackground3Mode2Pixel(const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuInternalRegisters* internal_registers,
                                 uint_fast8_t x, uint_fast8_t y,
                                 GbaPpuScreen* screen) {
  if (!registers->dispcnt.bg3_enable) {
    return;
  }

  GbaPpuAffineBackgroundPixelColor(memory, registers, internal_registers, x, y,
                                   GBA_PPU_AFFINE_BG3, screen);
}