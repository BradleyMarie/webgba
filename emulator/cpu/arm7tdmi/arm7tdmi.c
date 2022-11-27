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
  cpu->registers.execution_control.fiq =
      raised && !cpu->registers.current.user.cpsr.fiq_disable;
  cpu->registers.execution_control.fiq_raised = raised;
}

static void Arm7TdmiSetLevelIrq(void* context, bool raised) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  cpu->registers.execution_control.irq =
      raised && !cpu->registers.current.user.cpsr.irq_disable;
  cpu->registers.execution_control.irq_raised = raised;
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
  assert(cycles_executed < cpu->cycles_to_run);

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
  assert(cycles_executed < cpu->cycles_to_run);

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
  } while (cpu->registers.execution_control.mode == 1u &&
           cycles_executed < cpu->cycles_to_run);

  return cycles_executed;
}

static uint32_t Arm7TdmiInterrupt(Arm7Tdmi* cpu, Memory* memory,
                                  uint32_t cycles_executed) {
  assert(cpu->registers.execution_control.rst ||
         cpu->registers.execution_control.fiq ||
         cpu->registers.execution_control.irq);
  assert(cycles_executed < cpu->cycles_to_run);

  if (cpu->registers.execution_control.rst) {
    ArmExceptionRST(&cpu->registers);
    assert(cpu->registers.execution_control.rst);
  } else if (cpu->registers.execution_control.fiq) {
    ArmExceptionFIQ(&cpu->registers);
    assert(!cpu->registers.execution_control.rst);
  } else if (cpu->registers.execution_control.irq) {
    ArmExceptionIRQ(&cpu->registers);
    assert(!cpu->registers.execution_control.rst);
  }

  assert(!cpu->registers.execution_control.irq);
  assert(!cpu->registers.execution_control.fiq);
  assert(!cpu->registers.execution_control.thumb);

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
    return false;
  }

  *fiq = InterruptLineAllocate(*cpu, Arm7TdmiSetLevelFiq,
                               Arm7TdmiInterruptLineFree);
  if (*rst == NULL) {
    InterruptLineFree(*rst);
    free(*cpu);
    return false;
  }

  *irq = InterruptLineAllocate(*cpu, Arm7TdmiSetLevelIrq,
                               Arm7TdmiInterruptLineFree);
  if (*irq == NULL) {
    InterruptLineFree(*fiq);
    InterruptLineFree(*rst);
    free(*cpu);
    return false;
  }

  return true;
}

uint32_t Arm7TdmiStep(Arm7Tdmi* cpu, Memory* memory, uint32_t num_cycles) {
  cpu->cycles_to_run = num_cycles;

  uint32_t cycles_executed = 0u;
  while (cycles_executed < cpu->cycles_to_run) {
    if (cpu->registers.execution_control.mode == 1u) {
      cycles_executed = Arm7TdmiStepThumb(cpu, memory, cycles_executed);
    } else if (cpu->registers.execution_control.mode == 0u) {
      cycles_executed = Arm7TdmiStepArm(cpu, memory, cycles_executed);
    } else {
      cycles_executed = Arm7TdmiInterrupt(cpu, memory, cycles_executed);
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