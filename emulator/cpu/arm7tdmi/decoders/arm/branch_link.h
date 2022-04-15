#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_BRANCH_LINK_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_BRANCH_LINK_

#include "emulator/cpu/arm7tdmi/registers.h"
#include "util/macros.h"

static inline void ArmBL(ArmAllRegisters *registers, uint_fast32_t offset) {
  codegen_assert(!registers->current.user.cpsr.thumb);
  registers->current.user.gprs.lr = registers->current.user.gprs.pc - 4u;
  ArmLoadProgramCounter(registers, registers->current.user.gprs.pc + offset);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_BRANCH_LINK_