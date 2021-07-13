#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_BRANCH_LINK_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_BRANCH_LINK_

#include <assert.h>

#include "emulator/cpu/arm7tdmi/registers.h"

static inline void ThumbBL1(ArmGeneralPurposeRegisters *registers,
                            uint_fast32_t offset) {
  registers->lr = registers->pc + offset;
}

static inline void ThumbBL2(ArmGeneralPurposeRegisters *registers,
                            uint_fast16_t offset) {
  assert(offset <= 4094u);
  uint32_t pc = registers->pc;
  registers->pc = registers->lr + offset;
  registers->lr = pc - 1;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_BRANCH_LINK_