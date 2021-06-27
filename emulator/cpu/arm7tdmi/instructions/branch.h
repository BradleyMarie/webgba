#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_BRANCH_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_BRANCH_

#include <assert.h>

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline void ArmB(ArmGeneralPurposeRegisters *registers,
                        int_fast32_t offset) {
  assert(-8388608 <= offset && offset <= 8388607);
  offset = (int32_t)((uint32_t)offset << 2);
  registers->pc += offset;
}

static inline void ArmBL(ArmUserRegisters *registers, int_fast32_t offset) {
  assert(-8388608 <= offset && offset <= 8388607);
  const static uint_fast8_t next_instruction_pc_offset[2] = {4, 2};
  offset = (int32_t)((uint32_t)offset << 2);
  registers->gprs.lr = registers->gprs.pc -
                       next_instruction_pc_offset[registers->cpsr.thumb] +
                       registers->cpsr.thumb;
  registers->gprs.pc += offset;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_BRANCH_