#ifndef _WEBGBA_EMULATOR_MEMORY_MEMORY_
#define _WEBGBA_EMULATOR_MEMORY_MEMORY_

#include <stdbool.h>
#include <stdint.h>

typedef bool (*Load32LEFunction)(const void *context, uint32_t address,
                                 uint32_t *value);
typedef bool (*Load16LEFunction)(const void *context, uint32_t address,
                                 uint16_t *value);
typedef bool (*Load8Function)(const void *context, uint32_t address,
                              uint8_t *value);
typedef bool (*Store32LEFunction)(void *context, uint32_t address,
                                  uint32_t value);
typedef bool (*Store16LEFunction)(void *context, uint32_t address,
                                  uint16_t value);
typedef bool (*Store8Function)(void *context, uint32_t address, uint8_t value);
typedef void (*MemoryContextFree)(void *context);

typedef struct _Memory Memory;
Memory *MemoryAllocate(void *context, Load32LEFunction load_le_32,
                       Load16LEFunction load_le_16, Load8Function load_8,
                       Store32LEFunction store_le_32,
                       Store16LEFunction store_le_16, Store8Function store_8,
                       MemoryContextFree free_context);

void MemorySetIgnoreWrites(Memory *memory);

bool Load32LE(const Memory *memory, uint32_t address, uint32_t *value);
bool Load16LE(const Memory *memory, uint32_t address, uint16_t *value);
bool Load8(const Memory *memory, uint32_t address, uint8_t *value);

bool Load32SLE(const Memory *memory, uint32_t address, int32_t *value);
bool Load16SLE(const Memory *memory, uint32_t address, int16_t *value);
bool Load8S(const Memory *memory, uint32_t address, int8_t *value);

bool Store32LE(Memory *memory, uint32_t address, uint32_t value);
bool Store16LE(Memory *memory, uint32_t address, uint16_t value);
bool Store8(Memory *memory, uint32_t address, uint8_t value);

bool Store32SLE(Memory *memory, uint32_t address, int32_t value);
bool Store16SLE(Memory *memory, uint32_t address, int16_t value);
bool Store8S(Memory *memory, uint32_t address, int8_t value);

void MemoryFree(Memory *memory);

#endif  // _WEBGBA_EMULATOR_MEMORY_MEMORY_