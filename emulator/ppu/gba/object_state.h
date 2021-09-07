#ifndef _WEBGBA_EMULATOR_PPU_GBA_OBJECT_STATE_
#define _WEBGBA_EMULATOR_PPU_GBA_OBJECT_STATE_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/object_set.h"

typedef struct {
  GbaPpuObjectSet x_sets[GBA_FULL_FRAME_WIDTH];
  GbaPpuObjectSet y_sets[GBA_FULL_FRAME_HEIGHT];
} GbaPpuObjectState;

void GbaPpuObjectStateClear(const GbaPpuObjectAttributeMemory* oam,
                            uint_fast8_t object, GbaPpuObjectState* state);

void GbaPpuObjectStateAdd(const GbaPpuObjectAttributeMemory* oam,
                          uint_fast8_t object, GbaPpuObjectState* state);

static inline GbaPpuObjectSet GbaPpuObjectStateGetObjects(
    const GbaPpuObjectState* state, uint_fast8_t x, uint_fast8_t y) {
  assert(x < GBA_FULL_FRAME_WIDTH);
  assert(y < GBA_FULL_FRAME_HEIGHT);
  return GbaPpuObjectSetIntersection(&state->x_sets[x], &state->y_sets[y]);
}

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OBJECT_STATE_