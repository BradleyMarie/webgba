#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_BRANCH_LINK_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_BRANCH_LINK_

#include "emulator/cpu/arm7tdmi/registers.h"

static inline void ArmBL(ArmGeneralPurposeRegisters *registers,
                         uint_fast32_t offset) {
  registers->lr = registers->pc - 4u;
  registers->pc += offset + 4u;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_BRANCH_LINK_