#include "emulator/ppu/gba/bg/linear.h"

typedef enum {
  GBA_PPU_LINEAR_BG0,
  GBA_PPU_LINEAR_BG1,
  GBA_PPU_LINEAR_BG2,
  GBA_PPU_LINEAR_BG3,
} GbaPpuLinearBackground;

static bool GbaPpuLinearMosaic(const GbaPpuRegisters* registers, uint_fast8_t x,
                               uint_fast8_t y, uint_fast8_t bg_index,
                               uint_fast8_t* mosaic_pixel_x,
                               uint_fast8_t* mosaic_pixel_y) {
  if (!registers->bgcnt[bg_index].mosaic) {
    return false;
  }

  uint_fast8_t mosaic_size_x = registers->mosaic.bg_horiz + 1u;
  uint_fast8_t mosaic_size_y = registers->mosaic.bg_vert + 1u;

  uint_fast8_t mosaic_offset_x = x % mosaic_size_x;
  uint_fast8_t mosaic_offset_y = y % mosaic_size_y;
  if (mosaic_offset_x == 0u && mosaic_offset_y == 0u) {
    return false;
  }

  *mosaic_pixel_x = x / mosaic_size_x;
  *mosaic_pixel_y = y / mosaic_size_y;

  return true;
}

void GbaPpuLinearBackgroundPixelColor(
    const GbaPpuMemory* memory, const GbaPpuRegisters* registers,
    GbaPpuInternalRegisters* internal_registers, uint_fast8_t x, uint_fast8_t y,
    GbaPpuLinearBackground bg_index, uint16_t* color, uint8_t* priority) {
  static const uint_fast16_t bg_size_x[4] = {256u, 512u, 256u, 512u};
  static const uint_fast16_t bg_mask_x[4] = {0xFFu, 0x1FFu, 0xFFu, 0x1FFu};

  static const uint_fast16_t bg_size_y[4] = {256u, 256u, 512u, 512u};
  static const uint_fast16_t bg_mask_y[4] = {0xFFu, 0xFFu, 0x1FFu, 0x1FFu};

  uint_fast8_t lookup_x, lookup_y;
  if (!GbaPpuLinearMosaic(registers, x, y, bg_index, &lookup_x, &lookup_y)) {
    lookup_x = x;
    lookup_y = y;
  }

  lookup_x += registers->bg_offsets[bg_index].x;
  lookup_y += registers->bg_offsets[bg_index].y;

  if (registers->bgcnt[bg_index].wraparound) {
    lookup_x &= bg_mask_x[registers->bgcnt[bg_index].size];
    lookup_y &= bg_mask_y[registers->bgcnt[bg_index].size];
  } else if (lookup_x >= bg_size_x[registers->bgcnt[bg_index].size] ||
             lookup_y >= bg_size_y[registers->bgcnt[bg_index].size]) {
    *color = memory->palette.bg.large_palette[0u];
    *priority = GBA_PPU_SCREEN_TRANSPARENT_PRIORITY;
    return;
  }

  uint_fast8_t lookup_x_block = lookup_x / GBA_TILE_MAP_BLOCK_1D_SIZE_PIXELS;
  uint_fast8_t lookup_y_block = lookup_y / GBA_TILE_MAP_BLOCK_1D_SIZE_PIXELS;
  uint_fast8_t tile_map_block = registers->bgcnt[bg_index].tile_map_base_block +
                                2u * lookup_y_block + lookup_x_block;

  uint_fast8_t lookup_x_block_tile =
      (lookup_x % GBA_TILE_MAP_BLOCK_1D_SIZE_PIXELS) /
      GBA_TILE_MAP_BLOCK_1D_SIZE;
  uint_fast8_t lookup_y_block_tile =
      (lookup_y % GBA_TILE_MAP_BLOCK_1D_SIZE_PIXELS) /
      GBA_TILE_MAP_BLOCK_1D_SIZE;

  TileMapEntry entry = memory->vram.mode_012.bg.tile_map.blocks[tile_map_block]
                           .entries[lookup_x_block_tile][lookup_y_block_tile];

  uint_fast8_t lookup_x_tile_pixel = lookup_x % GBA_TILE_MAP_BLOCK_1D_SIZE;
  uint_fast8_t lookup_y_tile_pixel = lookup_y % GBA_TILE_MAP_BLOCK_1D_SIZE;

  if (entry.h_flip) {
    lookup_x_tile_pixel = GBA_TILE_MAP_BLOCK_1D_SIZE - 1u - lookup_x_tile_pixel;
  }

  if (entry.v_flip) {
    lookup_y_tile_pixel = GBA_TILE_MAP_BLOCK_1D_SIZE - 1u - lookup_y_tile_pixel;
  }

  if (registers->bgcnt[bg_index].palettes) {
    uint8_t shift_amount[2] = {0u, 4u};
    uint8_t color_index_pair =
        memory->vram.mode_012.bg.tiles
            .blocks[registers->bgcnt[bg_index].tile_base_block]
            .s_tiles->pixels[lookup_y_tile_pixel][lookup_x_tile_pixel >> 1u]
            .value;
    color_index_pair >>= shift_amount[lookup_x_tile_pixel & 1u];
    color_index_pair &= 0xFu;
    *color = memory->palette.bg.small_palettes[entry.palette][color_index_pair];
  } else {
    uint8_t color_index =
        memory->vram.mode_012.bg.tiles
            .blocks[registers->bgcnt[bg_index].tile_base_block]
            .d_tiles->pixels[lookup_y_tile_pixel][lookup_x_tile_pixel];
    *color = memory->palette.bg.large_palette[color_index];
  }

  *priority = registers->bgcnt[bg_index].priority;
}

void GbaPpuBackground0Mode0Pixel(const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuInternalRegisters* internal_registers,
                                 uint_fast8_t x, uint_fast8_t y,
                                 GbaPpuScreen* screen) {
  uint16_t color;
  uint8_t priority;
  GbaPpuLinearBackgroundPixelColor(memory, registers, internal_registers, x, y,
                                   GBA_PPU_LINEAR_BG0, &color, &priority);
  GbaPpuScreenDrawPixel(screen, x, y, color, priority);
}

void GbaPpuBackground0Mode1Pixel(const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuInternalRegisters* internal_registers,
                                 uint_fast8_t x, uint_fast8_t y,
                                 GbaPpuScreen* screen) {
  uint16_t color;
  uint8_t priority;
  GbaPpuLinearBackgroundPixelColor(memory, registers, internal_registers, x, y,
                                   GBA_PPU_LINEAR_BG0, &color, &priority);
  GbaPpuScreenDrawPixel(screen, x, y, color, priority);
}

void GbaPpuBackground1Mode0Pixel(const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuInternalRegisters* internal_registers,
                                 uint_fast8_t x, uint_fast8_t y,
                                 GbaPpuScreen* screen) {
  uint16_t color;
  uint8_t priority;
  GbaPpuLinearBackgroundPixelColor(memory, registers, internal_registers, x, y,
                                   GBA_PPU_LINEAR_BG1, &color, &priority);
  GbaPpuScreenDrawPixel(screen, x, y, color, priority);
}

void GbaPpuBackground1Mode1Pixel(const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuInternalRegisters* internal_registers,
                                 uint_fast8_t x, uint_fast8_t y,
                                 GbaPpuScreen* screen) {
  uint16_t color;
  uint8_t priority;
  GbaPpuLinearBackgroundPixelColor(memory, registers, internal_registers, x, y,
                                   GBA_PPU_LINEAR_BG1, &color, &priority);
  GbaPpuScreenDrawPixel(screen, x, y, color, priority);
}

void GbaPpuBackground2Mode0Pixel(const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuInternalRegisters* internal_registers,
                                 uint_fast8_t x, uint_fast8_t y,
                                 GbaPpuScreen* screen) {
  uint16_t color;
  uint8_t priority;
  GbaPpuLinearBackgroundPixelColor(memory, registers, internal_registers, x, y,
                                   GBA_PPU_LINEAR_BG2, &color, &priority);
  GbaPpuScreenDrawPixel(screen, x, y, color, priority);
}

void GbaPpuBackground3Mode0Pixel(const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuInternalRegisters* internal_registers,
                                 uint_fast8_t x, uint_fast8_t y,
                                 GbaPpuScreen* screen) {
  uint16_t color;
  uint8_t priority;
  GbaPpuLinearBackgroundPixelColor(memory, registers, internal_registers, x, y,
                                   GBA_PPU_LINEAR_BG3, &color, &priority);
  GbaPpuScreenDrawPixel(screen, x, y, color, priority);
}