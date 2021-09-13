#ifndef _WEBGBA_EMULATOR_PPU_GBA_BLEND_
#define _WEBGBA_EMULATOR_PPU_GBA_BLEND_

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  GBA_PPU_LAYER_BG0 = 0,
  GBA_PPU_LAYER_BG1 = 1,
  GBA_PPU_LAYER_BG2 = 2,
  GBA_PPU_LAYER_BG3 = 3,
  GBA_PPU_LAYER_OBJ = 4,
  GBA_PPU_LAYER_BACKDROP = 5,
} GbaPpuLayer;

typedef enum {
  GBA_PPU_LAYER_PRIORITY_0 = 0,
  GBA_PPU_LAYER_PRIORITY_1 = 1,
  GBA_PPU_LAYER_PRIORITY_2 = 2,
  GBA_PPU_LAYER_PRIORITY_3 = 3,
  GBA_PPU_LAYER_PRIORITY_TRANSPARENT = 4,
} GbaPpuLayerPriority;

typedef struct {
  uint16_t layers[6];
  GbaPpuLayerPriority priorities[6];
  GbaPpuLayerPriority top_priority;
  GbaPpuLayer top_layer;
  GbaPpuLayerPriority bottom_priorities[2];
  GbaPpuLayer bottom_layers[2];
} GbaPpuBlendUnit;

void GbaPpuBlendUnitReset(GbaPpuBlendUnit* blend_unit);

void GbaPpuBlendUnitSet(GbaPpuBlendUnit* blend_unit, GbaPpuLayer layer,
                        bool top, bool bottom, uint16_t color,
                        GbaPpuLayerPriority priority);

static inline uint16_t GbaPpuBlendUnitNoBlend(
    const GbaPpuBlendUnit* blend_unit) {
  return blend_unit->layers[blend_unit->top_layer];
}

uint16_t GbaPpuBlendUnitBlend(const GbaPpuBlendUnit* blend_unit,
                              uint_fast8_t top_weight,
                              uint_fast8_t bottom_weight);

uint16_t GbaPpuBlendUnitBlackBlend(const GbaPpuBlendUnit* blend_unit,
                                   uint_fast8_t weight);

uint16_t GbaPpuBlendUnitWhiteBlend(const GbaPpuBlendUnit* blend_unit,
                                   uint_fast8_t weight);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_BLEND_