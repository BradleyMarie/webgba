#include "emulator/ppu/gba/bg/bitmap.h"

typedef enum {
  GBA_PPU_BG2_MODE_3,
  GBA_PPU_BG2_MODE_4,
  GBA_PPU_BG2_MODE_5
} GbaPpuBackground2BitmapMode;

static bool GbaPpuBitmapMosaic(const GbaPpuRegisters* registers, uint_fast8_t x,
                               uint_fast8_t y, uint_fast8_t* mosaic_pixel_x,
                               uint_fast8_t* mosaic_pixel_y) {
  if (!registers->bg2cnt.mosaic) {
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

static void GbaPpuBackground2BitmapPixel(
    const GbaPpuMemory* memory, const GbaPpuRegisters* registers,
    GbaPpuBackground2BitmapMode mode, bool back_page,
    GbaPpuInternalRegisters* internal_registers, uint_fast8_t x, uint_fast8_t y,
    GbaPpuScreen* screen) {
  if (x == 0) {
    if (y == 0) {
      internal_registers->bg2_x_row_start = registers->bg2x;
      internal_registers->bg2_y_row_start = registers->bg2y;
    }
    internal_registers->bg2_x = internal_registers->bg2_x_row_start;
    internal_registers->bg2_y = internal_registers->bg2_y_row_start;
  }

  uint_fast8_t mosaic_pixel_x, mosaic_pixel_y;
  if (GbaPpuBitmapMosaic(registers, x, y, &mosaic_pixel_x, &mosaic_pixel_y)) {
    GbaPpuScreenCopyPixel(screen, mosaic_pixel_x, mosaic_pixel_y, x, y);
  } else {
    uint32_t lookup_x = internal_registers->bg2_x >> 8u;
    uint32_t lookup_y = internal_registers->bg2_y >> 8u;

    uint16_t color;
    uint8_t color_index;
    switch (mode) {
      case GBA_PPU_BG2_MODE_3:
        if (lookup_x >= GBA_FULL_FRAME_WIDTH ||
            lookup_y >= GBA_FULL_FRAME_HEIGHT) {
          color = memory->palette.bg.large_palette[0u];
        } else {
          color = memory->vram.mode_3.bg.pixels[y][x];
        }
        break;
      case GBA_PPU_BG2_MODE_4:
        if (lookup_x >= GBA_FULL_FRAME_WIDTH ||
            lookup_y >= GBA_FULL_FRAME_HEIGHT) {
          color = memory->palette.bg.large_palette[0u];
        } else {
          color_index = memory->vram.mode_4.bg.pages[back_page].pixels[y][x];
          color = memory->palette.bg.large_palette[color_index];
        }
        break;
      case GBA_PPU_BG2_MODE_5:
        if (lookup_x >= GBA_REDUCED_FRAME_WIDTH ||
            lookup_y >= GBA_REDUCED_FRAME_HEIGHT) {
          color = memory->palette.bg.large_palette[0u];
        } else {
          color_index = memory->vram.mode_5.bg.pages[back_page].pixels[y][x];
          color = memory->palette.bg.large_palette[color_index];
        }
        break;
    };
    GbaPpuScreenSetPixel(screen, x, y, color);
  }

  internal_registers->bg2_x += registers->bg2pa;
  internal_registers->bg2_y += registers->bg2pc;

  if (x == GBA_SCREEN_WIDTH - 1u) {
    internal_registers->bg2_x_row_start += registers->bg2pb;
    internal_registers->bg2_y_row_start += registers->bg2pd;
  }
}

void GbaPpuBackground2Mode3Pixel(const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuInternalRegisters* internal_registers,
                                 uint_fast8_t x, uint_fast8_t y,
                                 GbaPpuScreen* screen) {
  GbaPpuBackground2BitmapPixel(memory, registers, GBA_PPU_BG2_MODE_3,
                               /*back_page=*/false, internal_registers, x, y,
                               screen);
}

void GbaPpuBackground2Mode4Pixel(const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuInternalRegisters* internal_registers,
                                 uint_fast8_t x, uint_fast8_t y,
                                 GbaPpuScreen* screen) {
  GbaPpuBackground2BitmapPixel(memory, registers, GBA_PPU_BG2_MODE_4,
                               /*back_page=*/registers->dispcnt.page_select,
                               internal_registers, x, y, screen);
}

void GbaPpuBackground2Mode5Pixel(const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuInternalRegisters* internal_registers,
                                 uint_fast8_t x, uint_fast8_t y,
                                 GbaPpuScreen* screen) {
  GbaPpuBackground2BitmapPixel(memory, registers, GBA_PPU_BG2_MODE_5,
                               /*back_page=*/registers->dispcnt.page_select,
                               internal_registers, x, y, screen);
}