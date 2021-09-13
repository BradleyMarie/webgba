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
}

void GbaPpuBlendUnitSet(GbaPpuBlendUnit* blend_unit, GbaPpuLayer layer,
                        bool top, bool bottom, uint16_t color,
                        GbaPpuLayerPriority priority) {
  assert(priority != GBA_PPU_LAYER_PRIORITY_TRANSPARENT ||
         layer == GBA_PPU_LAYER_BACKDROP);
  assert(GBA_PPU_LAYER_BG0 <= layer);
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
      blend_unit->bottom_layers[1u] = blend_unit->bottom_layers[1u];
      blend_unit->bottom_priorities[0u] = priority;
      blend_unit->bottom_layers[0u] = layer;
    } else if (blend_unit->bottom_priorities[1u]) {
      blend_unit->bottom_priorities[1u] = priority;
      blend_unit->bottom_layers[1u] = layer;
    }
  }
}

uint16_t GbaPpuBlendUnitNoBlend(const GbaPpuBlendUnit* blend_unit);

uint16_t GbaPpuBlendUnitBlend(const GbaPpuBlendUnit* blend_unit,
                              uint_fast8_t top_weight,
                              uint_fast8_t bottom_weight) {
  uint_fast8_t bottom_index =
      blend_unit->top_layer == blend_unit->bottom_layers[0u];

  if (blend_unit->top_priority < blend_unit->bottom_priorities[bottom_index] ||
      blend_unit->top_priority > GBA_PPU_LAYER_PRIORITY_3 ||
      blend_unit->bottom_priorities[bottom_index] > GBA_PPU_LAYER_PRIORITY_3) {
    return blend_unit->layers[blend_unit->top_layer];
  }

  const static uint8_t clipped_weights[32u] = {
      0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u,  10u,
      11u, 12u, 13u, 14u, 15u, 16u, 16u, 16u, 16u, 16u, 16u,
      16u, 16u, 16u, 16u, 16u, 16u, 16u, 16u, 16u, 16u};

  assert(top_weight < 32u);
  top_weight = clipped_weights[top_weight];

  assert(bottom_weight < 32u);
  bottom_weight = clipped_weights[bottom_weight];

  uint_fast32_t top = blend_unit->layers[blend_unit->top_layer];
  uint_fast32_t t0 = (((top & 0x001Fu) * top_weight) >> 0u);
  uint_fast32_t t1 = (((top & 0x03E0u) * top_weight) >> 5u);
  uint_fast32_t t2 = (((top & 0x7C00u) * top_weight) >> 10u);

  uint_fast32_t bottom = blend_unit->layers[bottom_index];
  uint_fast32_t b0 = (((bottom & 0x001Fu) * bottom_weight) >> 0u);
  uint_fast32_t b1 = (((bottom & 0x03E0u) * bottom_weight) >> 5u);
  uint_fast32_t b2 = (((bottom & 0x7C00u) * bottom_weight) >> 10u);

  const static uint8_t clipped_values[64u] = {
      0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u,  10u, 11u, 12u,
      13u, 14u, 15u, 16u, 17u, 18u, 19u, 20u, 21u, 22u, 23u, 24u, 25u,
      26u, 27u, 28u, 29u, 30u, 31u, 31u, 33u, 34u, 35u, 36u, 37u, 38u,
      39u, 40u, 41u, 42u, 43u, 44u, 45u, 46u, 47u, 48u, 49u, 50u, 51u,
      52u, 53u, 54u, 55u, 56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u};

  assert((t0 + b0) >> 4u < 64u);
  uint_fast32_t s0 = clipped_values[(t0 + b0) >> 4u];

  assert((t1 + b1) >> 4u < 64u);
  uint_fast32_t s1 = clipped_values[(t1 + b1) >> 4u] << 5u;

  assert((t2 + b2) >> 4u < 64u);
  uint_fast32_t s2 = clipped_values[(t2 + b2) >> 4u] << 10u;

  return s0 | s1 | s2;
}

uint16_t GbaPpuBlendUnitBlackBlend(const GbaPpuBlendUnit* blend_unit,
                                   uint_fast8_t weight) {
  const static uint8_t clipped_weights[32u] = {
      0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u,  10u,
      11u, 12u, 13u, 14u, 15u, 16u, 16u, 16u, 16u, 16u, 16u,
      16u, 16u, 16u, 16u, 16u, 16u, 16u, 16u, 16u, 16u};

  assert(weight < 32u);
  weight = clipped_weights[weight];

  uint_fast32_t color = blend_unit->layers[blend_unit->top_layer];
  uint_fast32_t c0 = (((color & 0x001Fu) * weight) >> 4u);
  uint_fast32_t c1 = (((color & 0x03E0u) * weight) >> 4u) & 0x03E0u;
  uint_fast32_t c2 = (((color & 0x7C00u) * weight) >> 4u) & 0x7C00u;

  return c0 | c1 | c2;
}

uint16_t GbaPpuBlendUnitWhiteBlend(const GbaPpuBlendUnit* blend_unit,
                                   uint_fast8_t weight) {
  const static uint8_t clipped_weights[32u] = {
      0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u,  10u,
      11u, 12u, 13u, 14u, 15u, 16u, 16u, 16u, 16u, 16u, 16u,
      16u, 16u, 16u, 16u, 16u, 16u, 16u, 16u, 16u, 16u};

  assert(weight < 32u);
  weight = clipped_weights[weight];

  uint_fast32_t color = blend_unit->layers[blend_unit->top_layer];
  uint_fast32_t c0 = (((color & 0x001Fu) * weight) >> 0u);
  uint_fast32_t c1 = (((color & 0x03E0u) * weight) >> 5u);
  uint_fast32_t c2 = (((color & 0x7C00u) * weight) >> 10u);

  uint_fast8_t inverse_weight = 16u - weight;
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