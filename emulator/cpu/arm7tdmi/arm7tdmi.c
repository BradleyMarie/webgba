#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

#include <assert.h>
#include <stdlib.h>

#include "emulator/cpu/arm7tdmi/decoders/arm/execute.h"
#include "emulator/cpu/arm7tdmi/decoders/thumb/execute.h"
#include "emulator/cpu/arm7tdmi/exceptions.h"
#include "emulator/cpu/arm7tdmi/registers.h"

#define ARM_INSTRUCTION_OFFSET 8u
#define ARM_INSTRUCTION_SIZE 4u
#define THUMB_INSTRUCTION_OFFSET 4u
#define THUMB_INSTRUCTION_SIZE 2u

typedef struct _ExecutionMode {
  void (*step_routine)(Arm7Tdmi*, Memory*);
  uint32_t next_instruction_offset[2];
  uint32_t next_instruction_mask;
} ExecutionMode;

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

#define EXECUTION_MODE_COUNT 16u
static const ExecutionMode execution_modes[EXECUTION_MODE_COUNT];

static inline void Arm7TdmiStepFinish(Arm7Tdmi* cpu, bool thumb,
                                      bool pc_unmodified) {
  assert(cpu->registers.execution_control.value < EXECUTION_MODE_COUNT);
  cpu->registers.current.user.gprs.pc +=
      execution_modes[cpu->registers.execution_control.value]
          .next_instruction_offset[pc_unmodified];
  cpu->registers.current.user.gprs.pc &=
      execution_modes[cpu->registers.execution_control.value]
          .next_instruction_mask;
}

static inline bool Arm7TdmiStepMaybeFiq(Arm7Tdmi* cpu, Memory* memory) {
  if (cpu->registers.current.user.cpsr.fiq_disable) {
    return false;
  }

  ArmExceptionFIQ(&cpu->registers);
  Arm7TdmiStepFinish(cpu, /*thumb=*/false, /*pc_unmodified=*/false);

  return true;
}

static inline bool Arm7TdmiStepMaybeIrq(Arm7Tdmi* cpu, Memory* memory) {
  if (cpu->registers.current.user.cpsr.irq_disable) {
    return false;
  }

  ArmExceptionIRQ(&cpu->registers);
  Arm7TdmiStepFinish(cpu, /*thumb=*/false, /*pc_unmodified=*/false);

  return true;
}

//
// Step Routines
//

static void Arm7TdmiStepArm(Arm7Tdmi* cpu, Memory* memory) {
  uint32_t next_instruction_addr =
      cpu->registers.current.user.gprs.pc - ARM_INSTRUCTION_OFFSET;
  uint32_t next_instruction;
  bool success = Load32LE(memory, next_instruction_addr, &next_instruction);
  if (success) {
    bool pc_unmodified =
        ArmInstructionExecute(next_instruction, &cpu->registers, memory);
    Arm7TdmiStepFinish(cpu, /*thumb=*/cpu->registers.current.user.cpsr.thumb,
                       /*pc_unmodified=*/pc_unmodified);
  } else {
    ArmExceptionPrefetchABT(&cpu->registers);
    Arm7TdmiStepFinish(cpu, /*thumb=*/false, /*pc_unmodified=*/false);
  }
}

static void Arm7TdmiStepThumb(Arm7Tdmi* cpu, Memory* memory) {
  uint32_t next_instruction_addr =
      cpu->registers.current.user.gprs.pc - THUMB_INSTRUCTION_OFFSET;
  uint16_t next_instruction;
  bool success = Load16LE(memory, next_instruction_addr, &next_instruction);
  if (success) {
    bool pc_unmodified =
        ThumbInstructionExecute(next_instruction, &cpu->registers, memory);
    Arm7TdmiStepFinish(cpu, /*thumb=*/cpu->registers.current.user.cpsr.thumb,
                       /*pc_unmodified=*/pc_unmodified);
  } else {
    ArmExceptionPrefetchABT(&cpu->registers);
    Arm7TdmiStepFinish(cpu, /*thumb=*/false, /*pc_unmodified=*/false);
  }
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
  Arm7TdmiStepFinish(cpu, /*thumb=*/false, /*pc_unmodified=*/false);
}

//
// Static Data
//

#define EXECUTION_MODE_COUNT 16u
static const ExecutionMode execution_modes[EXECUTION_MODE_COUNT] = {
    {Arm7TdmiStepArm,
     {ARM_INSTRUCTION_OFFSET, ARM_INSTRUCTION_SIZE},
     0xFFFFFFFCu},
    {Arm7TdmiStepThumb,
     {THUMB_INSTRUCTION_OFFSET, THUMB_INSTRUCTION_SIZE},
     0xFFFFFFFEu},
    {Arm7TdmiStepIrqArm,
     {ARM_INSTRUCTION_OFFSET, ARM_INSTRUCTION_SIZE},
     0xFFFFFFFCu},
    {Arm7TdmiStepIrqThumb,
     {THUMB_INSTRUCTION_OFFSET, THUMB_INSTRUCTION_SIZE},
     0xFFFFFFFEu},
    {Arm7TdmiStepFiqArm,
     {ARM_INSTRUCTION_OFFSET, ARM_INSTRUCTION_SIZE},
     0xFFFFFFFCu},
    {Arm7TdmiStepFiqThumb,
     {THUMB_INSTRUCTION_OFFSET, THUMB_INSTRUCTION_SIZE},
     0xFFFFFFFEu},
    {Arm7TdmiStepFiqIrqArm,
     {ARM_INSTRUCTION_OFFSET, ARM_INSTRUCTION_SIZE},
     0xFFFFFFFCu},
    {Arm7TdmiStepFiqIrqThumb,
     {THUMB_INSTRUCTION_OFFSET, THUMB_INSTRUCTION_SIZE},
     0xFFFFFFFEu},
    {Arm7TdmiStepRst,
     {ARM_INSTRUCTION_OFFSET, ARM_INSTRUCTION_SIZE},
     0xFFFFFFFCu},
    {Arm7TdmiStepRst,
     {THUMB_INSTRUCTION_OFFSET, THUMB_INSTRUCTION_SIZE},
     0xFFFFFFFEu},
    {Arm7TdmiStepRst,
     {ARM_INSTRUCTION_OFFSET, ARM_INSTRUCTION_SIZE},
     0xFFFFFFFCu},
    {Arm7TdmiStepRst,
     {THUMB_INSTRUCTION_OFFSET, THUMB_INSTRUCTION_SIZE},
     0xFFFFFFFEu},
    {Arm7TdmiStepRst,
     {ARM_INSTRUCTION_OFFSET, ARM_INSTRUCTION_SIZE},
     0xFFFFFFFCu},
    {Arm7TdmiStepRst,
     {THUMB_INSTRUCTION_OFFSET, THUMB_INSTRUCTION_SIZE},
     0xFFFFFFFEu},
    {Arm7TdmiStepRst,
     {ARM_INSTRUCTION_OFFSET, ARM_INSTRUCTION_SIZE},
     0xFFFFFFFCu},
    {Arm7TdmiStepRst,
     {THUMB_INSTRUCTION_OFFSET, THUMB_INSTRUCTION_SIZE},
     0xFFFFFFFEu}};

//
// Public Functions
//

bool Arm7TdmiAllocate(Arm7Tdmi** cpu, InterruptLine** rst, InterruptLine** fiq,
                      InterruptLine** irq) {
  *cpu = (Arm7Tdmi*)calloc(1, sizeof(Arm7Tdmi));
  if (cpu == NULL) {
    return false;
  }

  (*cpu)->registers.current.user.cpsr.mode = MODE_SVC;
  (*cpu)->registers.current.user.gprs.pc = 0x8u;
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

void Arm7TdmiStep(Arm7Tdmi* cpu, Memory* memory) {
  assert(cpu->registers.execution_control.value < EXECUTION_MODE_COUNT);
  execution_modes[cpu->registers.execution_control.value].step_routine(cpu,
                                                                       memory);
}

void Arm7TdmiFree(Arm7Tdmi* cpu) {
  assert(cpu->reference_count != 0);
  cpu->reference_count -= 1u;
  if (cpu->reference_count == 0u) {
    free(cpu);
  }
}