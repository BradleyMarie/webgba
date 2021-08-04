#include "emulator/dma/gba/dma.h"

#include <assert.h>
#include <stdlib.h>

#define DMA0SAD_OFFSET 0x00u
#define DMA0DAD_OFFSET 0x04u
#define DMA0CNT_L_OFFSET 0x08u
#define DMA0CNT_H_OFFSET 0x0Au
#define DMA1SAD_OFFSET 0x0Cu
#define DMA1DAD_OFFSET 0x10u
#define DMA1CNT_L_OFFSET 0x14u
#define DMA1CNT_H_OFFSET 0x16u
#define DMA2SAD_OFFSET 0x18u
#define DMA2DAD_OFFSET 0x1Cu
#define DMA2CNT_L_OFFSET 0x20u
#define DMA2CNT_H_OFFSET 0x22u
#define DMA3SAD_OFFSET 0x24u
#define DMA3DAD_OFFSET 0x28u
#define DMA3CNT_L_OFFSET 0x2Cu
#define DMA3CNT_H_OFFSET 0x2Eu

#define GBA_DMA_UNIT_REGISTERS_SIZE 0x30u
#define GBA_NUM_DMA_UNITS 4u

#define GBA_DMA_IMMEDIATE 0u
#define GBA_DMA_VBLANK 1u
#define GBA_DMA_HBLANK 2u
#define GBA_DMA_SPECIAL 3u

typedef union {
  struct {
    unsigned short unused : 5;
    unsigned short dest_addr : 2;
    unsigned short src_addr : 2;
    bool repeat : 1;
    bool transfer_word : 1;
    bool gamepak_drq : 1;
    unsigned short start_timing : 2;
    bool irq_enable : 1;
    bool enabled : 1;
  };
  uint16_t value;
} DmaControlRegister;

typedef struct {
  uint32_t source;
  uint32_t destination;
  uint16_t word_count;
  DmaControlRegister control;
} DmaUnitRegisters;

typedef union {
  DmaUnitRegisters units[GBA_NUM_DMA_UNITS];
  uint32_t words[12];
  uint16_t half_words[24];
  uint8_t bytes[48];
} GbaDmaUnitRegisters;

struct _GbaDmaUnit {
  GbaDmaUnitRegisters registers;
  bool active[GBA_NUM_DMA_UNITS];
  GbaPlatform *platform;
  uint16_t reference_count;
};

static void GbaDmaUnitUpdateAfterWrite(GbaDmaUnit *dma_unit) {
  for (uint_fast8_t i = 0; i < GBA_NUM_DMA_UNITS; i++) {
    assert(!dma_unit->active[i]);
    dma_unit->active[i] =
        dma_unit->registers.units[i].control.enabled &&
        dma_unit->registers.units[i].control.start_timing == GBA_DMA_IMMEDIATE;
  }
}

static bool GbaDmaUnitRegistersLoad16LE(const void *context, uint32_t address,
                                        uint16_t *value) {
  assert((address & 0x1u) == 0u);

  if (address >= GBA_DMA_UNIT_REGISTERS_SIZE) {
    return false;
  }

  const GbaDmaUnit *dma_unit = (const GbaDmaUnit *)context;

  switch (address) {
    case DMA0CNT_H_OFFSET:
      *value = dma_unit->registers.units[0].control.value;
      return true;
    case DMA1CNT_H_OFFSET:
      *value = dma_unit->registers.units[1].control.value;
      return true;
    case DMA2CNT_H_OFFSET:
      *value = dma_unit->registers.units[2].control.value;
      return true;
    case DMA3CNT_H_OFFSET:
      *value = dma_unit->registers.units[3].control.value;
      return true;
  }

  return false;
}

static bool GbaDmaUnitRegistersLoad32LE(const void *context, uint32_t address,
                                        uint32_t *value) {
  assert((address & 0x3u) == 0u);

  uint16_t low_bits;
  bool low = GbaDmaUnitRegistersLoad16LE(context, address, &low_bits);
  if (!low) {
    return false;
  }

  uint16_t high_bits;
  bool high = GbaDmaUnitRegistersLoad16LE(context, address + 2u, &high_bits);
  if (high) {
    *value = (((uint32_t)high_bits) << 16u) | (uint32_t)low_bits;
  } else {
    *value = 0u;
  }

  return true;
}

static bool GbaDmaUnitRegistersLoad8(const void *context, uint32_t address,
                                     uint8_t *value) {
  uint32_t read_address = address & 0xFFFFFFFEu;

  uint16_t value16;
  bool success = GbaDmaUnitRegistersLoad16LE(context, read_address, &value16);
  if (success) {
    *value = (address == read_address) ? value16 : value16 >> 8u;
  }

  return success;
}

static bool GbaDmaUnitRegistersStore32LE(void *context, uint32_t address,
                                         uint32_t value) {
  assert((address & 0x3u) == 0u);

  if (address >= GBA_DMA_UNIT_REGISTERS_SIZE) {
    return true;
  }

  GbaDmaUnit *dma_unit = (GbaDmaUnit *)context;

  dma_unit->registers.words[address >> 2u] = value;

  // Update active bits
  GbaDmaUnitUpdateAfterWrite(dma_unit);

  return true;
}

static bool GbaDmaUnitRegistersStore16LE(void *context, uint32_t address,
                                         uint16_t value) {
  assert((address & 0x1u) == 0u);

  if (address >= GBA_DMA_UNIT_REGISTERS_SIZE) {
    return true;
  }

  GbaDmaUnit *dma_unit = (GbaDmaUnit *)context;

  dma_unit->registers.half_words[address >> 1u] = value;

  // Update active bits
  GbaDmaUnitUpdateAfterWrite(dma_unit);

  return true;
}

static bool GbaDmaUnitRegistersStore8(void *context, uint32_t address,
                                      uint8_t value) {
  if (address >= GBA_DMA_UNIT_REGISTERS_SIZE) {
    return true;
  }

  GbaDmaUnit *dma_unit = (GbaDmaUnit *)context;

  dma_unit->registers.bytes[address] = value;

  // Update active bits
  GbaDmaUnitUpdateAfterWrite(dma_unit);

  return true;
}

void GbaDmaUnitMemoryFree(void *context) {
  GbaDmaUnit *dma_unit = (GbaDmaUnit *)context;
  GbaDmaUnitFree(dma_unit);
}

bool GbaDmaUnitAllocate(GbaPlatform *platform, GbaDmaUnit **dma_unit,
                        Memory **registers) {
  *dma_unit = (GbaDmaUnit *)calloc(1, sizeof(GbaDmaUnit));
  if (*dma_unit == NULL) {
    return false;
  }

  *registers =
      MemoryAllocate(*dma_unit, GbaDmaUnitRegistersLoad32LE,
                     GbaDmaUnitRegistersLoad16LE, GbaDmaUnitRegistersLoad8,
                     GbaDmaUnitRegistersStore32LE, GbaDmaUnitRegistersStore16LE,
                     GbaDmaUnitRegistersStore8, GbaDmaUnitMemoryFree);
  if (*registers == NULL) {
    free(*dma_unit);
    return false;
  }

  (*dma_unit)->platform = platform;
  (*dma_unit)->reference_count = 2u;

  GbaPlatformRetain(platform);

  return true;
}

bool GbaDmaIsActive(const GbaDmaUnit *dma_unit) {
  bool active = dma_unit->active[0] || dma_unit->active[1] ||
                dma_unit->active[2] || dma_unit->active[3];
  return active;
}

void GbaDmaUnitStep(GbaDmaUnit *dma_unit, Memory *memory) {}

void GbaDmaUnitSignalHBlank(GbaDmaUnit *dma_unit, uint_fast8_t vcount) {
  for (uint_fast8_t i = 0; i < GBA_NUM_DMA_UNITS; i++) {
    if (dma_unit->registers.units[i].control.enabled &&
        dma_unit->registers.units[i].control.start_timing == GBA_DMA_HBLANK) {
      dma_unit->active[i] = true;
    }
  }

  if (dma_unit->registers.units[3].control.enabled &&
      dma_unit->registers.units[3].control.start_timing == GBA_DMA_SPECIAL &&
      vcount >= 2u && vcount < 162) {
    dma_unit->active[3] = true;
  }
}

void GbaDmaUnitSignalVBlank(GbaDmaUnit *dma_unit) {
  for (uint_fast8_t i = 0; i < GBA_NUM_DMA_UNITS; i++) {
    if (dma_unit->registers.units[i].control.enabled &&
        dma_unit->registers.units[i].control.start_timing == GBA_DMA_VBLANK) {
      dma_unit->active[i] = true;
    }
  }
}

void GbaDmaUnitSignalFifoRefresh(GbaDmaUnit *dma_unit) {
  for (uint_fast8_t i = 1; i <= 2; i++) {
    if (dma_unit->registers.units[i].control.enabled &&
        dma_unit->registers.units[i].control.start_timing == GBA_DMA_SPECIAL) {
      dma_unit->active[i] = true;
    }
  }
}

void GbaDmaUnitFree(GbaDmaUnit *dma_unit) {
  assert(dma_unit->reference_count != 0u);
  dma_unit->reference_count -= 1u;
  if (dma_unit->reference_count == 0u) {
    free(dma_unit);
  }
}

static_assert(sizeof(GbaDmaUnitRegisters) == GBA_DMA_UNIT_REGISTERS_SIZE,
              "sizeof(GbaDmaUnitRegisters) != GBA_DMA_UNIT_REGISTERS_SIZE");