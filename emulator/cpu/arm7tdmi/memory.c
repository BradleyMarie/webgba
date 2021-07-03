#include "emulator/cpu/arm7tdmi/memory.h"

#include <assert.h>

static inline uint32_t RotateRight(uint32_t value, uint_fast8_t amount) {
  return (value >> amount) | (value << (32u - amount));
}

static inline void RotateDataByAddress(uint32_t address, uint32_t *value) {
  uint_fast8_t rotate = address & 0x3u;
  switch (rotate) {
    case 0u:
      break;
    case 1u:
      *value = RotateRight(*value, 8u);
      break;
    case 2u:
      *value = RotateRight(*value, 16u);
      break;
    case 3u:
      *value = RotateRight(*value, 24u);
      break;
    default:
      assert(false);
  }
}

bool ArmLoad32LEWithRotation(const Memory *memory, uint32_t address,
                             uint32_t *value) {
  uint32_t masked_address = address & (~3u);
  bool result = Load32LE(memory, masked_address, value);
  RotateDataByAddress(address, value);
  return result;
}

bool ArmLoad32LE(const Memory *memory, uint32_t address, uint32_t *value) {
  uint32_t masked_address = address & (~3u);
  bool result = Load32LE(memory, masked_address, value);
  return result;
}

// Unaligned 16 bit reads have unpredictable behavior in the ARM
bool ArmLoad16LE(const Memory *memory, uint32_t address, uint16_t *value) {
  uint32_t masked_address = address & (~1u);
  bool result = Load16LE(memory, masked_address, value);
  return result;
}

bool ArmLoad32SLEWithRotation(const Memory *memory, uint32_t address,
                              int32_t *value) {
  uint32_t masked_address = address & (~3u);
  bool result = Load32SLE(memory, masked_address, value);
  RotateDataByAddress(address, (uint32_t *)value);
  return result;
}

bool ArmLoad32SLE(const Memory *memory, uint32_t address, int32_t *value) {
  uint32_t masked_address = address & (~3u);
  bool result = Load32SLE(memory, masked_address, value);
  return result;
}

// Unaligned 16 bit reads have unpredictable behavior in the ARM
bool ArmLoad16SLE(const Memory *memory, uint32_t address, int16_t *value) {
  uint32_t masked_address = address & (~1u);
  bool result = Load16SLE(memory, masked_address, value);
  return result;
}

bool ArmStore32LE(Memory *memory, uint32_t address, uint32_t value) {
  uint32_t masked_address = address & (~3u);
  return Store32LE(memory, masked_address, value);
}

// Unaligned 16 bit reads have unpredictable behavior in the ARM
bool ArmStore16LE(Memory *memory, uint32_t address, uint16_t value) {
  uint32_t masked_address = address & (~1u);
  return Store16LE(memory, masked_address, value);
}

bool ArmStore32SLE(Memory *memory, uint32_t address, int32_t value) {
  uint32_t masked_address = address & (~3u);
  return Store32SLE(memory, masked_address, value);
}

// Unaligned 16 bit reads have unpredictable behavior in the ARM
bool ArmStore16SLE(Memory *memory, uint32_t address, int16_t value) {
  uint32_t masked_address = address & (~1u);
  return Store16SLE(memory, masked_address, value);
}