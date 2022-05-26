#include "emulator/ppu/gba/software/blend.h"

#include <assert.h>

#define GBA_PPU_LAYER_PRIORITY_BACKDROP 5u
#define GBA_PPU_LAYER_PRIORITY_NOT_SET 6u

static void GbaPpuBlendUnitAddBackgroundInternal(GbaPpuBlendUnit* blend_unit,
                                                 bool top, bool bottom,
                                                 uint16_t color,
                                                 uint_fast8_t priority) {
  if (blend_unit->priorities[1u] <= priority) {
    return;
  }

  if (blend_unit->priorities[0u] > priority) {
    blend_unit->priorities[1u] = blend_unit->priorities[0u];
    blend_unit->layers[1u] = blend_unit->layers[0u];
    blend_unit->top[1u] = blend_unit->top[0u];
    blend_unit->bottom[1u] = blend_unit->bottom[0u];
    blend_unit->is_blended_object[1u] = blend_unit->is_blended_object[0u];
    blend_unit->priorities[0u] = priority;
    blend_unit->layers[0u] = color;
    blend_unit->top[0u] = top;
    blend_unit->bottom[0u] = bottom;
    blend_unit->is_blended_object[0u] = false;
  } else {
    blend_unit->priorities[1u] = priority;
    blend_unit->layers[1u] = color;
    blend_unit->top[1u] = top;
    blend_unit->bottom[1u] = bottom;
    blend_unit->is_blended_object[1u] = false;
  }
}

static uint16_t GbaPpuBlendUnitAdditiveBlendInternal(
    const GbaPpuBlendUnit* blend_unit, const GbaPpuRegisters* registers) {
  assert(blend_unit->top[0u]);
  assert(blend_unit->bottom[1u]);

  static const uint8_t clipped_weights[32u] = {
      0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u,  10u,
      11u, 12u, 13u, 14u, 15u, 16u, 16u, 16u, 16u, 16u, 16u,
      16u, 16u, 16u, 16u, 16u, 16u, 16u, 16u, 16u, 16u};
  assert(registers->bldalpha.eva < 32u);
  assert(registers->bldalpha.evb < 32u);

  uint_fast8_t eva = clipped_weights[registers->bldalpha.eva];
  uint_fast8_t evb = clipped_weights[registers->bldalpha.evb];

  uint_fast32_t top = blend_unit->layers[0u];
  uint_fast32_t t0 = (((top & 0x001Fu) * eva) >> 0u);
  uint_fast32_t t1 = (((top & 0x03E0u) * eva) >> 5u);
  uint_fast32_t t2 = (((top & 0x7C00u) * eva) >> 10u);

  uint_fast32_t bottom = blend_unit->layers[1u];
  uint_fast32_t b0 = (((bottom & 0x001Fu) * evb) >> 0u);
  uint_fast32_t b1 = (((bottom & 0x03E0u) * evb) >> 5u);
  uint_fast32_t b2 = (((bottom & 0x7C00u) * evb) >> 10u);

  static const uint8_t clipped_values[64u] = {
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

static uint16_t GbaPpuBlendUnitAdditiveBlend(const GbaPpuBlendUnit* blend_unit,
                                             const GbaPpuRegisters* registers) {
  if (!blend_unit->top[0u] | !blend_unit->bottom[1u]) {
    return GbaPpuBlendUnitNoBlend(blend_unit);
  }

  return GbaPpuBlendUnitAdditiveBlendInternal(blend_unit, registers);
}

static uint16_t GbaPpuBlendUnitNoBlendInternal(
    const GbaPpuBlendUnit* blend_unit, const GbaPpuRegisters* registers) {
  if ((blend_unit->is_blended_object[0u] | blend_unit->is_blended_object[1u]) &
      blend_unit->top[0u] & blend_unit->bottom[1u]) {
    return GbaPpuBlendUnitAdditiveBlendInternal(blend_unit, registers);
  }

  return GbaPpuBlendUnitNoBlend(blend_unit);
}

static uint16_t GbaPpuBlendUnitDarken(const GbaPpuBlendUnit* blend_unit,
                                      const GbaPpuRegisters* registers) {
  if (!blend_unit->top[0u]) {
    return GbaPpuBlendUnitNoBlend(blend_unit);
  }

  if ((blend_unit->is_blended_object[0u] | blend_unit->is_blended_object[1u]) &
      blend_unit->bottom[1u]) {
    return GbaPpuBlendUnitAdditiveBlendInternal(blend_unit, registers);
  }

  static const uint8_t clipped_weights[32u] = {
      16u, 15u, 14u, 13u, 12u, 11u, 10u, 9u, 8u, 7u, 6u, 5u, 4u, 3u, 2u, 1u,
      0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u};
  assert(registers->bldy.evy < 32u);

  uint_fast8_t evy = clipped_weights[registers->bldy.evy];

  uint_fast32_t color = blend_unit->layers[0u];
  uint_fast32_t c0 = (((color & 0x001Fu) * evy) >> 4u);
  uint_fast32_t c1 = (((color & 0x03E0u) * evy) >> 4u) & 0x03E0u;
  uint_fast32_t c2 = (((color & 0x7C00u) * evy) >> 4u) & 0x7C00u;

  return c0 | c1 | c2;
}

static uint16_t GbaPpuBlendUnitBrighten(const GbaPpuBlendUnit* blend_unit,
                                        const GbaPpuRegisters* registers) {
  if (!blend_unit->top[0u]) {
    return GbaPpuBlendUnitNoBlend(blend_unit);
  }

  if ((blend_unit->is_blended_object[0u] | blend_unit->is_blended_object[1u]) &
      blend_unit->bottom[1u]) {
    return GbaPpuBlendUnitAdditiveBlendInternal(blend_unit, registers);
  }

  static const uint8_t clipped_weights[32u] = {
      16u, 15u, 14u, 13u, 12u, 11u, 10u, 9u, 8u, 7u, 6u, 5u, 4u, 3u, 2u, 1u,
      0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u};
  assert(registers->bldy.evy < 32u);

  uint_fast8_t evy = clipped_weights[registers->bldy.evy];

  uint_fast32_t color = blend_unit->layers[0u];
  uint_fast32_t c0 = (((color & 0x001Fu) * evy) >> 0u);
  uint_fast32_t c1 = (((color & 0x03E0u) * evy) >> 5u);
  uint_fast32_t c2 = (((color & 0x7C00u) * evy) >> 10u);

  uint_fast8_t inverse_weight = 16u - evy;
  uint_fast32_t w0 = 31u * inverse_weight;
  uint_fast32_t w1 = w0;
  uint_fast32_t w2 = w1;

  static const uint8_t clipped_values[64u] = {
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

void GbaPpuBlendUnitAddObject(GbaPpuBlendUnit* blend_unit,
                              const GbaPpuRegisters* registers, uint16_t color,
                              uint_fast8_t priority, bool semi_transparent) {
  assert(priority < GBA_PPU_LAYER_PRIORITY_BACKDROP);
  assert(blend_unit->priorities[0u] == GBA_PPU_LAYER_PRIORITY_NOT_SET);
  assert(blend_unit->priorities[1u] == GBA_PPU_LAYER_PRIORITY_NOT_SET);

  blend_unit->priorities[0u] = priority;
  blend_unit->layers[0u] = color;
  blend_unit->top[0u] = semi_transparent | registers->bldcnt.a_obj;
  blend_unit->bottom[0u] = registers->bldcnt.b_obj;
  blend_unit->is_blended_object[0u] = semi_transparent;
}

void GbaPpuBlendUnitAddBackground0(GbaPpuBlendUnit* blend_unit,
                                   const GbaPpuRegisters* registers,
                                   uint16_t color) {
  GbaPpuBlendUnitAddBackgroundInternal(blend_unit, registers->bldcnt.a_bg0,
                                       registers->bldcnt.b_bg0, color,
                                       registers->bgcnt[0u].priority);
}

void GbaPpuBlendUnitAddBackground1(GbaPpuBlendUnit* blend_unit,
                                   const GbaPpuRegisters* registers,
                                   uint16_t color) {
  GbaPpuBlendUnitAddBackgroundInternal(blend_unit, registers->bldcnt.a_bg1,
                                       registers->bldcnt.b_bg1, color,
                                       registers->bgcnt[1u].priority);
}

void GbaPpuBlendUnitAddBackground2(GbaPpuBlendUnit* blend_unit,
                                   const GbaPpuRegisters* registers,
                                   uint16_t color) {
  GbaPpuBlendUnitAddBackgroundInternal(blend_unit, registers->bldcnt.a_bg2,
                                       registers->bldcnt.b_bg2, color,
                                       registers->bgcnt[2u].priority);
}

void GbaPpuBlendUnitAddBackground3(GbaPpuBlendUnit* blend_unit,
                                   const GbaPpuRegisters* registers,
                                   uint16_t color) {
  GbaPpuBlendUnitAddBackgroundInternal(blend_unit, registers->bldcnt.a_bg3,
                                       registers->bldcnt.b_bg3, color,
                                       registers->bgcnt[3u].priority);
}

void GbaPpuBlendUnitAddBackdrop(GbaPpuBlendUnit* blend_unit,
                                const GbaPpuRegisters* registers,
                                uint16_t color) {
  GbaPpuBlendUnitAddBackgroundInternal(blend_unit, registers->bldcnt.a_bd,
                                       registers->bldcnt.b_bd, color,
                                       GBA_PPU_LAYER_PRIORITY_BACKDROP);
}

typedef uint16_t (*BlendFunction)(const GbaPpuBlendUnit*,
                                  const GbaPpuRegisters*);

uint16_t GbaPpuBlendUnitBlend(const GbaPpuBlendUnit* blend_unit,
                              const GbaPpuRegisters* registers) {
  static const BlendFunction blend_table[4u] = {
      GbaPpuBlendUnitNoBlendInternal, GbaPpuBlendUnitAdditiveBlend,
      GbaPpuBlendUnitBrighten, GbaPpuBlendUnitDarken};
  assert(registers->bldcnt.mode < 4u);

  return blend_table[registers->bldcnt.mode](blend_unit, registers);
}