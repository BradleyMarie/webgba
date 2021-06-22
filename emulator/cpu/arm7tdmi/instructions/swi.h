#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SWI_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SWI_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline void ArmSWI(ArmAllRegisters *registers) {
  ArmProgramStatusRegister old_cpsr = registers->current.user.cpsr;

  ArmProgramStatusRegister next_status = old_cpsr;
  next_status.mode = MODE_SVC;
  next_status.thumb = false;
  next_status.irq_disable = true;
  ArmLoadCPSR(registers, next_status);

  const static uint_fast8_t next_instruction_pc_offset[2] = {4, 2};
  registers->current.user.gprs.r14 = registers->current.user.gprs.pc -
                                     next_instruction_pc_offset[old_cpsr.thumb];

  registers->current.spsr = old_cpsr;
  registers->current.user.gprs.pc = 0x8;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SWI_