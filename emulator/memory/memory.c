#include "emulator/memory/memory.h"

#include <stdlib.h>

struct _Memory {
  Load32LEFunction load_le_32;
  Load16LEFunction load_le_16;
  Load8Function load_8;
  Store32LEFunction store_le_32;
  Store16LEFunction store_le_16;
  Store8Function store_8;
  MemoryContextFree free_context;
  void *context;
};

static bool IgnoreStore32LE(void *context, uint32_t address, uint32_t value) {
  return true;
}

static bool IgnoreStore16LE(void *context, uint32_t address, uint16_t value) {
  return true;
}

static bool IgnoreStore8(void *context, uint32_t address, uint8_t value) {
  return true;
}

Memory *MemoryAllocate(void *context, Load32LEFunction load_le_32,
                       Load16LEFunction load_le_16, Load8Function load_8,
                       Store32LEFunction store_le_32,
                       Store16LEFunction store_le_16, Store8Function store_8,
                       MemoryContextFree free_context) {
  Memory *result = (Memory *)malloc(sizeof(Memory));
  if (result == NULL) {
    return NULL;
  }

  result->load_le_32 = load_le_32;
  result->load_le_16 = load_le_16;
  result->load_8 = load_8;
  result->store_le_32 = store_le_32;
  result->store_le_16 = store_le_16;
  result->store_8 = store_8;
  result->free_context = free_context;
  result->context = context;

  return result;
}

void MemorySetIgnoreWrites(Memory *memory) {
  memory->store_le_32 = IgnoreStore32LE;
  memory->store_le_16 = IgnoreStore16LE;
  memory->store_8 = IgnoreStore8;
}

bool Load32LE(const Memory *memory, uint32_t address, uint32_t *value) {
  return memory->load_le_32(memory->context, address, value);
}

bool Load16LE(const Memory *memory, uint32_t address, uint16_t *value) {
  return memory->load_le_16(memory->context, address, value);
}

bool Load8(const Memory *memory, uint32_t address, uint8_t *value) {
  return memory->load_8(memory->context, address, value);
}

bool Load32SLE(const Memory *memory, uint32_t address, int32_t *value) {
  return memory->load_le_32(memory->context, address,
                            (uint32_t *)(void *)value);
}

bool Load16SLE(const Memory *memory, uint32_t address, int16_t *value) {
  return memory->load_le_16(memory->context, address,
                            (uint16_t *)(void *)value);
}

bool Load8S(const Memory *memory, uint32_t address, int8_t *value) {
  return memory->load_8(memory->context, address, (uint8_t *)(void *)value);
}

bool Store32LE(Memory *memory, uint32_t address, uint32_t value) {
  return memory->store_le_32(memory->context, address, value);
}

bool Store16LE(Memory *memory, uint32_t address, uint16_t value) {
  return memory->store_le_16(memory->context, address, value);
}

bool Store8(Memory *memory, uint32_t address, uint8_t value) {
  return memory->store_8(memory->context, address, value);
}

bool Store32SLE(Memory *memory, uint32_t address, int32_t value) {
  return memory->store_le_32(memory->context, address, (uint32_t)value);
}

bool Store16SLE(Memory *memory, uint32_t address, int16_t value) {
  return memory->store_le_16(memory->context, address, (uint16_t)value);
}

bool Store8S(Memory *memory, uint32_t address, int8_t value) {
  return memory->store_8(memory->context, address, (uint8_t)value);
}

void MemoryFree(Memory *memory) {
  if (memory->free_context != NULL) {
    memory->free_context(memory->context);
  }
  free(memory);
}