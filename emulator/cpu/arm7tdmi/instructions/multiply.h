#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline void ArmMUL(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rs,
                          ArmRegisterIndex Rm) {
  registers->gprs[Rd] = registers->gprs[Rs] * registers->gprs[Rm];
}

static inline void ArmMULS(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rs, ArmRegisterIndex Rm) {
  registers->gprs.gprs[Rd] =
      registers->gprs.gprs[Rs] * registers->gprs.gprs[Rm];
  if (registers->gprs.gprs[Rd] == 0) {
    registers->cpsr.zero = 1;
    registers->cpsr.negative = 0;
  } else if (registers->gprs.gprs[Rd] & (1 << 31)) {
    registers->cpsr.zero = 0;
    registers->cpsr.negative = 1;
  } else {
    registers->cpsr.zero = 0;
    registers->cpsr.negative = 0;
  }
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_