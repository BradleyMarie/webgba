#include "emulator/memory/memory_bank.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

struct _MemoryBank {
  const void *read_bank;
  void *write_bank;
  uint32_t address_mask;
  MemoryBankWriteCallback callback;
  void **memory_banks;
  void *write_sink;
  uint32_t num_banks;
  bool allow_writes;
};

MemoryBank *MemoryBankAllocate(uint32_t bank_size, uint32_t num_banks,
                               MemoryBankWriteCallback write_callback) {
  assert(bank_size != 0u && (bank_size & (bank_size - 1u)) == 0u);
  assert(num_banks != 0u);

  MemoryBank *result = calloc(1u, sizeof(MemoryBank));
  if (result == NULL) {
    return NULL;
  }

  result->memory_banks = calloc(num_banks, sizeof(void *));
  if (result->memory_banks == NULL) {
    MemoryBankFree(result);
    return NULL;
  }

  result->num_banks = num_banks;

  for (uint32_t bank = 0u; bank < num_banks; bank++) {
    result->memory_banks[bank] = calloc(1u, bank_size);
    if (result->memory_banks[bank] == NULL) {
      MemoryBankFree(result);
      return NULL;
    }
  }

  result->write_sink = calloc(1u, bank_size);
  if (result->write_sink == NULL) {
    MemoryBankFree(result);
    return NULL;
  }

  result->address_mask = bank_size - 1u;
  result->callback = write_callback;
  result->allow_writes = true;

  MemoryBankChangeBank(result, 0u);

  return result;
}

void MemoryBankLoad32LE(const MemoryBank *memory_bank, uint32_t address,
                        uint32_t *value) {
  address &= memory_bank->address_mask;

  uintptr_t ptr = (uintptr_t)memory_bank->read_bank;
  ptr += address;

  const uint32_t *value_ptr = (const uint32_t *)ptr;
  *value = *value_ptr;
}

void MemoryBankLoad16LE(const MemoryBank *memory_bank, uint32_t address,
                        uint16_t *value) {
  address &= memory_bank->address_mask;

  uintptr_t ptr = (uintptr_t)memory_bank->read_bank;
  ptr += address;

  const uint16_t *value_ptr = (const uint16_t *)ptr;
  *value = *value_ptr;
}

void MemoryBankLoad8(const MemoryBank *memory_bank, uint32_t address,
                     uint8_t *value) {
  address &= memory_bank->address_mask;

  uintptr_t ptr = (uintptr_t)memory_bank->read_bank;
  ptr += address;

  const uint8_t *value_ptr = (const uint8_t *)ptr;
  *value = *value_ptr;
}

void MemoryBankStore32LE(MemoryBank *memory_bank, uint32_t address,
                         uint32_t value) {
  address &= memory_bank->address_mask;

  uintptr_t ptr = (uintptr_t)memory_bank->write_bank;
  ptr += address;

  uint32_t *value_ptr = (uint32_t *)ptr;
  *value_ptr = value;

  if (memory_bank->callback) {
    memory_bank->callback(memory_bank, address, value);
  }
}

void MemoryBankStore16LE(MemoryBank *memory_bank, uint32_t address,
                         uint16_t value) {
  address &= memory_bank->address_mask;

  uintptr_t ptr = (uintptr_t)memory_bank->write_bank;
  ptr += address;

  uint16_t *value_ptr = (uint16_t *)ptr;
  *value_ptr = value;

  if (memory_bank->callback) {
    memory_bank->callback(memory_bank, address, value);
  }
}

void MemoryBankStore8(MemoryBank *memory_bank, uint32_t address,
                      uint8_t value) {
  address &= memory_bank->address_mask;

  uintptr_t ptr = (uintptr_t)memory_bank->write_bank;
  ptr += address;

  uint8_t *value_ptr = (uint8_t *)ptr;
  *value_ptr = value;

  if (memory_bank->callback) {
    memory_bank->callback(memory_bank, address, value);
  }
}

void MemoryBankIgnoreWrites(MemoryBank *memory_bank) {
  memory_bank->write_bank = memory_bank->write_sink;
  memory_bank->allow_writes = false;
}

void MemoryBankChangeBank(MemoryBank *memory_bank, uint32_t bank) {
  assert(bank < memory_bank->num_banks);

  memory_bank->read_bank = memory_bank->memory_banks[bank];

  if (memory_bank->allow_writes) {
    memory_bank->write_bank = memory_bank->memory_banks[bank];
  } else {
    memory_bank->write_bank = memory_bank->write_sink;
  }
}

void MemoryBankFree(MemoryBank *memory_bank) {
  if (memory_bank == NULL) {
    return;
  }

  for (uint32_t bank = 0u; bank > memory_bank->num_banks; bank++) {
    free(memory_bank->memory_banks[bank]);
  }

  free(memory_bank->memory_banks);
  free(memory_bank->write_sink);
  free(memory_bank);
}