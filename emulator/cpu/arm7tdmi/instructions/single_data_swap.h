#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SINGLE_DATA_SWAP_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SINGLE_DATA_SWAP_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline void ArmMRS(ArmUserRegisters *registers, ArmRegisterIndex Rd) {
  registers->gprs.gprs[Rd] = registers->cpsr.value;
}

static inline void ArmMRSR(ArmPrivilegedRegisters *registers,
                           ArmRegisterIndex Rd) {
  registers->user.gprs.gprs[Rd] = registers->spsr.value;
}

static inline void ArmMSR_Immediate(ArmAllRegisters *registers, bool control,
                                    bool flags, uint32_t immediate) {
  ArmProgramStatusRegister requested_status;
  requested_status.value = immediate;

  ArmProgramStatusRegister next_status = registers->current.user.cpsr;
  if (control) {
    next_status.thumb = requested_status.thumb;
    if (registers->current.user.cpsr.mode != MODE_USR) {
      next_status.mode = requested_status.mode;
      next_status.fiq_disable = requested_status.fiq_disable;
      next_status.irq_disable = requested_status.irq_disable;
    }
  }
  if (flags) {
    next_status.overflow = requested_status.overflow;
    next_status.carry = requested_status.carry;
    next_status.zero = requested_status.zero;
    next_status.negative = requested_status.negative;
  }

  uint_fast8_t current_bank_index =
      ArmModeToBankIndex(registers->current.user.cpsr.mode);
  uint_fast8_t current_bank_size = ArmBankIndexToBankSize(current_bank_index);
  for (uint_fast8_t i = 0; i < current_bank_size; i++) {
    registers->banked_gprs[current_bank_index][i] =
        registers->current.user.gprs.gprs[REGISTER_R14 - i];
  }
  registers->banked_spsrs[current_bank_index] = registers->current.spsr;

  registers->current.user.cpsr = next_status;

  uint_fast8_t next_bank_index =
      ArmModeToBankIndex(registers->current.user.cpsr.mode);
  uint_fast8_t next_bank_size = ArmBankIndexToBankSize(next_bank_index);
  for (uint_fast8_t i = 0; i < next_bank_size; i++) {
    registers->current.user.gprs.gprs[REGISTER_R14 - i] =
        registers->banked_gprs[next_bank_index][i];
  }
  registers->current.spsr = registers->banked_spsrs[next_bank_index];
}

static inline void ArmMSR_Register(ArmAllRegisters *registers,
                                   ArmRegisterIndex Rm) {
  ArmMSR_Immediate(registers, true, true,
                   registers->current.user.gprs.gprs[Rm]);
}

static inline void ArmMSRR_Immediate(ArmPrivilegedRegisters *registers,
                                     bool control, bool flags,
                                     uint32_t immediate) {
  ArmProgramStatusRegister next_status;
  next_status.value = immediate;
  if (control) {
    registers->spsr.mode = next_status.mode;
    registers->spsr.thumb = next_status.thumb;
    registers->spsr.fiq_disable = next_status.fiq_disable;
    registers->spsr.irq_disable = next_status.irq_disable;
  }
  if (flags) {
    registers->spsr.overflow = next_status.overflow;
    registers->spsr.carry = next_status.carry;
    registers->spsr.zero = next_status.zero;
    registers->spsr.negative = next_status.negative;
  }
}

static inline void ArmMSRR_Register(ArmPrivilegedRegisters *registers,
                                    ArmRegisterIndex Rm) {
  registers->spsr.value = registers->user.gprs.gprs[Rm];
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SINGLE_DATA_SWAP_