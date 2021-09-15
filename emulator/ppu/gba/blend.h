#ifndef _WEBGBA_EMULATOR_PPU_GBA_BLEND_
#define _WEBGBA_EMULATOR_PPU_GBA_BLEND_

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  GBA_PPU_LAYER_PRIORITY_0 = 0,
  GBA_PPU_LAYER_PRIORITY_1 = 1,
  GBA_PPU_LAYER_PRIORITY_2 = 2,
  GBA_PPU_LAYER_PRIORITY_3 = 3,
  GBA_PPU_LAYER_PRIORITY_BACKDROP = 4,
  GBA_PPU_LAYER_PRIORITY_NOT_SET = 5,
} GbaPpuLayerPriority;

typedef struct {
  uint16_t layers[2];
  GbaPpuLayerPriority priorities[2];
  bool top[2];
  bool bottom[2];
  bool obj_semi_transparent;
} GbaPpuBlendUnit;

void GbaPpuBlendUnitReset(GbaPpuBlendUnit* blend_unit);

void GbaPpuBlendUnitAddBackground(GbaPpuBlendUnit* blend_unit, bool top,
                                  bool bottom, uint16_t color,
                                  GbaPpuLayerPriority priority);

void GbaPpuBlendUnitAddObject(GbaPpuBlendUnit* blend_unit, bool top,
                              bool bottom, uint16_t color,
                              GbaPpuLayerPriority priority,
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