#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_CONDITION_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_CONDITION_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

bool ArmInstructionShouldExecute(ArmProgramStatusRegister cpsr,
                                 uint32_t instruction);

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_CONDITION_