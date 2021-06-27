#include "emulator/cpu/arm7tdmi/decoders/arm/condition.h"

#define ARM_CONDITION_EQ 0u
#define ARM_CONDITION_NE 1u
#define ARM_CONDITION_CS 2u
#define ARM_CONDITION_CC 3u
#define ARM_CONDITION_MI 4u
#define ARM_CONDITION_PL 5u
#define ARM_CONDITION_VS 6u
#define ARM_CONDITION_VC 7u
#define ARM_CONDITION_HI 8u
#define ARM_CONDITION_LS 9u
#define ARM_CONDITION_GE 10u
#define ARM_CONDITION_LT 11u
#define ARM_CONDITION_GT 12u
#define ARM_CONDITION_LE 13u
#define ARM_CONDITION_AL 14u
#define ARM_CONDITION_NV 15u

static inline uint_fast8_t ArmInstructionCondition(uint32_t instruction) {
  return instruction >> 28;
}

bool ArmInstructionShouldExecute(ArmProgramStatusRegister cpsr,
                                 uint32_t instruction) {
  uint_fast8_t condition = ArmInstructionCondition(instruction);

  switch (condition) {
    case ARM_CONDITION_AL:
      return true;
    case ARM_CONDITION_EQ:
      return cpsr.zero;
    case ARM_CONDITION_NE:
      return !cpsr.zero;
    case ARM_CONDITION_CS:
      return cpsr.carry;
    case ARM_CONDITION_CC:
      return !cpsr.carry;
    case ARM_CONDITION_MI:
      return cpsr.negative;
    case ARM_CONDITION_PL:
      return !cpsr.negative;
    case ARM_CONDITION_VS:
      return cpsr.overflow;
    case ARM_CONDITION_VC:
      return !cpsr.overflow;
    case ARM_CONDITION_HI:
      return !cpsr.zero && cpsr.carry;
    case ARM_CONDITION_LS:
      return cpsr.zero || !cpsr.carry;
    case ARM_CONDITION_GE:
      return cpsr.negative == cpsr.overflow;
    case ARM_CONDITION_LT:
      return cpsr.negative != cpsr.overflow;
    case ARM_CONDITION_GT:
      return !cpsr.zero && cpsr.negative == cpsr.overflow;
    case ARM_CONDITION_LE:
      return cpsr.zero || cpsr.negative != cpsr.overflow;
    case ARM_CONDITION_NV:
      return false;
  }

  assert(false);
  return false;
}