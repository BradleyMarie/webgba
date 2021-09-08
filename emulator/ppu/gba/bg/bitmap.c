#include "emulator/ppu/gba/bg/bitmap.h"

typedef enum {
  GBA_PPU_BG2_MODE_3,
  GBA_PPU_BG2_MODE_4,
  GBA_PPU_BG2_MODE_5
} GbaPpuBackground2BitmapMode;

static inline bool GbaPpuBitmapMosaic(const GbaPpuRegisters* registers,
                                      uint_fast8_t x, uint_fast8_t y,
                                      uint_fast8_t* mosaic_pixel_x,
                                      uint_fast8_t* mosaic_pixel_y) {
  if (!registers->bgcnt[2u].mosaic) {
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

static inline void GbaPpuBackground2BitmapPixel(
    const GbaPpuMemory* memory, const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuBackground2BitmapMode mode, bool back_page, uint_fast8_t x,
    uint_fast8_t y, GbaPpuScreen* screen) {
  uint_fast8_t lookup_x, lookup_y;
  if (!GbaPpuBitmapMosaic(registers, x, y, &lookup_x, &lookup_y)) {
    lookup_x =
        (internal_registers->affine[0u].x + registers->affine[0u].pa * x) >> 8u;
    lookup_y =
        (internal_registers->affine[0u].y + registers->affine[0u].pc * x) >> 8u;
  }

  uint16_t color;
  uint8_t color_index;
  uint8_t priority;
  switch (mode) {
    case GBA_PPU_BG2_MODE_3:
      if (lookup_x >= GBA_FULL_FRAME_WIDTH ||
          lookup_y >= GBA_FULL_FRAME_HEIGHT) {
        return;
      } else {
        color = memory->vram.mode_3.bg.pixels[lookup_y][lookup_x];
        priority = registers->bgcnt[2u].priority;
      }
      break;
    case GBA_PPU_BG2_MODE_4:
      if (lookup_x >= GBA_FULL_FRAME_WIDTH ||
          lookup_y >= GBA_FULL_FRAME_HEIGHT) {
        return;
      } else {
        color_index =
            memory->vram.mode_4.bg.pages[back_page].pixels[lookup_y][lookup_x];
        if (color_index == 0u) {
          return;
        }

        color = memory->palette.bg.large_palette[color_index];
        priority = registers->bgcnt[2u].priority;
      }
      break;
    case GBA_PPU_BG2_MODE_5:
      if (lookup_x >= GBA_REDUCED_FRAME_WIDTH ||
          lookup_y >= GBA_REDUCED_FRAME_HEIGHT) {
        return;
      } else {
        color =
            memory->vram.mode_5.bg.pages[back_page].pixels[lookup_y][lookup_x];
        priority = registers->bgcnt[2u].priority;
      }
      break;
  };

  GbaPpuScreenDrawBackgroundPixel(screen, x, y, color, priority);
}

void GbaPpuBitmapMode3Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            const GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y,
                            GbaPpuScreen* screen) {
  GbaPpuBackground2BitmapPixel(memory, registers, internal_registers,
                               GBA_PPU_BG2_MODE_3,
                               /*back_page=*/false, x, y, screen);
}

void GbaPpuBitmapMode4Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            const GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y,
                            GbaPpuScreen* screen) {
  GbaPpuBackground2BitmapPixel(
      memory, registers, internal_registers, GBA_PPU_BG2_MODE_4,
      /*back_page=*/registers->dispcnt.page_select, x, y, screen);
}

void GbaPpuBitmapMode5Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            const GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y,
                            GbaPpuScreen* screen) {
  GbaPpuBackground2BitmapPixel(
      memory, registers, internal_registers, GBA_PPU_BG2_MODE_5,
      /*back_page=*/registers->dispcnt.page_select, x, y, screen);
}