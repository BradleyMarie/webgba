#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_LOAD_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_LOAD_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"
#include "emulator/cpu/arm7tdmi/memory.h"

static inline void ThumbLDR_PC_IB(ArmGeneralPurposeRegisters *registers,
                                  const Memory *memory, ArmRegisterIndex Rd,
                                  uint_fast16_t offset) {
  assert((registers->pc & 1u) == 0u);
  uint32_t base = registers->pc >> 2u;
  base <<= 2u;
  bool success = ArmLoad32LE(memory, base + offset, &registers->gprs[Rd]);
  assert(success);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_LOAD_