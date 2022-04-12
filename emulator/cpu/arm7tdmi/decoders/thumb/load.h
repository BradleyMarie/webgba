#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_LOAD_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_LOAD_

#include <assert.h>

#include "emulator/cpu/arm7tdmi/exceptions.h"
#include "emulator/cpu/arm7tdmi/memory.h"
#include "emulator/cpu/arm7tdmi/registers.h"

static inline void ThumbLDR_PC_IB(ArmAllRegisters *registers,
                                  const Memory *memory, ArmRegisterIndex Rd,
                                  uint_fast16_t offset) {
  assert(Rd <= REGISTER_R7);

  uint32_t base = (registers->current.user.gprs.pc >> 2u) << 2u;
  bool success = ArmLoad32LE(memory, base + offset,
                             &registers->current.user.gprs.gprs[Rd]);
  if (!success) {
    ArmExceptionDataABT(registers);
  }
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_LOAD_