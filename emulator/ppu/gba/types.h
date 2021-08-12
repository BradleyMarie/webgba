#ifndef _WEBGBA_EMULATOR_PPU_GBA_TYPES_
#define _WEBGBA_EMULATOR_PPU_GBA_TYPES_

#include <assert.h>
#include <stdalign.h>
#include <stdint.h>

#define PALETTE_SIZE 1024u
#define VRAM_SIZE (96u * 1024u)
#define VRAM_BG_SIZE (64u * 1024u)
#define VRAM_OBJ_SIZE (32u * 1024u)
#define VRAM_BANK_SIZE (128u * 1024u)
#define OAM_SIZE 1024u

typedef struct {
  alignas(unsigned) unsigned char palette[PALETTE_SIZE];
  alignas(unsigned) unsigned char vram[VRAM_SIZE];
  alignas(unsigned) unsigned char oam[OAM_SIZE];
  void* free_address;
  uint16_t reference_count;
} GbaPpuMemory;

#define GBA_PPU_REGISTERS_SIZE 88u

#define DISPCNT_OFFSET 0x00u
#define GREENSWP_OFFSET 0x02u
#define DISPSTAT_OFFSET 0x04u
#define VCOUNT_OFFSET 0x06u
#define BG0CNT_OFFSET 0x08u
#define BG1CNT_OFFSET 0x0Au
#define BG2CNT_OFFSET 0x0Cu
#define BG3CNT_OFFSET 0x0Eu
#define BG0HOFS_OFFSET 0x10u
#define BG0VOFS_OFFSET 0x12u
#define BG1HOFS_OFFSET 0x14u
#define BG1VOFS_OFFSET 0x16u
#define BG2HOFS_OFFSET 0x18u
#define BG2VOFS_OFFSET 0x1Au
#define BG3HOFS_OFFSET 0x1Cu
#define BG3VOFS_OFFSET 0x1Eu
#define BG2PA_OFFSET 0x20u
#define BG2PB_OFFSET 0x22u
#define BG2PC_OFFSET 0x24u
#define BG2PD_OFFSET 0x26u
#define BG2X_OFFSET 0x28u
#define BG2X_OFFSET_HI 0x2Au
#define BG2Y_OFFSET 0x2Cu
#define BG2Y_OFFSET_HI 0x2Eu
#define BG3PA_OFFSET 0x30u
#define BG3PB_OFFSET 0x32u
#define BG3PC_OFFSET 0x34u
#define BG3PD_OFFSET 0x36u
#define BG3X_OFFSET 0x38u
#define BG3X_OFFSET_HI 0x3Au
#define BG3Y_OFFSET 0x3Cu
#define BG3Y_OFFSET_HI 0x3Eu
#define WIN0H_OFFSET 0x40u
#define WIN1H_OFFSET 0x42u
#define WIN0V_OFFSET 0x44u
#define WIN1V_OFFSET 0x46u
#define WININ_OFFSET 0x48u
#define WINOUT_OFFSET 0x4Au
#define MOSAIC_OFFSET 0x4Cu
#define BLDCNT_OFFSET 0x50u
#define BLDALPHA_OFFSET 0x52u
#define BLDY_OFFSET 0x54u

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

typedef struct {
  DispCntRegister dispcnt;
  uint16_t greenswp;  // Unimplemented
  DispStatRegister dispstat;
  uint16_t vcount;
  uint16_t bg0cnt;
  uint16_t bg1cnt;
  uint16_t bg2cnt;
  uint16_t bg3cnt;
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
  uint16_t mosaic;
  uint16_t unused0;
  uint16_t bldcnt;
  uint16_t bldalpha;
  uint16_t bldy;
  uint16_t unused2;
} GbaPpuRegisters;

static_assert(sizeof(GbaPpuRegisters) == GBA_PPU_REGISTERS_SIZE,
              "sizeof(GbaPpuRegisters) != GBA_PPU_REGISTERS_SIZE");

#endif  // _WEBGBA_EMULATOR_PPU_GBA_TYPES_