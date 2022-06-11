#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_DRAW_MANAGER_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_DRAW_MANAGER_

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  bool blank;
  uint8_t mode;
  MosaicRegister mosaic;
  union {
    struct {
      bool bg0_enabled;
      bool bg1_enabled;
      bool bg2_enabled;
      bool bg3_enabled;
      bool bg0_mosaic;
      bool bg1_mosaic;
      bool bg2_mosaic;
      bool bg3_mosaic;
    } mode_0;
    struct {
      bool bg0_enabled;
      bool bg1_enabled;
      bool bg2_enabled;
      bool bg0_mosaic;
      bool bg1_mosaic;
      bool bg2_mosaic;
    } mode_1;
    struct {
      bool bg2_enabled;
      bool bg3_enabled;
      bool bg2_mosaic;
      bool bg3_mosaic;
    } mode_2;
    struct {
      bool bg2_enabled;
      bool bg2_mosaic;
    } mode_3;
    struct {
      bool bg2_enabled;
      bool bg2_mosaic;
      bool page;
    } mode_4;
    struct {
      bool bg2_enabled;
      bool bg2_mosaic;
      bool page;
    } mode_5;
  };
  bool frame_dirtied;
} GbaPpuDrawManager;

void GbaPpuDrawManagerStartFrame(GbaPpuDrawManager* draw_manager,
                                 const GbaPpuRegisters* registers,
                                 const GbaPpuDirtyBits* dirty_bits);

bool GbaPpuDrawManagerShouldFlush(GbaPpuDrawManager* draw_manager,
                                  const GbaPpuRegisters* registers,
                                  const GbaPpuDirtyBits* dirty_bits);

bool GbaPpuDrawManagerEndFrame(const GbaPpuDrawManager* draw_manager);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_DRAW_MANAGER_