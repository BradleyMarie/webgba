#ifndef _WEBGBA_EMULATOR_PPU_GBA_DRAW_MANAGER_
#define _WEBGBA_EMULATOR_PPU_GBA_DRAW_MANAGER_

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  bool blank;
  uint8_t mode;
  MosaicRegister mosaic;
  union {
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
  bool previous_frame_copyable;
  bool flush_triggered;
} GbaPpuDrawManager;

void GbaPpuDrawManagerInitialize(GbaPpuDrawManager* draw_manager);

bool GbaPpuDrawManagerShouldFlush(GbaPpuDrawManager* draw_manager,
                                  const GbaPpuRegisters* registers,
                                  const GbaPpuDirtyBits* dirty_bits);

bool GbaPpuDrawManagerEndFrame(GbaPpuDrawManager* draw_manager,
                               const GbaPpuRegisters* registers,
                               const GbaPpuDirtyBits* dirty_bits);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_DRAW_MANAGER_