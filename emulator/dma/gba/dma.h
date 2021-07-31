#ifndef _WEBGBA_EMULATOR_DMA_GBA_DMA_
#define _WEBGBA_EMULATOR_DMA_GBA_DMA_

#include "emulator/memory/memory.h"

typedef struct _GbaDmaUnit GbaDmaUnit;

bool GbaDmaUnitAllocate(GbaDmaUnit **dma_unit, Memory **registers);

void GbaDmaUnitStep(GbaDmaUnit *dma_unit);

void GbaDmaUnitFree(GbaDmaUnit *dma_unit);

#endif  // _WEBGBA_EMULATOR_DMA_GBA_DMA_