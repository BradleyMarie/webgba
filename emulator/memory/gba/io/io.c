#include "emulator/memory/gba/io/io.h"

#include <stdlib.h>

typedef struct {
  Memory* banks[32u];
  Memory* ppu;
  Memory* sound;
  Memory* dma;
  Memory* timer;
  Memory* peripherals;
  Memory* platform;
} IoMemory;

static Memory* IoMemorySelectBank(const IoMemory* memory, uint32_t* address) {
  uint32_t bank = *address >> 4u;

  if (bank >= 32u) {
    *address -= 0x200u;
    return memory->platform;
  }

  static const uint32_t bank_base[32] = {
      0x000u,  // 0x000
      0x000u,  // 0x010
      0x000u,  // 0x020
      0x000u,  // 0x030
      0x000u,  // 0x040
      0x000u,  // 0x050
      0x060u,  // 0x060
      0x060u,  // 0x070
      0x060u,  // 0x080
      0x060u,  // 0x090
      0x060u,  // 0x0A0
      0x0B0u,  // 0x0B0
      0x0B0u,  // 0x0C0
      0x0B0u,  // 0x0D0
      0x0B0u,  // 0x0E0
      0x0B0u,  // 0x0F0
      0x100u,  // 0x100
      0x100u,  // 0x110
      0x120u,  // 0x120
      0x120u,  // 0x130
      0x120u,  // 0x140
      0x120u,  // 0x150
      0x120u,  // 0x160
      0x120u,  // 0x170
      0x120u,  // 0x180
      0x120u,  // 0x190
      0x120u,  // 0x1A0
      0x120u,  // 0x1B0
      0x120u,  // 0x1C0
      0x120u,  // 0x1D0
      0x120u,  // 0x1E0
      0x120u,  // 0x1F0
  };

  *address -= bank_base[bank];

  return memory->banks[bank];
}

static bool IoMemoryLoad32LE(const void* context, uint32_t address,
                             uint32_t* value) {
  const IoMemory* io_memory = (const IoMemory*)context;
  Memory* memory = IoMemorySelectBank(io_memory, &address);
  return Load32LE(memory, address, value);
}

static bool IoMemoryLoad16LE(const void* context, uint32_t address,
                             uint16_t* value) {
  const IoMemory* io_memory = (const IoMemory*)context;
  Memory* memory = IoMemorySelectBank(io_memory, &address);
  return Load16LE(memory, address, value);
}

static bool IoMemoryLoad8(const void* context, uint32_t address,
                          uint8_t* value) {
  const IoMemory* io_memory = (const IoMemory*)context;
  Memory* memory = IoMemorySelectBank(io_memory, &address);
  return Load8(memory, address, value);
}

static bool IoMemoryStore32LE(void* context, uint32_t address, uint32_t value) {
  IoMemory* io_memory = (IoMemory*)context;
  Memory* memory = IoMemorySelectBank(io_memory, &address);
  return Store32LE(memory, address, value);
}

static bool IoMemoryStore16LE(void* context, uint32_t address, uint16_t value) {
  IoMemory* io_memory = (IoMemory*)context;
  Memory* memory = IoMemorySelectBank(io_memory, &address);
  return Store16LE(memory, address, value);
}

static bool IoMemoryStore8(void* context, uint32_t address, uint8_t value) {
  IoMemory* io_memory = (IoMemory*)context;
  Memory* memory = IoMemorySelectBank(io_memory, &address);
  return Store8(memory, address, value);
}

static void IoMemoryFree(void* context) {
  IoMemory* io_memory = (IoMemory*)context;
  MemoryFree(io_memory->ppu);
  MemoryFree(io_memory->sound);
  MemoryFree(io_memory->dma);
  MemoryFree(io_memory->timer);
  MemoryFree(io_memory->peripherals);
  MemoryFree(io_memory->platform);
  free(io_memory);
}

Memory* IoMemoryAllocate(Memory* ppu, Memory* sound, Memory* dma, Memory* timer,
                         Memory* peripherals, Memory* platform) {
  IoMemory* io_memory = (IoMemory*)malloc(sizeof(IoMemory));
  if (io_memory == NULL) {
    return NULL;
  }

  io_memory->banks[0x00u] = ppu;
  io_memory->banks[0x01u] = ppu;
  io_memory->banks[0x02u] = ppu;
  io_memory->banks[0x03u] = ppu;
  io_memory->banks[0x04u] = ppu;
  io_memory->banks[0x05u] = sound;
  io_memory->banks[0x06u] = sound;
  io_memory->banks[0x07u] = sound;
  io_memory->banks[0x08u] = sound;
  io_memory->banks[0x09u] = sound;
  io_memory->banks[0x0Au] = sound;
  io_memory->banks[0x0Bu] = dma;
  io_memory->banks[0x0Cu] = dma;
  io_memory->banks[0x0Du] = dma;
  io_memory->banks[0x0Eu] = dma;
  io_memory->banks[0x0Fu] = dma;
  io_memory->banks[0x10u] = timer;
  io_memory->banks[0x11u] = timer;
  io_memory->banks[0x12u] = peripherals;
  io_memory->banks[0x13u] = peripherals;
  io_memory->banks[0x14u] = peripherals;
  io_memory->banks[0x15u] = peripherals;
  io_memory->banks[0x16u] = peripherals;
  io_memory->banks[0x17u] = peripherals;
  io_memory->banks[0x18u] = peripherals;
  io_memory->banks[0x19u] = peripherals;
  io_memory->banks[0x1Au] = peripherals;
  io_memory->banks[0x1Bu] = peripherals;
  io_memory->banks[0x1Cu] = peripherals;
  io_memory->banks[0x1Du] = peripherals;
  io_memory->banks[0x1Eu] = peripherals;
  io_memory->banks[0x1Fu] = peripherals;

  io_memory->ppu = ppu;
  io_memory->sound = sound;
  io_memory->dma = dma;
  io_memory->timer = timer;
  io_memory->peripherals = peripherals;
  io_memory->platform = platform;

  Memory* result = MemoryAllocate(
      io_memory, IoMemoryLoad32LE, IoMemoryLoad16LE, IoMemoryLoad8,
      IoMemoryStore32LE, IoMemoryStore16LE, IoMemoryStore8, IoMemoryFree);
  if (result == NULL) {
    free(io_memory);
    return NULL;
  }

  return result;
}