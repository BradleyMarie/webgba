#include "emulator/ppu/gba/blend.h"

#include <assert.h>

void GbaPpuBlendUnitReset(GbaPpuBlendUnit* blend_unit) {
  blend_unit->priorities[0u] = GBA_PPU_LAYER_PRIORITY_TRANSPARENT;
  blend_unit->priorities[1u] = GBA_PPU_LAYER_PRIORITY_TRANSPARENT;
  blend_unit->priorities[2u] = GBA_PPU_LAYER_PRIORITY_TRANSPARENT;
  blend_unit->priorities[3u] = GBA_PPU_LAYER_PRIORITY_TRANSPARENT;
  blend_unit->priorities[4u] = GBA_PPU_LAYER_PRIORITY_TRANSPARENT;
  blend_unit->priorities[5u] = GBA_PPU_LAYER_PRIORITY_TRANSPARENT;
  blend_unit->top_priority = GBA_PPU_LAYER_PRIORITY_TRANSPARENT;
  blend_unit->top_layer = GBA_PPU_LAYER_BACKDROP;
  blend_unit->bottom_priorities[0u] = GBA_PPU_LAYER_PRIORITY_TRANSPARENT;
  blend_unit->bottom_layers[0u] = GBA_PPU_LAYER_BACKDROP;
  blend_unit->bottom_priorities[1u] = GBA_PPU_LAYER_PRIORITY_TRANSPARENT;
  blend_unit->bottom_layers[1u] = GBA_PPU_LAYER_BACKDROP;
  blend_unit->first_layer_priority = GBA_PPU_LAYER_PRIORITY_TRANSPARENT;
  blend_unit->first_layer = GBA_PPU_LAYER_BACKDROP;
  blend_unit->obj_semi_transparent = false;
}

void GbaPpuBlendUnitSet(GbaPpuBlendUnit* blend_unit, GbaPpuLayer layer,
                        bool top, bool bottom, uint16_t color,
                        GbaPpuLayerPriority priority) {
  assert(priority != GBA_PPU_LAYER_PRIORITY_TRANSPARENT ||
         layer == GBA_PPU_LAYER_BACKDROP);
  assert(GBA_PPU_LAYER_OBJ <= layer);
  assert(layer <= GBA_PPU_LAYER_BACKDROP);
  assert(GBA_PPU_LAYER_PRIORITY_0 <= priority);
  assert(priority <= GBA_PPU_LAYER_PRIORITY_TRANSPARENT);

  blend_unit->layers[layer] = color;
  blend_unit->priorities[layer] = priority;

  if (top && priority < blend_unit->top_priority) {
    blend_unit->top_priority = priority;
    blend_unit->top_layer = layer;
  }

  if (bottom) {
    if (priority < blend_unit->bottom_priorities[0u]) {
      blend_unit->bottom_priorities[1u] = blend_unit->bottom_priorities[0u];
      blend_unit->bottom_layers[1u] = blend_unit->bottom_layers[0u];
      blend_unit->bottom_priorities[0u] = priority;
      blend_unit->bottom_layers[0u] = layer;
    } else if (blend_unit->bottom_priorities[1u]) {
      blend_unit->bottom_priorities[1u] = priority;
      blend_unit->bottom_layers[1u] = layer;
    }
  }

  if (priority < blend_unit->first_layer_priority) {
    blend_unit->first_layer_priority = priority;
    blend_unit->first_layer = layer;
  }
}

void GbaPpuBlendUnitSetObj(GbaPpuBlendUnit* blend_unit, bool top, bool bottom,
                           uint16_t color, GbaPpuLayerPriority priority,
                           bool semi_transparent) {
  if (!semi_transparent) {
    GbaPpuBlendUnitSet(blend_unit, GBA_PPU_LAYER_OBJ, top, bottom, color,
                       priority);
  } else {
    blend_unit->layers[GBA_PPU_LAYER_OBJ] = color;
    blend_unit->priorities[GBA_PPU_LAYER_OBJ] = GBA_PPU_LAYER_PRIORITY_0;
    blend_unit->top_layer = GBA_PPU_LAYER_OBJ;
    blend_unit->top_priority = GBA_PPU_LAYER_PRIORITY_0;
    blend_unit->obj_semi_transparent = true;
  }
}

uint16_t GbaPpuBlendUnitNoBlend(const GbaPpuBlendUnit* blend_unit) {
  return blend_unit->layers[blend_unit->first_layer];
}

uint16_t GbaPpuBlendUnitBlend(const GbaPpuBlendUnit* blend_unit,
                              uint_fast8_t eva, uint_fast8_t evb) {
  bool bottom_index = blend_unit->top_layer == blend_unit->bottom_layers[0u];

  if (blend_unit->top_priority > blend_unit->first_layer_priority ||
      blend_unit->top_priority > blend_unit->bottom_priorities[bottom_index] ||
      blend_unit->top_priority > GBA_PPU_LAYER_PRIORITY_3 ||
      blend_unit->bottom_priorities[bottom_index] > GBA_PPU_LAYER_PRIORITY_3) {
    return GbaPpuBlendUnitNoBlend(blend_unit);
  }

  const static uint8_t clipped_weights[32u] = {
      0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u,  10u,
      11u, 12u, 13u, 14u, 15u, 16u, 16u, 16u, 16u, 16u, 16u,
      16u, 16u, 16u, 16u, 16u, 16u, 16u, 16u, 16u, 16u};

  assert(eva < 32u);
  eva = clipped_weights[eva];

  assert(evb < 32u);
  evb = clipped_weights[evb];

  uint_fast32_t top = blend_unit->layers[blend_unit->top_layer];
  uint_fast32_t t0 = (((top & 0x001Fu) * eva) >> 0u);
  uint_fast32_t t1 = (((top & 0x03E0u) * eva) >> 5u);
  uint_fast32_t t2 = (((top & 0x7C00u) * eva) >> 10u);

  uint_fast32_t bottom =
      blend_unit->layers[blend_unit->bottom_layers[bottom_index]];
  uint_fast32_t b0 = (((bottom & 0x001Fu) * evb) >> 0u);
  uint_fast32_t b1 = (((bottom & 0x03E0u) * evb) >> 5u);
  uint_fast32_t b2 = (((bottom & 0x7C00u) * evb) >> 10u);

  const static uint8_t clipped_values[64u] = {
      0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u,  10u, 11u, 12u,
      13u, 14u, 15u, 16u, 17u, 18u, 19u, 20u, 21u, 22u, 23u, 24u, 25u,
      26u, 27u, 28u, 29u, 30u, 31u, 31u, 31u, 31u, 31u, 31u, 31u, 31u,
      31u, 31u, 31u, 31u, 31u, 31u, 31u, 31u, 31u, 31u, 31u, 31u, 31u,
      31u, 31u, 31u, 31u, 56u, 31u, 31u, 31u, 31u, 31u, 31u, 31u};

  assert((t0 + b0) >> 4u < 64u);
  uint_fast32_t s0 = clipped_values[(t0 + b0) >> 4u];

  assert((t1 + b1) >> 4u < 64u);
  uint_fast32_t s1 = clipped_values[(t1 + b1) >> 4u] << 5u;

  assert((t2 + b2) >> 4u < 64u);
  uint_fast32_t s2 = clipped_values[(t2 + b2) >> 4u] << 10u;

  return s0 | s1 | s2;
}

uint16_t GbaPpuBlendUnitDarken(const GbaPpuBlendUnit* blend_unit,
                               uint_fast8_t eva, uint_fast8_t evb,
                               uint_fast8_t evy) {
  if (blend_unit->top_priority > blend_unit->first_layer_priority ||
      blend_unit->top_layer == GBA_PPU_LAYER_BACKDROP) {
    return GbaPpuBlendUnitNoBlend(blend_unit);
  }

  if (blend_unit->obj_semi_transparent &&
      blend_unit->top_layer == GBA_PPU_LAYER_OBJ &&
      blend_unit->bottom_layers[0u] != GBA_PPU_LAYER_BACKDROP) {
    return GbaPpuBlendUnitBlend(blend_unit, eva, evb);
  }

  const static uint8_t clipped_weights[32u] = {
      16u, 15u, 14u, 13u, 12u, 11u, 10u, 9u, 8u, 7u, 6u, 5u, 4u, 3u, 2u, 1u,
      0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u};

  assert(evy < 32u);
  evy = clipped_weights[evy];

  uint_fast32_t color = blend_unit->layers[blend_unit->top_layer];
  uint_fast32_t c0 = (((color & 0x001Fu) * evy) >> 4u);
  uint_fast32_t c1 = (((color & 0x03E0u) * evy) >> 4u) & 0x03E0u;
  uint_fast32_t c2 = (((color & 0x7C00u) * evy) >> 4u) & 0x7C00u;

  return c0 | c1 | c2;
}

uint16_t GbaPpuBlendUnitBrighten(const GbaPpuBlendUnit* blend_unit,
                                 uint_fast8_t eva, uint_fast8_t evb,
                                 uint_fast8_t evy) {
  if (blend_unit->top_priority > blend_unit->first_layer_priority ||
      blend_unit->top_layer == GBA_PPU_LAYER_BACKDROP) {
    return GbaPpuBlendUnitNoBlend(blend_unit);
  }

  if (blend_unit->obj_semi_transparent &&
      blend_unit->top_layer == GBA_PPU_LAYER_OBJ &&
      blend_unit->bottom_layers[0u] != GBA_PPU_LAYER_BACKDROP) {
    return GbaPpuBlendUnitBlend(blend_unit, eva, evb);
  }

  const static uint8_t clipped_weights[32u] = {
      16u, 15u, 14u, 13u, 12u, 11u, 10u, 9u, 8u, 7u, 6u, 5u, 4u, 3u, 2u, 1u,
      0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u};

  assert(evy < 32u);
  evy = clipped_weights[evy];

  uint_fast32_t color = blend_unit->layers[blend_unit->top_layer];
  uint_fast32_t c0 = (((color & 0x001Fu) * evy) >> 0u);
  uint_fast32_t c1 = (((color & 0x03E0u) * evy) >> 5u);
  uint_fast32_t c2 = (((color & 0x7C00u) * evy) >> 10u);

  uint_fast8_t inverse_weight = 16u - evy;
  uint_fast32_t w0 = 31u * inverse_weight;
  uint_fast32_t w1 = w0;
  uint_fast32_t w2 = w1;

  const static uint8_t clipped_values[64u] = {
      0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u,  10u, 11u, 12u,
      13u, 14u, 15u, 16u, 17u, 18u, 19u, 20u, 21u, 22u, 23u, 24u, 25u,
      26u, 27u, 28u, 29u, 30u, 31u, 31u, 33u, 34u, 35u, 36u, 37u, 38u,
      39u, 40u, 41u, 42u, 43u, 44u, 45u, 46u, 47u, 48u, 49u, 50u, 51u,
      52u, 53u, 54u, 55u, 56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u};

  assert((c0 + w0) >> 4u < 64u);
  uint_fast32_t s0 = clipped_values[(c0 + w0) >> 4u];

  assert((c1 + w1) >> 4u < 64u);
  uint_fast32_t s1 = clipped_values[(c1 + w1) >> 4u] << 5u;

  assert((c2 + w2) >> 4u < 64u);
  uint_fast32_t s2 = clipped_values[(c2 + w2) >> 4u] << 10u;

  return s0 | s1 | s2;
}