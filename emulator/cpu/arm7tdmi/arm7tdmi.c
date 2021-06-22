#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

#define USR_BANK_INDEX 0u
#define SYS_BANK_INDEX 0u
#define FIQ_BANK_INDEX 1u
#define IRQ_BANK_INDEX 2u
#define SVC_BANK_INDEX 3u
#define ABT_BANK_INDEX 4u
#define UND_BANK_INDEX 5u

#define BANKED_R8_INDEX 6u
#define BANKED_R9_INDEX 5u
#define BANKED_R10_INDEX 4u
#define BANKED_R11_INDEX 3u
#define BANKED_R12_INDEX 2u
#define BANKED_R13_INDEX 1u
#define BANKED_R14_INDEX 0u

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

static inline uint_fast8_t ArmBankIndexToBankSize(uint_fast8_t bank_index) {
  assert(USR_BANK_INDEX <= bank_index && bank_index <= UND_BANK_INDEX);
  const static uint8_t bank_index_size[6] = {7u, 7u, 2u, 2u, 2u, 2u};
  return bank_index_size[bank_index];
}

void ArmLoadCPSR(ArmAllRegisters* registers, ArmProgramStatusRegister cpsr) {
  unsigned current_mode = registers->current.user.cpsr.mode;
  unsigned next_mode = cpsr.mode;
  if (current_mode != next_mode) {
    uint_fast8_t current_bank_index = ArmModeToBankIndex(current_mode);
    uint_fast8_t current_bank_size = ArmBankIndexToBankSize(current_bank_index);
    for (uint_fast8_t i = 0; i < current_bank_size; i++) {
      registers->banked_gprs[current_bank_index][i] =
          registers->current.user.gprs.gprs[REGISTER_R14 - i];
    }
    registers->banked_spsrs[current_bank_index] = registers->current.spsr;

    uint_fast8_t next_bank_index = ArmModeToBankIndex(next_mode);
    uint_fast8_t next_bank_size = ArmBankIndexToBankSize(next_bank_index);
    for (uint_fast8_t i = 0; i < next_bank_size; i++) {
      registers->current.user.gprs.gprs[REGISTER_R14 - i] =
          registers->banked_gprs[next_bank_index][i];
    }
    registers->current.spsr = registers->banked_spsrs[next_bank_index];
  }

  registers->current.user.cpsr = cpsr;
}