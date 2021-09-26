#include "emulator/memory/gba/open_bus/open_bus.h"

#include <stdlib.h>

typedef struct {
  Memory *memory;
} OpenBus;

static bool OpenBusLoad32LE(const void *context, uint32_t address,
                            uint32_t *value) {
  OpenBus *open_bus = (OpenBus *)context;
  bool result = Load32LE(open_bus->memory, address, value);
  if (!result) {
    // TODO: Implement Open Bus
    *value = 0u;
  }

  return true;
}

static bool OpenBusLoad16LE(const void *context, uint32_t address,
                            uint16_t *value) {
  OpenBus *open_bus = (OpenBus *)context;
  bool result = Load16LE(open_bus->memory, address, value);
  if (!result) {
    // TODO: Implement Open Bus
    *value = 0u;
  }

  return true;
}

static bool OpenBusLoad8(const void *context, uint32_t address,
                         uint8_t *value) {
  OpenBus *open_bus = (OpenBus *)context;
  bool result = Load8(open_bus->memory, address, value);
  if (!result) {
    // TODO: Implement Open Bus
    *value = 0u;
  }

  return true;
}

static bool OpenBusStore32LE(void *context, uint32_t address, uint32_t value) {
  OpenBus *open_bus = (OpenBus *)context;
  Store32LE(open_bus->memory, address, value);
  return true;
}

static bool OpenBusStore16LE(void *context, uint32_t address, uint16_t value) {
  OpenBus *open_bus = (OpenBus *)context;
  Store16LE(open_bus->memory, address, value);
  return true;
}

static bool OpenBusStore8(void *context, uint32_t address, uint8_t value) {
  OpenBus *open_bus = (OpenBus *)context;
  Store8(open_bus->memory, address, value);
  return true;
}

static void OpenBusFree(void *context) {
  OpenBus *open_bus = (OpenBus *)context;
  free(open_bus->memory);
}

Memory *OpenBusAllocate(Memory *memory) {
  OpenBus *allocation = (OpenBus *)calloc(1, sizeof(OpenBus));
  if (allocation == NULL) {
    return NULL;
  }

  allocation->memory = memory;

  Memory *result = MemoryAllocate(allocation, OpenBusLoad32LE, OpenBusLoad16LE,
                                  OpenBusLoad8, OpenBusStore32LE,
                                  OpenBusStore16LE, OpenBusStore8, OpenBusFree);
  if (result == NULL) {
    free(allocation);
  }

  return result;
}