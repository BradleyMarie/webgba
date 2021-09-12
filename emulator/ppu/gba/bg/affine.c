#include "emulator/ppu/gba/bg/affine.h"

#include <assert.h>

static inline BgCntRegister GetBgCnt(const GbaPpuRegisters* registers,
                                     GbaPpuAffineBackground background) {
  assert(2u + background < 4u);
  return registers->bgcnt[2u + background];
}

void GbaPpuAffineBackgroundPixel(
    const GbaPpuMemory* memory, const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuAffineBackground background, uint_fast8_t x, uint_fast8_t y,
    GbaPpuScreen* screen) {
  int32_t lookup_x = (internal_registers->affine[background].x +
                      registers->affine[background].pa * x) >>
                     8u;
  int32_t lookup_y = (internal_registers->affine[background].y +
                      registers->affine[background].pc * x) >>
                     8u;

  if (registers->bgcnt[background].mosaic) {
    lookup_x -= lookup_x % (registers->mosaic.bg_horiz + 1u);
    lookup_y -= lookup_y % (registers->mosaic.bg_vert + 1u);
  }

  static const uint_fast16_t screen_sizes_pixels[4] = {128u, 256u, 512u, 1024u};
  static const uint_fast16_t screen_sizes_tiles[4] = {16u, 32u, 64u, 128u};

  BgCntRegister bgcnt = GetBgCnt(registers, background);
  uint_fast16_t screen_size = screen_sizes_pixels[bgcnt.size];

  if (bgcnt.wraparound) {
    static const uint_fast16_t screen_size_masks[4] = {0x07Fu, 0x0FFu, 0x1FFu,
                                                       0x3FFu};
    lookup_x &= screen_size_masks[bgcnt.size];
    lookup_y &= screen_size_masks[bgcnt.size];
  } else if (lookup_x >= screen_size || lookup_y >= screen_size) {
    return;
  }

  uint_fast16_t screen_size_tiles = screen_sizes_tiles[bgcnt.size];

  uint_fast8_t tile_x = lookup_x / GBA_TILE_1D_SIZE;
  uint_fast8_t tile_y = lookup_y / GBA_TILE_1D_SIZE;

  uint8_t tile_index =
      memory->vram.mode_012.bg.tile_map.blocks[bgcnt.tile_map_base_block]
          .indices[tile_y * screen_size_tiles + tile_x];

  uint_fast8_t tile_lookup_x = lookup_x % GBA_TILE_1D_SIZE;
  uint_fast8_t tile_lookup_y = lookup_y % GBA_TILE_1D_SIZE;

  // TODO: Handle accesses to obj tiles
  uint8_t color_index =
      memory->vram.mode_012.bg.tiles.blocks[bgcnt.tile_base_block]
          .d_tiles[tile_index]
          .pixels[tile_lookup_y][tile_lookup_x];
  if (color_index == 0u) {
    return;
  }

  uint16_t color = memory->palette.bg.large_palette[color_index];

  GbaPpuScreenDrawBackgroundPixel(screen, x, y, color, bgcnt.priority);
}