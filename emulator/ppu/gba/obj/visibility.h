#ifndef _WEBGBA_EMULATOR_PPU_GBA_OBJECT_STATE_
#define _WEBGBA_EMULATOR_PPU_GBA_OBJECT_STATE_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/obj/set.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GbaPpuObjectSet x_sets[GBA_FULL_FRAME_WIDTH];
  GbaPpuObjectSet y_sets[GBA_FULL_FRAME_HEIGHT];
} GbaPpuObjectVisibility;

void GbaPpuObjectVisibilityHidden(const GbaPpuObjectAttributeMemory* oam,
                                  const GbaPpuInternalRegisters* registers,
                                  uint_fast8_t object,
                                  GbaPpuObjectVisibility* visibility);

void GbaPpuObjectVisibilityDrawn(const GbaPpuObjectAttributeMemory* oam,
                                 uint_fast8_t object,
                                 GbaPpuInternalRegisters* registers,
                                 GbaPpuObjectVisibility* visibility);

static inline GbaPpuObjectSet GbaPpuObjectVisibilityGet(
    const GbaPpuObjectVisibility* visibility, uint_fast8_t x, uint_fast8_t y) {
  assert(x < GBA_FULL_FRAME_WIDTH);
  assert(y < GBA_FULL_FRAME_HEIGHT);
  return GbaPpuObjectSetIntersection(&visibility->x_sets[x],
                                     &visibility->y_sets[y]);
}

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OBJ_VISIBILITY_