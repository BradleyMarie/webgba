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

struct _Arm7Tdmi {
  unsigned int step_routine_index;
  ArmAllRegisters registers;
  uint16_t reference_count;
};

//
// Interrupt Line
//

static void Arm7TdmiSetLevelRst(void* context, bool raised) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  cpu->step_routine_index &= 7u;
  cpu->step_routine_index |= raised << 3u;
}

static void Arm7TdmiSetLevelFiq(void* context, bool raised) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  cpu->step_routine_index &= 11u;
  cpu->step_routine_index |= raised << 2u;
}

static void Arm7TdmiSetLevelIrq(void* context, bool raised) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  cpu->step_routine_index &= 13u;
  cpu->step_routine_index |= raised << 1u;
}

static void Arm7TdmiInterruptLineFree(void* context) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  Arm7TdmiFree(cpu);
}

//
// Common Functions
//

static inline void Arm7TdmiSetThumbMode(Arm7Tdmi* cpu, bool thumb) {
  cpu->step_routine_index &= 14u;
  cpu->step_routine_index |= thumb;
}

static inline bool Arm7TdmiStepMaybeFiq(Arm7Tdmi* cpu, Memory* memory) {
  if (cpu->registers.current.user.cpsr.fiq_disable) {
    return false;
  }

  ArmExceptionFIQ(&cpu->registers);
  Arm7TdmiSetThumbMode(cpu, false);
  cpu->registers.current.user.gprs.pc += ARM_INSTRUCTION_OFFSET;

  return true;
}

static inline bool Arm7TdmiStepMaybeIrq(Arm7Tdmi* cpu, Memory* memory) {
  if (cpu->registers.current.user.cpsr.irq_disable) {
    return false;
  }

  ArmExceptionIRQ(&cpu->registers);
  Arm7TdmiSetThumbMode(cpu, false);
  cpu->registers.current.user.gprs.pc += ARM_INSTRUCTION_OFFSET;

  return true;
}

//
// Step Routines
//

static void Arm7TdmiStepArm(Arm7Tdmi* cpu, Memory* memory) {
  cpu->registers.current.user.gprs.pc &= 0xFFFFFFFCu;

  uint32_t next_instruction_addr =
      cpu->registers.current.user.gprs.pc - ARM_INSTRUCTION_OFFSET;
  uint32_t next_instruction;
  bool success = Load32LE(memory, next_instruction_addr, &next_instruction);
  if (!success) {
    ArmExceptionPrefetchABT(&cpu->registers);
    Arm7TdmiSetThumbMode(cpu, false);
    cpu->registers.current.user.gprs.pc += ARM_INSTRUCTION_OFFSET;
    return;
  }

  bool modified_pc =
      ArmInstructionExecute(next_instruction, &cpu->registers, memory);
  bool thumb = cpu->registers.current.user.cpsr.thumb;

  static const uint32_t pc_offset[2u][2u] = {
      {ARM_INSTRUCTION_SIZE, THUMB_INSTRUCTION_SIZE},
      {ARM_INSTRUCTION_OFFSET, THUMB_INSTRUCTION_OFFSET}};
  cpu->registers.current.user.gprs.pc += pc_offset[modified_pc][thumb];

  Arm7TdmiSetThumbMode(cpu, thumb);
}

static void Arm7TdmiStepThumb(Arm7Tdmi* cpu, Memory* memory) {
  cpu->registers.current.user.gprs.pc &= 0xFFFFFFFEu;

  uint32_t next_instruction_addr =
      cpu->registers.current.user.gprs.pc - THUMB_INSTRUCTION_OFFSET;
  uint16_t next_instruction;
  bool success = Load16LE(memory, next_instruction_addr, &next_instruction);
  if (!success) {
    ArmExceptionPrefetchABT(&cpu->registers);
    Arm7TdmiSetThumbMode(cpu, false);
    cpu->registers.current.user.gprs.pc += ARM_INSTRUCTION_OFFSET;
    return;
  }

  bool modified_pc =
      ThumbInstructionExecute(next_instruction, &cpu->registers, memory);
  bool thumb = cpu->registers.current.user.cpsr.thumb;

  static const uint32_t pc_offset[2u][2u] = {
      {ARM_INSTRUCTION_SIZE, THUMB_INSTRUCTION_SIZE},
      {ARM_INSTRUCTION_OFFSET, THUMB_INSTRUCTION_OFFSET}};
  cpu->registers.current.user.gprs.pc += pc_offset[modified_pc][thumb];

  Arm7TdmiSetThumbMode(cpu, thumb);
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
  Arm7TdmiSetThumbMode(cpu, false);
  cpu->registers.current.user.gprs.pc += ARM_INSTRUCTION_OFFSET;
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
  static const void (*step_routines[16u])(Arm7Tdmi*, Memory*) = {
      Arm7TdmiStepArm,       Arm7TdmiStepThumb,       Arm7TdmiStepIrqArm,
      Arm7TdmiStepIrqThumb,  Arm7TdmiStepFiqArm,      Arm7TdmiStepFiqThumb,
      Arm7TdmiStepFiqIrqArm, Arm7TdmiStepFiqIrqThumb, Arm7TdmiStepRst,
      Arm7TdmiStepRst,       Arm7TdmiStepRst,         Arm7TdmiStepRst,
      Arm7TdmiStepRst,       Arm7TdmiStepRst,         Arm7TdmiStepRst,
      Arm7TdmiStepRst};
  assert(cpu->step_routine_index < 16u);

  step_routines[cpu->step_routine_index](cpu, memory);
}

void Arm7TdmiFree(Arm7Tdmi* cpu) {
  assert(cpu->reference_count != 0u);
  cpu->reference_count -= 1u;
  if (cpu->reference_count == 0u) {
    free(cpu);
  }
}