#ifndef _WEBGBA_EMULATOR_DMA_GBA_DMA_
#define _WEBGBA_EMULATOR_DMA_GBA_DMA_

#include "emulator/memory/memory.h"
#include "emulator/platform/gba/platform.h"

typedef struct _GbaDmaUnit GbaDmaUnit;

bool GbaDmaUnitAllocate(GbaPlatform *platform, GbaDmaUnit **dma_unit,
                        Memory **registers);

// Step
bool GbaDmaIsActive(const GbaDmaUnit *dma_unit);
void GbaDmaUnitStep(GbaDmaUnit *dma_unit, Memory *memory);

// Triggers
void GbaDmaUnitSignalHBlank(GbaDmaUnit *dma_unit, uint_fast8_t vcount);
void GbaDmaUnitSignalVBlank(GbaDmaUnit *dma_unit);
void GbaDmaUnitSignalFifoRefresh(GbaDmaUnit *dma_unit);

void GbaDmaUnitFree(GbaDmaUnit *dma_unit);

#endif  // _WEBGBA_EMULATOR_DMA_GBA_DMA_