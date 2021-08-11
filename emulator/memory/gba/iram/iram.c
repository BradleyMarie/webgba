#include "emulator/memory/gba/iram/iram.h"

#include <stdlib.h>

#define IRAM_SIZE (32u * 1024u)

// TODO: Mirror memory

static bool IRamLoad32LEFunction(const void *context, uint32_t address,
                                 uint32_t *value) {
  if (IRAM_SIZE < address + 4u) {
    return false;
  }

  const unsigned char *first_byte = (const unsigned char *)context + address;
  *value = *(const uint32_t *)(const void *)first_byte;
  return true;
}

static bool IRamLoad16LEFunction(const void *context, uint32_t address,
                                 uint16_t *value) {
  if (IRAM_SIZE < address + 2u) {
    return false;
  }

  const unsigned char *first_byte = (const unsigned char *)context + address;
  *value = *(const uint16_t *)(const void *)first_byte;
  return true;
}

static bool IRamLoad8Function(const void *context, uint32_t address,
                              uint8_t *value) {
  if (IRAM_SIZE <= address) {
    return false;
  }

  const unsigned char *first_byte = (const unsigned char *)context + address;
  *value = *(const uint8_t *)(const void *)first_byte;
  return true;
}

static bool IRamStore32LEFunction(void *context, uint32_t address,
                                  uint32_t value) {
  if (IRAM_SIZE < address + 4u) {
    return false;
  }

  unsigned char *first_byte = (unsigned char *)context + address;
  uint32_t *memory_cell = (uint32_t *)(void *)first_byte;
  *memory_cell = value;
  return true;
}

static bool IRamStore16LEFunction(void *context, uint32_t address,
                                  uint16_t value) {
  if (IRAM_SIZE < address + 2u) {
    return false;
  }

  unsigned char *first_byte = (unsigned char *)context + address;
  uint16_t *memory_cell = (uint16_t *)(void *)first_byte;
  *memory_cell = value;
  return true;
}

static bool IRamStore8Function(void *context, uint32_t address, uint8_t value) {
  if (IRAM_SIZE <= address) {
    return false;
  }

  unsigned char *first_byte = (unsigned char *)context + address;
  uint8_t *memory_cell = (uint8_t *)(void *)first_byte;
  *memory_cell = value;
  return true;
}

static void IRamFree(void *context) { free(context); }

Memory *IRamAllocate() {
  void *allocation = calloc(1, IRAM_SIZE);
  if (allocation == NULL) {
    return NULL;
  }

  Memory *result =
      MemoryAllocate(allocation, IRamLoad32LEFunction, IRamLoad16LEFunction,
                     IRamLoad8Function, IRamStore32LEFunction,
                     IRamStore16LEFunction, IRamStore8Function, IRamFree);
  if (result == NULL) {
    free(allocation);
  }

  return result;
}