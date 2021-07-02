#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

#define USR_BANK_INDEX 0u
#define SYS_BANK_INDEX 0u
#define FIQ_BANK_INDEX 1u
#define IRQ_BANK_INDEX 2u
#define SVC_BANK_INDEX 3u
#define ABT_BANK_INDEX 4u
#define UND_BANK_INDEX 5u

static inline uint_fast8_t ArmModeToBankIndex(unsigned mode) {
  assert(MODE_USR <= mode && mode <= MODE_SYS);
  const static uint8_t bank_index[32] = {
      0xFFu, 0xFFu,          0xFFu,          0xFFu,          0xFFu,
      0xFFu, 0xFFu,          0xFFu,          0xFFu,          0xFFu,
      0xFFu, 0xFFu,          0xFFu,          0xFFu,          0xFFu,
      0xFFu, USR_BANK_INDEX, FIQ_BANK_INDEX, IRQ_BANK_INDEX, SVC_BANK_INDEX,
      0xFFu, ABT_BANK_INDEX, 0xFFu,          0xFFu,          0xFFu,
      0xFFu, 0xFFu,          UND_BANK_INDEX, 0xFFu,          0xFFu,
      0xFFu, SYS_BANK_INDEX};
  assert(bank_index[mode] != 0xFFu);
  return bank_index[mode];
}

inline void ArmLoadCPSR(ArmAllRegisters* registers,
                        ArmProgramStatusRegister cpsr) {
  unsigned current_mode = registers->current.user.cpsr.mode;
  unsigned next_mode = cpsr.mode;
  if (current_mode != next_mode) {
    uint_fast8_t current_bank_index = ArmModeToBankIndex(current_mode);
    for (uint_fast8_t i = 0; i < 2; i++) {
      registers->banked_splrs[current_bank_index][i] =
          registers->current.user.gprs.gprs[REGISTER_R14 - i];
    }

    if (next_mode == MODE_FIQ || current_mode == MODE_FIQ) {
      for (uint_fast8_t i = 0; i < 5; i++) {
        uint32_t temp = registers->banked_fiq_gprs[i];
        registers->banked_fiq_gprs[i] =
            registers->current.user.gprs.gprs[REGISTER_R12 - i];
        registers->current.user.gprs.gprs[REGISTER_R12 - i] = temp;
      }
    }

    registers->banked_spsrs[current_bank_index] = registers->current.spsr;

    uint_fast8_t next_bank_index = ArmModeToBankIndex(next_mode);
    for (uint_fast8_t i = 0; i < 2; i++) {
      registers->current.user.gprs.gprs[REGISTER_R14 - i] =
          registers->banked_splrs[next_bank_index][i];
    }
    registers->current.spsr = registers->banked_spsrs[next_bank_index];
  }

  registers->current.user.cpsr = cpsr;
}

static inline void ArmException(ArmAllRegisters* registers, unsigned mode) {
  assert(mode == MODE_UND || mode == MODE_SVC || mode == MODE_ABT ||
         mode == MODE_FIQ || mode == MODE_IRQ);

  ArmProgramStatusRegister old_cpsr = registers->current.user.cpsr;
  ArmProgramStatusRegister next_status = old_cpsr;
  next_status.mode = mode;
  next_status.thumb = false;
  next_status.irq_disable = true;
  ArmLoadCPSR(registers, next_status);

  const static uint_fast8_t next_instruction_pc_offset[2] = {4, 2};
  registers->current.user.gprs.r14 = registers->current.user.gprs.pc -
                                     next_instruction_pc_offset[old_cpsr.thumb];
  registers->current.spsr = old_cpsr;
}

void ArmExceptionUND(ArmAllRegisters* registers) {
  ArmException(registers, MODE_UND);
  registers->current.user.gprs.pc = 0x4;
}

void ArmExceptionSWI(ArmAllRegisters* registers) {
  ArmException(registers, MODE_SVC);
  registers->current.user.gprs.pc = 0x8;
}