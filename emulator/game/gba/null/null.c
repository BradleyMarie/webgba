#include "emulator/game/gba/null/null.h"

#include <string.h>

static bool GbaNullSramLoad32LE(const void *context, uint32_t address,
                                uint32_t *value) {
  *value = 0u;
  return true;
}

static bool GbaNullSramLoad16LE(const void *context, uint32_t address,
                                uint16_t *value) {
  *value = 0u;
  return true;
}

static bool GbaNullSramLoad8(const void *context, uint32_t address,
                             uint8_t *value) {
  *value = 0u;
  return true;
}

static bool GbaNullSramStore32LE(void *context, uint32_t address,
                                 uint32_t value) {
  return true;
}

static bool GbaNullSramStore16LE(void *context, uint32_t address,
                                 uint16_t value) {
  return true;
}

static bool GbaNullSramStore8(void *context, uint32_t address, uint8_t value) {
  return true;
}

Memory *GbaNullSramAllocate() {
  return MemoryAllocate(NULL, GbaNullSramLoad32LE, GbaNullSramLoad16LE,
                        GbaNullSramLoad8, GbaNullSramStore32LE,
                        GbaNullSramStore16LE, GbaNullSramStore8, NULL);
}