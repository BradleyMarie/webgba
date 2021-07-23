#ifndef _WEBGBA_EMULATOR_PPU_GBA_TYPES_
#define _WEBGBA_EMULATOR_PPU_GBA_TYPES_

#define PRAM_SIZE 1024u
#define VRAM_SIZE (96u * 1024u)
#define OAM_SIZE 1024u

#include <stdint.h>

typedef struct {
  unsigned char pram[PRAM_SIZE];
  unsigned char vram[VRAM_SIZE];
  unsigned char oam[OAM_SIZE];
} GbaPpuMemory;

typedef struct {
  uint16_t dispcnt;
  uint16_t unimplemented;
  uint16_t dispstat;
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

#endif  // _WEBGBA_EMULATOR_PPU_GBA_TYPES_