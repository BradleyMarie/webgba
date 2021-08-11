#include "emulator/memory/gba/bad/bad.h"

#include <stddef.h>

static bool BadMemoryLoad32LE(const void *context, uint32_t address,
                              uint32_t *value) {
  return false;
}

static bool BadMemoryLoad16LE(const void *context, uint32_t address,
                              uint16_t *value) {
  return false;
}

static bool BadMemoryLoad8(const void *context, uint32_t address,
                           uint8_t *value) {
  return false;
}

static bool BadMemoryStore32LE(void *context, uint32_t address,
                               uint32_t value) {
  return false;
}

static bool BadMemoryStore16LE(void *context, uint32_t address,
                               uint16_t value) {
  return false;
}

static bool BadMemoryStore8(void *context, uint32_t address, uint8_t value) {
  return false;
}

Memory *BadMemoryAllocate() {
  Memory *result = MemoryAllocate(NULL, BadMemoryLoad32LE, BadMemoryLoad16LE,
                                  BadMemoryLoad8, BadMemoryStore32LE,
                                  BadMemoryStore16LE, BadMemoryStore8, NULL);
  return result;
}