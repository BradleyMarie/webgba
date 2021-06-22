#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline uint_fast8_t ArmModeToBankIndex(unsigned mode) {
  assert(MODE_USR <= mode && mode <= MODE_SYS);
  const static uint8_t bank_index[32] = {
      0xFFu, 0xFFu,          0xFFu,          0xFFu,          0xFFu,
      0xFFu, 0xFFu,          0xFFu,          0xFFu,          0xFFu,
      0xFFu, 0xFFu,          0xFFu,          0xFFu,          0xFFu,
      0xFFu, USR_BANK_INDEX, FIQ_BANK_INDEX, IRQ_BANK_INDEX, SVC_BANK_INDEX,
      0xFFu, 0xFFu,          0xFFu,          0xFFu,          0xFFu,
      0xFFu, 0xFFu,          ABT_BANK_INDEX, 0xFFu,          0xFFu,
      0xFFu, SYS_BANK_INDEX};
  assert(bank_index[mode] != 0xFFu);
  return bank_index[mode];
}

static inline uint_fast8_t ArmBankIndexToBankSize(uint_fast8_t bank_index) {
  assert(USR_BANK_INDEX <= bank_index && bank_index <= UND_BANK_INDEX);
  const static uint8_t bank_index_size[6] = {7u, 7u, 2u, 2u, 2u, 2u};
  return bank_index_size[bank_index];
}

void ArmLoadCPSR(ArmAllRegisters* registers, ArmProgramStatusRegister cpsr) {
  uint_fast8_t current_bank_index =
      ArmModeToBankIndex(registers->current.user.cpsr.mode);
  uint_fast8_t current_bank_size = ArmBankIndexToBankSize(current_bank_index);
  for (uint_fast8_t i = 0; i < current_bank_size; i++) {
    registers->banked_gprs[current_bank_index][i] =
        registers->current.user.gprs.gprs[REGISTER_R14 - i];
  }
  registers->banked_spsrs[current_bank_index] = registers->current.spsr;

  uint_fast8_t next_bank_index = ArmModeToBankIndex(cpsr.mode);
  uint_fast8_t next_bank_size = ArmBankIndexToBankSize(next_bank_index);
  for (uint_fast8_t i = 0; i < next_bank_size; i++) {
    registers->current.user.gprs.gprs[REGISTER_R14 - i] =
        registers->banked_gprs[next_bank_index][i];
  }
  registers->current.spsr = registers->banked_spsrs[next_bank_index];

  registers->current.user.cpsr = cpsr;
}