#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_CONDITION_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_CONDITION_

#include <assert.h>

#include "emulator/cpu/arm7tdmi/registers.h"

static inline bool ThumbShouldBranch(ArmProgramStatusRegister cpsr,
                                     uint_fast8_t condition) {
  switch (condition) {
    case 0u:  // THUMB_CONDITION_EQ
      return cpsr.zero;
    case 1u:  // THUMB_CONDITION_NE
      return !cpsr.zero;
    case 2u:  // THUMB_CONDITION_CS
      return cpsr.carry;
    case 3u:  // THUMB_CONDITION_CC
      return !cpsr.carry;
    case 4u:  // THUMB_CONDITION_MI
      return cpsr.negative;
    case 5u:  // THUMB_CONDITION_PL
      return !cpsr.negative;
    case 6u:  // THUMB_CONDITION_VS
      return cpsr.overflow;
    case 7u:  // THUMB_CONDITION_VC
      return !cpsr.overflow;
    case 8u:  // THUMB_CONDITION_HI
      return !cpsr.zero && cpsr.carry;
    case 9u:  // THUMB_CONDITION_LS
      return cpsr.zero || !cpsr.carry;
    case 10u:  // THUMB_CONDITION_GE
      return cpsr.negative == cpsr.overflow;
    case 11u:  // THUMB_CONDITION_LT
      return cpsr.negative != cpsr.overflow;
    case 12u:  // THUMB_CONDITION_GT
      return !cpsr.zero && cpsr.negative == cpsr.overflow;
    case 13u:  // THUMB_CONDITION_LE
      return cpsr.zero || cpsr.negative != cpsr.overflow;
  }

  assert(false);
  return false;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_CONDITION_