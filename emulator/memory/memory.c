#include "emulator/memory/memory.h"

#include <assert.h>
#include <stdlib.h>

struct _Memory {
  MemoryBank **memory_banks;
  uint32_t bank_shift;
  uint32_t num_banks;
  Load32LEFunction load_le_32;
  Load16LEFunction load_le_16;
  Load8Function load_8;
  Store32LEFunction store_le_32;
  Store16LEFunction store_le_16;
  Store8Function store_8;
  MemoryContextFree free_context;
  void *context;
};

Memory *MemoryAllocateWithBanks(void *context, MemoryBank **banks,
                                uint32_t num_banks, Load32LEFunction load_le_32,
                                Load16LEFunction load_le_16,
                                Load8Function load_8,
                                Store32LEFunction store_le_32,
                                Store16LEFunction store_le_16,
                                Store8Function store_8,
                                MemoryContextFree free_context) {
  assert((num_banks & (num_banks - 1u)) == 0u);

  Memory *result = (Memory *)malloc(sizeof(Memory));
  if (result == NULL) {
    return NULL;
  }

  size_t allocated_banks = num_banks;
  if (allocated_banks < 2u) {
    allocated_banks = 2u;
  }

  result->memory_banks = calloc(allocated_banks, sizeof(MemoryBank *));
  if (result->memory_banks == NULL) {
    free(result);
    return NULL;
  }

  if (num_banks == 1u) {
    for (size_t i = 0u; i < allocated_banks; i++) {
      result->memory_banks[i] = banks[0u];
    }
  } else {
    for (size_t i = 0u; i < num_banks; i++) {
      result->memory_banks[i] = banks[i];
    }
  }

  result->bank_shift = 32u - __builtin_ctz(allocated_banks);
  result->num_banks = num_banks;
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

Memory *MemoryAllocate(void *context, Load32LEFunction load_le_32,
                       Load16LEFunction load_le_16, Load8Function load_8,
                       Store32LEFunction store_le_32,
                       Store16LEFunction store_le_16, Store8Function store_8,
                       MemoryContextFree free_context) {
  return MemoryAllocateWithBanks(context, NULL, 0u, load_le_32, load_le_16,
                                 load_8, store_le_32, store_le_16, store_8,
                                 free_context);
}

inline bool Load32LE(const Memory *memory, uint32_t address, uint32_t *value) {
  const MemoryBank *memory_bank =
      memory->memory_banks[address >> memory->bank_shift];
  if (memory_bank != NULL) {
    MemoryBankLoad32LE(memory_bank, address, value);
    return true;
  }

  return memory->load_le_32(memory->context, address, value);
}

inline bool Load16LE(const Memory *memory, uint32_t address, uint16_t *value) {
  const MemoryBank *memory_bank =
      memory->memory_banks[address >> memory->bank_shift];
  if (memory_bank != NULL) {
    MemoryBankLoad16LE(memory_bank, address, value);
    return true;
  }

  return memory->load_le_16(memory->context, address, value);
}

inline bool Load8(const Memory *memory, uint32_t address, uint8_t *value) {
  const MemoryBank *memory_bank =
      memory->memory_banks[address >> memory->bank_shift];
  if (memory_bank != NULL) {
    MemoryBankLoad8(memory_bank, address, value);
    return true;
  }

  return memory->load_8(memory->context, address, value);
}

bool Load32SLE(const Memory *memory, uint32_t address, int32_t *value) {
  return Load32LE(memory, address, (uint32_t *)(void *)value);
}

bool Load16SLE(const Memory *memory, uint32_t address, int16_t *value) {
  return Load16LE(memory, address, (uint16_t *)(void *)value);
}

bool Load8S(const Memory *memory, uint32_t address, int8_t *value) {
  return Load8(memory, address, (uint8_t *)(void *)value);
}

inline bool Store32LE(Memory *memory, uint32_t address, uint32_t value) {
  MemoryBank *memory_bank = memory->memory_banks[address >> memory->bank_shift];
  if (memory_bank != NULL) {
    MemoryBankStore32LE(memory_bank, address, value);
    return true;
  }

  return memory->store_le_32(memory->context, address, value);
}

inline bool Store16LE(Memory *memory, uint32_t address, uint16_t value) {
  MemoryBank *memory_bank = memory->memory_banks[address >> memory->bank_shift];
  if (memory_bank != NULL) {
    MemoryBankStore16LE(memory_bank, address, value);
    return true;
  }

  return memory->store_le_16(memory->context, address, value);
}

inline bool Store8(Memory *memory, uint32_t address, uint8_t value) {
  MemoryBank *memory_bank = memory->memory_banks[address >> memory->bank_shift];
  if (memory_bank != NULL) {
    MemoryBankStore8(memory_bank, address, value);
    return true;
  }

  return memory->store_8(memory->context, address, value);
}

bool Store32SLE(Memory *memory, uint32_t address, int32_t value) {
  return Store32LE(memory, address, (uint32_t)value);
}

bool Store16SLE(Memory *memory, uint32_t address, int16_t value) {
  return Store16LE(memory, address, (uint16_t)value);
}

bool Store8S(Memory *memory, uint32_t address, int8_t value) {
  return Store8(memory, address, (uint8_t)value);
}

void MemoryFree(Memory *memory) {
  if (memory == NULL) {
    return;
  }

  if (memory->free_context != NULL) {
    memory->free_context(memory->context);
  }

  for (size_t i = 0u; i < memory->num_banks; i++) {
    MemoryBankFree(memory->memory_banks[i]);
  }

  free(memory->memory_banks);
  free(memory);
}