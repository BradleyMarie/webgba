#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_DATA_PROCESSING_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_DATA_PROCESSING_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline void ArmADD(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                          uint32_t operand2) {
  registers->gprs[Rd] = registers->gprs[Rn] + operand2;
}

static inline void ArmADDS(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  uint64_t sum = (uint64_t)registers->gprs.gprs[Rn] + (uint64_t)operand2;
  int64_t sum_s =
      (int64_t)registers->gprs.gprs_s[Rn] + (int64_t)(int32_t)operand2;
  registers->gprs.gprs[Rd] = (uint32_t)sum;
  registers->cpsr.negative = ((int32_t)sum_s < 0);
  registers->cpsr.zero = (sum == 0);
  registers->cpsr.carry = (sum > UINT32_MAX);
  registers->cpsr.overflow = !!((sum_s > INT32_MAX) | (sum_s < INT32_MIN));
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_DATA_PROCESSING_