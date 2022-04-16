#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_BRANCH_LINK_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_BRANCH_LINK_

#include <assert.h>

#include "emulator/cpu/arm7tdmi/registers.h"
#include "util/macros.h"

static inline void ThumbBL1(ArmAllRegisters *registers, uint_fast32_t offset) {
  codegen_assert(registers->current.user.cpsr.thumb);
  registers->current.user.gprs.lr = registers->current.user.gprs.pc + offset;
  ArmAdvanceProgramCounter(registers);
}

static inline void ThumbBL2(ArmAllRegisters *registers, uint_fast16_t offset) {
  codegen_assert(registers->current.user.cpsr.thumb);
  assert(offset <= 4094u);
  uint32_t pc = registers->current.user.gprs.pc;
  ArmLoadProgramCounter(registers, registers->current.user.gprs.lr + offset);
  registers->current.user.gprs.lr = pc - 1;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_BRANCH_LINK_