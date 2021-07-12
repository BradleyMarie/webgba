#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_FLAGS_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_FLAGS_

#include <assert.h>
#include <stdint.h>

static inline bool ArmNegativeFlag(uint32_t result) { return result >> 31; }

static inline bool ArmZeroFlagInt32(int32_t result) { return result == 0; }

static inline bool ArmZeroFlagInt64(int64_t result) { return result == 0; }

static inline bool ArmZeroFlagUInt32(uint32_t result) { return result == 0; }

static inline bool ArmZeroFlagUInt64(uint64_t result) { return result == 0; }

static inline bool ArmAdditionCarryFlag(uint64_t result) {
  assert(result < (uint64_t)UINT32_MAX + (uint64_t)UINT32_MAX + 1);
  return !!(result >> 32);
}

static inline bool ArmSubtractionCarryFlag(uint64_t result) {
  return !(result >> 63);
}

static inline bool ArmOverflowFlag(int64_t result) {
  return (result < (int64_t)INT32_MIN) | (result > (int64_t)INT32_MAX);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_FLAGS_