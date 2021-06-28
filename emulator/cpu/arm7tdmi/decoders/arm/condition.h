#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_CONDITION_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_CONDITION_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline bool ArmInstructionShouldExecute(ArmProgramStatusRegister cpsr,
                                               uint32_t instruction) {
  uint_fast8_t condition = instruction >> 28u;

  switch (condition) {
    case 14u:  // ARM_CONDITION_AL
      return true;
    case 0u:  // ARM_CONDITION_EQ
      return cpsr.zero;
    case 1u:  // ARM_CONDITION_NE
      return !cpsr.zero;
    case 2u:  // ARM_CONDITION_CS
      return cpsr.carry;
    case 3u:  // ARM_CONDITION_CC
      return !cpsr.carry;
    case 4u:  // ARM_CONDITION_MI
      return cpsr.negative;
    case 5u:  // ARM_CONDITION_PL
      return !cpsr.negative;
    case 6u:  // ARM_CONDITION_VS
      return cpsr.overflow;
    case 7u:  // ARM_CONDITION_VC
      return !cpsr.overflow;
    case 8u:  // ARM_CONDITION_HI
      return !cpsr.zero && cpsr.carry;
    case 9u:  // ARM_CONDITION_LS
      return cpsr.zero || !cpsr.carry;
    case 10u:  // ARM_CONDITION_GE
      return cpsr.negative == cpsr.overflow;
    case 11u:  // ARM_CONDITION_LT
      return cpsr.negative != cpsr.overflow;
    case 12u:  // ARM_CONDITION_GT
      return !cpsr.zero && cpsr.negative == cpsr.overflow;
    case 13u:  // 1ARM_CONDITION_LE
      return cpsr.zero || cpsr.negative != cpsr.overflow;
    case 15u:  // ARM_CONDITION_NV
      return false;
  }

  assert(false);
  return false;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_CONDITION_