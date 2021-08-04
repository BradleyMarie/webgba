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

#define GBA_DMA_REGISTERS_SIZE 0x30u

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
  DmaUnitRegisters registers[4];
  uint32_t words[12];
  uint16_t half_words[24];
  uint8_t bytes[48];
} GbaDmaUnitRegisters;

struct _GbaDmaUnit {
  GbaDmaUnitRegisters registers;
  bool active[4];
  GbaPlatform *platform;
  uint16_t reference_count;
};

bool GbaDmaUnitAllocate(GbaPlatform *platform, GbaDmaUnit **dma_unit,
                        Memory **registers) {
  return false;
}

bool GbaDmaIsActive(const GbaDmaUnit *dma_unit) {
  bool active = dma_unit->active[0] || dma_unit->active[1] ||
                dma_unit->active[2] || dma_unit->active[3];
  return active;
}

void GbaDmaUnitStep(GbaDmaUnit *dma_unit, Memory *memory) {}

void GbaDmaUnitSignalHBlank(GbaDmaUnit *dma_unit, uint_fast8_t vcount) {}

void GbaDmaUnitSignalVBlank(GbaDmaUnit *dma_unit) {}

void GbaDmaUnitSignalFifoRefresh() {}

void GbaDmaUnitFree(GbaDmaUnit *dma_unit) {}

static_assert(sizeof(GbaDmaUnitRegisters) == GBA_DMA_REGISTERS_SIZE,
              "sizeof(GbaDmaUnitRegisters) != GBA_DMA_REGISTERS_SIZE");