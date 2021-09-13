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
    unsigned char tile_base_block : 2;
    unsigned char unused : 2;
    bool mosaic : 1;
    bool large_palette : 1;
    unsigned char tile_map_base_block : 5;
    bool wraparound : 1;
    unsigned char size : 2;
  };
  uint16_t value;
} BgCntRegister;

typedef struct {
  unsigned short x : 9;
  unsigned short unused0 : 7;
  unsigned short y : 9;
  unsigned short unused1 : 7;
} BackgroundOffsetRegister;

typedef struct {
  int16_t pa;
  int16_t pb;
  int16_t pc;
  int16_t pd;
  int32_t x;
  int32_t y;
} BackgroundAffineRegister;

typedef union {
  struct {
    unsigned char bg_horiz : 4;
    unsigned char bg_vert : 4;
    unsigned char obj_horiz : 4;
    unsigned char obj_vert : 4;
  };
  uint16_t value;
} MosaicRegister;

typedef union {
  struct {
    bool a_bg0 : 1;
    bool a_bg1 : 1;
    bool a_bg2 : 1;
    bool a_bg3 : 1;
    bool a_obj : 1;
    bool a_bd : 1;
    unsigned char mode : 2;
    bool b_bg0 : 1;
    bool b_bg1 : 1;
    bool b_bg2 : 1;
    bool b_bg3 : 1;
    bool b_obj : 1;
    bool b_bd : 1;
    unsigned char unused : 2;
  };
  uint16_t value;
} BldCntRegister;

typedef union {
  struct {
    unsigned char eva : 5;
    unsigned char unused0 : 3;
    unsigned char evb : 5;
    unsigned char unused1 : 3;
  };
  uint16_t value;
} BldAlphaRegister;

typedef union {
  struct {
    unsigned char evy : 5;
    unsigned short unused : 11;
  };
  uint16_t value;
} BldYRegister;

#define GBA_PPU_REGISTERS_SIZE 88u

typedef union {
  struct {
    DispCntRegister dispcnt;
    uint16_t greenswp;  // Unimplemented
    DispStatRegister dispstat;
    uint16_t vcount;
    BgCntRegister bgcnt[4];
    BackgroundOffsetRegister bg_offsets[4];
    BackgroundAffineRegister affine[2];
    uint16_t win0h;
    uint16_t win1h;
    uint16_t win0v;
    uint16_t win1v;
    uint16_t winin;
    uint16_t winout;
    MosaicRegister mosaic;
    uint16_t unused0;
    BldCntRegister bldcnt;
    BldAlphaRegister bldalpha;
    BldYRegister bldy;
    uint16_t unused2;
  };
  uint16_t half_words[GBA_PPU_REGISTERS_SIZE >> 1u];
} GbaPpuRegisters;

static_assert(sizeof(GbaPpuRegisters) == GBA_PPU_REGISTERS_SIZE,
              "sizeof(GbaPpuRegisters) != GBA_PPU_REGISTERS_SIZE");

typedef struct {
  int32_t x;
  int32_t y;
} GbaPpuInternalAffineRegisters;

typedef struct {
  int16_t x;
  int16_t y;
  int16_t x_center;
  int16_t y_center;
  uint8_t x_size;
  uint8_t y_size;
} GbaPpuInternalObjectCoordinates;

typedef struct {
  // TODO: Fix these magic number
  GbaPpuInternalAffineRegisters affine[2];
  GbaPpuInternalObjectCoordinates object_coordinates[128];
} GbaPpuInternalRegisters;

#endif  // _WEBGBA_EMULATOR_PPU_GBA_REGISTERS_