#ifndef _WEBGBA_EMULATOR_DMA_GBA_DMA_
#define _WEBGBA_EMULATOR_DMA_GBA_DMA_

#include "emulator/dma/status.h"
#include "emulator/memory/memory.h"
#include "emulator/platform/gba/platform.h"

typedef struct _GbaDmaUnit GbaDmaUnit;

bool GbaDmaUnitAllocate(DmaStatus *dma_status, GbaPlatform *platform,
                        GbaDmaUnit **dma_unit, Memory **registers);

// Step
void GbaDmaUnitStep(GbaDmaUnit *dma_unit, Memory *memory);

// Triggers
void GbaDmaUnitSignalHBlank(GbaDmaUnit *dma_unit, uint_fast8_t vcount);
void GbaDmaUnitSignalVBlank(GbaDmaUnit *dma_unit);
void GbaDmaUnitSignalFifoRefresh(GbaDmaUnit *dma_unit, uint32_t destination);

// Reference Counting
void GbaDmaUnitRetain(GbaDmaUnit *dma_unit);
void GbaDmaUnitRelease(GbaDmaUnit *dma_unit);

#endif  // _WEBGBA_EMULATOR_DMA_GBA_DMA_