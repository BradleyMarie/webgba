#include "emulator/memory/gba/wram/wram.h"

#include <stdlib.h>

#define WRAM_SIZE (256u * 1024u)

bool WRamLoad32LEFunction(const void *context, uint32_t address,
                          uint32_t *value) {
  return false;
}

bool WRamLoad16LEFunction(const void *context, uint32_t address,
                          uint16_t *value) {
  if (WRAM_SIZE < address + 2u) {
    return false;
  }

  const unsigned char *first_byte = (const unsigned char *)context + address;
  *value = *(const uint16_t *)(const void *)first_byte;
  return true;
}

bool WRamLoad8Function(const void *context, uint32_t address, uint8_t *value) {
  if (WRAM_SIZE <= address) {
    return false;
  }

  const unsigned char *first_byte = (const unsigned char *)context + address;
  *value = *(const uint8_t *)(const void *)first_byte;
  return true;
}

bool WRamStore32LEFunction(void *context, uint32_t address, uint32_t value) {
  return false;
}

bool WRamStore16LEFunction(void *context, uint32_t address, uint16_t value) {
  if (WRAM_SIZE < address + 2u) {
    return false;
  }

  unsigned char *first_byte = (unsigned char *)context + address;
  uint16_t *memory_cell = (uint16_t *)(void *)first_byte;
  *memory_cell = value;
  return true;
}

bool WRamStore8Function(void *context, uint32_t address, uint8_t value) {
  if (WRAM_SIZE <= address) {
    return false;
  }

  unsigned char *first_byte = (unsigned char *)context + address;
  uint8_t *memory_cell = (uint8_t *)(void *)first_byte;
  *memory_cell = value;
  return true;
}

void WRamFree(void *context) { free(context); }

Memory *AllocateWRam() {
  void *allocation = calloc(1, WRAM_SIZE);
  if (allocation == NULL) {
    return NULL;
  }

  Memory *result =
      MemoryAllocate(allocation, WRamLoad32LEFunction, WRamLoad16LEFunction,
                     WRamLoad8Function, WRamStore32LEFunction,
                     WRamStore16LEFunction, WRamStore8Function, WRamFree);
  if (result == NULL) {
    free(allocation);
  }

  return result;
}