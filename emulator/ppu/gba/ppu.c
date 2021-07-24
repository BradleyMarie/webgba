#include "emulator/ppu/gba/ppu.h"

#include <assert.h>
#include <stdlib.h>

#include "emulator/ppu/gba/types.h"

#define REGISTERS_SIZE 88u

struct _GbaPpu {
  GbaInterruptController *interrupt_controller;
  GbaPpuMemory memory;
  union {
    GbaPpuRegisters registers;
    uint16_t register_half_words[44];
  };
  uint16_t reference_count;
};

bool PRamLoad32LEFunction(const void *context, uint32_t address,
                          uint32_t *value) {
  if (PRAM_SIZE < address + 4u) {
    return false;
  }

  const GbaPpu *ppu = (const GbaPpu *)context;
  const unsigned char *first_byte = ppu->memory.pram + address;
  *value = *(const uint32_t *)(const void *)first_byte;
  return true;
}

bool PRamLoad16LEFunction(const void *context, uint32_t address,
                          uint16_t *value) {
  if (PRAM_SIZE < address + 2u) {
    return false;
  }

  const GbaPpu *ppu = (const GbaPpu *)context;
  const unsigned char *first_byte = ppu->memory.pram + address;
  *value = *(const uint16_t *)(const void *)first_byte;
  return true;
}

bool PRamLoad8Function(const void *context, uint32_t address, uint8_t *value) {
  if (PRAM_SIZE <= address) {
    return false;
  }

  const GbaPpu *ppu = (const GbaPpu *)context;
  const unsigned char *first_byte = ppu->memory.pram + address;
  *value = *(const uint8_t *)(const void *)first_byte;
  return true;
}

bool PRamStore32LEFunction(void *context, uint32_t address, uint32_t value) {
  if (PRAM_SIZE < address + 4u) {
    return false;
  }

  GbaPpu *ppu = (GbaPpu *)context;
  unsigned char *first_byte = ppu->memory.pram + address;
  uint32_t *memory_cell = (uint32_t *)(void *)first_byte;
  *memory_cell = value;
  return true;
}

bool PRamStore16LEFunction(void *context, uint32_t address, uint16_t value) {
  if (PRAM_SIZE < address + 2u) {
    return false;
  }

  GbaPpu *ppu = (GbaPpu *)context;
  unsigned char *first_byte = ppu->memory.pram + address;
  uint16_t *memory_cell = (uint16_t *)(void *)first_byte;
  *memory_cell = value;
  return true;
}

bool VRamLoad32LEFunction(const void *context, uint32_t address,
                          uint32_t *value) {
  if (VRAM_SIZE < address + 4u) {
    return false;
  }

  const GbaPpu *ppu = (const GbaPpu *)context;
  const unsigned char *first_byte = ppu->memory.vram + address;
  *value = *(const uint32_t *)(const void *)first_byte;
  return true;
}

bool VRamLoad16LEFunction(const void *context, uint32_t address,
                          uint16_t *value) {
  if (VRAM_SIZE < address + 2u) {
    return false;
  }

  const GbaPpu *ppu = (const GbaPpu *)context;
  const unsigned char *first_byte = ppu->memory.vram + address;
  *value = *(const uint16_t *)(const void *)first_byte;
  return true;
}

bool VRamLoad8Function(const void *context, uint32_t address, uint8_t *value) {
  if (VRAM_SIZE <= address) {
    return false;
  }

  const GbaPpu *ppu = (const GbaPpu *)context;
  const unsigned char *first_byte = ppu->memory.vram + address;
  *value = *(const uint8_t *)(const void *)first_byte;
  return true;
}

bool VRamStore32LEFunction(void *context, uint32_t address, uint32_t value) {
  if (VRAM_SIZE < address + 4u) {
    return false;
  }

  GbaPpu *ppu = (GbaPpu *)context;
  unsigned char *first_byte = ppu->memory.vram + address;
  uint32_t *memory_cell = (uint32_t *)(void *)first_byte;
  *memory_cell = value;
  return true;
}

bool VRamStore16LEFunction(void *context, uint32_t address, uint16_t value) {
  if (VRAM_SIZE < address + 2u) {
    return false;
  }

  GbaPpu *ppu = (GbaPpu *)context;
  unsigned char *first_byte = ppu->memory.vram + address;
  uint16_t *memory_cell = (uint16_t *)(void *)first_byte;
  *memory_cell = value;
  return true;
}

bool OamLoad32LEFunction(const void *context, uint32_t address,
                         uint32_t *value) {
  if (OAM_SIZE < address + 4u) {
    return false;
  }

  const GbaPpu *ppu = (const GbaPpu *)context;
  const unsigned char *first_byte = ppu->memory.oam + address;
  *value = *(const uint32_t *)(const void *)first_byte;
  return true;
}

bool OamLoad16LEFunction(const void *context, uint32_t address,
                         uint16_t *value) {
  if (OAM_SIZE < address + 2u) {
    return false;
  }

  const GbaPpu *ppu = (const GbaPpu *)context;
  const unsigned char *first_byte = ppu->memory.oam + address;
  *value = *(const uint16_t *)(const void *)first_byte;
  return true;
}

bool OamLoad8Function(const void *context, uint32_t address, uint8_t *value) {
  if (OAM_SIZE <= address) {
    return false;
  }

  const GbaPpu *ppu = (const GbaPpu *)context;
  const unsigned char *first_byte = ppu->memory.oam + address;
  *value = *(const uint8_t *)(const void *)first_byte;
  return true;
}

bool OamStore32LEFunction(void *context, uint32_t address, uint32_t value) {
  if (OAM_SIZE < address + 4u) {
    return false;
  }

  GbaPpu *ppu = (GbaPpu *)context;
  unsigned char *first_byte = ppu->memory.oam + address;
  uint32_t *memory_cell = (uint32_t *)(void *)first_byte;
  *memory_cell = value;
  return true;
}

bool OamStore16LEFunction(void *context, uint32_t address, uint16_t value) {
  if (OAM_SIZE < address + 2u) {
    return false;
  }

  GbaPpu *ppu = (GbaPpu *)context;
  unsigned char *first_byte = ppu->memory.oam + address;
  uint16_t *memory_cell = (uint16_t *)(void *)first_byte;
  *memory_cell = value;
  return true;
}

bool PpuMemoryStore8Function(void *context, uint32_t address, uint8_t value) {
  return false;
}

bool GbaPpuRegistersLoad16LEFunction(const void *context, uint32_t address,
                                     uint16_t *value) {
  if (REGISTERS_SIZE < address + 2u) {
    return false;
  }

  const GbaPpu *ppu = (const GbaPpu *)context;

  assert((address & 0x1u) == 0u);
  switch (address) {
    case DISPCNT_OFFSET:
      *value = ppu->registers.dispcnt;
      return true;
    case GREENSWP_OFFSET:
      *value = ppu->registers.greenswp;
      return true;
    case DISPSTAT_OFFSET:
      *value = ppu->registers.dispstat;
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

  // Attempting to read a register that does not have read support. For these
  // reads report success, but leave value unmodified.

  return true;
}

bool GbaPpuRegistersLoad32LEFunction(const void *context, uint32_t address,
                                     uint32_t *value) {
  if (REGISTERS_SIZE < address + 4u) {
    assert(false);
  }

  const GbaPpu *ppu = (const GbaPpu *)context;

  assert((address & 0x3u) == 0u);
  switch (address) {
    case BG2X_OFFSET:
      *value = ppu->registers.bg2x;
      return true;
    case BG2Y_OFFSET:
      *value = ppu->registers.bg2y;
      return true;
    case BG3X_OFFSET:
      *value = ppu->registers.bg3x;
      return true;
    case BG3Y_OFFSET:
      *value = ppu->registers.bg3y;
      return true;
  }

  // If low_success or high_success are false, we are attempting to read a
  // register that does not have read support. In this case, ignore the error
  // and leave that portion of the bits in value unmodified.

  uint16_t low;
  bool low_success = GbaPpuRegistersLoad16LEFunction(context, address, &low);
  if (low_success) {
    *value &= 0xFFFF0000;
    *value |= low;
  }

  uint16_t high;
  bool high_success =
      GbaPpuRegistersLoad16LEFunction(context, address + 2u, &high);
  if (high_success) {
    *value &= 0x0000FFFF;
    *value |= ((uint32_t)high) << 16u;
  }

  return true;
}

bool GbaPpuRegistersLoad8Function(const void *context, uint32_t address,
                                  uint8_t *value) {
  if (REGISTERS_SIZE <= address) {
    assert(false);
  }

  uint32_t read_address = address & 0xFFFFFFFEu;

  uint16_t value16;
  bool success =
      GbaPpuRegistersLoad16LEFunction(context, read_address, &value16);
  if (success) {
    *value = (address == read_address) ? value16 : value16 >> 8u;
  }

  // If success is false, we are attempting to read a register that does not
  // have read support. In this case, ignore the error and leave value
  // unmodified.

  return true;
}

bool GbaPpuRegistersStore16LEFunction(void *context, uint32_t address,
                                      uint16_t value) {
  if (REGISTERS_SIZE < address + 2u) {
    assert(false);
  }

  // If address equals VCOUNT_OFFSET, we are attempting to write to a read-only
  // register. In this case, ignore the write and leave the register unmodified.
  if (address == VCOUNT_OFFSET) {
    return true;
  }

  GbaPpu *ppu = (GbaPpu *)context;

  assert((address & 0x1u) == 0u);

  // If address equals DISPSTAT_OFFSET, and any of the lower 3 bits of value are
  // set, we are attempting to modify read-only bits in the DISPSTAT register.
  // In this case, ignore mask out those bits in value so they are not modified.
  if (address == DISPSTAT_OFFSET) {
    value &= 0xFFF8u;
  }

  ppu->register_half_words[address >> 1u] = value;

  return true;
}

bool GbaPpuRegistersStore32LEFunction(void *context, uint32_t address,
                                      uint32_t value) {
  if (REGISTERS_SIZE < address + 4u) {
    assert(false);
  }

  GbaPpu *ppu = (GbaPpu *)context;

  assert((address & 0x3u) == 0u);
  switch (address) {
    case BG2X_OFFSET:
      ppu->registers.bg2x = value;
      return true;
    case BG2Y_OFFSET:
      ppu->registers.bg2y = value;
      return true;
    case BG3X_OFFSET:
      ppu->registers.bg3x = value;
      return true;
    case BG3Y_OFFSET:
      ppu->registers.bg3y = value;
      return true;
  }

  GbaPpuRegistersStore16LEFunction(context, address, value);
  GbaPpuRegistersStore16LEFunction(context, address + 2u, value >> 16u);

  return true;
}

bool GbaPpuRegistersStore8Function(void *context, uint32_t address,
                                   uint8_t value) {
  if (REGISTERS_SIZE <= address) {
    assert(false);
  }

  GbaPpu *ppu = (GbaPpu *)context;

  uint32_t read_address = address & 0xFFFFFFFEu;
  uint16_t value16 = ppu->register_half_words[read_address >> 1u];
  if (address == read_address) {
    value16 &= 0xFF00;
    value16 |= value;
  } else {
    value16 &= 0x00FF;
    value16 |= value << 16u;
  }

  GbaPpuRegistersStore16LEFunction(context, read_address, value16);

  return true;
}

void PpuMemoryFree(void *context) {
  GbaPpu *ppu = (GbaPpu *)context;
  GbaPpuFree(ppu);
}

bool GbaPpuAllocate(GbaInterruptController *interrupt_controller, GbaPpu **ppu,
                    Memory **pram, Memory **vram, Memory **oam,
                    Memory **registers) {
  *ppu = (GbaPpu *)calloc(1, sizeof(GbaPpu));
  if (*ppu == NULL) {
    return false;
  }

  (*ppu)->reference_count = 5u;

  *pram = MemoryAllocate(*ppu, PRamLoad32LEFunction, PRamLoad16LEFunction,
                         PRamLoad8Function, PRamStore32LEFunction,
                         PRamStore16LEFunction, PpuMemoryStore8Function,
                         PpuMemoryFree);
  if (*pram == NULL) {
    free(*ppu);
    return false;
  }

  *vram = MemoryAllocate(*ppu, VRamLoad32LEFunction, VRamLoad16LEFunction,
                         VRamLoad8Function, VRamStore32LEFunction,
                         VRamStore16LEFunction, PpuMemoryStore8Function,
                         PpuMemoryFree);
  if (*vram == NULL) {
    MemoryFree(*pram);
    free(*ppu);
    return false;
  }

  *oam = MemoryAllocate(*ppu, OamLoad32LEFunction, OamLoad16LEFunction,
                        OamLoad8Function, OamStore32LEFunction,
                        OamStore16LEFunction, PpuMemoryStore8Function,
                        PpuMemoryFree);
  if (*oam == NULL) {
    MemoryFree(*vram);
    MemoryFree(*pram);
    free(*ppu);
    return false;
  }

  *registers = MemoryAllocate(
      *ppu, GbaPpuRegistersLoad32LEFunction, GbaPpuRegistersLoad16LEFunction,
      GbaPpuRegistersLoad8Function, GbaPpuRegistersStore32LEFunction,
      GbaPpuRegistersStore16LEFunction, PpuMemoryStore8Function, PpuMemoryFree);
  if (*registers == NULL) {
    MemoryFree(*oam);
    MemoryFree(*vram);
    MemoryFree(*pram);
    free(*ppu);
    return false;
  }

  (*ppu)->interrupt_controller = interrupt_controller;

  return true;
}

bool GbaPpuStep(GbaPpu *ppu, void *output) { return false; }

void GbaPpuFree(GbaPpu *ppu) {
  assert(ppu->reference_count != 0u);
  ppu->reference_count -= 1u;
  if (ppu->reference_count == 0u) {
    GbaInterruptControllerRelease(ppu->interrupt_controller);
    free(ppu);
  }
}