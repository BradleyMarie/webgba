#ifndef _WEBGBA_EMULATOR_GAME_GBA_GAME_
#define _WEBGBA_EMULATOR_GAME_GBA_GAME_

#include <stdbool.h>

#include "emulator/memory/memory_bank.h"

// Numbers represent the size in kilobits
typedef enum {
  SAVE_STORAGE_NONE,
  SAVE_STORAGE_EEPROM_4,
  SAVE_STORAGE_EEPROM_64,
  SAVE_STORAGE_SRAM_256,
  SAVE_STORAGE_SRAM_512,
  SAVE_STORAGE_FLASH_512,
  SAVE_STORAGE_FLASH_1024
} SaveStorageType;

bool GbaGameLoad(const unsigned char *rom_data, uint32_t rom_size,
                 SaveStorageType *save_storage_type, MemoryBank **game_rom);

#endif  // _WEBGBA_EMULATOR_GAME_GBA_GAME_