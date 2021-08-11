#ifndef _WEBGBA_EMULATOR_GAME_GBA_GAME_
#define _WEBGBA_EMULATOR_GAME_GBA_GAME_

#include "emulator/memory/memory.h"

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

bool GbaGameLoad(const char *rom_data, uint32_t rom_size,
                 SaveStorageType **save_storage_type, Memory **game_rom);

#endif  // _WEBGBA_EMULATOR_GAME_GBA_GAME_