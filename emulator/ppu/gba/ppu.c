#include "emulator/ppu/gba/ppu.h"

#include <assert.h>
#include <stdlib.h>

#include "emulator/ppu/gba/oam/oam.h"
#include "emulator/ppu/gba/palette/palette.h"
#include "emulator/ppu/gba/types.h"
#include "emulator/ppu/gba/vram/vram.h"

struct _GbaPpu {
  GbaPlatform *platform;
  GbaPpuMemory memory;
  union {
    GbaPpuRegisters registers;
    uint16_t register_half_words[44];
  };
};

static bool GbaPpuRegistersLoad16(const void *context, uint32_t address,
                                  uint16_t *value) {
  assert((address & 0x1u) == 0u);

  const GbaPpu *ppu = (const GbaPpu *)context;

  switch (address) {
    case DISPCNT_OFFSET:
      *value = ppu->registers.dispcnt.value;
      return true;
    case GREENSWP_OFFSET:
      *value = ppu->registers.greenswp;
      return true;
    case DISPSTAT_OFFSET:
      *value = ppu->registers.dispstat.value;
      return true;
    case VCOUNT_OFFSET:
      *value = ppu->registers.vcount;
      return true;
    case BG0CNT_OFFSET:
      *value = ppu->registers.bg0cnt;
      return true;
    case BG1CNT_OFFSET:
      *value = ppu->registers.bg1cnt;
      return true;
    case BG2CNT_OFFSET:
      *value = ppu->registers.bg2cnt;
      return true;
    case BG3CNT_OFFSET:
      *value = ppu->registers.bg3cnt;
      return true;
    case WININ_OFFSET:
      *value = ppu->registers.winin;
      return true;
    case WINOUT_OFFSET:
      *value = ppu->registers.winout;
      return true;
    case BLDCNT_OFFSET:
      *value = ppu->registers.bldcnt;
      return true;
  }

  return false;
}

static bool GbaPpuRegistersLoad32(const void *context, uint32_t address,
                                  uint32_t *value) {
  assert((address & 0x3u) == 0u);

  uint16_t low_bits;
  bool low = GbaPpuRegistersLoad16(context, address, &low_bits);
  if (!low) {
    return false;
  }

  uint16_t high_bits;
  bool high = GbaPpuRegistersLoad16(context, address + 2u, &high_bits);
  if (high) {
    *value = (((uint32_t)high_bits) << 16u) | (uint32_t)low_bits;
  } else {
    *value = 0u;
  }

  return true;
}

static bool GbaPpuRegistersLoad8(const void *context, uint32_t address,
                                 uint8_t *value) {
  uint32_t read_address = address & 0xFFFFFFFEu;

  uint16_t value16;
  bool success = GbaPpuRegistersLoad16(context, read_address, &value16);
  if (success) {
    *value = (address == read_address) ? value16 : value16 >> 8u;
  }

  return success;
}

static bool GbaPpuRegistersStore16(void *context, uint32_t address,
                                   uint16_t value) {
  assert((address & 0x1u) == 0u);

  if (address >= GBA_PPU_REGISTERS_SIZE) {
    return false;
  }

  GbaPpu *ppu = (GbaPpu *)context;

  // If address equals VCOUNT_OFFSET, we are attempting to write to a read-only
  // register. In this case, ignore the write and leave the register unmodified.
  if (address == VCOUNT_OFFSET) {
    return true;
  }

  // If address equals DISPSTAT_OFFSET, and any of the lower 3 bits of value are
  // set, we are attempting to modify read-only bits in the DISPSTAT register.
  // In this case, ignore mask out those bits in value so they are not modified.
  if (address == DISPSTAT_OFFSET) {
    value &= 0xFFF8u;
  }

  ppu->register_half_words[address >> 1u] = value;

  return true;
}

static bool GbaPpuRegistersStore32(void *context, uint32_t address,
                                   uint32_t value) {
  GbaPpuRegistersStore16(context, address, value);
  GbaPpuRegistersStore16(context, address + 2u, value >> 16u);
  return true;
}

static bool GbaPpuRegistersStore8(void *context, uint32_t address,
                                  uint8_t value) {
  GbaPpu *ppu = (GbaPpu *)context;

  uint32_t read_address = address & 0xFFFFFFFEu;
  uint16_t value16 = ppu->register_half_words[read_address >> 1u];
  if (address == read_address) {
    value16 &= 0xFF00;
    value16 |= value;
  } else {
    value16 &= 0x00FF;
    value16 |= (uint16_t)value << 8u;
  }

  GbaPpuRegistersStore16(context, read_address, value16);

  return true;
}

void GbaPpuRegistersFree(void *context) {
  GbaPpu *ppu = (GbaPpu *)context;
  GbaPpuFree(ppu);
}

bool GbaPpuAllocate(GbaPlatform *platform, GbaPpu **ppu, Memory **palette,
                    Memory **vram, Memory **oam, Memory **registers) {
  *ppu = (GbaPpu *)calloc(1, sizeof(GbaPpu));
  if (*ppu == NULL) {
    return false;
  }

  (*ppu)->memory.free_address = *ppu;
  (*ppu)->memory.reference_count = 1u;

  *palette = PaletteAllocate(&(*ppu)->memory);
  if (*palette == NULL) {
    free(*ppu);
    return false;
  }

  *vram = VRamAllocate(&(*ppu)->memory);
  if (*vram == NULL) {
    MemoryFree(*palette);
    free(*ppu);
    return false;
  }

  *oam = PaletteAllocate(&(*ppu)->memory);
  if (*oam == NULL) {
    MemoryFree(*vram);
    MemoryFree(*palette);
    free(*ppu);
    return false;
  }

  *registers = MemoryAllocate(*ppu, GbaPpuRegistersLoad32,
                              GbaPpuRegistersLoad16, GbaPpuRegistersLoad8,
                              GbaPpuRegistersStore32, GbaPpuRegistersStore16,
                              GbaPpuRegistersStore8, GbaPpuRegistersFree);
  if (*registers == NULL) {
    MemoryFree(*oam);
    MemoryFree(*vram);
    MemoryFree(*palette);
    free(*ppu);
    return false;
  }

  (*ppu)->platform = platform;
  (*ppu)->registers.dispcnt.forced_blank = true;
  (*ppu)->memory.reference_count += 1u;

  GbaPlatformRetain(platform);

  return true;
}

void GbaPpuStep(GbaPpu *ppu, GLuint framebuffer,
                PpuFrameDoneFunction done_function) {}

void GbaPpuReloadContext(GbaPpu *ppu) {}

void GbaPpuFree(GbaPpu *ppu) {
  assert(ppu->memory.reference_count != 0u);
  ppu->memory.reference_count -= 1u;
  if (ppu->memory.reference_count == 0u) {
    GbaPlatformRelease(ppu->platform);
    free(ppu);
  }
}