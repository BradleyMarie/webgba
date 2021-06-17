#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SWI_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SWI_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline void ArmSWI(ArmAllRegisters *registers) {
  uint_fast8_t current_bank =
      ArmModeToBankIndex(registers->current.user.cpsr.mode);
  registers->banked_gprs[current_bank][BANKED_R13_INDEX] =
      registers->current.user.gprs.r13;
  registers->banked_gprs[current_bank][BANKED_R14_INDEX] =
      registers->current.user.gprs.r14;
  registers->banked_spsrs[current_bank] = registers->current.spsr;

  const static uint_fast8_t next_instruction_pc_offset[2] = {4, 2};
  registers->current.user.gprs.r13 =
      registers->banked_gprs[SVC_BANK_INDEX][BANKED_R13_INDEX];
  registers->current.user.gprs.r14 =
      registers->current.user.gprs.pc -
      next_instruction_pc_offset[registers->current.user.cpsr.thumb];

  registers->current.spsr = registers->current.user.cpsr;
  registers->current.user.cpsr.mode = MODE_SVC;
  registers->current.user.cpsr.thumb = false;
  registers->current.user.cpsr.irq_disable = true;
  registers->current.user.gprs.pc = 0x8;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SWI_