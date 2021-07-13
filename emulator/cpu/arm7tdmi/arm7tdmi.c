#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

#include "emulator/cpu/arm7tdmi/decoders/arm/execute.h"
#include "emulator/cpu/arm7tdmi/decoders/thumb/execute.h"
#include "emulator/cpu/arm7tdmi/exceptions.h"

void ArmCpuStep(ArmCpu* cpu, Memory* memory) {
  const static uint8_t thumb_instruction_offset = 4u;
  const static uint8_t arm_instruction_offset = 8u;
  const static uint8_t thumb_instruction_size = 2u;
  const static uint8_t arm_instruction_size = 4u;
  const static uint8_t next_pc_offset[2][2] = {
      {arm_instruction_size, thumb_instruction_size},
      {arm_instruction_offset, thumb_instruction_offset}};

  bool modified_pc;
  if (cpu->pending_rst) {
    ArmExceptionRST(&cpu->registers);
    cpu->pending_rst = false;
    modified_pc = true;
  } else if (cpu->pending_fiq &&
             !cpu->registers.current.user.cpsr.fiq_disable) {
    ArmExceptionFIQ(&cpu->registers);
    modified_pc = true;
  } else if (cpu->pending_irq &&
             !cpu->registers.current.user.cpsr.irq_disable) {
    ArmExceptionIRQ(&cpu->registers);
    modified_pc = true;
  } else if (cpu->registers.current.user.cpsr.thumb) {
    cpu->registers.current.user.gprs.pc &= 0xFFFFFFFEu;

    uint32_t next_instruction_addr =
        cpu->registers.current.user.gprs.pc - thumb_instruction_offset;
    uint16_t next_instruction;
    bool success = Load16LE(memory, next_instruction_addr, &next_instruction);
    if (!success) {
      ArmExceptionPrefetchABT(&cpu->registers);
      modified_pc = true;
    } else {
      modified_pc =
          ThumbInstructionExecute(next_instruction, &cpu->registers, memory);
    }
  } else {
    cpu->registers.current.user.gprs.pc &= 0xFFFFFFFCu;

    uint32_t next_instruction_addr =
        cpu->registers.current.user.gprs.pc - arm_instruction_offset;
    uint32_t next_instruction;
    bool success = Load32LE(memory, next_instruction_addr, &next_instruction);
    if (!success) {
      ArmExceptionPrefetchABT(&cpu->registers);
      modified_pc = true;
    } else {
      modified_pc =
          ArmInstructionExecute(next_instruction, &cpu->registers, memory);
    }
  }

  cpu->registers.current.user.gprs.pc +=
      next_pc_offset[modified_pc][cpu->registers.current.user.cpsr.thumb];
}

void ArmCpuRun(ArmCpu* cpu, Memory* memory, uint32_t num_steps) {
  for (uint32_t i = 0; i < num_steps; i++) {
    ArmCpuStep(cpu, memory);
  }
}