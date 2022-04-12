#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_

#include "emulator/cpu/arm7tdmi/flags.h"
#include "emulator/cpu/arm7tdmi/registers.h"

static inline uint32_t ArmMUL(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rm, ArmRegisterIndex Rs) {
  uint32_t product = registers->current.user.gprs.gprs[Rm] *
                     registers->current.user.gprs.gprs[Rs];
  ArmLoadGPSR(registers, Rd, product);
  return product;
}

static inline void ArmMULS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rm, ArmRegisterIndex Rs) {
  uint32_t product = ArmMUL(registers, Rd, Rm, Rs);
  registers->current.user.cpsr.zero = ArmZeroFlagUInt32(product);
  registers->current.user.cpsr.negative = ArmNegativeFlagUInt32(product);
}

static inline uint32_t ArmMLA(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rm, ArmRegisterIndex Rs,
                              ArmRegisterIndex Rn) {
  uint32_t product = registers->current.user.gprs.gprs[Rn] +
                     (registers->current.user.gprs.gprs[Rm] *
                      registers->current.user.gprs.gprs[Rs]);
  ArmLoadGPSR(registers, Rd, product);
  return product;
}

static inline void ArmMLAS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rm, ArmRegisterIndex Rs,
                           ArmRegisterIndex Rn) {
  uint32_t product = ArmMLA(registers, Rd, Rm, Rs, Rn);
  registers->current.user.cpsr.zero = ArmZeroFlagUInt32(product);
  registers->current.user.cpsr.negative = ArmNegativeFlagUInt32(product);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_