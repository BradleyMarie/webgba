#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_DATA_PROCESSING_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_DATA_PROCESSING_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline uint64_t ArmADD(ArmGeneralPurposeRegisters *registers,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t operand2) {
  uint64_t sum = (uint64_t)registers->gprs[Rn] + (uint64_t)operand2;
  registers->gprs[Rd] = (uint32_t)sum;
  return sum;
}

static inline void ArmADDS(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  int64_t sum_s =
      (int64_t)registers->gprs.gprs_s[Rn] + (int64_t)(int32_t)operand2;
  uint64_t sum = ArmADD(&registers->gprs, Rd, Rn, operand2);
  registers->cpsr.negative = ArmNegativeFlagUInt32(registers->gprs.gprs_s[Rd]);
  registers->cpsr.zero = ArmZeroFlagUInt64(sum);
  registers->cpsr.carry = ArmCarryFlag(sum);
  registers->cpsr.overflow = ArmOverflowFlag(sum_s);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_DATA_PROCESSING_