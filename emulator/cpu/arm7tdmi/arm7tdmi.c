#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

#include <stdlib.h>

#include "emulator/cpu/arm7tdmi/decoders/arm/execute.h"
#include "emulator/cpu/arm7tdmi/decoders/thumb/execute.h"
#include "emulator/cpu/arm7tdmi/exceptions.h"

#define ARM_INSTRUCTION_OFFSET 8u
#define ARM_INSTRUCTION_SIZE 4u
#define THUMB_INSTRUCTION_OFFSET 4u
#define THUMB_INSTRUCTION_SIZE 2u

Arm7Tdmi* Arm7TdmiAllocate(InterruptLine* rst, InterruptLine* fiq,
                           InterruptLine* irq) {
  Arm7Tdmi* cpu = (Arm7Tdmi*)calloc(1, sizeof(Arm7Tdmi));
  if (cpu != NULL) {
    cpu->registers.current.user.cpsr.mode = MODE_SVC;
    cpu->registers.current.user.gprs.pc = 0x8u;
    cpu->rst = rst;
    cpu->fiq = fiq;
    cpu->irq = irq;
  }
  return cpu;
}

void Arm7TdmiStep(Arm7Tdmi* cpu, Memory* memory) {
  if (InterruptLineIsRaised(cpu->rst)) {
    ArmExceptionRST(&cpu->registers);
    cpu->registers.current.user.gprs.pc += ARM_INSTRUCTION_OFFSET;
    return;
  }

  if (InterruptLineIsRaised(cpu->fiq) &&
      !cpu->registers.current.user.cpsr.fiq_disable) {
    ArmExceptionFIQ(&cpu->registers);
    cpu->registers.current.user.gprs.pc += ARM_INSTRUCTION_OFFSET;
    return;
  }

  if (InterruptLineIsRaised(cpu->irq) &&
      !cpu->registers.current.user.cpsr.irq_disable) {
    ArmExceptionIRQ(&cpu->registers);
    cpu->registers.current.user.gprs.pc += ARM_INSTRUCTION_OFFSET;
    return;
  }

  if (cpu->registers.current.user.cpsr.thumb) {
    cpu->registers.current.user.gprs.pc &= 0xFFFFFFFEu;

    uint32_t next_instruction_addr =
        cpu->registers.current.user.gprs.pc - THUMB_INSTRUCTION_OFFSET;
    uint16_t next_instruction;
    bool success = Load16LE(memory, next_instruction_addr, &next_instruction);
    if (!success) {
      ArmExceptionPrefetchABT(&cpu->registers);
      cpu->registers.current.user.gprs.pc += ARM_INSTRUCTION_OFFSET;
      return;
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

    return;
  }

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

void Arm7TdmiFree(Arm7Tdmi* cpu) {
  InterruptLineFree(cpu->rst);
  InterruptLineFree(cpu->fiq);
  InterruptLineFree(cpu->irq);
  free(cpu);
}