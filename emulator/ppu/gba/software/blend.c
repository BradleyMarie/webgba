#include "emulator/ppu/gba/software/blend.h"

#include <assert.h>

#include "util/macros.h"

#define GBA_PPU_LAYER_PRIORITY_BACKDROP 5u
#define GBA_PPU_LAYER_PRIORITY_NOT_SET 6u

static uint_fast16_t UInt5To8(uint_fast8_t value) {
  static const uint8_t mapped_values[32u] = {
      0u,   8u,   16u,  25u,  33u,  41u,  49u,  58u,  66u,  74u,  82u,
      90u,  99u,  107u, 115u, 123u, 132u, 140u, 148u, 156u, 165u, 173u,
      181u, 189u, 197u, 206u, 214u, 222u, 230u, 239u, 247u, 255u,
  };

  assert(value < 32u);
  return mapped_values[value];
}

static void GbaPpuBlendUnitAddBackgroundInternal(GbaPpuBlendUnit* blend_unit,
                                                 bool top, bool bottom,
                                                 uint16_t color,
                                                 uint_fast8_t priority) {
  assert(blend_unit->priorities[1u] > priority);

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

static void GbaPpuBlendUnitAdditiveBlendInternal(
    const GbaPpuBlendUnit* blend_unit, const GbaPpuRegisters* registers,
    uint8_t rgb[3u]) {
  assert(blend_unit->top[0u]);
  assert(blend_unit->bottom[1u]);

  uint_fast16_t top_r = UInt5To8((blend_unit->layers[0u] & 0x003Eu) >> 1u);
  uint_fast16_t top_g = UInt5To8((blend_unit->layers[0u] & 0x07C0u) >> 6u);
  uint_fast16_t top_b = UInt5To8((blend_unit->layers[0u] & 0xF800u) >> 11u);

  uint_fast16_t bot_r = UInt5To8((blend_unit->layers[1u] & 0x003Eu) >> 1u);
  uint_fast16_t bot_g = UInt5To8((blend_unit->layers[1u] & 0x07C0u) >> 6u);
  uint_fast16_t bot_b = UInt5To8((blend_unit->layers[1u] & 0xF800u) >> 11u);

  uint_fast16_t eva =
      registers->bldalpha.eva > 16u ? 16u : registers->bldalpha.eva;
  uint_fast16_t evb =
      registers->bldalpha.evb > 16u ? 16u : registers->bldalpha.evb;

  uint_fast16_t r = ((top_r * eva) + (bot_r * evb)) >> 4u;
  uint_fast16_t g = ((top_g * eva) + (bot_g * evb)) >> 4u;
  uint_fast16_t b = ((top_b * eva) + (bot_b * evb)) >> 4u;

  rgb[0u] = r > UINT8_MAX ? UINT8_MAX : r;
  rgb[1u] = g > UINT8_MAX ? UINT8_MAX : g;
  rgb[2u] = b > UINT8_MAX ? UINT8_MAX : b;
}

static void GbaPpuBlendUnitAdditiveBlend(const GbaPpuBlendUnit* blend_unit,
                                         const GbaPpuRegisters* registers,
                                         uint8_t rgb[3u]) {
  if (!blend_unit->top[0u] | !blend_unit->bottom[1u]) {
    GbaPpuBlendUnitNoBlend(blend_unit, rgb);
    return;
  }

  GbaPpuBlendUnitAdditiveBlendInternal(blend_unit, registers, rgb);
}

static void GbaPpuBlendUnitNoBlendInternal(const GbaPpuBlendUnit* blend_unit,
                                           const GbaPpuRegisters* registers,
                                           uint8_t rgb[3u]) {
  if ((blend_unit->is_blended_object[0u] | blend_unit->is_blended_object[1u]) &
      blend_unit->top[0u] & blend_unit->bottom[1u]) {
    return GbaPpuBlendUnitAdditiveBlendInternal(blend_unit, registers, rgb);
  }

  GbaPpuBlendUnitNoBlend(blend_unit, rgb);
}

static void GbaPpuBlendUnitDarken(const GbaPpuBlendUnit* blend_unit,
                                  const GbaPpuRegisters* registers,
                                  uint8_t rgb[3u]) {
  if (!blend_unit->top[0u]) {
    GbaPpuBlendUnitNoBlend(blend_unit, rgb);
    return;
  }

  if ((blend_unit->is_blended_object[0u] | blend_unit->is_blended_object[1u]) &
      blend_unit->bottom[1u]) {
    GbaPpuBlendUnitAdditiveBlendInternal(blend_unit, registers, rgb);
    return;
  }

  uint_fast16_t r = UInt5To8((blend_unit->layers[0u] & 0x003Eu) >> 1u);
  uint_fast16_t g = UInt5To8((blend_unit->layers[0u] & 0x07C0u) >> 6u);
  uint_fast16_t b = UInt5To8((blend_unit->layers[0u] & 0xF800u) >> 11u);

  uint_fast16_t evy =
      16u - ((registers->bldy.evy > 16u) ? 16u : registers->bldy.evy);

  rgb[0u] = (r * evy) >> 4u;
  rgb[1u] = (g * evy) >> 4u;
  rgb[2u] = (b * evy) >> 4u;
}

static void GbaPpuBlendUnitBrighten(const GbaPpuBlendUnit* blend_unit,
                                    const GbaPpuRegisters* registers,
                                    uint8_t rgb[3u]) {
  if (!blend_unit->top[0u]) {
    GbaPpuBlendUnitNoBlend(blend_unit, rgb);
    return;
  }

  if ((blend_unit->is_blended_object[0u] | blend_unit->is_blended_object[1u]) &
      blend_unit->bottom[1u]) {
    GbaPpuBlendUnitAdditiveBlendInternal(blend_unit, registers, rgb);
    return;
  }

  uint_fast16_t r = UInt5To8((blend_unit->layers[0u] & 0x003Eu) >> 1u);
  uint_fast16_t g = UInt5To8((blend_unit->layers[0u] & 0x07C0u) >> 6u);
  uint_fast16_t b = UInt5To8((blend_unit->layers[0u] & 0xF800u) >> 11u);

  uint_fast16_t evy = registers->bldy.evy > 16u ? 16u : registers->bldy.evy;

  r += ((UINT8_MAX - r) * evy) >> 4u;
  g += ((UINT8_MAX - g) * evy) >> 4u;
  b += ((UINT8_MAX - b) * evy) >> 4u;

  rgb[0u] = r > UINT8_MAX ? UINT8_MAX : r;
  rgb[1u] = g > UINT8_MAX ? UINT8_MAX : g;
  rgb[2u] = b > UINT8_MAX ? UINT8_MAX : b;
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
  if (GBA_PPU_LAYER_PRIORITY_BACKDROP < blend_unit->priorities[1u]) {
    if (GBA_PPU_LAYER_PRIORITY_BACKDROP < blend_unit->priorities[0]) {
      blend_unit->layers[0u] = color;
      blend_unit->top[0u] = registers->bldcnt.a_bd;
      blend_unit->bottom[0u] = registers->bldcnt.b_bd;
      blend_unit->is_blended_object[0u] = false;
    } else {
      blend_unit->layers[1u] = color;
      blend_unit->top[1u] = registers->bldcnt.a_bd;
      blend_unit->bottom[1u] = registers->bldcnt.b_bd;
      blend_unit->is_blended_object[1u] = false;
    }
  }
}

typedef void (*BlendFunction)(const GbaPpuBlendUnit*, const GbaPpuRegisters*,
                              uint8_t rgb[3u]);

void GbaPpuBlendUnitBlend(const GbaPpuBlendUnit* blend_unit,
                          const GbaPpuRegisters* registers, uint8_t rgb[3u]) {
  switch (registers->bldcnt.mode) {
    case 0:
      GbaPpuBlendUnitNoBlendInternal(blend_unit, registers, rgb);
      break;
    case 1:
      GbaPpuBlendUnitAdditiveBlend(blend_unit, registers, rgb);
      break;
    case 2:
      GbaPpuBlendUnitBrighten(blend_unit, registers, rgb);
      break;
    case 3:
      GbaPpuBlendUnitDarken(blend_unit, registers, rgb);
      break;
    default:
      codegen_assert(false);
  }
}

void GbaPpuBlendUnitNoBlend(const GbaPpuBlendUnit* blend_unit,
                            uint8_t rgb[3u]) {
  rgb[0u] = UInt5To8((blend_unit->layers[0u] & 0x003Eu) >> 1u);
  rgb[1u] = UInt5To8((blend_unit->layers[0u] & 0x07C0u) >> 6u);
  rgb[2u] = UInt5To8((blend_unit->layers[0u] & 0xF800u) >> 11u);
}