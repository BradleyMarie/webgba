#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_SHIFT_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_SHIFT_

#include <assert.h>

#include "emulator/cpu/arm7tdmi/flags.h"
#include "emulator/cpu/arm7tdmi/registers.h"
#include "util/macros.h"

static inline void ThumbASRS_I(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rm, uint_fast8_t shift_amount) {
  codegen_assert(registers->current.user.cpsr.thumb);
  assert(Rd <= REGISTER_R7);
  assert(Rm <= REGISTER_R7);
  assert(shift_amount < 32u);

  ArmAdvanceProgramCounter(registers);

  if (shift_amount == 0u) {
    if (registers->current.user.gprs.gprs[Rm] >> 31u) {
      registers->current.user.cpsr.carry = true;
      registers->current.user.gprs.gprs_s[Rd] = 0xFFFFFFFFu;
    } else {
      registers->current.user.cpsr.carry = false;
      registers->current.user.gprs.gprs_s[Rd] = 0u;
    }
  } else {
    registers->current.user.cpsr.carry =
        (registers->current.user.gprs.gprs[Rm] >> (shift_amount - 1u)) & 0x1u;
    // Technically this triggers implementation defined behavior
    registers->current.user.gprs.gprs_s[Rd] =
        registers->current.user.gprs.gprs_s[Rm] >> shift_amount;
  }

  registers->current.user.cpsr.negative =
      ArmNegativeFlagUInt32(registers->current.user.gprs.gprs[Rd]);
  registers->current.user.cpsr.zero =
      ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
}

static inline void ThumbASRS_R(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rs) {
  codegen_assert(registers->current.user.cpsr.thumb);
  assert(Rd <= REGISTER_R7);
  assert(Rs <= REGISTER_R7);

  ArmAdvanceProgramCounter(registers);

  uint8_t shift_amount = registers->current.user.gprs.gprs_s[Rs];
  if (shift_amount != 0u && shift_amount < 32u) {
    registers->current.user.cpsr.carry =
        (registers->current.user.gprs.gprs[Rd] >> (shift_amount - 1u)) & 0x1u;
    // Technically this triggers implementation defined behavior
    registers->current.user.gprs.gprs_s[Rd] >>= shift_amount;
  } else if (shift_amount >= 32u) {
    if (registers->current.user.gprs.gprs[Rd] >> 31u) {
      registers->current.user.cpsr.carry = true;
      registers->current.user.gprs.gprs_s[Rd] = 0xFFFFFFFFu;
    } else {
      registers->current.user.cpsr.carry = false;
      registers->current.user.gprs.gprs_s[Rd] = 0u;
    }
  }

  registers->current.user.cpsr.negative =
      ArmNegativeFlagUInt32(registers->current.user.gprs.gprs[Rd]);
  registers->current.user.cpsr.zero =
      ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
}

static inline void ThumbLSLS_I(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rm, uint_fast8_t shift_amount) {
  codegen_assert(registers->current.user.cpsr.thumb);
  assert(Rd <= REGISTER_R7);
  assert(Rm <= REGISTER_R7);
  assert(shift_amount < 32u);

  ArmAdvanceProgramCounter(registers);

  if (shift_amount == 0u) {
    registers->current.user.gprs.gprs[Rd] =
        registers->current.user.gprs.gprs[Rm];
  } else {
    registers->current.user.cpsr.carry =
        (registers->current.user.gprs.gprs[Rm] >> (32u - shift_amount)) & 0x1u;
    registers->current.user.gprs.gprs[Rd] =
        registers->current.user.gprs.gprs[Rm] << shift_amount;
  }

  registers->current.user.cpsr.negative =
      ArmNegativeFlagUInt32(registers->current.user.gprs.gprs[Rd]);
  registers->current.user.cpsr.zero =
      ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
}

static inline void ThumbLSLS_R(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rs) {
  codegen_assert(registers->current.user.cpsr.thumb);
  assert(Rd <= REGISTER_R7);
  assert(Rs <= REGISTER_R7);

  ArmAdvanceProgramCounter(registers);

  uint8_t shift_amount = registers->current.user.gprs.gprs_s[Rs];
  if (shift_amount != 0u && shift_amount < 32u) {
    registers->current.user.cpsr.carry =
        (registers->current.user.gprs.gprs[Rd] >> (32u - shift_amount)) & 0x1u;
    registers->current.user.gprs.gprs[Rd] <<= shift_amount;
  } else if (shift_amount == 32u) {
    registers->current.user.cpsr.carry =
        registers->current.user.gprs.gprs[Rd] & 0x1u;
    registers->current.user.gprs.gprs[Rd] = 0u;
  } else if (shift_amount > 32u) {
    registers->current.user.cpsr.carry = false;
    registers->current.user.gprs.gprs[Rd] = 0u;
  }

  registers->current.user.cpsr.negative =
      ArmNegativeFlagUInt32(registers->current.user.gprs.gprs[Rd]);
  registers->current.user.cpsr.zero =
      ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
}

static inline void ThumbLSRS_I(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rm, uint_fast8_t shift_amount) {
  codegen_assert(registers->current.user.cpsr.thumb);
  assert(Rd <= REGISTER_R7);
  assert(Rm <= REGISTER_R7);
  assert(shift_amount < 32u);

  ArmAdvanceProgramCounter(registers);

  if (shift_amount == 0u) {
    registers->current.user.cpsr.carry =
        (registers->current.user.gprs.gprs[Rm] >> 31u) & 0x1u;
    registers->current.user.gprs.gprs[Rd] = 0u;
  } else {
    registers->current.user.cpsr.carry =
        (registers->current.user.gprs.gprs[Rm] >> (shift_amount - 1u)) & 0x1u;
    registers->current.user.gprs.gprs[Rd] =
        registers->current.user.gprs.gprs[Rm] >> shift_amount;
  }

  registers->current.user.cpsr.negative =
      ArmNegativeFlagUInt32(registers->current.user.gprs.gprs[Rd]);
  registers->current.user.cpsr.zero =
      ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
}

static inline void ThumbLSRS_R(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rm) {
  codegen_assert(registers->current.user.cpsr.thumb);
  assert(Rd <= REGISTER_R7);
  assert(Rm <= REGISTER_R7);

  ArmAdvanceProgramCounter(registers);

  uint8_t shift_amount = registers->current.user.gprs.gprs_s[Rm];
  if (shift_amount != 0u && shift_amount < 32u) {
    registers->current.user.cpsr.carry =
        (registers->current.user.gprs.gprs[Rd] >> (shift_amount - 1u)) & 0x1u;
    registers->current.user.gprs.gprs[Rd] >>= shift_amount;
  } else if (shift_amount == 32u) {
    registers->current.user.cpsr.carry =
        (registers->current.user.gprs.gprs[Rd] >> 31u) & 0x1u;
    registers->current.user.gprs.gprs[Rd] = 0u;
  } else if (shift_amount > 32u) {
    registers->current.user.cpsr.carry = false;
    registers->current.user.gprs.gprs[Rd] = 0u;
  }

  registers->current.user.cpsr.negative =
      ArmNegativeFlagUInt32(registers->current.user.gprs.gprs[Rd]);
  registers->current.user.cpsr.zero =
      ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
}

static inline void ThumbRORS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                             ArmRegisterIndex Rm) {
  codegen_assert(registers->current.user.cpsr.thumb);
  assert(Rd <= REGISTER_R7);
  assert(Rm <= REGISTER_R7);

  ArmAdvanceProgramCounter(registers);

  uint8_t shift_amount = registers->current.user.gprs.gprs_s[Rm];
  if (shift_amount == 0) {
    return;
  }

  shift_amount &= 0x1Fu;
  if (shift_amount == 0) {
    registers->current.user.cpsr.carry =
        (registers->current.user.gprs.gprs[Rd] >> 31) & 0x1u;
  } else {
    registers->current.user.cpsr.carry =
        (registers->current.user.gprs.gprs[Rd] >> (shift_amount - 1u)) & 0x1u;
    registers->current.user.gprs.gprs[Rd] =
        (registers->current.user.gprs.gprs[Rd] >> shift_amount) |
        (registers->current.user.gprs.gprs[Rd] << (32u - shift_amount));
  }

  registers->current.user.cpsr.negative =
      ArmNegativeFlagUInt32(registers->current.user.gprs.gprs[Rd]);
  registers->current.user.cpsr.zero =
      ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_SHIFT_