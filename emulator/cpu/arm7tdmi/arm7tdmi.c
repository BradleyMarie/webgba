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