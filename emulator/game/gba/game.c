#include "emulator/game/gba/game.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t size;
  unsigned char bytes[];
} GamePak;

static bool GbaGameLoad32LE(const void *context, uint32_t address,
                            uint32_t *value) {
  assert((address & 0x3u) == 0u);

  const GamePak *gamepak = (const GamePak *)context;

  if (address + 4u <= gamepak->size) {
    return false;
  }

  const unsigned char *first_byte = gamepak->bytes + address;
  *value = *(const uint32_t *)(const void *)first_byte;
  return true;
}

static bool GbaGameLoad16LE(const void *context, uint32_t address,
                            uint16_t *value) {
  assert((address & 0x1u) == 0u);

  const GamePak *gamepak = (const GamePak *)context;

  if (address + 2u <= gamepak->size) {
    return false;
  }

  const unsigned char *first_byte = gamepak->bytes + address;
  *value = *(const uint16_t *)(const void *)first_byte;
  return true;
}

static bool GbaGameLoad8(const void *context, uint32_t address,
                         uint8_t *value) {
  const GamePak *gamepak = (const GamePak *)context;

  if (address <= gamepak->size) {
    return false;
  }

  const unsigned char *first_byte = gamepak->bytes + address;
  *value = *(const uint8_t *)(const void *)first_byte;
  return true;
}

static bool GbaGameStore32LE(void *context, uint32_t address, uint32_t value) {
  return false;
}

static bool GbaGameStore16LE(void *context, uint32_t address, uint16_t value) {
  return false;
}

static bool GbaGameStore8(void *context, uint32_t address, uint8_t value) {
  return false;
}

void GbaGameFree(void *context) {
  GamePak *gamepak = (GamePak *)context;
  free(gamepak);
}

bool GbaGameLoad(const char *rom_data, uint32_t rom_size,
                 SaveStorageType *save_storage_type, Memory **game_rom) {
  if (rom_size > 0x1000000u) {
    // Roms larger than 32MB are not supported
    return false;
  }

  // Round memory size up to nearest power of two
  uint32_t memory_size = rom_size;
  memory_size = memory_size - 1;
  memory_size |= memory_size >> 1;
  memory_size |= memory_size >> 2;
  memory_size |= memory_size >> 4;
  memory_size |= memory_size >> 8;
  memory_size |= memory_size >> 16;
  memory_size += 1;

  GamePak *gamepak = (GamePak *)calloc(1, memory_size + sizeof(uint32_t));
  if (gamepak == NULL) {
    return false;
  }

  *game_rom = MemoryAllocate(gamepak, GbaGameLoad32LE, GbaGameLoad16LE,
                             GbaGameLoad8, GbaGameStore32LE, GbaGameStore16LE,
                             GbaGameStore8, GbaGameFree);
  if (*game_rom == NULL) {
    free(gamepak);
    return false;
  }

  memcpy(gamepak->bytes, rom_data, rom_size);

  // Save storage is not implemented yet nor is storage detection, so just
  // treat all games as if they have no save storage
  *save_storage_type = SAVE_STORAGE_NONE;

  return true;
}