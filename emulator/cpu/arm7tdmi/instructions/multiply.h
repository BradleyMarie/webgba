#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_

#include "emulator/cpu/arm7tdmi/flags.h"
#include "emulator/cpu/arm7tdmi/registers.h"

static inline void ArmMUL(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rm,
                          ArmRegisterIndex Rs) {
  registers->gprs[Rd] = registers->gprs[Rm] * registers->gprs[Rs];
}

static inline void ArmMULS(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rm, ArmRegisterIndex Rs) {
  ArmMUL(&registers->gprs, Rd, Rm, Rs);
  registers->cpsr.zero = ArmZeroFlagUInt32(registers->gprs.gprs[Rd]);
  registers->cpsr.negative = ArmNegativeFlag(registers->gprs.gprs[Rd]);
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
  ArmMLA(&registers->gprs, Rd, Rm, Rs, Rn);
  registers->cpsr.zero = ArmZeroFlagUInt32(registers->gprs.gprs[Rd]);
  registers->cpsr.negative = ArmNegativeFlag(registers->gprs.gprs[Rd]);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_