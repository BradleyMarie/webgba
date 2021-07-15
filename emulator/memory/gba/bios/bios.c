#include "emulator/memory/gba/bios/bios.h"

#include <stddef.h>

#include "emulator/memory/gba/bios/bios_data.h"

bool GBABiosLoad32LEFunction(const void *context, uint32_t address,
                             uint32_t *value) {
  if (bios_size < address + 4u) {
    return false;
  }

  const unsigned char *first_byte = bios_data + address;
  *value = *(const uint32_t *)(const void *)first_byte;
  return true;
}

bool GBABiosLoad16LEFunction(const void *context, uint32_t address,
                             uint16_t *value) {
  if (bios_size < address + 2u) {
    return false;
  }

  const unsigned char *first_byte = bios_data + address;
  *value = *(const uint16_t *)(const void *)first_byte;
  return true;
}

bool GBABiosLoad8Function(const void *context, uint32_t address,
                          uint8_t *value) {
  if (bios_size <= address) {
    return false;
  }

  const unsigned char *first_byte = bios_data + address;
  *value = *(const uint8_t *)(const void *)first_byte;
  return true;
}

bool GBABiosStore32LEFunction(void *context, uint32_t address, uint32_t value) {
  return false;
}

bool GBABiosStore16LEFunction(void *context, uint32_t address, uint16_t value) {
  return false;
}

bool GBABiosStore8Function(void *context, uint32_t address, uint8_t value) {
  return false;
}

Memory *GBABiosAllocate() {
  Memory *result =
      MemoryAllocate(NULL, GBABiosLoad32LEFunction, GBABiosLoad16LEFunction,
                     GBABiosLoad8Function, GBABiosStore32LEFunction,
                     GBABiosStore16LEFunction, GBABiosStore8Function, NULL);
  return result;
}