#ifndef _WEBGBA_EMULATOR_PPU_GBA_SOFTWARE_BLEND_
#define _WEBGBA_EMULATOR_PPU_GBA_SOFTWARE_BLEND_

#include <stdbool.h>
#include <stdint.h>

#include "emulator/ppu/gba/registers.h"

typedef struct {
  uint16_t layers[2u];
  uint_fast8_t priorities[2u];
  bool top[2u];
  bool bottom[2u];
  bool is_blended_object[2u];
} GbaPpuBlendUnit;

//
// This module makes strong assumptions about the order in which the calls to it
// are made in order to simplify its logic and also save on a bit of
// performance. The correct ordering of these calls is as follows.
//
//   1) GbaPpuBlendUnitReset
//   2) GbaPpuBlendUnitAddObject
//   3) GbaPpuBlendUnitAddBackground0
//   4) GbaPpuBlendUnitAddBackground1
//   5) GbaPpuBlendUnitAddBackground2
//   6) GbaPpuBlendUnitAddBackground3
//   7) GbaPpuBlendUnitAddBackdrop
//   8) GbaPpuBlendUnitBlend / GbaPpuBlendUnitNoBlend
//

static inline void GbaPpuBlendUnitReset(GbaPpuBlendUnit* blend_unit) {
  blend_unit->priorities[0u] = 6u;
  blend_unit->priorities[1u] = 6u;
  blend_unit->top[1u] = false;
  blend_unit->bottom[1u] = false;
  blend_unit->is_blended_object[0u] = false;
  blend_unit->is_blended_object[1u] = false;
}

void GbaPpuBlendUnitAddObject(GbaPpuBlendUnit* blend_unit,
                              const GbaPpuRegisters* registers, uint16_t color,
                              uint_fast8_t priority, bool semi_transparent);

void GbaPpuBlendUnitAddBackground0(GbaPpuBlendUnit* blend_unit,
                                   const GbaPpuRegisters* registers,
                                   uint16_t color);

void GbaPpuBlendUnitAddBackground1(GbaPpuBlendUnit* blend_unit,
                                   const GbaPpuRegisters* registers,
                                   uint16_t color);

void GbaPpuBlendUnitAddBackground2(GbaPpuBlendUnit* blend_unit,
                                   const GbaPpuRegisters* registers,
                                   uint16_t color);

void GbaPpuBlendUnitAddBackground3(GbaPpuBlendUnit* blend_unit,
                                   const GbaPpuRegisters* registers,
                                   uint16_t color);

void GbaPpuBlendUnitAddBackdrop(GbaPpuBlendUnit* blend_unit,
                                const GbaPpuRegisters* registers,
                                uint16_t color);

void GbaPpuBlendUnitBlend(const GbaPpuBlendUnit* blend_unit,
                          const GbaPpuRegisters* registers, uint8_t rgb[3u]);

void GbaPpuBlendUnitNoBlend(const GbaPpuBlendUnit* blend_unit, uint8_t rgb[3u]);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_SOFTWARE_BLEND_