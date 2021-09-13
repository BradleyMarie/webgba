#include "emulator/ppu/gba/bg/bitmap.h"

typedef enum {
  GBA_PPU_BG2_MODE_3,
  GBA_PPU_BG2_MODE_4,
  GBA_PPU_BG2_MODE_5
} GbaPpuBackground2BitmapMode;

static inline bool GbaPpuBackground2BitmapPixel(
    const GbaPpuMemory* memory, const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuBackground2BitmapMode mode, bool back_page, uint_fast8_t x,
    uint_fast8_t y, uint16_t* color) {
  int32_t lookup_x =
      (internal_registers->affine[0u].x + registers->affine[0u].pa * x) >> 8u;
  int32_t lookup_y =
      (internal_registers->affine[0u].y + registers->affine[0u].pc * x) >> 8u;
  if (lookup_x < 0 || lookup_y < 0) {
    return false;
  }

  if (registers->bgcnt[2u].mosaic) {
    lookup_x -= lookup_x % (registers->mosaic.bg_horiz + 1u);
    lookup_y -= lookup_y % (registers->mosaic.bg_vert + 1u);
  }

  uint8_t color_index;
  switch (mode) {
    case GBA_PPU_BG2_MODE_3:
      if (lookup_x >= GBA_FULL_FRAME_WIDTH ||
          lookup_y >= GBA_FULL_FRAME_HEIGHT) {
        return false;
      } else {
        *color = memory->vram.mode_3.bg.pixels[lookup_y][lookup_x];
      }
      break;
    case GBA_PPU_BG2_MODE_4:
      if (lookup_x >= GBA_FULL_FRAME_WIDTH ||
          lookup_y >= GBA_FULL_FRAME_HEIGHT) {
        return false;
      } else {
        color_index =
            memory->vram.mode_4.bg.pages[back_page].pixels[lookup_y][lookup_x];
        if (color_index == 0u) {
          return false;
        }

        *color = memory->palette.bg.large_palette[color_index];
      }
      break;
    case GBA_PPU_BG2_MODE_5:
      if (lookup_x >= GBA_REDUCED_FRAME_WIDTH ||
          lookup_y >= GBA_REDUCED_FRAME_HEIGHT) {
        return false;
      } else {
        *color =
            memory->vram.mode_5.bg.pages[back_page].pixels[lookup_y][lookup_x];
      }
      break;
  };

  return true;
}

bool GbaPpuBitmapMode3Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            const GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y, uint16_t* color) {
  return GbaPpuBackground2BitmapPixel(memory, registers, internal_registers,
                                      GBA_PPU_BG2_MODE_3,
                                      /*back_page=*/false, x, y, color);
}

bool GbaPpuBitmapMode4Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            const GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y, uint16_t* color) {
  return GbaPpuBackground2BitmapPixel(
      memory, registers, internal_registers, GBA_PPU_BG2_MODE_4,
      /*back_page=*/registers->dispcnt.page_select, x, y, color);
}

bool GbaPpuBitmapMode5Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            const GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y, uint16_t* color) {
  return GbaPpuBackground2BitmapPixel(
      memory, registers, internal_registers, GBA_PPU_BG2_MODE_5,
      /*back_page=*/registers->dispcnt.page_select, x, y, color);
}