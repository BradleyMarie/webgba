#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

#include <assert.h>
#include <stdlib.h>

#include "emulator/cpu/arm7tdmi/decoders/arm/execute.h"
#include "emulator/cpu/arm7tdmi/decoders/thumb/execute.h"
#include "emulator/cpu/arm7tdmi/exceptions.h"
#include "emulator/cpu/arm7tdmi/registers.h"

struct _Arm7Tdmi {
  ArmAllRegisters registers;
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
// Common Functions
//

static inline bool Arm7TdmiStepMaybeFiq(Arm7Tdmi* cpu, Memory* memory) {
  if (cpu->registers.current.user.cpsr.fiq_disable) {
    return false;
  }

  ArmExceptionFIQ(&cpu->registers);

  return true;
}

static inline bool Arm7TdmiStepMaybeIrq(Arm7Tdmi* cpu, Memory* memory) {
  if (cpu->registers.current.user.cpsr.irq_disable) {
    return false;
  }

  ArmExceptionIRQ(&cpu->registers);

  return true;
}

//
// Step Routines
//

static void Arm7TdmiStepArm(Arm7Tdmi* cpu, Memory* memory) {
  cpu->registers.current.user.gprs.pc += 4u;
  uint32_t next_instruction_addr = cpu->registers.current.user.gprs.pc - 8u;

  uint32_t next_instruction;
  bool success = Load32LE(memory, next_instruction_addr, &next_instruction);
  if (!success) {
    ArmExceptionPrefetchABT(&cpu->registers);
    return;
  }

  ArmInstructionExecute(next_instruction, &cpu->registers, memory);
}

static void Arm7TdmiStepThumb(Arm7Tdmi* cpu, Memory* memory) {
  cpu->registers.current.user.gprs.pc += 2u;
  uint32_t next_instruction_addr = cpu->registers.current.user.gprs.pc - 4u;

  uint16_t next_instruction;
  bool success = Load16LE(memory, next_instruction_addr, &next_instruction);
  if (!success) {
    ArmExceptionPrefetchABT(&cpu->registers);
    return;
  }

  ThumbInstructionExecute(next_instruction, &cpu->registers, memory);
}

static void Arm7TdmiStepIrqArm(Arm7Tdmi* cpu, Memory* memory) {
  if (Arm7TdmiStepMaybeIrq(cpu, memory)) {
    return;
  }

  Arm7TdmiStepArm(cpu, memory);
}

static void Arm7TdmiStepIrqThumb(Arm7Tdmi* cpu, Memory* memory) {
  if (Arm7TdmiStepMaybeIrq(cpu, memory)) {
    return;
  }

  Arm7TdmiStepThumb(cpu, memory);
}

static void Arm7TdmiStepFiqArm(Arm7Tdmi* cpu, Memory* memory) {
  if (Arm7TdmiStepMaybeFiq(cpu, memory)) {
    return;
  }

  Arm7TdmiStepArm(cpu, memory);
}

static void Arm7TdmiStepFiqThumb(Arm7Tdmi* cpu, Memory* memory) {
  if (Arm7TdmiStepMaybeFiq(cpu, memory)) {
    return;
  }

  Arm7TdmiStepThumb(cpu, memory);
}

static void Arm7TdmiStepFiqIrqArm(Arm7Tdmi* cpu, Memory* memory) {
  if (Arm7TdmiStepMaybeFiq(cpu, memory)) {
    return;
  }

  if (Arm7TdmiStepMaybeIrq(cpu, memory)) {
    return;
  }

  Arm7TdmiStepArm(cpu, memory);
}

static void Arm7TdmiStepFiqIrqThumb(Arm7Tdmi* cpu, Memory* memory) {
  if (Arm7TdmiStepMaybeFiq(cpu, memory)) {
    return;
  }

  if (Arm7TdmiStepMaybeIrq(cpu, memory)) {
    return;
  }

  Arm7TdmiStepThumb(cpu, memory);
}

static void Arm7TdmiStepRst(Arm7Tdmi* cpu, Memory* memory) {
  ArmExceptionRST(&cpu->registers);
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

typedef void (*StepRoutine)(Arm7Tdmi*, Memory*);
void Arm7TdmiStep(Arm7Tdmi* cpu, Memory* memory) {
  static const StepRoutine step_routines[16] = {
      Arm7TdmiStepArm,       Arm7TdmiStepThumb,       Arm7TdmiStepIrqArm,
      Arm7TdmiStepIrqThumb,  Arm7TdmiStepFiqArm,      Arm7TdmiStepFiqThumb,
      Arm7TdmiStepFiqIrqArm, Arm7TdmiStepFiqIrqThumb, Arm7TdmiStepRst,
      Arm7TdmiStepRst,       Arm7TdmiStepRst,         Arm7TdmiStepRst,
      Arm7TdmiStepRst,       Arm7TdmiStepRst,         Arm7TdmiStepRst,
      Arm7TdmiStepRst};
  assert(cpu->registers.execution_control.value < 16);

  step_routines[cpu->registers.execution_control.value](cpu, memory);
}

void Arm7TdmiFree(Arm7Tdmi* cpu) {
  assert(cpu->reference_count != 0);
  cpu->reference_count -= 1u;
  if (cpu->reference_count == 0u) {
    free(cpu);
  }
}