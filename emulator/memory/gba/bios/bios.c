#include "emulator/memory/gba/bios/bios.h"

#include <stddef.h>

#include "tools/bios_data/data.h"

static bool GBABiosLoad32LEFunction(const void *context, uint32_t address,
                                    uint32_t *value) {
  if (bios_size < address + 4u) {
    return false;
  }

  // TODO: Read Protect Bios Memory

  const unsigned char *first_byte = bios_data + address;
  *value = *(const uint32_t *)(const void *)first_byte;
  return true;
}

static bool GBABiosLoad16LEFunction(const void *context, uint32_t address,
                                    uint16_t *value) {
  if (bios_size < address + 2u) {
    return false;
  }

  // TODO: Read Protect Bios Memory

  const unsigned char *first_byte = bios_data + address;
  *value = *(const uint16_t *)(const void *)first_byte;
  return true;
}

static bool GBABiosLoad8Function(const void *context, uint32_t address,
                                 uint8_t *value) {
  if (bios_size <= address) {
    return false;
  }

  // TODO: Read Protect Bios Memory

  const unsigned char *first_byte = bios_data + address;
  *value = *(const uint8_t *)(const void *)first_byte;
  return true;
}

static bool GBABiosStore32LEFunction(void *context, uint32_t address,
                                     uint32_t value) {
  return false;
}

static bool GBABiosStore16LEFunction(void *context, uint32_t address,
                                     uint16_t value) {
  return false;
}

static bool GBABiosStore8Function(void *context, uint32_t address,
                                  uint8_t value) {
  return false;
}

Memory *GBABiosAllocate() {
  Memory *result =
      MemoryAllocate(NULL, GBABiosLoad32LEFunction, GBABiosLoad16LEFunction,
                     GBABiosLoad8Function, GBABiosStore32LEFunction,
                     GBABiosStore16LEFunction, GBABiosStore8Function, NULL);
  return result;
}