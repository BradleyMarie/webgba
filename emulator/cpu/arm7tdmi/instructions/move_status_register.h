#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MOVE_STATUS_REGISTER_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MOVE_STATUS_REGISTER_

#include "emulator/cpu/arm7tdmi/registers.h"

static inline void ArmMRS_CPSR(ArmAllRegisters *registers,
                               ArmRegisterIndex Rd) {
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, registers->current.user.cpsr.value);
}

static inline void ArmMRS_SPSR(ArmAllRegisters *registers,
                               ArmRegisterIndex Rd) {
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, registers->current.spsr.value);
}

static inline void ArmMSR_CPSR(ArmAllRegisters *registers, bool control,
                               bool flags, uint32_t value) {
  ArmProgramStatusRegister requested_status;
  requested_status.value = value;

  ArmProgramStatusRegister next_status = registers->current.user.cpsr;
  if (registers->current.user.cpsr.mode != MODE_USR) {
    if (control) {
      // Writes to the thumb bit trigger unpredictable behavior and are ignored
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

  ArmLoadCPSR(registers, next_status);
  ArmAdvanceProgramCounter(registers);
}

static inline void ArmMSR_SPSR(ArmAllRegisters *registers, bool control,
                               bool flags, uint32_t value) {
  ArmProgramStatusRegister next_status;
  next_status.value = value;

  if (control) {
    registers->current.spsr.mode = next_status.mode;
    registers->current.spsr.thumb = next_status.thumb;
    registers->current.spsr.fiq_disable = next_status.fiq_disable;
    registers->current.spsr.irq_disable = next_status.irq_disable;
  }

  if (flags) {
    registers->current.spsr.overflow = next_status.overflow;
    registers->current.spsr.carry = next_status.carry;
    registers->current.spsr.zero = next_status.zero;
    registers->current.spsr.negative = next_status.negative;
  }

  ArmAdvanceProgramCounter(registers);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MOVE_STATUS_REGISTER_