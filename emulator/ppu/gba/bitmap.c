#include "emulator/ppu/gba/bitmap.h"

#define GBA_MODE_5_FRAME_WIDTH 160u
#define GBA_MODE_5_FRAME_HEIGHT 128u
#define GBA_BACK_PAGE_OFFSET 0xA000u

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

static void GbaPpuRenderBitmapPixel(
    const GbaPpuMemory* memory, const GbaPpuRegisters* registers,
    uint_fast8_t frame_width, uint_fast8_t frame_height,
    bool frame_pixels_are_colors, bool back_page,
    GbaPpuInternalRegisters* internal_registers, uint_fast8_t x, uint_fast8_t y,
    GbaPpuFrameBuffer* framebuffer) {
  assert(x < GBA_SCREEN_WIDTH);
  assert(y < GBA_SCREEN_HEIGHT);

  if (x == 0) {
    if (y == 0) {
      internal_registers->bg2_x_row_start = registers->bg2x;
      internal_registers->bg2_y_row_start = registers->bg2y;
    }
    internal_registers->bg2_x = internal_registers->bg2_x_row_start;
    internal_registers->bg2_y = internal_registers->bg2_y_row_start;
  }

  uint16_t color;
  uint_fast8_t mosaic_pixel_x, mosaic_pixel_y;
  if (GbaPpuBitmapMosaic(registers, x, y, &mosaic_pixel_x, &mosaic_pixel_y)) {
    color = framebuffer->pixels[mosaic_pixel_y][mosaic_pixel_x];
  } else {
    uint32_t lookup_x = internal_registers->bg2_x >> 8u;
    uint32_t lookup_y = internal_registers->bg2_y >> 8u;

    if (lookup_x >= frame_width || lookup_y >= frame_height) {
      color = memory->palette.bg.colors[0];
    } else {
      uint32_t row_offset = lookup_y * frame_width;
      uint32_t column_offset = lookup_x;
      uint32_t pixel_offset = row_offset + column_offset;

      if (frame_pixels_are_colors) {
        color = memory->vram.paged_half_words[back_page][pixel_offset];
      } else {
        uint8_t color_index = memory->vram.paged_bytes[back_page][pixel_offset];
        color = memory->palette.bg.colors[color_index];
      }
    }
  }

  framebuffer->pixels[y][x] = color;

  internal_registers->bg2_x += registers->bg2pa;
  internal_registers->bg2_y += registers->bg2pc;

  if (x == GBA_SCREEN_WIDTH - 1u) {
    internal_registers->bg2_x_row_start += registers->bg2pb;
    internal_registers->bg2_y_row_start += registers->bg2pd;
  }
}

void GbaPpuRenderMode3Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y,
                            GbaPpuFrameBuffer* framebuffer) {
  GbaPpuRenderBitmapPixel(memory, registers, /*frame_width=*/GBA_SCREEN_WIDTH,
                          /*frame_height=*/GBA_SCREEN_HEIGHT,
                          /*frame_pixels_are_colors=*/true, /*back_page=*/false,
                          internal_registers, x, y, framebuffer);
}

void GbaPpuRenderMode4Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y,
                            GbaPpuFrameBuffer* framebuffer) {
  GbaPpuRenderBitmapPixel(memory, registers, /*frame_width=*/GBA_SCREEN_WIDTH,
                          /*frame_height=*/GBA_SCREEN_HEIGHT,
                          /*frame_pixels_are_colors=*/false,
                          /*back_page=*/registers->dispcnt.page_select,
                          internal_registers, x, y, framebuffer);
}

void GbaPpuRenderMode5Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y,
                            GbaPpuFrameBuffer* framebuffer) {
  GbaPpuRenderBitmapPixel(memory, registers,
                          /*frame_width=*/GBA_MODE_5_FRAME_WIDTH,
                          /*frame_height=*/GBA_MODE_5_FRAME_HEIGHT,
                          /*frame_pixels_are_colors=*/true,
                          /*back_page=*/registers->dispcnt.page_select,
                          internal_registers, x, y, framebuffer);
}