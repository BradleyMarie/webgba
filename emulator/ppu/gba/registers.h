#ifndef _WEBGBA_EMULATOR_PPU_GBA_REGISTERS_
#define _WEBGBA_EMULATOR_PPU_GBA_REGISTERS_

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef union {
  struct {
    unsigned char mode : 3;
    bool gbc_mode : 1;  // Read-only from non-BIOS code
    bool page_select : 1;
    bool oam_hblank : 1;
    bool object_mode : 1;
    bool forced_blank : 1;
    bool bg0_enable : 1;
    bool bg1_enable : 1;
    bool bg2_enable : 1;
    bool bg3_enable : 1;
    bool object_enable : 1;
    bool win0_enable : 1;
    bool win1_enable : 1;
    bool winobj_enable : 1;
  };
  uint16_t value;
} DispCntRegister;

typedef union {
  struct {
    bool vblank_status : 1;  // Read-only
    bool hblank_status : 1;  // Read-only
    bool vcount_status : 1;  // Read-only
    bool vblank_irq_enable : 1;
    bool hblank_irq_enable : 1;
    bool vcount_irq_enable : 1;
    unsigned char unused : 2;
    unsigned char vcount_trigger;
  };
  uint16_t value;
} DispStatRegister;

typedef union {
  struct {
    unsigned char priority : 2;
    unsigned char character_base_block : 2;
    unsigned char unused : 2;
    bool mosaic : 1;
    bool palettes : 1;
    unsigned char screen_base_block : 5;
    bool wraparound : 1;
    unsigned char screen_size : 2;
  };
  uint16_t value;
} BgCntRegister;

typedef union {
  struct {
    unsigned char bg_horiz : 4;
    unsigned char bg_vert : 4;
    unsigned char obj_horiz : 4;
    unsigned char obj_vert : 4;
  };
  uint16_t value;
} MosaicRegister;

#define GBA_PPU_REGISTERS_SIZE 88u

typedef union {
  struct {
    DispCntRegister dispcnt;
    uint16_t greenswp;  // Unimplemented
    DispStatRegister dispstat;
    uint16_t vcount;
    BgCntRegister bg0cnt;
    BgCntRegister bg1cnt;
    BgCntRegister bg2cnt;
    BgCntRegister bg3cnt;
    uint16_t bg0hofs;
    uint16_t bg0vofs;
    uint16_t bg1hofs;
    uint16_t bg1vofs;
    uint16_t bg2hofs;
    uint16_t bg2vofs;
    uint16_t bg3hofs;
    uint16_t bg3vofs;
    uint16_t bg2pa;
    uint16_t bg2pb;
    uint16_t bg2pc;
    uint16_t bg2pd;
    uint32_t bg2x;
    uint32_t bg2y;
    uint16_t bg3pa;
    uint16_t bg3pb;
    uint16_t bg3pc;
    uint16_t bg3pd;
    uint32_t bg3x;
    uint32_t bg3y;
    uint16_t win0h;
    uint16_t win1h;
    uint16_t win0v;
    uint16_t win1v;
    uint16_t winin;
    uint16_t winout;
    MosaicRegister mosaic;
    uint16_t unused0;
    uint16_t bldcnt;
    uint16_t bldalpha;
    uint16_t bldy;
    uint16_t unused2;
  };
  uint16_t half_words[GBA_PPU_REGISTERS_SIZE >> 1u];
} GbaPpuRegisters;

static_assert(sizeof(GbaPpuRegisters) == GBA_PPU_REGISTERS_SIZE,
              "sizeof(GbaPpuRegisters) != GBA_PPU_REGISTERS_SIZE");

typedef struct {
  int32_t bg2_x_row_start;
  int32_t bg2_y_row_start;
  int32_t bg2_x;
  int32_t bg2_y;
} GbaPpuInternalRegisters;

#endif  // _WEBGBA_EMULATOR_PPU_GBA_REGISTERS_