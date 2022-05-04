#ifndef _WEBGBA_EMULATOR_DMA_DMA_STATUS_
#define _WEBGBA_EMULATOR_DMA_DMA_STATUS_

#include <stdbool.h>

typedef void (*DmaStatusSetFunction)(void *context, bool active);
typedef void (*DmaStatusContextFree)(void *context);

typedef struct _DmaStatus DmaStatus;
DmaStatus *DmaStatusAllocate(void *context, DmaStatusSetFunction set,
                             DmaStatusContextFree free_context);

void DmaStatusSet(DmaStatus *dma_status, bool active);

void DmaStatusFree(DmaStatus *dma_status);

#endif  // _WEBGBA_EMULATOR_DMA_DMA_STATUS_