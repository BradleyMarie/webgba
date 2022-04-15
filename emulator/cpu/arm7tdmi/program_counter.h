#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_PROGRAM_COUNTER_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_PROGRAM_COUNTER_

#include "emulator/cpu/arm7tdmi/registers.h"

static inline uint32_t ArmNextInstruction(const ArmAllRegisters* registers) {
  return registers->current.user.gprs.pc -
         (4u >> registers->current.user.cpsr.thumb);
}

static inline uint32_t ArmCurrentInstruction(const ArmAllRegisters* registers) {
  return registers->current.user.gprs.pc -
         (8u >> registers->current.user.cpsr.thumb);
}

static inline void ArmAdvanceProgramCounter(ArmAllRegisters* registers) {
  registers->current.user.gprs.pc += 4u >> registers->current.user.cpsr.thumb;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_PROGRAM_COUNTER_