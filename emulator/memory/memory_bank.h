#ifndef _WEBGBA_EMULATOR_MEMORY_MEMORY_BANK_
#define _WEBGBA_EMULATOR_MEMORY_MEMORY_BANK_

#include <stdint.h>

// A memory bank is a mirrored, contiguous region of memory which can be written
// and read directly without any intervening logic. Writes to the region may be
// monitored, in order to enable bank switching. The bank must be a power of two
// in size and is assumed to be mapped to addresses that start are aligned with
// that size so that upper bits may be masked off.
typedef struct _MemoryBank MemoryBank;

typedef void (*MemoryBankWriteCallback)(MemoryBank *memory_bank,
                                        uint32_t address, uint32_t value);

MemoryBank *MemoryBankAllocate(uint32_t bank_size, uint32_t num_banks,
                               MemoryBankWriteCallback write_callback);

void MemoryBankLoad32LE(const MemoryBank *memory_bank, uint32_t address,
                        uint32_t *value);
void MemoryBankLoad16LE(const MemoryBank *memory_bank, uint32_t address,
                        uint16_t *value);
void MemoryBankLoad8(const MemoryBank *memory_bank, uint32_t address,
                     uint8_t *value);

void MemoryBankStore32LE(MemoryBank *memory_bank, uint32_t address,
                         uint32_t value);
void MemoryBankStore16LE(MemoryBank *memory_bank, uint32_t address,
                         uint16_t value);
void MemoryBankStore8(MemoryBank *memory_bank, uint32_t address, uint8_t value);

void MemoryBankIgnoreWrites(MemoryBank *memory_bank);
void MemoryBankChangeBank(MemoryBank *memory_bank, uint32_t bank);

void MemoryBankFree(MemoryBank *MemoryBank);

#endif  // _WEBGBA_EMULATOR_MEMORY_MEMORY_BANK_