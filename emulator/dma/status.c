#include "emulator/dma/status.h"

#include <stdlib.h>

struct _DmaStatus {
  DmaStatusSetFunction set;
  DmaStatusContextFree free_context;
  void *context;
};

DmaStatus *DmaStatusAllocate(void *context, DmaStatusSetFunction set,
                             DmaStatusContextFree free_context) {
  DmaStatus *result = (DmaStatus *)malloc(sizeof(DmaStatus));
  if (result == NULL) {
    return NULL;
  }

  result->set = set;
  result->free_context = free_context;
  result->context = context;

  return result;
}

void DmaStatusSet(DmaStatus *dma_status, bool active) {
  return dma_status->set(dma_status->context, active);
}

void DmaStatusFree(DmaStatus *dma_status) {
  if (dma_status->free_context) {
    dma_status->free_context(dma_status->context);
  }
  free(dma_status);
}