#include "emulator/memory/memory_bank.h"

#include <assert.h>
#include <stdlib.h>

struct _MemoryBank {
  const void *current_read_bank;
  void *current_write_bank;
  void *current_write_bank_with_callback;
  uint32_t address_mask;
  MemoryBankWriteCallback callback;
  void **memory_banks;
  uint32_t num_banks;
  bool allow_writes;
};

MemoryBank *MemoryBankAllocate(uint32_t bank_size, uint32_t num_banks,
                               bool allow_writes,
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

  result->address_mask = bank_size - 1u;
  result->allow_writes = allow_writes;
  result->callback = write_callback;

  MemoryBankChangeBank(result, 0u);

  return result;
}

void MemoryBankLoad32LE(const MemoryBank *memory_bank, uint32_t address,
                        uint32_t *value) {
  address &= memory_bank->address_mask;

  uintptr_t ptr = (uintptr_t)memory_bank->current_read_bank;
  ptr += address;

  const uint32_t *value_ptr = (const uint32_t *)ptr;
  *value = *value_ptr;
}

void MemoryBankLoad16LE(const MemoryBank *memory_bank, uint32_t address,
                        uint16_t *value) {
  address &= memory_bank->address_mask;

  uintptr_t ptr = (uintptr_t)memory_bank->current_read_bank;
  ptr += address;

  const uint16_t *value_ptr = (const uint16_t *)ptr;
  *value = *value_ptr;
}

void MemoryBankLoad8(const MemoryBank *memory_bank, uint32_t address,
                     uint8_t *value) {
  address &= memory_bank->address_mask;

  uintptr_t ptr = (uintptr_t)memory_bank->current_read_bank;
  ptr += address;

  const uint8_t *value_ptr = (const uint8_t *)ptr;
  *value = *value_ptr;
}

void MemoryBankStore32LE(MemoryBank *memory_bank, uint32_t address,
                         uint32_t value) {
  address &= memory_bank->address_mask;

  if (memory_bank->current_write_bank) {
    uintptr_t ptr = (uintptr_t)memory_bank->current_write_bank;
    ptr += address;

    uint32_t *value_ptr = (uint32_t *)ptr;
    *value_ptr = value;
  } else if (memory_bank->current_write_bank_with_callback) {
    uintptr_t ptr = (uintptr_t)memory_bank->current_write_bank_with_callback;
    ptr += address;

    uint32_t *value_ptr = (uint32_t *)ptr;
    *value_ptr = value;

    memory_bank->callback(memory_bank, address, value);
  }
}

void MemoryBankStore16LE(MemoryBank *memory_bank, uint32_t address,
                         uint16_t value) {
  address &= memory_bank->address_mask;

  if (memory_bank->current_write_bank) {
    uintptr_t ptr = (uintptr_t)memory_bank->current_write_bank;
    ptr += address;

    uint16_t *value_ptr = (uint16_t *)ptr;
    *value_ptr = value;
  } else if (memory_bank->current_write_bank_with_callback) {
    uintptr_t ptr = (uintptr_t)memory_bank->current_write_bank_with_callback;
    ptr += address;

    uint16_t *value_ptr = (uint16_t *)ptr;
    *value_ptr = value;

    memory_bank->callback(memory_bank, address, value);
  }
}

void MemoryBankStore8(MemoryBank *memory_bank, uint32_t address,
                      uint8_t value) {
  address &= memory_bank->address_mask;

  if (memory_bank->current_write_bank) {
    uintptr_t ptr = (uintptr_t)memory_bank->current_write_bank;
    ptr += address;

    uint8_t *value_ptr = (uint8_t *)ptr;
    *value_ptr = value;
  } else if (memory_bank->current_write_bank_with_callback) {
    uintptr_t ptr = (uintptr_t)memory_bank->current_write_bank_with_callback;
    ptr += address;

    uint8_t *value_ptr = (uint8_t *)ptr;
    *value_ptr = value;

    memory_bank->callback(memory_bank, address, value);
  }
}

void MemoryBankChangeBank(MemoryBank *memory_bank, uint32_t bank) {
  assert(bank < memory_bank->num_banks);

  void *next_bank = memory_bank->memory_banks[bank];

  memory_bank->current_read_bank = next_bank;

  if (memory_bank->allow_writes) {
    if (memory_bank->callback) {
      memory_bank->current_write_bank = NULL;
      memory_bank->current_write_bank_with_callback = next_bank;
    } else {
      memory_bank->current_write_bank = next_bank;
      memory_bank->current_write_bank_with_callback = NULL;
    }
  } else {
    memory_bank->current_write_bank = NULL;
    memory_bank->current_write_bank_with_callback = NULL;
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
  free(memory_bank);
}