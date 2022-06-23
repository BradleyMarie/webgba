#include "emulator/ppu/gba/software/blend.h"

#include <assert.h>
#include <math.h>

#define GBA_PPU_LAYER_PRIORITY_BACKDROP 5u
#define GBA_PPU_LAYER_PRIORITY_NOT_SET 6u

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

  float top_b = (float)((blend_unit->layers[0u] & 0x003Eu) >> 1u) / 31.0f;
  float top_g = (float)((blend_unit->layers[0u] & 0x07C0u) >> 6u) / 31.0f;
  float top_r = (float)((blend_unit->layers[0u] & 0xF800u) >> 11u) / 31.0f;

  float bot_b = (float)((blend_unit->layers[1u] & 0x003Eu) >> 1u) / 31.0f;
  float bot_g = (float)((blend_unit->layers[1u] & 0x07C0u) >> 6u) / 31.0f;
  float bot_r = (float)((blend_unit->layers[1u] & 0xF800u) >> 11u) / 31.0f;

  float eva = fminf((float)registers->bldalpha.eva / 16.0f, 1.0f);
  float evb = fminf((float)registers->bldalpha.evb / 16.0f, 1.0f);

  float r = fminf((top_r * eva) + (bot_r * evb), 1.0f);
  float g = fminf((top_g * eva) + (bot_g * evb), 1.0f);
  float b = fminf((top_b * eva) + (bot_b * evb), 1.0f);

  rgb[0u] = roundf(r * 255.0f);
  rgb[1u] = roundf(g * 255.0f);
  rgb[2u] = roundf(b * 255.0f);
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

  float b = (float)((blend_unit->layers[0u] & 0x003Eu) >> 1u) / 31.0f;
  float g = (float)((blend_unit->layers[0u] & 0x07C0u) >> 6u) / 31.0f;
  float r = (float)((blend_unit->layers[0u] & 0xF800u) >> 11u) / 31.0f;

  float evy = fmaxf(1.0f - (float)registers->bldy.evy / 16.0f, 0.0f);

  r *= evy;
  g *= evy;
  b *= evy;

  rgb[0u] = roundf(r * 255.0f);
  rgb[1u] = roundf(g * 255.0f);
  rgb[2u] = roundf(b * 255.0f);
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

  float b = (float)((blend_unit->layers[0u] & 0x003Eu) >> 1u) / 31.0f;
  float g = (float)((blend_unit->layers[0u] & 0x07C0u) >> 6u) / 31.0f;
  float r = (float)((blend_unit->layers[0u] & 0xF800u) >> 11u) / 31.0f;

  float evy = fminf((float)registers->bldy.evy / 16.0f, 1.0f);

  r += (1.0f - r) * evy;
  g += (1.0f - g) * evy;
  b += (1.0f - b) * evy;

  rgb[0u] = roundf(r * 255.0f);
  rgb[1u] = roundf(g * 255.0f);
  rgb[2u] = roundf(b * 255.0f);
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
  static const BlendFunction blend_table[4u] = {
      GbaPpuBlendUnitNoBlendInternal, GbaPpuBlendUnitAdditiveBlend,
      GbaPpuBlendUnitBrighten, GbaPpuBlendUnitDarken};
  assert(registers->bldcnt.mode < 4u);

  return blend_table[registers->bldcnt.mode](blend_unit, registers, rgb);
}

void GbaPpuBlendUnitNoBlend(const GbaPpuBlendUnit* blend_unit,
                            uint8_t rgb[3u]) {
  float b = (float)((blend_unit->layers[0u] & 0x003Eu) >> 1u) / 31.0f;
  float g = (float)((blend_unit->layers[0u] & 0x07C0u) >> 6u) / 31.0f;
  float r = (float)((blend_unit->layers[0u] & 0xF800u) >> 11u) / 31.0f;

  rgb[0u] = roundf(r * 255.0f);
  rgb[1u] = roundf(g * 255.0f);
  rgb[2u] = roundf(b * 255.0f);
}