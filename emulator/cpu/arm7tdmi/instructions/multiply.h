#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline void ArmMUL(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rm,
                          ArmRegisterIndex Rs) {
  registers->gprs[Rd] = registers->gprs[Rm] * registers->gprs[Rs];
}

static inline void ArmMULS(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rm, ArmRegisterIndex Rs) {
  registers->gprs.gprs[Rd] =
      registers->gprs.gprs[Rm] * registers->gprs.gprs[Rs];
  registers->cpsr.zero = (registers->gprs.gprs[Rd] == 0);
  registers->cpsr.negative = !!(registers->gprs.gprs[Rd] & (1 << 31));
}

static inline void ArmMLA(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rm,
                          ArmRegisterIndex Rs, ArmRegisterIndex Rn) {
  registers->gprs[Rd] =
      registers->gprs[Rn] + (registers->gprs[Rm] * registers->gprs[Rs]);
}

static inline void ArmMLAS(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rm, ArmRegisterIndex Rs,
                           ArmRegisterIndex Rn) {
  registers->gprs.gprs[Rd] =
      registers->gprs.gprs[Rn] +
      (registers->gprs.gprs[Rm] * registers->gprs.gprs[Rs]);
  registers->cpsr.zero = (registers->gprs.gprs[Rd] == 0);
  registers->cpsr.negative = !!(registers->gprs.gprs[Rd] & (1 << 31));
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_