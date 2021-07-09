#include "emulator/cpu/arm7tdmi/execute.h"

#include "emulator/cpu/arm7tdmi/decoders/arm/execute.h"
#include "emulator/cpu/arm7tdmi/decoders/thumb/execute.h"

void ArmCpuStep(ArmAllRegisters* registers, Memory* memory) {
  const static uint8_t thumb_instruction_offset = 4u;
  const static uint8_t arm_instruction_offset = 8u;
  const static uint8_t instruction_offsets[2] = {arm_instruction_offset,
                                                 thumb_instruction_offset};

  bool modified_pc;
  if (registers->current.user.cpsr.thumb) {
    registers->current.user.gprs.pc &= 0xFFFFFFFEu;

    uint32_t next_instruction_addr =
        registers->current.user.gprs.pc - thumb_instruction_offset;
    uint16_t next_instruction;
    bool success = Load16LE(memory, next_instruction_addr, &next_instruction);
    assert(success);

    modified_pc = ThumbInstructionExecute(next_instruction, registers, memory);
  } else {
    registers->current.user.gprs.pc &= 0xFFFFFFFCu;

    uint32_t next_instruction_addr =
        registers->current.user.gprs.pc - arm_instruction_offset;
    uint32_t next_instruction;
    bool success = Load32LE(memory, next_instruction_addr, &next_instruction);
    assert(success);

    modified_pc = ArmInstructionExecute(next_instruction, registers, memory);
  }

  if (modified_pc) {
    registers->current.user.gprs.pc -=
        instruction_offsets[registers->current.user.cpsr.thumb];
  }
}

void ArmCpuRun(ArmAllRegisters* registers, Memory* memory, uint32_t num_steps) {
  for (uint32_t i = 0; i < num_steps; i++) {
    ArmCpuStep(registers, memory);
  }
}