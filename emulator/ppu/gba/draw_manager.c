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
      // TODO
      break;
    case 1:
      // TODO
      break;
    case 2:
      // TODO
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
  // TODO
  return false;
}

static bool GbaPpuDrawManagerCheckMode1(const GbaPpuDrawManager* draw_manager,
                                        const GbaPpuRegisters* registers,
                                        const GbaPpuDirtyBits* dirty_bits) {
  // TODO
  return false;
}

static bool GbaPpuDrawManagerCheckMode2(const GbaPpuDrawManager* draw_manager,
                                        const GbaPpuRegisters* registers,
                                        const GbaPpuDirtyBits* dirty_bits) {
  // TODO
  return false;
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

  if (dirty_bits->composite.bg_affine[0u]) {
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

  if (dirty_bits->composite.bg_affine[0u]) {
    return true;
  }

  return dirty_bits->vram.mode_4.pages[registers->dispcnt.page_select];
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

  if (dirty_bits->composite.bg_affine[0u]) {
    return true;
  }

  return dirty_bits->vram.mode_5.pages[registers->dispcnt.page_select];
}

static bool GbaPpuDrawManagerIsDirty(const GbaPpuDrawManager* draw_manager,
                                     const GbaPpuRegisters* registers,
                                     const GbaPpuDirtyBits* dirty_bits) {
  if (registers->dispcnt.forced_blank != draw_manager->blank) {
    return true;
  }

  if (registers->dispcnt.mode != draw_manager->mode) {
    return true;
  }

  if (dirty_bits->composite.window) {
    return true;
  }

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

  return dirty_bits->palette.bg_palette;
}

void GbaPpuDrawManagerInitialize(GbaPpuDrawManager* draw_manager) {
  draw_manager->previous_frame_copyable = false;
  draw_manager->flush_triggered = false;
}

void GbaPpuDrawManagerStartFrame(GbaPpuDrawManager* draw_manager,
                                 const GbaPpuRegisters* registers,
                                 const GbaPpuDirtyBits* dirty_bits) {
  draw_manager->flush_triggered =
      GbaPpuDrawManagerIsDirty(draw_manager, registers, dirty_bits);
  GbaPpuDrawManagerUpdate(draw_manager, registers, dirty_bits);
}

bool GbaPpuDrawManagerShouldFlush(GbaPpuDrawManager* draw_manager,
                                  const GbaPpuRegisters* registers,
                                  const GbaPpuDirtyBits* dirty_bits) {
  bool dirty = GbaPpuDrawManagerIsDirty(draw_manager, registers, dirty_bits);
  draw_manager->flush_triggered |= dirty;
  GbaPpuDrawManagerUpdate(draw_manager, registers, dirty_bits);
  return dirty;
}

bool GbaPpuDrawManagerEndFrame(GbaPpuDrawManager* draw_manager) {
  bool can_skip =
      !draw_manager->flush_triggered && draw_manager->previous_frame_copyable;
  draw_manager->previous_frame_copyable = !draw_manager->flush_triggered;
  draw_manager->flush_triggered = false;
  return can_skip;
}

void GbaPpuDrawManagerInvalidatePreviousFrame(GbaPpuDrawManager* draw_manager) {
  draw_manager->previous_frame_copyable = false;
}