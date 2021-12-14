#include "emulator/ppu/gba/window.h"

static inline bool IsInsideWindow1D(uint_fast8_t lower_bound,
                                    uint_fast8_t upper_bound,
                                    uint_fast8_t value) {
  if (upper_bound < lower_bound) {
    return lower_bound <= value || value < upper_bound;
  }

  return lower_bound <= value && value < upper_bound;
}

static inline bool IsInsideWindow2D(const WindowBoundsRegister *winh,
                                    const WindowBoundsRegister *winv,
                                    uint_fast8_t x, uint_fast8_t y) {
  return IsInsideWindow1D(winh->start, winh->end, x) &&
         IsInsideWindow1D(winv->start, winv->end, y);
}

void GbaPpuWindowCheck(const GbaPpuRegisters *registers, uint_fast8_t x,
                       uint_fast8_t y, bool on_obj_mask, bool *draw_obj,
                       bool *draw_bg0, bool *draw_bg1, bool *draw_bg2,
                       bool *draw_bg3, bool *enable_blending) {
  bool winout_enabled = false;
  if (registers->dispcnt.win0_enable) {
    winout_enabled = true;
    if (IsInsideWindow2D(&registers->win0h, &registers->win0v, x, y)) {
      *draw_obj = registers->winin.win0.obj;
      *draw_bg0 = registers->winin.win0.bg0;
      *draw_bg1 = registers->winin.win0.bg1;
      *draw_bg2 = registers->winin.win0.bg2;
      *draw_bg3 = registers->winin.win0.bg3;
      *enable_blending = registers->winin.win0.bld;
      return;
    }
  }

  if (registers->dispcnt.win1_enable) {
    winout_enabled = true;
    if (IsInsideWindow2D(&registers->win1h, &registers->win1v, x, y)) {
      *draw_obj = registers->winin.win1.obj;
      *draw_bg0 = registers->winin.win1.bg0;
      *draw_bg1 = registers->winin.win1.bg1;
      *draw_bg2 = registers->winin.win1.bg2;
      *draw_bg3 = registers->winin.win1.bg3;
      *enable_blending = registers->winin.win1.bld;
      return;
    }
  }

  if (registers->dispcnt.winobj_enable) {
    winout_enabled = true;
    if (on_obj_mask) {
      *draw_obj = registers->winout.winobj.obj;
      *draw_bg0 = registers->winout.winobj.bg0;
      *draw_bg1 = registers->winout.winobj.bg1;
      *draw_bg2 = registers->winout.winobj.bg2;
      *draw_bg3 = registers->winout.winobj.bg3;
      *enable_blending = registers->winout.winobj.bld;
      return;
    }
  }

  // If winout is not enabled, then no windows were enabled in which case all
  // layers should be drawn.
  if (!winout_enabled) {
    *draw_obj = true;
    *draw_bg0 = true;
    *draw_bg1 = true;
    *draw_bg2 = true;
    *draw_bg3 = true;
    *enable_blending = true;
    return;
  }

  *draw_obj = registers->winout.winout.obj;
  *draw_bg0 = registers->winout.winout.bg0;
  *draw_bg1 = registers->winout.winout.bg1;
  *draw_bg2 = registers->winout.winout.bg2;
  *draw_bg3 = registers->winout.winout.bg3;
  *enable_blending = registers->winout.winout.bld;
}