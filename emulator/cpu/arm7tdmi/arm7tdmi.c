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

#define EXECUTION_MODE_THUMB 0u
#define EXECUTION_MODE_ARM 1u
#define EXECUTION_MODE_THUMB_IRQ 2u
#define EXECUTION_MODE_ARM_IRQ 3u
#define EXECUTION_MODE_THUMB_FIQ 4u
#define EXECUTION_MODE_ARM_FIQ 5u
#define EXECUTION_MODE_THUMB_FIQ_IRQ 6u
#define EXECUTION_MODE_ARM_FIQ_IRQ 7u
#define EXECUTION_MODE_THUMB_RST 8u
#define EXECUTION_MODE_ARM_RST 9u
#define EXECUTION_MODE_THUMB_RST_IRQ 10u
#define EXECUTION_MODE_ARM_RST_IRQ 11u
#define EXECUTION_MODE_THUMB_RST_FIQ 12u
#define EXECUTION_MODE_ARM_RST_FIQ 13u
#define EXECUTION_MODE_THUMB_RST_FIQ_IRQ 13u
#define EXECUTION_MODE_ARM_RST_FIQ_IRQ 15u
#define EXECUTION_MODE_COUNT 16u
#define EXCEUTION_MODE_MASK EXECUTION_MODE_ARM_RST_FIQ_IRQ

#define IRQ_BIT_INDEX 1u
#define FIQ_BIT_INDEX 2u
#define RST_BIT_INDEX 3u

typedef struct _ExecutionMode {
  void (*step_routine)(Arm7Tdmi*, Memory*);
  const struct _ExecutionMode* next_execution_mode[2];
  uint32_t next_instruction_offset[2];
  uint32_t next_instruction_mask;
} ExecutionMode;

struct _Arm7Tdmi {
  const ExecutionMode* execution_mode;
  ArmAllRegisters registers;
  uint16_t reference_count;
};

//
// Interrupt Line
//

static const ExecutionMode execution_modes[EXECUTION_MODE_COUNT];

static inline size_t Arm7TdmiGetExecutionModeIndex(const Arm7Tdmi* cpu) {
  return cpu->execution_mode - execution_modes;
}

static void Arm7TdmiSetLevelRst(void* context, bool raised) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  size_t current_mode_index = Arm7TdmiGetExecutionModeIndex(cpu);
  size_t next_mode_index =
      (current_mode_index & (EXCEUTION_MODE_MASK ^ (1u << RST_BIT_INDEX))) |
      (raised << RST_BIT_INDEX);
  assert(next_mode_index < EXECUTION_MODE_COUNT);

  cpu->execution_mode = &execution_modes[next_mode_index];
}

static void Arm7TdmiSetLevelFiq(void* context, bool raised) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  size_t current_mode_index = Arm7TdmiGetExecutionModeIndex(cpu);
  size_t next_mode_index =
      (current_mode_index & (EXCEUTION_MODE_MASK ^ (1u << FIQ_BIT_INDEX))) |
      (raised << FIQ_BIT_INDEX);
  assert(next_mode_index < EXECUTION_MODE_COUNT);

  cpu->execution_mode = &execution_modes[next_mode_index];
}

static void Arm7TdmiSetLevelIrq(void* context, bool raised) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  size_t current_mode_index = Arm7TdmiGetExecutionModeIndex(cpu);
  size_t next_mode_index =
      (current_mode_index & (EXCEUTION_MODE_MASK ^ (1u << IRQ_BIT_INDEX))) |
      (raised << IRQ_BIT_INDEX);
  assert(next_mode_index < EXECUTION_MODE_COUNT);

  cpu->execution_mode = &execution_modes[next_mode_index];
}

static void Arm7TdmiInterruptLineFree(void* context) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  Arm7TdmiFree(cpu);
}

//
// Common Functions
//

static inline void Arm7TdmiStepFinish(Arm7Tdmi* cpu, bool thumb,
                                      bool modified_pc) {
  cpu->execution_mode = cpu->execution_mode->next_execution_mode[thumb];
  cpu->registers.current.user.gprs.pc +=
      cpu->execution_mode->next_instruction_offset[modified_pc];
  cpu->registers.current.user.gprs.pc &=
      cpu->execution_mode->next_instruction_mask;
}

static inline bool Arm7TdmiStepMaybeFiq(Arm7Tdmi* cpu, Memory* memory) {
  if (cpu->registers.current.user.cpsr.fiq_disable) {
    return false;
  }

  ArmExceptionFIQ(&cpu->registers);
  Arm7TdmiStepFinish(cpu, /*thumb=*/false, /*modified_pc=*/true);

  return true;
}

static inline bool Arm7TdmiStepMaybeIrq(Arm7Tdmi* cpu, Memory* memory) {
  if (cpu->registers.current.user.cpsr.irq_disable) {
    return false;
  }

  ArmExceptionIRQ(&cpu->registers);
  Arm7TdmiStepFinish(cpu, /*thumb=*/false, /*modified_pc=*/true);

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
    bool modified_pc =
        ArmInstructionExecute(next_instruction, &cpu->registers, memory);
    Arm7TdmiStepFinish(cpu, /*thumb=*/cpu->registers.current.user.cpsr.thumb,
                       /*modified_pc=*/modified_pc);
  } else {
    ArmExceptionPrefetchABT(&cpu->registers);
    Arm7TdmiStepFinish(cpu, /*thumb=*/false, /*modified_pc=*/true);
  }
}

static void Arm7TdmiStepThumb(Arm7Tdmi* cpu, Memory* memory) {
  uint32_t next_instruction_addr =
      cpu->registers.current.user.gprs.pc - THUMB_INSTRUCTION_OFFSET;
  uint16_t next_instruction;
  bool success = Load16LE(memory, next_instruction_addr, &next_instruction);
  if (success) {
    bool modified_pc =
        ThumbInstructionExecute(next_instruction, &cpu->registers, memory);
    Arm7TdmiStepFinish(cpu, /*thumb=*/cpu->registers.current.user.cpsr.thumb,
                       /*modified_pc=*/modified_pc);
  } else {
    ArmExceptionPrefetchABT(&cpu->registers);
    Arm7TdmiStepFinish(cpu, /*thumb=*/false, /*modified_pc=*/true);
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
  Arm7TdmiStepFinish(cpu, /*thumb=*/false, /*modified_pc=*/true);
}

//
// Static Data
//

static const ExecutionMode execution_modes[16] = {
    {Arm7TdmiStepThumb,
     {&execution_modes[EXECUTION_MODE_ARM],
      &execution_modes[EXECUTION_MODE_THUMB]},
     {THUMB_INSTRUCTION_SIZE, THUMB_INSTRUCTION_OFFSET},
     0xFFFFFFFEu},
    {Arm7TdmiStepArm,
     {&execution_modes[EXECUTION_MODE_ARM],
      &execution_modes[EXECUTION_MODE_THUMB]},
     {ARM_INSTRUCTION_SIZE, ARM_INSTRUCTION_OFFSET},
     0xFFFFFFFCu},
    {Arm7TdmiStepIrqThumb,
     {&execution_modes[EXECUTION_MODE_ARM_IRQ],
      &execution_modes[EXECUTION_MODE_THUMB_IRQ]},
     {THUMB_INSTRUCTION_SIZE, THUMB_INSTRUCTION_OFFSET},
     0xFFFFFFFEu},
    {Arm7TdmiStepIrqArm,
     {&execution_modes[EXECUTION_MODE_ARM_IRQ],
      &execution_modes[EXECUTION_MODE_THUMB_IRQ]},
     {ARM_INSTRUCTION_SIZE, ARM_INSTRUCTION_OFFSET},
     0xFFFFFFFCu},
    {Arm7TdmiStepFiqThumb,
     {&execution_modes[EXECUTION_MODE_ARM_FIQ],
      &execution_modes[EXECUTION_MODE_THUMB_FIQ]},
     {THUMB_INSTRUCTION_SIZE, THUMB_INSTRUCTION_OFFSET},
     0xFFFFFFFEu},
    {Arm7TdmiStepFiqArm,
     {&execution_modes[EXECUTION_MODE_ARM_FIQ],
      &execution_modes[EXECUTION_MODE_THUMB_FIQ]},
     {ARM_INSTRUCTION_SIZE, ARM_INSTRUCTION_OFFSET},
     0xFFFFFFFCu},
    {Arm7TdmiStepFiqIrqThumb,
     {&execution_modes[EXECUTION_MODE_ARM_FIQ_IRQ],
      &execution_modes[EXECUTION_MODE_THUMB_FIQ_IRQ]},
     {THUMB_INSTRUCTION_SIZE, THUMB_INSTRUCTION_OFFSET},
     0xFFFFFFFEu},
    {Arm7TdmiStepFiqIrqArm,
     {&execution_modes[EXECUTION_MODE_ARM_FIQ_IRQ],
      &execution_modes[EXECUTION_MODE_THUMB_FIQ_IRQ]},
     {ARM_INSTRUCTION_SIZE, ARM_INSTRUCTION_OFFSET},
     0xFFFFFFFCu},
    {Arm7TdmiStepRst,
     {&execution_modes[EXECUTION_MODE_ARM_RST],
      &execution_modes[EXECUTION_MODE_THUMB_RST]},
     {THUMB_INSTRUCTION_SIZE, THUMB_INSTRUCTION_OFFSET},
     0xFFFFFFFEu},
    {Arm7TdmiStepRst,
     {&execution_modes[EXECUTION_MODE_ARM_RST],
      &execution_modes[EXECUTION_MODE_THUMB_RST]},
     {ARM_INSTRUCTION_SIZE, ARM_INSTRUCTION_OFFSET},
     0xFFFFFFFCu},
    {Arm7TdmiStepRst,
     {&execution_modes[EXECUTION_MODE_ARM_RST_IRQ],
      &execution_modes[EXECUTION_MODE_THUMB_RST_IRQ]},
     {THUMB_INSTRUCTION_SIZE, THUMB_INSTRUCTION_OFFSET},
     0xFFFFFFFEu},
    {Arm7TdmiStepRst,
     {&execution_modes[EXECUTION_MODE_ARM_RST_IRQ],
      &execution_modes[EXECUTION_MODE_THUMB_RST_IRQ]},
     {ARM_INSTRUCTION_SIZE, ARM_INSTRUCTION_OFFSET},
     0xFFFFFFFCu},
    {Arm7TdmiStepRst,
     {&execution_modes[EXECUTION_MODE_ARM_RST_FIQ],
      &execution_modes[EXECUTION_MODE_THUMB_RST_FIQ]},
     {THUMB_INSTRUCTION_SIZE, THUMB_INSTRUCTION_OFFSET},
     0xFFFFFFFEu},
    {Arm7TdmiStepRst,
     {&execution_modes[EXECUTION_MODE_ARM_RST_FIQ],
      &execution_modes[EXECUTION_MODE_THUMB_RST_FIQ]},
     {ARM_INSTRUCTION_SIZE, ARM_INSTRUCTION_OFFSET},
     0xFFFFFFFCu},
    {Arm7TdmiStepRst,
     {&execution_modes[EXECUTION_MODE_ARM_RST_FIQ_IRQ],
      &execution_modes[EXECUTION_MODE_THUMB_RST_FIQ_IRQ]},
     {THUMB_INSTRUCTION_SIZE, THUMB_INSTRUCTION_OFFSET},
     0xFFFFFFFEu},
    {Arm7TdmiStepRst,
     {&execution_modes[EXECUTION_MODE_ARM_RST_FIQ_IRQ],
      &execution_modes[EXECUTION_MODE_THUMB_RST_FIQ_IRQ]},
     {ARM_INSTRUCTION_SIZE, ARM_INSTRUCTION_OFFSET},
     0xFFFFFFFCu}};

//
// Public Functions
//

bool Arm7TdmiAllocate(Arm7Tdmi** cpu, InterruptLine** rst, InterruptLine** fiq,
                      InterruptLine** irq) {
  *cpu = (Arm7Tdmi*)calloc(1, sizeof(Arm7Tdmi));
  if (cpu == NULL) {
    return false;
  }

  (*cpu)->execution_mode = &execution_modes[EXECUTION_MODE_ARM];
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
  cpu->execution_mode->step_routine(cpu, memory);
}

void Arm7TdmiFree(Arm7Tdmi* cpu) {
  assert(cpu->reference_count != 0);
  cpu->reference_count -= 1u;
  if (cpu->reference_count == 0u) {
    free(cpu);
  }
}