#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_DATA_PROCESSING_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_DATA_PROCESSING_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline uint64_t ArmADC(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t operand2) {
  uint64_t sum = (uint64_t)registers->gprs.gprs[Rn] + (uint64_t)operand2 +
                 (uint64_t)registers->cpsr.carry;
  registers->gprs.gprs[Rd] = (uint32_t)sum;
  return sum;
}

static inline void ArmADCS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  int64_t sum_s = (int64_t)registers->current.user.gprs.gprs_s[Rn] +
                  (int64_t)(int32_t)operand2 +
                  (int64_t)registers->current.user.cpsr.carry;
  uint64_t sum = ArmADC(&registers->current.user, Rd, Rn, operand2);
  if (Rd != REGISTER_R15) {
    registers->current.user.cpsr.negative =
        ArmNegativeFlag(registers->current.user.gprs.gprs_s[Rd]);
    registers->current.user.cpsr.zero =
        ArmZeroFlagUInt32(registers->current.user.gprs.gprs_s[Rd]);
    registers->current.user.cpsr.carry = ArmAdditionCarryFlag(sum);
    registers->current.user.cpsr.overflow = ArmOverflowFlag(sum_s);
  } else {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

static inline uint64_t ArmADD(ArmGeneralPurposeRegisters *registers,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t operand2) {
  uint64_t sum = (uint64_t)registers->gprs[Rn] + (uint64_t)operand2;
  registers->gprs[Rd] = (uint32_t)sum;
  return sum;
}

static inline void ArmADDS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  int64_t sum_s = (int64_t)registers->current.user.gprs.gprs_s[Rn] +
                  (int64_t)(int32_t)operand2;
  uint64_t sum = ArmADD(&registers->current.user.gprs, Rd, Rn, operand2);
  if (Rd != REGISTER_R15) {
    registers->current.user.cpsr.negative =
        ArmNegativeFlag(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.zero =
        ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.carry = ArmAdditionCarryFlag(sum);
    registers->current.user.cpsr.overflow = ArmOverflowFlag(sum_s);
  } else {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

static inline void ArmAND(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                          uint32_t operand2) {
  registers->gprs[Rd] = registers->gprs[Rn] & operand2;
}

static inline void ArmANDS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2,
                           bool operand2_carry) {
  ArmAND(&registers->current.user.gprs, Rd, Rn, operand2);
  if (Rd != REGISTER_R15) {
    registers->current.user.cpsr.negative =
        ArmNegativeFlag(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.zero =
        ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.carry = operand2_carry;
  } else {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

static inline void ArmBIC(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                          uint32_t operand2) {
  registers->gprs[Rd] = registers->gprs[Rn] & ~operand2;
}

static inline void ArmBICS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  ArmBIC(&registers->current.user.gprs, Rd, Rn, operand2);
  if (Rd != REGISTER_R15) {
    registers->current.user.cpsr.negative =
        ArmNegativeFlag(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.zero =
        ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
  } else {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

static inline void ArmCMN(ArmUserRegisters *registers, ArmRegisterIndex Rn,
                          uint32_t operand2) {
  uint64_t sum = (uint64_t)registers->gprs.gprs[Rn] + (uint64_t)operand2;
  int64_t sum_s =
      (int64_t)registers->gprs.gprs_s[Rn] + (int64_t)(int32_t)operand2;
  registers->cpsr.negative = ArmNegativeFlag((uint32_t)sum);
  registers->cpsr.zero = ArmZeroFlagUInt32((uint32_t)sum);
  registers->cpsr.carry = ArmAdditionCarryFlag(sum);
  registers->cpsr.overflow = ArmOverflowFlag(sum_s);
}

static inline void ArmCMP(ArmUserRegisters *registers, ArmRegisterIndex Rn,
                          uint32_t operand2) {
  uint64_t difference = (uint64_t)registers->gprs.gprs[Rn] - (uint64_t)operand2;
  int64_t difference_s =
      (int64_t)registers->gprs.gprs_s[Rn] - (int64_t)(int32_t)operand2;
  registers->cpsr.negative = ArmNegativeFlag((uint32_t)difference);
  registers->cpsr.zero = ArmZeroFlagUInt32((uint32_t)difference);
  registers->cpsr.carry = ArmSubtractionCarryFlag(difference);
  registers->cpsr.overflow = ArmOverflowFlag(difference_s);
}

static inline void ArmEOR(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                          uint32_t operand2) {
  registers->gprs[Rd] = registers->gprs[Rn] ^ operand2;
}

static inline void ArmEORS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2,
                           bool operand2_carry) {
  ArmEOR(&registers->current.user.gprs, Rd, Rn, operand2);
  if (Rd != REGISTER_R15) {
    registers->current.user.cpsr.negative =
        ArmNegativeFlag(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.zero =
        ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.carry = operand2_carry;
  } else {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

static inline void ArmMOV(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, uint32_t operand2) {
  registers->gprs[Rd] = operand2;
}

static inline void ArmMOVS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           uint32_t operand2, bool operand2_carry) {
  ArmMOV(&registers->current.user.gprs, Rd, operand2);
  if (Rd != REGISTER_R15) {
    registers->current.user.cpsr.negative =
        ArmNegativeFlag(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.zero =
        ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.carry = operand2_carry;
  } else {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

static inline void ArmMVN(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, uint32_t operand2) {
  registers->gprs[Rd] = ~operand2;
}

static inline void ArmMVNS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           uint32_t operand2, bool operand2_carry) {
  ArmMVN(&registers->current.user.gprs, Rd, operand2);
  if (Rd != REGISTER_R15) {
    registers->current.user.cpsr.negative =
        ArmNegativeFlag(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.zero =
        ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.carry = operand2_carry;
  } else {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

static inline void ArmORR(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                          uint32_t operand2) {
  registers->gprs[Rd] = registers->gprs[Rn] | operand2;
}

static inline void ArmORRS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2,
                           bool operand2_carry) {
  ArmORR(&registers->current.user.gprs, Rd, Rn, operand2);
  if (Rd != REGISTER_R15) {
    registers->current.user.cpsr.negative =
        ArmNegativeFlag(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.zero =
        ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.carry = operand2_carry;
  } else {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

static inline uint64_t ArmRSB(ArmGeneralPurposeRegisters *registers,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t operand2) {
  uint64_t difference = (uint64_t)operand2 - (uint64_t)registers->gprs[Rn];
  registers->gprs[Rd] = (uint32_t)difference;
  return difference;
}

static inline void ArmRSBS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  int64_t difference_s = (int64_t)(int32_t)operand2 -
                         (int64_t)registers->current.user.gprs.gprs_s[Rn];
  uint64_t difference = ArmRSB(&registers->current.user.gprs, Rd, Rn, operand2);
  if (Rd != REGISTER_R15) {
    registers->current.user.cpsr.negative =
        ArmNegativeFlag(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.zero =
        ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.carry = ArmSubtractionCarryFlag(difference);
    registers->current.user.cpsr.overflow = ArmOverflowFlag(difference_s);
  } else {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

static inline uint64_t ArmRSC(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t operand2) {
  uint64_t difference = (uint64_t)operand2 -
                        (uint64_t)registers->gprs.gprs[Rn] -
                        (uint64_t)!registers->cpsr.carry;
  registers->gprs.gprs[Rd] = (uint32_t)difference;
  return difference;
}

static inline void ArmRSCS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  int64_t difference_s = (int64_t)(int32_t)operand2 -
                         (int64_t)registers->current.user.gprs.gprs_s[Rn] -
                         (int64_t)!registers->current.user.cpsr.carry;
  uint64_t difference = ArmRSC(&registers->current.user, Rd, Rn, operand2);
  if (Rd != REGISTER_R15) {
    registers->current.user.cpsr.negative =
        ArmNegativeFlag(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.zero =
        ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.carry = ArmSubtractionCarryFlag(difference);
    registers->current.user.cpsr.overflow = ArmOverflowFlag(difference_s);
  } else {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

static inline uint64_t ArmSBC(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t operand2) {
  uint64_t difference = (uint64_t)registers->gprs.gprs[Rn] -
                        (uint64_t)operand2 - (uint64_t)!registers->cpsr.carry;
  registers->gprs.gprs[Rd] = (uint32_t)difference;
  return difference;
}

static inline void ArmSBCS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  int64_t difference_s = (int64_t)registers->current.user.gprs.gprs_s[Rn] -
                         (int64_t)(int32_t)operand2 -
                         (int64_t)!registers->current.user.cpsr.carry;
  uint64_t difference = ArmSBC(&registers->current.user, Rd, Rn, operand2);
  if (Rd != REGISTER_R15) {
    registers->current.user.cpsr.negative =
        ArmNegativeFlag(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.zero =
        ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.carry = ArmSubtractionCarryFlag(difference);
    registers->current.user.cpsr.overflow = ArmOverflowFlag(difference_s);
  } else {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

static inline uint64_t ArmSUB(ArmGeneralPurposeRegisters *registers,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t operand2) {
  uint64_t difference = (uint64_t)registers->gprs[Rn] - (uint64_t)operand2;
  registers->gprs[Rd] = (uint32_t)difference;
  return difference;
}

static inline void ArmSUBS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  int64_t difference_s = (int64_t)registers->current.user.gprs.gprs_s[Rn] -
                         (int64_t)(int32_t)operand2;
  uint64_t difference = ArmSUB(&registers->current.user.gprs, Rd, Rn, operand2);
  if (Rd != REGISTER_R15) {
    registers->current.user.cpsr.negative =
        ArmNegativeFlag(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.zero =
        ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.carry = ArmSubtractionCarryFlag(difference);
    registers->current.user.cpsr.overflow = ArmOverflowFlag(difference_s);
  } else {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

static inline void ArmTEQ(ArmUserRegisters *registers, ArmRegisterIndex Rn,
                          uint32_t operand2, bool operand2_carry) {
  uint32_t result = registers->gprs.gprs[Rn] ^ operand2;
  registers->cpsr.negative = ArmNegativeFlag(result);
  registers->cpsr.zero = ArmZeroFlagUInt32(result);
  registers->cpsr.carry = operand2_carry;
}

static inline void ArmTST(ArmUserRegisters *registers, ArmRegisterIndex Rn,
                          uint32_t operand2, bool operand2_carry) {
  uint32_t result = registers->gprs.gprs[Rn] & operand2;
  registers->cpsr.negative = ArmNegativeFlag(result);
  registers->cpsr.zero = ArmZeroFlagUInt32(result);
  registers->cpsr.carry = operand2_carry;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_DATA_PROCESSING_