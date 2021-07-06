#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_BRANCH_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_BRANCH_

#include <assert.h>

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline void ArmB(ArmGeneralPurposeRegisters *registers,
                        int_fast32_t offset) {
  assert(-8388608 <= offset && offset <= 8388607);
  offset = (int32_t)((uint32_t)offset << 2u);
  registers->pc += offset;
}

static inline void ArmBL(ArmGeneralPurposeRegisters *registers,
                         int_fast32_t offset) {
  assert(-8388608 <= offset && offset <= 8388607);
  offset = (int32_t)((uint32_t)offset << 2u);
  registers->lr = registers->pc - 4u;
  registers->pc += offset;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_BRANCH_