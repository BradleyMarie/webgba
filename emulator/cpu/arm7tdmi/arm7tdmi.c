#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

#include <assert.h>
#include <stdlib.h>

#include "emulator/cpu/arm7tdmi/decoders/arm/execute.h"
#include "emulator/cpu/arm7tdmi/decoders/thumb/execute.h"
#include "emulator/cpu/arm7tdmi/exceptions.h"
#include "emulator/cpu/arm7tdmi/registers.h"
#include "util/macros.h"

struct _Arm7Tdmi {
  ArmAllRegisters registers;
  uint32_t cycles_to_run;
  uint16_t reference_count;
};

//
// Interrupt Line
//

static void Arm7TdmiSetLevelRst(void* context, bool raised) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  cpu->registers.execution_control.rst = raised;
}

static void Arm7TdmiSetLevelFiq(void* context, bool raised) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  cpu->registers.execution_control.fiq = raised;
}

static void Arm7TdmiSetLevelIrq(void* context, bool raised) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  cpu->registers.execution_control.irq = raised;
}

static void Arm7TdmiInterruptLineFree(void* context) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  Arm7TdmiFree(cpu);
}

//
// Step Routines
//

static uint32_t Arm7TdmiStepArm(Arm7Tdmi* cpu, Memory* memory,
                                uint32_t cycles_executed) {
  assert(cycles_executed < cpu->cycles_to_run ||
         cycles_executed == UINT32_MAX - 1u);

  do {
    codegen_assert(!cpu->registers.current.user.cpsr.thumb);
    cycles_executed += 1u;

    uint32_t next_instruction_32;
    bool success = Load32LE(memory, ArmCurrentInstruction(&cpu->registers),
                            &next_instruction_32);
    if (!success) {
      ArmExceptionPrefetchABT(&cpu->registers);
    } else {
      ArmInstructionExecute(next_instruction_32, &cpu->registers, memory);
    }
  } while (cpu->registers.execution_control.mode == 0u &&
           cycles_executed < cpu->cycles_to_run);

  return cycles_executed;
}

static uint32_t Arm7TdmiStepThumb(Arm7Tdmi* cpu, Memory* memory,
                                  uint32_t cycles_executed) {
  assert(cycles_executed < cpu->cycles_to_run ||
         cycles_executed == UINT32_MAX - 1u);

  do {
    codegen_assert(cpu->registers.current.user.cpsr.thumb);
    cycles_executed += 1u;

    uint16_t next_instruction_16;
    bool success = Load16LE(memory, ArmCurrentInstruction(&cpu->registers),
                            &next_instruction_16);
    if (!success) {
      ArmExceptionPrefetchABT(&cpu->registers);
      break;
    }

    ThumbInstructionExecute(next_instruction_16, &cpu->registers, memory);
  } while (cpu->registers.execution_control.mode != 0u &&
           cycles_executed < cpu->cycles_to_run);

  return cycles_executed;
}

static bool Arm7TdmiStepMaybeFiq(Arm7Tdmi* cpu, Memory* memory) {
  assert(cpu->registers.execution_control.fiq);

  if (cpu->registers.current.user.cpsr.fiq_disable) {
    return false;
  }

  ArmExceptionFIQ(&cpu->registers);

  return true;
}

static bool Arm7TdmiStepMaybeIrq(Arm7Tdmi* cpu, Memory* memory) {
  assert(cpu->registers.execution_control.irq);

  if (cpu->registers.current.user.cpsr.irq_disable) {
    return false;
  }

  ArmExceptionIRQ(&cpu->registers);

  return true;
}

static uint32_t Arm7TdmiStepAny(Arm7Tdmi* cpu, Memory* memory,
                                uint32_t cycles_executed) {
  switch (cpu->registers.execution_control.mode) {
    case ARM_EXECUTION_MODE_NORST_FIQ_NOIRQ_ARM:
      if (Arm7TdmiStepMaybeFiq(cpu, memory)) {
        break;
      }
      goto arm_execute;
    case ARM_EXECUTION_MODE_NORST_FIQ_IRQ_ARM:
      if (Arm7TdmiStepMaybeFiq(cpu, memory)) {
        break;
      }
    case ARM_EXECUTION_MODE_NORST_NOFIQ_IRQ_ARM:
      if (Arm7TdmiStepMaybeIrq(cpu, memory)) {
        break;
      }
    arm_execute:
    case ARM_EXECUTION_MODE_NORST_NOFIQ_NOIRQ_ARM:
      Arm7TdmiStepArm(cpu, memory, UINT32_MAX - 1u);
      break;
    case ARM_EXECUTION_MODE_NORST_FIQ_NOIRQ_THUMB:
      if (Arm7TdmiStepMaybeFiq(cpu, memory)) {
        break;
      }
      goto thumb_execute;
    case ARM_EXECUTION_MODE_NORST_FIQ_IRQ_THUMB:
      if (Arm7TdmiStepMaybeFiq(cpu, memory)) {
        break;
      }
    case ARM_EXECUTION_MODE_NORST_NOFIQ_IRQ_THUMB:
      if (Arm7TdmiStepMaybeIrq(cpu, memory)) {
        break;
      }
    thumb_execute:
    case ARM_EXECUTION_MODE_NORST_NOFIQ_NOIRQ_THUMB:
      Arm7TdmiStepThumb(cpu, memory, UINT32_MAX - 1u);
      break;
    case ARM_EXECUTION_MODE_RST_NOFIQ_NOIRQ_ARM:
    case ARM_EXECUTION_MODE_RST_NOFIQ_NOIRQ_THUMB:
    case ARM_EXECUTION_MODE_RST_NOFIQ_IRQ_ARM:
    case ARM_EXECUTION_MODE_RST_NOFIQ_IRQ_THUMB:
    case ARM_EXECUTION_MODE_RST_FIQ_NOIRQ_ARM:
    case ARM_EXECUTION_MODE_RST_FIQ_NOIRQ_THUMB:
    case ARM_EXECUTION_MODE_RST_FIQ_IRQ_ARM:
    case ARM_EXECUTION_MODE_RST_FIQ_IRQ_THUMB:
      ArmExceptionRST(&cpu->registers);
      break;
    default:
      codegen_assert(false);
  }

  return cycles_executed + 1u;
}

//
// Public Functions
//

bool Arm7TdmiAllocate(Arm7Tdmi** cpu, InterruptLine** rst, InterruptLine** fiq,
                      InterruptLine** irq) {
  *cpu = (Arm7Tdmi*)calloc(1, sizeof(Arm7Tdmi));
  if (cpu == NULL) {
    return false;
  }

  ArmLoadProgramCounter(&(*cpu)->registers, 0x0u);
  (*cpu)->registers.current.user.cpsr.mode = MODE_SVC;
  (*cpu)->reference_count = 4u;

  *rst = InterruptLineAllocate(*cpu, Arm7TdmiSetLevelRst,
                               Arm7TdmiInterruptLineFree);
  if (*rst == NULL) {
    free(*cpu);
  }

  *fiq = InterruptLineAllocate(*cpu, Arm7TdmiSetLevelFiq,
                               Arm7TdmiInterruptLineFree);
  if (*rst == NULL) {
    InterruptLineFree(*rst);
    free(*cpu);
  }

  *irq = InterruptLineAllocate(*cpu, Arm7TdmiSetLevelIrq,
                               Arm7TdmiInterruptLineFree);
  if (*irq == NULL) {
    InterruptLineFree(*fiq);
    InterruptLineFree(*rst);
    free(*cpu);
  }

  return cpu;
}

uint32_t Arm7TdmiStep(Arm7Tdmi* cpu, Memory* memory, uint32_t num_cycles) {
  cpu->cycles_to_run = num_cycles;

  uint32_t cycles_executed = 0u;
  while (cpu->registers.execution_control.mode & 0xEu) {
    cycles_executed = Arm7TdmiStepAny(cpu, memory, cycles_executed);
    if (cpu->cycles_to_run <= cycles_executed) {
      return cycles_executed;
    }
  }

  if (cpu->registers.execution_control.mode != 0u) {
    assert(cpu->registers.execution_control.mode == 1u);
    for (; cycles_executed < cpu->cycles_to_run;) {
      cycles_executed = Arm7TdmiStepThumb(cpu, memory, cycles_executed);
      if (cpu->cycles_to_run <= cycles_executed) {
        return cycles_executed;
      }
      cycles_executed = Arm7TdmiStepArm(cpu, memory, cycles_executed);
    }
  } else {
    for (; cycles_executed < cpu->cycles_to_run;) {
      cycles_executed = Arm7TdmiStepArm(cpu, memory, cycles_executed);
      if (cpu->cycles_to_run <= cycles_executed) {
        return cycles_executed;
      }
      cycles_executed = Arm7TdmiStepThumb(cpu, memory, cycles_executed);
    }
  }

  return cycles_executed;
}

void Arm7TdmiHalt(Arm7Tdmi* cpu) { cpu->cycles_to_run = 0u; }

void Arm7TdmiFree(Arm7Tdmi* cpu) {
  assert(cpu->reference_count != 0);
  cpu->reference_count -= 1u;
  if (cpu->reference_count == 0u) {
    free(cpu);
  }
}