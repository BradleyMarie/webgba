#include "emulator/cpu/arm7tdmi/registers.h"

#include <assert.h>

#define USR_BANK_INDEX 0u
#define SYS_BANK_INDEX 0u
#define FIQ_BANK_INDEX 1u
#define IRQ_BANK_INDEX 2u
#define SVC_BANK_INDEX 3u
#define ABT_BANK_INDEX 4u
#define UND_BANK_INDEX 5u
#define INVALID_BANK 255u

static inline uint_fast8_t ArmModeToBankIndex(unsigned mode) {
  assert(mode <= MODE_SYS);
  static const uint8_t bank_index[32] = {
      INVALID_BANK,   INVALID_BANK,   INVALID_BANK,   INVALID_BANK,
      INVALID_BANK,   INVALID_BANK,   INVALID_BANK,   INVALID_BANK,
      INVALID_BANK,   INVALID_BANK,   INVALID_BANK,   INVALID_BANK,
      INVALID_BANK,   INVALID_BANK,   INVALID_BANK,   INVALID_BANK,
      USR_BANK_INDEX, FIQ_BANK_INDEX, IRQ_BANK_INDEX, SVC_BANK_INDEX,
      INVALID_BANK,   ABT_BANK_INDEX, INVALID_BANK,   INVALID_BANK,
      INVALID_BANK,   INVALID_BANK,   INVALID_BANK,   UND_BANK_INDEX,
      INVALID_BANK,   INVALID_BANK,   INVALID_BANK,   SYS_BANK_INDEX};
  return bank_index[mode];
}

void ArmLoadCPSR(ArmAllRegisters* registers, ArmProgramStatusRegister cpsr) {
  unsigned current_mode = registers->current.user.cpsr.mode;
  uint_fast8_t current_bank_index = ArmModeToBankIndex(current_mode);
  assert(current_bank_index != INVALID_BANK);

  unsigned next_mode = cpsr.mode;
  uint_fast8_t next_bank_index = ArmModeToBankIndex(next_mode);
  if (next_bank_index == INVALID_BANK) {
    next_bank_index = current_bank_index;
    next_mode = current_mode;
    cpsr.mode = current_mode;
  }

  if (current_bank_index != next_bank_index) {
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

    for (uint_fast8_t i = 0; i < 2; i++) {
      registers->current.user.gprs.gprs[REGISTER_R14 - i] =
          registers->banked_splrs[next_bank_index][i];
    }
    registers->current.spsr = registers->banked_spsrs[next_bank_index];
  }

  registers->current.user.cpsr = cpsr;
}