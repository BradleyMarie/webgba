#include "emulator/ppu/gba/ppu.h"

#include <assert.h>
#include <stdlib.h>

#include "emulator/ppu/gba/types.h"

struct _GbaPpu {
  GbaInterruptController *interrupt_controller;
  GbaPpuMemory memory;
  union {
    GbaPpuRegisters registers;
    const uint16_t register_half_words[44];
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

  (*ppu)->reference_count = 4u;

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

  // TODO: Registers

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