#ifndef _WEBGBA_EMULATOR_PPU_GBA_SOFTWARE_OBJ_VISIBILITY_
#define _WEBGBA_EMULATOR_PPU_GBA_SOFTWARE_OBJ_VISIBILITY_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/set.h"

typedef struct {
  GbaPpuSet x_sets[GBA_SCREEN_WIDTH];
  GbaPpuSet y_sets[GBA_SCREEN_HEIGHT];
  struct {
    uint8_t pixel_x_start;
    uint8_t pixel_x_end;
    uint8_t pixel_x_size;
    uint8_t pixel_y_start;
    uint8_t pixel_y_end;
    uint8_t pixel_y_size;
    int16_t true_x_start;
    int16_t true_x_center;
    int16_t true_y_start;
    int16_t true_y_center;
  } object_coordinates[OAM_NUM_OBJECTS];
} GbaPpuObjectVisibility;

void GbaPpuObjectVisibilityHidden(GbaPpuObjectVisibility* visibility,
                                  const GbaPpuObjectAttributeMemory* oam,
                                  uint_fast8_t object);

void GbaPpuObjectVisibilityDrawn(GbaPpuObjectVisibility* visibility,
                                 const GbaPpuObjectAttributeMemory* oam,
                                 uint_fast8_t object);

static inline GbaPpuSet GbaPpuObjectVisibilityGet(
    const GbaPpuObjectVisibility* visibility, uint_fast8_t x, uint_fast8_t y) {
  assert(x < GBA_SCREEN_WIDTH);
  assert(y < GBA_SCREEN_HEIGHT);
  return GbaPpuSetIntersection(&visibility->x_sets[x], &visibility->y_sets[y]);
}

#endif  // _WEBGBA_EMULATOR_PPU_GBA_SOFTWARE_OBJ_VISIBILITY_