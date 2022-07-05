#include "emulator/game/gba/game.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define GBA_GAME_MAX_SIZE 0x2000000u  // 32MB

bool GbaGameLoad(const unsigned char *rom_data, uint32_t rom_size,
                 SaveStorageType *save_storage_type, MemoryBank **game_rom) {
  if (rom_size > GBA_GAME_MAX_SIZE) {
    return false;
  }

  *game_rom = MemoryBankAllocate(GBA_GAME_MAX_SIZE, 1u, NULL);
  if (*game_rom == NULL) {
    return false;
  }

  for (uint32_t i = 0u; i < rom_size; i++) {
    MemoryBankStore8(*game_rom, i, (char)rom_data[i]);
  }

  MemoryBankIgnoreWrites(*game_rom);

  // Save storage is not implemented yet nor is storage detection, so just
  // treat all games as if they have no save storage
  *save_storage_type = SAVE_STORAGE_NONE;

  return true;
}