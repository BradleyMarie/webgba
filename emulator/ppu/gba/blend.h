#ifndef _WEBGBA_EMULATOR_PPU_GBA_BLEND_
#define _WEBGBA_EMULATOR_PPU_GBA_BLEND_

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  GBA_PPU_LAYER_OBJ = 0,
  GBA_PPU_LAYER_BG0 = 1,
  GBA_PPU_LAYER_BG1 = 2,
  GBA_PPU_LAYER_BG2 = 3,
  GBA_PPU_LAYER_BG3 = 4,
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
  GbaPpuLayerPriority first_layer_priority;
  GbaPpuLayer first_layer;
  bool obj_semi_transparent;
} GbaPpuBlendUnit;

void GbaPpuBlendUnitReset(GbaPpuBlendUnit* blend_unit);

void GbaPpuBlendUnitSet(GbaPpuBlendUnit* blend_unit, GbaPpuLayer layer,
                        bool top, bool bottom, uint16_t color,
                        GbaPpuLayerPriority priority);

void GbaPpuBlendUnitSetObj(GbaPpuBlendUnit* blend_unit, bool top, bool bottom,
                           uint16_t color, GbaPpuLayerPriority priority,
                           bool semi_transparent);

uint16_t GbaPpuBlendUnitNoBlend(const GbaPpuBlendUnit* blend_unit);

uint16_t GbaPpuBlendUnitBlend(const GbaPpuBlendUnit* blend_unit,
                              uint_fast8_t eva, uint_fast8_t evb);

uint16_t GbaPpuBlendUnitDarken(const GbaPpuBlendUnit* blend_unit,
                               uint_fast8_t eva, uint_fast8_t evb,
                               uint_fast8_t evy);

uint16_t GbaPpuBlendUnitBrighten(const GbaPpuBlendUnit* blend_unit,
                                 uint_fast8_t eva, uint_fast8_t evb,
                                 uint_fast8_t evy);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_BLEND_