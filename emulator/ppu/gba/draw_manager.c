#include "emulator/ppu/gba/draw_manager.h"

static void GbaPpuDrawManagerUpdate(GbaPpuDrawManager* draw_manager,
                                    const GbaPpuRegisters* registers,
                                    const GbaPpuDirtyBits* dirty_bits) {
  if (registers->dispcnt.forced_blank) {
    draw_manager->blank = true;
    return;
  }

  draw_manager->mode = registers->dispcnt.mode;

  switch (registers->dispcnt.mode) {
    case 0:
      draw_manager->mode_0.bg0_enabled = registers->dispcnt.bg0_enable;
      draw_manager->mode_0.bg1_enabled = registers->dispcnt.bg1_enable;
      draw_manager->mode_0.bg2_enabled = registers->dispcnt.bg2_enable;
      draw_manager->mode_0.bg3_enabled = registers->dispcnt.bg3_enable;
      draw_manager->mode_0.bg0_mosaic = registers->bgcnt[0u].mosaic;
      draw_manager->mode_0.bg1_mosaic = registers->bgcnt[1u].mosaic;
      draw_manager->mode_0.bg2_mosaic = registers->bgcnt[2u].mosaic;
      draw_manager->mode_0.bg3_mosaic = registers->bgcnt[3u].mosaic;
      break;
    case 1:
      draw_manager->mode_1.bg0_enabled = registers->dispcnt.bg0_enable;
      draw_manager->mode_1.bg1_enabled = registers->dispcnt.bg1_enable;
      draw_manager->mode_1.bg2_enabled = registers->dispcnt.bg2_enable;
      draw_manager->mode_1.bg0_mosaic = registers->bgcnt[0u].mosaic;
      draw_manager->mode_1.bg1_mosaic = registers->bgcnt[1u].mosaic;
      draw_manager->mode_1.bg2_mosaic = registers->bgcnt[2u].mosaic;
      break;
    case 2:
      draw_manager->mode_2.bg2_enabled = registers->dispcnt.bg2_enable;
      draw_manager->mode_2.bg3_enabled = registers->dispcnt.bg3_enable;
      draw_manager->mode_2.bg2_mosaic = registers->bgcnt[2u].mosaic;
      draw_manager->mode_2.bg3_mosaic = registers->bgcnt[3u].mosaic;
      break;
    case 3:
      draw_manager->mode_3.bg2_enabled = registers->dispcnt.bg2_enable;
      draw_manager->mode_3.bg2_mosaic = registers->bgcnt[2u].mosaic;
      break;
    case 4:
      draw_manager->mode_4.bg2_enabled = registers->dispcnt.bg2_enable;
      draw_manager->mode_4.bg2_mosaic = registers->bgcnt[2u].mosaic;
      draw_manager->mode_4.page = registers->dispcnt.page_select;
      break;
    case 5:
      draw_manager->mode_5.bg2_enabled = registers->dispcnt.bg2_enable;
      draw_manager->mode_5.bg2_mosaic = registers->bgcnt[2u].mosaic;
      draw_manager->mode_5.page = registers->dispcnt.page_select;
      break;
  }
}

static bool GbaPpuDrawManagerCheckMode0(const GbaPpuDrawManager* draw_manager,
                                        const GbaPpuRegisters* registers,
                                        const GbaPpuDirtyBits* dirty_bits) {
  if (registers->dispcnt.bg0_enable != draw_manager->mode_0.bg0_enabled ||
      registers->dispcnt.bg1_enable != draw_manager->mode_0.bg1_enabled ||
      registers->dispcnt.bg2_enable != draw_manager->mode_0.bg2_enabled ||
      registers->dispcnt.bg3_enable != draw_manager->mode_0.bg3_enabled) {
    return true;
  }

  if (registers->bgcnt[0u].mosaic != draw_manager->mode_0.bg0_mosaic ||
      registers->bgcnt[1u].mosaic != draw_manager->mode_0.bg1_mosaic ||
      registers->bgcnt[2u].mosaic != draw_manager->mode_0.bg2_mosaic ||
      registers->bgcnt[3u].mosaic != draw_manager->mode_0.bg3_mosaic) {
    return true;
  }

  if ((registers->bgcnt[0u].mosaic || registers->bgcnt[1u].mosaic ||
       registers->bgcnt[2u].mosaic || registers->bgcnt[3u].mosaic) &&
      (registers->mosaic.bg_horiz != draw_manager->mosaic.bg_horiz ||
       registers->mosaic.bg_vert != draw_manager->mosaic.bg_vert)) {
    return true;
  }

  if (dirty_bits->io.bg_offset[0u] || dirty_bits->io.bg_offset[1u] ||
      dirty_bits->io.bg_offset[2u] || dirty_bits->io.bg_offset[3u]) {
    return true;
  }

  // TODO: Check tilemap
  // TODO: Check tiles
  // TODO: Check bgcnt

  return dirty_bits->palette.bg_palette;
}

static bool GbaPpuDrawManagerCheckMode1(const GbaPpuDrawManager* draw_manager,
                                        const GbaPpuRegisters* registers,
                                        const GbaPpuDirtyBits* dirty_bits) {
  if (registers->dispcnt.bg0_enable != draw_manager->mode_1.bg0_enabled ||
      registers->dispcnt.bg1_enable != draw_manager->mode_1.bg1_enabled ||
      registers->dispcnt.bg2_enable != draw_manager->mode_1.bg2_enabled) {
    return true;
  }

  if (registers->bgcnt[0u].mosaic != draw_manager->mode_1.bg0_mosaic ||
      registers->bgcnt[1u].mosaic != draw_manager->mode_1.bg1_mosaic ||
      registers->bgcnt[2u].mosaic != draw_manager->mode_1.bg2_mosaic) {
    return true;
  }

  if ((registers->bgcnt[0u].mosaic || registers->bgcnt[1u].mosaic ||
       registers->bgcnt[2u].mosaic) &&
      (registers->mosaic.bg_horiz != draw_manager->mosaic.bg_horiz ||
       registers->mosaic.bg_vert != draw_manager->mosaic.bg_vert)) {
    return true;
  }

  if (dirty_bits->io.bg_offset[0u] || dirty_bits->io.bg_offset[1u] ||
      dirty_bits->io.bg_offset[2u]) {
    return true;
  }

  // TODO: Check tilemap
  // TODO: Check tiles
  // TODO: Check bgcnt

  return dirty_bits->palette.bg_palette;
}

static bool GbaPpuDrawManagerCheckMode2(const GbaPpuDrawManager* draw_manager,
                                        const GbaPpuRegisters* registers,
                                        const GbaPpuDirtyBits* dirty_bits) {
  if (registers->dispcnt.bg2_enable != draw_manager->mode_2.bg2_enabled ||
      registers->dispcnt.bg3_enable != draw_manager->mode_2.bg3_enabled) {
    return true;
  }

  if (registers->bgcnt[2u].mosaic != draw_manager->mode_2.bg2_mosaic ||
      registers->bgcnt[3u].mosaic != draw_manager->mode_2.bg3_mosaic) {
    return true;
  }

  if ((registers->bgcnt[2u].mosaic || registers->bgcnt[3u].mosaic) &&
      (registers->mosaic.bg_horiz != draw_manager->mosaic.bg_horiz ||
       registers->mosaic.bg_vert != draw_manager->mosaic.bg_vert)) {
    return true;
  }

  if (dirty_bits->io.bg_offset[2u] || dirty_bits->io.bg_offset[3u]) {
    return true;
  }

  // TODO: Check tilemap
  // TODO: Check tiles
  // TODO: Check bgcnt

  return dirty_bits->palette.bg_palette;
}

static bool GbaPpuDrawManagerCheckMode3(const GbaPpuDrawManager* draw_manager,
                                        const GbaPpuRegisters* registers,
                                        const GbaPpuDirtyBits* dirty_bits) {
  if (registers->dispcnt.bg2_enable != draw_manager->mode_3.bg2_enabled) {
    return true;
  }

  if (registers->bgcnt[2u].mosaic != draw_manager->mode_3.bg2_mosaic) {
    return true;
  }

  if (registers->bgcnt[2u].mosaic &&
      (registers->mosaic.bg_horiz != draw_manager->mosaic.bg_horiz ||
       registers->mosaic.bg_vert != draw_manager->mosaic.bg_vert)) {
    return true;
  }

  if (dirty_bits->io.bg_affine_x[0u] || dirty_bits->io.bg_affine_y[0u] ||
      dirty_bits->io.bg_affine_params[0u]) {
    return true;
  }

  return dirty_bits->vram.mode_3.overall;
}

static bool GbaPpuDrawManagerCheckMode4(const GbaPpuDrawManager* draw_manager,
                                        const GbaPpuRegisters* registers,
                                        const GbaPpuDirtyBits* dirty_bits) {
  if (registers->dispcnt.bg2_enable != draw_manager->mode_4.bg2_enabled) {
    return true;
  }

  if (registers->dispcnt.page_select != draw_manager->mode_4.page) {
    return true;
  }

  if (registers->bgcnt[2u].mosaic != draw_manager->mode_4.bg2_mosaic) {
    return true;
  }

  if (registers->bgcnt[2u].mosaic &&
      (registers->mosaic.bg_horiz != draw_manager->mosaic.bg_horiz ||
       registers->mosaic.bg_vert != draw_manager->mosaic.bg_vert)) {
    return true;
  }

  if (dirty_bits->io.bg_affine_x[0u] || dirty_bits->io.bg_affine_y[0u] ||
      dirty_bits->io.bg_affine_params[0u]) {
    return true;
  }

  return dirty_bits->vram.mode_4.pages[registers->dispcnt.page_select] ||
         dirty_bits->palette.bg_palette;
}

static bool GbaPpuDrawManagerCheckMode5(const GbaPpuDrawManager* draw_manager,
                                        const GbaPpuRegisters* registers,
                                        const GbaPpuDirtyBits* dirty_bits) {
  if (registers->dispcnt.bg2_enable != draw_manager->mode_5.bg2_enabled) {
    return true;
  }

  if (registers->dispcnt.page_select != draw_manager->mode_5.page) {
    return true;
  }

  if (registers->bgcnt[2u].mosaic != draw_manager->mode_5.bg2_mosaic) {
    return true;
  }

  if (registers->bgcnt[2u].mosaic &&
      (registers->mosaic.bg_horiz != draw_manager->mosaic.bg_horiz ||
       registers->mosaic.bg_vert != draw_manager->mosaic.bg_vert)) {
    return true;
  }

  if (dirty_bits->io.bg_affine_x[0u] || dirty_bits->io.bg_affine_y[0u] ||
      dirty_bits->io.bg_affine_params[0u]) {
    return true;
  }

  return dirty_bits->vram.mode_5.pages[registers->dispcnt.page_select];
}

static bool GbaPpuDrawManagerIsDirty(const GbaPpuDrawManager* draw_manager,
                                     const GbaPpuRegisters* registers,
                                     const GbaPpuDirtyBits* dirty_bits) {
  if (dirty_bits->io.dispcnt) {
    return true;
  }

  if (dirty_bits->io.blend) {
    return true;
  }

  if (dirty_bits->composite.window) {
    return true;
  }

  // TODO: Check objects and rotations

  bool bg_changed = false;
  switch (registers->dispcnt.mode) {
    case 0:
      bg_changed =
          GbaPpuDrawManagerCheckMode0(draw_manager, registers, dirty_bits);
      break;
    case 1:
      bg_changed =
          GbaPpuDrawManagerCheckMode1(draw_manager, registers, dirty_bits);
      break;
    case 2:
      bg_changed =
          GbaPpuDrawManagerCheckMode2(draw_manager, registers, dirty_bits);
      break;
    case 3:
      bg_changed =
          GbaPpuDrawManagerCheckMode3(draw_manager, registers, dirty_bits);
      break;
    case 4:
      bg_changed =
          GbaPpuDrawManagerCheckMode4(draw_manager, registers, dirty_bits);
      break;
    case 5:
      bg_changed =
          GbaPpuDrawManagerCheckMode5(draw_manager, registers, dirty_bits);
      break;
  }

  if (bg_changed) {
    return true;
  }

  return false;
}

void GbaPpuDrawManagerStartFrame(GbaPpuDrawManager* draw_manager,
                                 const GbaPpuRegisters* registers,
                                 const GbaPpuDirtyBits* dirty_bits) {
  GbaPpuDrawManagerUpdate(draw_manager, registers, dirty_bits);
}

bool GbaPpuDrawManagerShouldFlush(GbaPpuDrawManager* draw_manager,
                                  const GbaPpuRegisters* registers,
                                  const GbaPpuDirtyBits* dirty_bits) {
  bool dirty = GbaPpuDrawManagerIsDirty(draw_manager, registers, dirty_bits);
  GbaPpuDrawManagerUpdate(draw_manager, registers, dirty_bits);
  return dirty;
}