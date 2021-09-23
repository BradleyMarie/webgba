#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

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
  unsigned int interrupts_raised;
  ArmAllRegisters registers;
  uint16_t reference_count;
};

static void Arm7TdmiSetLevelRst(void* context, bool raised) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  cpu->interrupts_raised &= 3u;
  cpu->interrupts_raised |= raised << 2u;
}

static void Arm7TdmiSetLevelFiq(void* context, bool raised) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  cpu->interrupts_raised &= 5u;
  cpu->interrupts_raised |= raised << 1u;
}

static void Arm7TdmiSetLevelIrq(void* context, bool raised) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  cpu->interrupts_raised &= 6u;
  cpu->interrupts_raised |= raised;
}

static void Arm7TdmiInterruptLineFree(void* context) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)context;
  Arm7TdmiFree(cpu);
}

static void Arm7TdmiStepRst(Arm7Tdmi* cpu, Memory* memory) {
  ArmExceptionRST(&cpu->registers);
  cpu->registers.current.user.gprs.pc += ARM_INSTRUCTION_OFFSET;
}

static inline bool Arm7TdmiStepMaybeFiq(Arm7Tdmi* cpu, Memory* memory) {
  if (cpu->registers.current.user.cpsr.fiq_disable) {
    return false;
  }

  ArmExceptionFIQ(&cpu->registers);
  cpu->registers.current.user.gprs.pc += ARM_INSTRUCTION_OFFSET;

  return true;
}

static inline bool Arm7TdmiStepMaybeIrq(Arm7Tdmi* cpu, Memory* memory) {
  if (cpu->registers.current.user.cpsr.irq_disable) {
    return false;
  }

  ArmExceptionIRQ(&cpu->registers);
  cpu->registers.current.user.gprs.pc += ARM_INSTRUCTION_OFFSET;

  return true;
}

static inline bool Arm7TdmiStepMaybeThumbExecute(Arm7Tdmi* cpu,
                                                 Memory* memory) {
  if (!cpu->registers.current.user.cpsr.thumb) {
    return false;
  }

  cpu->registers.current.user.gprs.pc &= 0xFFFFFFFEu;

  uint32_t next_instruction_addr =
      cpu->registers.current.user.gprs.pc - THUMB_INSTRUCTION_OFFSET;
  uint16_t next_instruction;
  bool success = Load16LE(memory, next_instruction_addr, &next_instruction);
  if (!success) {
    ArmExceptionPrefetchABT(&cpu->registers);
    cpu->registers.current.user.gprs.pc += ARM_INSTRUCTION_OFFSET;
    return true;
  }

  bool modified_pc =
      ThumbInstructionExecute(next_instruction, &cpu->registers, memory);

  cpu->registers.current.user.gprs.pc += THUMB_INSTRUCTION_SIZE;

  if (modified_pc) {
    static const uint32_t pc_offset[2u] = {
        ARM_INSTRUCTION_OFFSET - THUMB_INSTRUCTION_SIZE,
        THUMB_INSTRUCTION_OFFSET - THUMB_INSTRUCTION_SIZE};
    cpu->registers.current.user.gprs.pc +=
        pc_offset[cpu->registers.current.user.cpsr.thumb];
  }

  return true;
}

static inline void Arm7TdmiStepArmExecute(Arm7Tdmi* cpu, Memory* memory) {
  cpu->registers.current.user.gprs.pc &= 0xFFFFFFFCu;

  uint32_t next_instruction_addr =
      cpu->registers.current.user.gprs.pc - ARM_INSTRUCTION_OFFSET;
  uint32_t next_instruction;
  bool success = Load32LE(memory, next_instruction_addr, &next_instruction);
  if (!success) {
    ArmExceptionPrefetchABT(&cpu->registers);
    cpu->registers.current.user.gprs.pc += ARM_INSTRUCTION_OFFSET;
    return;
  }

  bool modified_pc =
      ArmInstructionExecute(next_instruction, &cpu->registers, memory);

  cpu->registers.current.user.gprs.pc += ARM_INSTRUCTION_SIZE;

  if (modified_pc) {
    static const uint32_t pc_offset[2u] = {
        ARM_INSTRUCTION_OFFSET - ARM_INSTRUCTION_SIZE,
        THUMB_INSTRUCTION_OFFSET - ARM_INSTRUCTION_SIZE};
    cpu->registers.current.user.gprs.pc +=
        pc_offset[cpu->registers.current.user.cpsr.thumb];
  }
}

static inline void Arm7TdmiStepExecute(Arm7Tdmi* cpu, Memory* memory) {
  if (Arm7TdmiStepMaybeThumbExecute(cpu, memory)) {
    return;
  }

  Arm7TdmiStepArmExecute(cpu, memory);
}

static void Arm7TdmiStepFiqIrq(Arm7Tdmi* cpu, Memory* memory) {
  if (Arm7TdmiStepMaybeFiq(cpu, memory)) {
    return;
  }

  if (Arm7TdmiStepMaybeIrq(cpu, memory)) {
    return;
  }

  Arm7TdmiStepExecute(cpu, memory);
}

static void Arm7TdmiStepFiq(Arm7Tdmi* cpu, Memory* memory) {
  if (Arm7TdmiStepMaybeFiq(cpu, memory)) {
    return;
  }

  Arm7TdmiStepExecute(cpu, memory);
}

static void Arm7TdmiStepIrq(Arm7Tdmi* cpu, Memory* memory) {
  if (Arm7TdmiStepMaybeIrq(cpu, memory)) {
    return;
  }

  Arm7TdmiStepExecute(cpu, memory);
}

static void Arm7TdmiStepClear(Arm7Tdmi* cpu, Memory* memory) {
  Arm7TdmiStepExecute(cpu, memory);
}

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
  static const void (*step_routines[8u])(Arm7Tdmi*, Memory*) = {
      Arm7TdmiStepClear, Arm7TdmiStepIrq, Arm7TdmiStepFiq, Arm7TdmiStepFiqIrq,
      Arm7TdmiStepRst,   Arm7TdmiStepRst, Arm7TdmiStepRst, Arm7TdmiStepRst};
  assert(cpu->interrupts_raised < 8u);

  step_routines[cpu->interrupts_raised](cpu, memory);
}

void Arm7TdmiFree(Arm7Tdmi* cpu) {
  assert(cpu->reference_count != 0u);
  cpu->reference_count -= 1u;
  if (cpu->reference_count == 0u) {
    free(cpu);
  }
}