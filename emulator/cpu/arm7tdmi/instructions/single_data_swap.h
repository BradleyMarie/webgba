#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SINGLE_DATA_SWAP_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SINGLE_DATA_SWAP_

#include <assert.h>

#include "emulator/cpu/arm7tdmi/memory.h"
#include "emulator/cpu/arm7tdmi/registers.h"

static inline void ArmSWP(ArmGeneralPurposeRegisters *registers, Memory *memory,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rm,
                          ArmRegisterIndex Rn) {
  uint32_t temp;
  bool success = ArmLoad32LE(memory, registers->gprs[Rn], &temp);
  assert(success);

  success = ArmStore32LE(memory, registers->gprs[Rn], registers->gprs[Rm]);
  assert(success);

  registers->gprs[Rd] = temp;
}

static inline void ArmSWPB(ArmGeneralPurposeRegisters *registers,
                           Memory *memory, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rm, ArmRegisterIndex Rn) {
  uint8_t temp;
  bool success = Load8(memory, registers->gprs[Rn], &temp);
  assert(success);

  success = Store8(memory, registers->gprs[Rn], (uint8_t)registers->gprs[Rm]);
  assert(success);

  registers->gprs[Rd] = temp;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SINGLE_DATA_SWAP_