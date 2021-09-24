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

#define GBA_DMA_ADDR_INCREMENT 0u
#define GBA_DMA_ADDR_DECREMENT 1u
#define GBA_DMA_ADDR_FIXED 2u
#define GBA_DMA_ADDR_INCREMENT_RELOAD 3u

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
    bool gamepak_drq : 1;  // Not Implemented
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
  unsigned active;
  bool enabled[GBA_NUM_DMA_UNITS];
  uint32_t current_source[GBA_NUM_DMA_UNITS];
  uint32_t current_destination[GBA_NUM_DMA_UNITS];
  uint16_t transfers_remaining[GBA_NUM_DMA_UNITS];
  GbaDmaUnitRegisters registers;
  GbaPlatform *platform;
  uint16_t reference_count;
};

static void GbaDmaUnitClearActive(GbaDmaUnit *dma_unit, uint_fast8_t index) {
  assert(index < GBA_NUM_DMA_UNITS);
  unsigned unset_bit_mask = ~(1u << index);
  dma_unit->active &= unset_bit_mask;
}

static void GbaDmaUnitSetActiveBitTo(GbaDmaUnit *dma_unit, uint_fast8_t index,
                                     bool active) {
  assert(index < GBA_NUM_DMA_UNITS);
  GbaDmaUnitClearActive(dma_unit, index);
  dma_unit->active |= (unsigned)active << index;
}

static bool GbaDmaUnitIsActiveByIndex(const GbaDmaUnit *dma_unit,
                                      uint_fast8_t index) {
  unsigned bit_to_check = 1u << index;
  return dma_unit->active & bit_to_check;
}

static void GbaDmaUnitReloadSource(GbaDmaUnit *dma_unit, uint_fast8_t index) {
  assert(index < GBA_NUM_DMA_UNITS);
  static const uint32_t address_mask[GBA_NUM_DMA_UNITS] = {
      0x07FFFFFFu, 0x0FFFFFFFu, 0x0FFFFFFFu, 0x0FFFFFFFu};
  dma_unit->current_source[index] =
      dma_unit->registers.units[index].source & address_mask[index];
}

static void GbaDmaUnitReloadDestination(GbaDmaUnit *dma_unit,
                                        uint_fast8_t index) {
  assert(index < GBA_NUM_DMA_UNITS);
  static const uint32_t address_mask[GBA_NUM_DMA_UNITS] = {
      0x07FFFFFFu, 0x0FFFFFFFu, 0x0FFFFFFFu, 0x0FFFFFFFu};
  dma_unit->current_destination[index] =
      dma_unit->registers.units[index].destination & address_mask[index];
}

static void GbaDmaUnitReloadTransferSize(GbaDmaUnit *dma_unit,
                                         uint_fast8_t index) {
  assert(index < GBA_NUM_DMA_UNITS);
  static const uint16_t transfer_mask[GBA_NUM_DMA_UNITS] = {0x3FFFu, 0x3FFFu,
                                                            0x3FFFu, 0xFFFFu};
  dma_unit->transfers_remaining[index] =
      dma_unit->registers.units[index].word_count & transfer_mask[index];
}

static void GbaDmaUnitUpdateAfterWrite(GbaDmaUnit *dma_unit) {
  for (uint_fast8_t i = 0; i < GBA_NUM_DMA_UNITS; i++) {
    bool active =
        dma_unit->registers.units[i].control.enabled &&
        dma_unit->registers.units[i].control.start_timing == GBA_DMA_IMMEDIATE;
    GbaDmaUnitSetActiveBitTo(dma_unit, i, active);
    if (dma_unit->registers.units[i].control.enabled && !dma_unit->enabled[i]) {
      GbaDmaUnitReloadSource(dma_unit, i);
      GbaDmaUnitReloadDestination(dma_unit, i);
      GbaDmaUnitReloadTransferSize(dma_unit, i);
    }
    dma_unit->enabled[i] = dma_unit->registers.units[i].control.enabled;
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

  uint16_t high_bits = 0u;
  bool success = GbaDmaUnitRegistersLoad16LE(context, address + 2u, &high_bits);

  if (success) {
    assert(address == DMA0CNT_L_OFFSET || address == DMA1CNT_L_OFFSET ||
           address == DMA2CNT_L_OFFSET || address == DMA3CNT_L_OFFSET);
    *value = ((uint32_t)high_bits) << 16u;
  }

  return success;
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
  GbaDmaUnitRelease(dma_unit);
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

bool GbaDmaUnitIsActive(const GbaDmaUnit *dma_unit) { return dma_unit->active; }

void GbaDmaUnitStep(GbaDmaUnit *dma_unit, Memory *memory) {
  assert(GbaDmaUnitIsActive(dma_unit));
  for (uint_fast8_t i = 0; i < GBA_NUM_DMA_UNITS; i++) {
    if (!GbaDmaUnitIsActiveByIndex(dma_unit, i)) {
      continue;
    }

    uint32_t transfer_size;
    if (dma_unit->registers.units[i].control.transfer_word) {
      const static uint32_t address_mask = 0xFFFFFFFCu;
      uint32_t value;
      Load32LE(memory, dma_unit->current_source[i] & address_mask, &value);
      Store32LE(memory, dma_unit->current_destination[i] & address_mask, value);
      transfer_size = 4u;
    } else {
      const static uint32_t address_mask = 0xFFFFFFFEu;
      uint16_t value;
      Load16LE(memory, dma_unit->current_source[i] & address_mask, &value);
      Store16LE(memory, dma_unit->current_destination[i] & address_mask, value);
      transfer_size = 2u;
    }

    switch (dma_unit->registers.units[i].control.src_addr) {
      case GBA_DMA_ADDR_INCREMENT:
        dma_unit->current_source[i] += transfer_size;
        break;
      case GBA_DMA_ADDR_DECREMENT:
        dma_unit->current_source[i] -= transfer_size;
        break;
    }

    switch (dma_unit->registers.units[i].control.dest_addr) {
      case GBA_DMA_ADDR_INCREMENT:
        dma_unit->current_destination[i] += transfer_size;
        break;
      case GBA_DMA_ADDR_DECREMENT:
        dma_unit->current_destination[i] -= transfer_size;
        break;
      case GBA_DMA_ADDR_INCREMENT_RELOAD:
        dma_unit->current_destination[i] += transfer_size;
        break;
    }

    dma_unit->transfers_remaining[i] -= 1u;
    if (dma_unit->transfers_remaining[i] == 0u) {
      GbaDmaUnitClearActive(dma_unit, i);
      if (!dma_unit->registers.units[i].control.repeat) {
        dma_unit->registers.units[i].control.enabled = false;
      } else {
        if (dma_unit->registers.units[i].control.dest_addr ==
            GBA_DMA_ADDR_INCREMENT_RELOAD) {
          GbaDmaUnitReloadDestination(dma_unit, i);
        }
        GbaDmaUnitReloadTransferSize(dma_unit, i);
      }
      if (dma_unit->registers.units[i].control.irq_enable) {
        GbaPlatformRaiseDmaInterrupt(dma_unit->platform, i);
      }
    }
    break;
  }
}

void GbaDmaUnitSignalHBlank(GbaDmaUnit *dma_unit, uint_fast8_t vcount) {
  for (uint_fast8_t i = 0; i < GBA_NUM_DMA_UNITS; i++) {
    if (dma_unit->registers.units[i].control.enabled &&
        dma_unit->registers.units[i].control.start_timing == GBA_DMA_HBLANK) {
      GbaDmaUnitSetActiveBitTo(dma_unit, i, true);
    }
  }

  if (dma_unit->registers.units[3].control.enabled &&
      dma_unit->registers.units[3].control.start_timing == GBA_DMA_SPECIAL &&
      vcount >= 2u && vcount < 162) {
    GbaDmaUnitSetActiveBitTo(dma_unit, 3, true);
  }
}

void GbaDmaUnitSignalVBlank(GbaDmaUnit *dma_unit) {
  for (uint_fast8_t i = 0; i < GBA_NUM_DMA_UNITS; i++) {
    if (dma_unit->registers.units[i].control.enabled &&
        dma_unit->registers.units[i].control.start_timing == GBA_DMA_VBLANK) {
      GbaDmaUnitSetActiveBitTo(dma_unit, i, true);
    }
  }
}

void GbaDmaUnitSignalFifoRefresh(GbaDmaUnit *dma_unit) {
  for (uint_fast8_t i = 1; i <= 2; i++) {
    if (dma_unit->registers.units[i].control.enabled &&
        dma_unit->registers.units[i].control.start_timing == GBA_DMA_SPECIAL) {
      GbaDmaUnitSetActiveBitTo(dma_unit, i, true);
    }
  }
}

void GbaDmaUnitRetain(GbaDmaUnit *dma_unit) { dma_unit->reference_count += 1u; }

void GbaDmaUnitRelease(GbaDmaUnit *dma_unit) {
  assert(dma_unit->reference_count != 0u);
  dma_unit->reference_count -= 1u;
  if (dma_unit->reference_count == 0u) {
    free(dma_unit);
  }
}

static_assert(sizeof(GbaDmaUnitRegisters) == GBA_DMA_UNIT_REGISTERS_SIZE,
              "sizeof(GbaDmaUnitRegisters) != GBA_DMA_UNIT_REGISTERS_SIZE");