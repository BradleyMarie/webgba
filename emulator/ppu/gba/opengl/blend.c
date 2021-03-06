#include "emulator/ppu/gba/opengl/blend.h"

bool OpenGlBlendStage(OpenGlBlend* context, const GbaPpuRegisters* registers,
                      GbaPpuDirtyBits* dirty_bits) {
  if ((!registers->dispcnt.win0_enable || !registers->winin.win0.bld) &&
      (!registers->dispcnt.win1_enable || !registers->winin.win1.bld) &&
      (!registers->dispcnt.winobj_enable || !registers->winout.winobj.bld) &&
      ((registers->dispcnt.win0_enable || registers->dispcnt.win1_enable ||
        registers->dispcnt.winobj_enable) &&
       !registers->winout.winout.bld)) {
    return false;
  }

  // First Entry - (mode, eva)
  uint16_t new_value = registers->bldcnt.mode;
  new_value |= ((registers->bldalpha.eva > 16u) ? 16u : registers->bldalpha.eva)
               << 8u;
  if (new_value != context->staging[0u]) {
    context->staging[0u] = new_value;
    context->dirty = true;
  }

  // Second Entry - (top, evb)
  new_value = registers->bldcnt.value & 0x3Fu;
  new_value |= ((registers->bldalpha.evb > 16u) ? 16u : registers->bldalpha.evb)
               << 8u;
  if (new_value != context->staging[1u]) {
    context->staging[1u] = new_value;
    context->dirty = true;
  }

  // Third Entry - (bot, evy)
  new_value = (registers->bldcnt.value >> 8u) & 0x3Fu;
  new_value |= ((registers->bldy.evy > 16u) ? 16u : registers->bldy.evy) << 8u;
  if (new_value != context->staging[2u]) {
    context->staging[2u] = new_value;
    context->dirty = true;
  }

  return context->dirty;
}

void OpenGlBlendBind(const OpenGlBlend* context,
                     const UniformLocations* locations) {
  glUniform3ui(locations->blend_control_ev, context->bldcnt_ev[0u],
               context->bldcnt_ev[1u], context->bldcnt_ev[2u]);
}

void OpenGlBlendReload(OpenGlBlend* context) {
  if (context->dirty) {
    context->bldcnt_ev[0u] = context->staging[0u];
    context->bldcnt_ev[1u] = context->staging[1u];
    context->bldcnt_ev[2u] = context->staging[2u];
    context->dirty = false;
  }
}