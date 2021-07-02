#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MOVE_STATUS_REGISTER_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MOVE_STATUS_REGISTER_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline void ArmMRS_CPSR(ArmUserRegisters *registers,
                               ArmRegisterIndex Rd) {
  registers->gprs.gprs[Rd] = registers->cpsr.value;
}

static inline void ArmMRS_SPSR(ArmPrivilegedRegisters *registers,
                               ArmRegisterIndex Rd) {
  registers->user.gprs.gprs[Rd] = registers->spsr.value;
}

static inline void ArmMSR_CPSR(ArmAllRegisters *registers, bool control,
                                 bool flags, uint32_t value) {
  ArmProgramStatusRegister requested_status;
  requested_status.value = value;

  ArmProgramStatusRegister next_status = registers->current.user.cpsr;
  if (registers->current.user.cpsr.mode != MODE_USR) {
    if (control) {
      next_status.thumb = requested_status.thumb;
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
}

static inline void ArmMSR_SPSR(ArmPrivilegedRegisters *registers,
                                 bool control, bool flags, uint32_t value) {
  ArmProgramStatusRegister next_status;
  next_status.value = value;
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

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MOVE_STATUS_REGISTER_