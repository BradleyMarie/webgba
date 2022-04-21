#include "emulator/cpu/arm7tdmi/memory.h"

#include <assert.h>

static inline uint32_t RotateRight(uint32_t value, uint_fast8_t amount) {
  return (value >> amount) | (value << (32u - amount));
}

static inline void RotateDataByAddress(uint32_t address, uint32_t *value) {
  uint_fast8_t rotate = address & 0x3u;
  if (rotate) {
    assert(0u < rotate && rotate < 4u);
    *value = RotateRight(*value, rotate * 8u);
  }
}

bool ArmLoad32LEWithRotation(const Memory *memory, uint32_t address,
                             uint32_t *value) {
  uint32_t masked_address = address & 0xFFFFFFFCu;
  bool result = Load32LE(memory, masked_address, value);
  RotateDataByAddress(address, value);
  return result;
}

bool ArmLoad32LE(const Memory *memory, uint32_t address, uint32_t *value) {
  uint32_t masked_address = address & 0xFFFFFFFCu;
  bool result = Load32LE(memory, masked_address, value);
  return result;
}

// Unaligned 16 bit reads have unpredictable behavior in the ARM
bool ArmLoad16LEWithRotation(const Memory *memory, uint32_t address,
                             uint32_t *value) {
  uint32_t masked_address = address & 0xFFFFFFFEu;
  uint16_t temp;
  bool result = Load16LE(memory, masked_address, &temp);
  *value = temp;
  if (address & 0x1u) {
    *value = RotateRight(*value, 8u);
  }
  return result;
}

bool ArmLoad32SLEWithRotation(const Memory *memory, uint32_t address,
                              int32_t *value) {
  uint32_t masked_address = address & 0xFFFFFFFCu;
  bool result = Load32SLE(memory, masked_address, value);
  RotateDataByAddress(address, (uint32_t *)value);
  return result;
}

bool ArmLoad32SLE(const Memory *memory, uint32_t address, int32_t *value) {
  uint32_t masked_address = address & 0xFFFFFFFCu;
  bool result = Load32SLE(memory, masked_address, value);
  return result;
}

// Unaligned 16 bit reads have unpredictable behavior in the ARM
bool ArmLoad16SLEWithRotation(const Memory *memory, uint32_t address,
                              int32_t *value) {
  uint32_t masked_address = address & 0xFFFFFFFEu;
  int16_t temp;
  bool result = Load16SLE(memory, masked_address, &temp);
  *value = temp;
  if (address & 0x1u) {
    *value >>= 8u;
  }
  return result;
}

bool ArmStore32LE(Memory *memory, uint32_t address, uint32_t value) {
  uint32_t masked_address = address & 0xFFFFFFFCu;
  return Store32LE(memory, masked_address, value);
}

// Unaligned 16 bit reads have unpredictable behavior in the ARM
bool ArmStore16LE(Memory *memory, uint32_t address, uint16_t value) {
  uint32_t masked_address = address & 0xFFFFFFFEu;
  return Store16LE(memory, masked_address, value);
}

bool ArmStore32SLE(Memory *memory, uint32_t address, int32_t value) {
  uint32_t masked_address = address & 0xFFFFFFFCu;
  return Store32SLE(memory, masked_address, value);
}

// Unaligned 16 bit reads have unpredictable behavior in the ARM
bool ArmStore16SLE(Memory *memory, uint32_t address, int16_t value) {
  uint32_t masked_address = address & 0xFFFFFFFEu;
  return Store16SLE(memory, masked_address, value);
}