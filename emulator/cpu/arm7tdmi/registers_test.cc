extern "C" {
#include "emulator/cpu/arm7tdmi/registers.h"
}

#include <string.h>

#include <tuple>

#include "googletest/include/gtest/gtest.h"

#define USR_BANK_INDEX 0u
#define SYS_BANK_INDEX 0u
#define FIQ_BANK_INDEX 1u
#define IRQ_BANK_INDEX 2u
#define SVC_BANK_INDEX 3u
#define ABT_BANK_INDEX 4u
#define UND_BANK_INDEX 5u

#define BANKED_R8_INDEX 4u
#define BANKED_R9_INDEX 3u
#define BANKED_R10_INDEX 2u
#define BANKED_R11_INDEX 1u
#define BANKED_R12_INDEX 0u
#define BANKED_R13_INDEX 1u
#define BANKED_R14_INDEX 0u

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  return registers;
}

bool ArmPrivilegedRegistersAreZero(const ArmAllRegisters& regs) {
  auto zero = CreateArmAllRegisters();
  return !memcmp(&zero.current, &regs.current, sizeof(ArmPrivilegedRegisters));
}

class ArmLoadCPSRTest : public testing::TestWithParam<unsigned> {
 public:
  void SetUp() override {
    memset(&registers_, 0, sizeof(ArmAllRegisters));

    for (size_t i = 0; i < 16; i++) {
      registers_.current.user.gprs.gprs[i] = i;
    }

    registers_.current.user.cpsr.mode = GetParam();
    registers_.current.spsr.value = 65535u;

    registers_.banked_splrs[USR_BANK_INDEX][BANKED_R13_INDEX] =
        (MODE_USR << 4) | 13u;
    registers_.banked_splrs[USR_BANK_INDEX][BANKED_R14_INDEX] =
        (MODE_USR << 4) | 14u;
    registers_.banked_spsrs[USR_BANK_INDEX].value = (MODE_USR << 4);

    registers_.banked_splrs[SVC_BANK_INDEX][BANKED_R13_INDEX] =
        (MODE_SVC << 4) | 13u;
    registers_.banked_splrs[SVC_BANK_INDEX][BANKED_R14_INDEX] =
        (MODE_SVC << 4) | 14u;
    registers_.banked_spsrs[SVC_BANK_INDEX].value = (MODE_SVC << 4);

    registers_.banked_splrs[ABT_BANK_INDEX][BANKED_R13_INDEX] =
        (MODE_ABT << 4) | 13u;
    registers_.banked_splrs[ABT_BANK_INDEX][BANKED_R14_INDEX] =
        (MODE_ABT << 4) | 14u;
    registers_.banked_spsrs[ABT_BANK_INDEX].value = (MODE_ABT << 4);

    registers_.banked_splrs[UND_BANK_INDEX][BANKED_R13_INDEX] =
        (MODE_UND << 4) | 13u;
    registers_.banked_splrs[UND_BANK_INDEX][BANKED_R14_INDEX] =
        (MODE_UND << 4) | 14u;
    registers_.banked_spsrs[UND_BANK_INDEX].value = (MODE_UND << 4);

    registers_.banked_splrs[IRQ_BANK_INDEX][BANKED_R13_INDEX] =
        (MODE_IRQ << 4) | 13u;
    registers_.banked_splrs[IRQ_BANK_INDEX][BANKED_R14_INDEX] =
        (MODE_IRQ << 4) | 14u;
    registers_.banked_spsrs[IRQ_BANK_INDEX].value = (MODE_IRQ << 4);

    registers_.banked_splrs[FIQ_BANK_INDEX][BANKED_R13_INDEX] =
        (MODE_FIQ << 4) | 13u;
    registers_.banked_splrs[FIQ_BANK_INDEX][BANKED_R14_INDEX] =
        (MODE_FIQ << 4) | 14u;
    registers_.banked_spsrs[FIQ_BANK_INDEX].value = (MODE_FIQ << 4);

    registers_.banked_fiq_gprs[BANKED_R8_INDEX] = (MODE_FIQ << 4) | 8u;
    registers_.banked_fiq_gprs[BANKED_R9_INDEX] = (MODE_FIQ << 4) | 9u;
    registers_.banked_fiq_gprs[BANKED_R10_INDEX] = (MODE_FIQ << 4) | 10u;
    registers_.banked_fiq_gprs[BANKED_R11_INDEX] = (MODE_FIQ << 4) | 11u;
    registers_.banked_fiq_gprs[BANKED_R12_INDEX] = (MODE_FIQ << 4) | 12u;
  }

  void ValidateBanksUnmodifiedExcept(unsigned new_mode) {
    unsigned old_mode = GetParam();
    if (old_mode != MODE_USR && new_mode != MODE_USR && old_mode != MODE_SYS &&
        new_mode != MODE_SYS) {
      EXPECT_EQ((MODE_USR << 4) | 13u,
                registers_.banked_splrs[USR_BANK_INDEX][BANKED_R13_INDEX]);
      EXPECT_EQ((MODE_USR << 4) | 14u,
                registers_.banked_splrs[USR_BANK_INDEX][BANKED_R14_INDEX]);
      EXPECT_EQ(MODE_USR << 4, registers_.banked_spsrs[USR_BANK_INDEX].value);
    }

    if (old_mode != MODE_SVC && new_mode != MODE_SVC) {
      EXPECT_EQ((MODE_SVC << 4) | 13u,
                registers_.banked_splrs[SVC_BANK_INDEX][BANKED_R13_INDEX]);
      EXPECT_EQ((MODE_SVC << 4) | 14u,
                registers_.banked_splrs[SVC_BANK_INDEX][BANKED_R14_INDEX]);
      EXPECT_EQ(MODE_SVC << 4, registers_.banked_spsrs[SVC_BANK_INDEX].value);
    }

    if (old_mode != MODE_ABT && new_mode != MODE_ABT) {
      EXPECT_EQ((MODE_ABT << 4) | 13u,
                registers_.banked_splrs[ABT_BANK_INDEX][BANKED_R13_INDEX]);
      EXPECT_EQ((MODE_ABT << 4) | 14u,
                registers_.banked_splrs[ABT_BANK_INDEX][BANKED_R14_INDEX]);
      EXPECT_EQ(MODE_ABT << 4, registers_.banked_spsrs[ABT_BANK_INDEX].value);
    }

    if (old_mode != MODE_UND && new_mode != MODE_UND) {
      EXPECT_EQ((MODE_UND << 4) | 13u,
                registers_.banked_splrs[UND_BANK_INDEX][BANKED_R13_INDEX]);
      EXPECT_EQ((MODE_UND << 4) | 14u,
                registers_.banked_splrs[UND_BANK_INDEX][BANKED_R14_INDEX]);
      EXPECT_EQ(MODE_UND << 4, registers_.banked_spsrs[UND_BANK_INDEX].value);
    }

    if (old_mode != MODE_IRQ && new_mode != MODE_IRQ) {
      EXPECT_EQ((MODE_IRQ << 4) | 13u,
                registers_.banked_splrs[IRQ_BANK_INDEX][BANKED_R13_INDEX]);
      EXPECT_EQ((MODE_IRQ << 4) | 14u,
                registers_.banked_splrs[IRQ_BANK_INDEX][BANKED_R14_INDEX]);
      EXPECT_EQ(MODE_IRQ << 4, registers_.banked_spsrs[IRQ_BANK_INDEX].value);
    }

    if (old_mode != MODE_FIQ && new_mode != MODE_FIQ) {
      EXPECT_EQ((MODE_FIQ << 4) | 8u,
                registers_.banked_fiq_gprs[BANKED_R8_INDEX]);
      EXPECT_EQ((MODE_FIQ << 4) | 9u,
                registers_.banked_fiq_gprs[BANKED_R9_INDEX]);
      EXPECT_EQ((MODE_FIQ << 4) | 10u,
                registers_.banked_fiq_gprs[BANKED_R10_INDEX]);
      EXPECT_EQ((MODE_FIQ << 4) | 11u,
                registers_.banked_fiq_gprs[BANKED_R11_INDEX]);
      EXPECT_EQ((MODE_FIQ << 4) | 12u,
                registers_.banked_fiq_gprs[BANKED_R12_INDEX]);
      EXPECT_EQ((MODE_FIQ << 4) | 13u,
                registers_.banked_splrs[FIQ_BANK_INDEX][BANKED_R13_INDEX]);
      EXPECT_EQ((MODE_FIQ << 4) | 14u,
                registers_.banked_splrs[FIQ_BANK_INDEX][BANKED_R14_INDEX]);
      EXPECT_EQ(MODE_FIQ << 4, registers_.banked_spsrs[FIQ_BANK_INDEX].value);
    }
  }

  void ValidateNewlyBankedRegisters(unsigned new_mode) {
    size_t old_bank_index;
    switch (GetParam()) {
      case MODE_USR:
      case MODE_SYS:
        old_bank_index = USR_BANK_INDEX;
        break;
      case MODE_SVC:
        old_bank_index = SVC_BANK_INDEX;
        break;
      case MODE_ABT:
        old_bank_index = ABT_BANK_INDEX;
        break;
      case MODE_UND:
        old_bank_index = UND_BANK_INDEX;
        break;
      case MODE_IRQ:
        old_bank_index = IRQ_BANK_INDEX;
        break;
      case MODE_FIQ:
        old_bank_index = FIQ_BANK_INDEX;
        break;
      default:
        ASSERT_FALSE(true);
        break;
    }

    for (size_t i = 0; i < 2; i++) {
      EXPECT_EQ(14u - i, registers_.banked_splrs[old_bank_index][i]);
    }

    if (GetParam() == MODE_FIQ || new_mode == MODE_FIQ) {
      for (size_t i = 0; i < 5; i++) {
        EXPECT_EQ(12u - i, registers_.banked_fiq_gprs[i]);
      }
    }

    EXPECT_EQ(65535u, registers_.banked_spsrs[old_bank_index].value);
  }

 protected:
  ArmAllRegisters registers_;
};

TEST_P(ArmLoadCPSRTest, ToUSR) {
  ArmProgramStatusRegister next_cpsr;
  next_cpsr.mode = MODE_USR;
  ArmLoadCPSR(&registers_, next_cpsr);

  // Unmodified Registers
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);
  EXPECT_EQ(4u, registers_.current.user.gprs.r4);
  EXPECT_EQ(5u, registers_.current.user.gprs.r5);
  EXPECT_EQ(6u, registers_.current.user.gprs.r6);
  EXPECT_EQ(7u, registers_.current.user.gprs.r7);
  EXPECT_EQ(15u, registers_.current.user.gprs.r15);

  // Definitely Modified Registers
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);

  // Potentially Modified Registers
  if (MODE_USR != GetParam() && MODE_SYS != GetParam()) {
    if (MODE_FIQ == GetParam()) {
      EXPECT_EQ((MODE_FIQ << 4) | 8u, registers_.current.user.gprs.r8);
      EXPECT_EQ((MODE_FIQ << 4) | 9u, registers_.current.user.gprs.r9);
      EXPECT_EQ((MODE_FIQ << 4) | 10u, registers_.current.user.gprs.r10);
      EXPECT_EQ((MODE_FIQ << 4) | 11u, registers_.current.user.gprs.r11);
      EXPECT_EQ((MODE_FIQ << 4) | 12u, registers_.current.user.gprs.r12);
    } else {
      EXPECT_EQ(8u, registers_.current.user.gprs.r8);
      EXPECT_EQ(9u, registers_.current.user.gprs.r9);
      EXPECT_EQ(10u, registers_.current.user.gprs.r10);
      EXPECT_EQ(11u, registers_.current.user.gprs.r11);
      EXPECT_EQ(12u, registers_.current.user.gprs.r12);
    }
    EXPECT_EQ((MODE_USR << 4) | 13u, registers_.current.user.gprs.r13);
    EXPECT_EQ((MODE_USR << 4) | 14u, registers_.current.user.gprs.r14);
    EXPECT_EQ(MODE_USR << 4, registers_.current.spsr.value);
    ValidateNewlyBankedRegisters(MODE_USR);
  } else {
    EXPECT_EQ(8u, registers_.current.user.gprs.r8);
    EXPECT_EQ(9u, registers_.current.user.gprs.r9);
    EXPECT_EQ(10u, registers_.current.user.gprs.r10);
    EXPECT_EQ(11u, registers_.current.user.gprs.r11);
    EXPECT_EQ(12u, registers_.current.user.gprs.r12);
    EXPECT_EQ(13u, registers_.current.user.gprs.r13);
    EXPECT_EQ(14u, registers_.current.user.gprs.r14);
    EXPECT_EQ(65535u, registers_.current.spsr.value);
  }

  ValidateBanksUnmodifiedExcept(MODE_USR);
}

TEST_P(ArmLoadCPSRTest, ToSYS) {
  ArmProgramStatusRegister next_cpsr;
  next_cpsr.mode = MODE_SYS;
  ArmLoadCPSR(&registers_, next_cpsr);

  // Unmodified Registers
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);
  EXPECT_EQ(4u, registers_.current.user.gprs.r4);
  EXPECT_EQ(5u, registers_.current.user.gprs.r5);
  EXPECT_EQ(6u, registers_.current.user.gprs.r6);
  EXPECT_EQ(7u, registers_.current.user.gprs.r7);
  EXPECT_EQ(15u, registers_.current.user.gprs.r15);

  // Definitely Modified Registers
  EXPECT_EQ(MODE_SYS, registers_.current.user.cpsr.mode);

  // Potentially Modified Registers
  if (MODE_SYS != GetParam() && MODE_USR != GetParam()) {
    if (MODE_FIQ == GetParam()) {
      EXPECT_EQ((MODE_FIQ << 4) | 8u, registers_.current.user.gprs.r8);
      EXPECT_EQ((MODE_FIQ << 4) | 9u, registers_.current.user.gprs.r9);
      EXPECT_EQ((MODE_FIQ << 4) | 10u, registers_.current.user.gprs.r10);
      EXPECT_EQ((MODE_FIQ << 4) | 11u, registers_.current.user.gprs.r11);
      EXPECT_EQ((MODE_FIQ << 4) | 12u, registers_.current.user.gprs.r12);
    } else {
      EXPECT_EQ(8u, registers_.current.user.gprs.r8);
      EXPECT_EQ(9u, registers_.current.user.gprs.r9);
      EXPECT_EQ(10u, registers_.current.user.gprs.r10);
      EXPECT_EQ(11u, registers_.current.user.gprs.r11);
      EXPECT_EQ(12u, registers_.current.user.gprs.r12);
    }
    EXPECT_EQ((MODE_USR << 4) | 13u, registers_.current.user.gprs.r13);
    EXPECT_EQ((MODE_USR << 4) | 14u, registers_.current.user.gprs.r14);
    EXPECT_EQ(MODE_USR << 4, registers_.current.spsr.value);
    ValidateNewlyBankedRegisters(MODE_SYS);
  } else {
    EXPECT_EQ(8u, registers_.current.user.gprs.r8);
    EXPECT_EQ(9u, registers_.current.user.gprs.r9);
    EXPECT_EQ(10u, registers_.current.user.gprs.r10);
    EXPECT_EQ(11u, registers_.current.user.gprs.r11);
    EXPECT_EQ(12u, registers_.current.user.gprs.r12);
    EXPECT_EQ(13u, registers_.current.user.gprs.r13);
    EXPECT_EQ(14u, registers_.current.user.gprs.r14);
    EXPECT_EQ(65535u, registers_.current.spsr.value);
  }

  ValidateBanksUnmodifiedExcept(MODE_SYS);
}

TEST_P(ArmLoadCPSRTest, ToABT) {
  ArmProgramStatusRegister next_cpsr;
  next_cpsr.mode = MODE_ABT;
  ArmLoadCPSR(&registers_, next_cpsr);

  // Unmodified Registers
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);
  EXPECT_EQ(4u, registers_.current.user.gprs.r4);
  EXPECT_EQ(5u, registers_.current.user.gprs.r5);
  EXPECT_EQ(6u, registers_.current.user.gprs.r6);
  EXPECT_EQ(7u, registers_.current.user.gprs.r7);
  EXPECT_EQ(15u, registers_.current.user.gprs.r15);

  // Definitely Modified Registers
  EXPECT_EQ(MODE_ABT, registers_.current.user.cpsr.mode);

  // Potentially Modified Registers
  if (MODE_ABT != GetParam()) {
    if (MODE_FIQ == GetParam()) {
      EXPECT_EQ((MODE_FIQ << 4) | 8u, registers_.current.user.gprs.r8);
      EXPECT_EQ((MODE_FIQ << 4) | 9u, registers_.current.user.gprs.r9);
      EXPECT_EQ((MODE_FIQ << 4) | 10u, registers_.current.user.gprs.r10);
      EXPECT_EQ((MODE_FIQ << 4) | 11u, registers_.current.user.gprs.r11);
      EXPECT_EQ((MODE_FIQ << 4) | 12u, registers_.current.user.gprs.r12);
    } else {
      EXPECT_EQ(8u, registers_.current.user.gprs.r8);
      EXPECT_EQ(9u, registers_.current.user.gprs.r9);
      EXPECT_EQ(10u, registers_.current.user.gprs.r10);
      EXPECT_EQ(11u, registers_.current.user.gprs.r11);
      EXPECT_EQ(12u, registers_.current.user.gprs.r12);
    }
    EXPECT_EQ((MODE_ABT << 4) | 13u, registers_.current.user.gprs.r13);
    EXPECT_EQ((MODE_ABT << 4) | 14u, registers_.current.user.gprs.r14);
    EXPECT_EQ(MODE_ABT << 4, registers_.current.spsr.value);
    ValidateNewlyBankedRegisters(MODE_ABT);
  } else {
    EXPECT_EQ(13u, registers_.current.user.gprs.r13);
    EXPECT_EQ(14u, registers_.current.user.gprs.r14);
    EXPECT_EQ(65535u, registers_.current.spsr.value);
  }

  ValidateBanksUnmodifiedExcept(MODE_ABT);
}

TEST_P(ArmLoadCPSRTest, ToFIQ) {
  ArmProgramStatusRegister next_cpsr;
  next_cpsr.mode = MODE_FIQ;
  ArmLoadCPSR(&registers_, next_cpsr);

  // Unmodified Registers
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);
  EXPECT_EQ(4u, registers_.current.user.gprs.r4);
  EXPECT_EQ(5u, registers_.current.user.gprs.r5);
  EXPECT_EQ(6u, registers_.current.user.gprs.r6);
  EXPECT_EQ(7u, registers_.current.user.gprs.r7);
  EXPECT_EQ(15u, registers_.current.user.gprs.r15);

  // Definitely Modified Registers
  EXPECT_EQ(MODE_FIQ, registers_.current.user.cpsr.mode);

  // Potentially Modified Registers
  if (MODE_FIQ != GetParam()) {
    EXPECT_EQ((MODE_FIQ << 4) | 8u, registers_.current.user.gprs.r8);
    EXPECT_EQ((MODE_FIQ << 4) | 9u, registers_.current.user.gprs.r9);
    EXPECT_EQ((MODE_FIQ << 4) | 10u, registers_.current.user.gprs.r10);
    EXPECT_EQ((MODE_FIQ << 4) | 11u, registers_.current.user.gprs.r11);
    EXPECT_EQ((MODE_FIQ << 4) | 12u, registers_.current.user.gprs.r12);
    EXPECT_EQ((MODE_FIQ << 4) | 13u, registers_.current.user.gprs.r13);
    EXPECT_EQ((MODE_FIQ << 4) | 14u, registers_.current.user.gprs.r14);
    EXPECT_EQ(MODE_FIQ << 4, registers_.current.spsr.value);
    ValidateNewlyBankedRegisters(MODE_FIQ);
  } else {
    EXPECT_EQ(8u, registers_.current.user.gprs.r8);
    EXPECT_EQ(9u, registers_.current.user.gprs.r9);
    EXPECT_EQ(10u, registers_.current.user.gprs.r10);
    EXPECT_EQ(11u, registers_.current.user.gprs.r11);
    EXPECT_EQ(12u, registers_.current.user.gprs.r12);
    EXPECT_EQ(13u, registers_.current.user.gprs.r13);
    EXPECT_EQ(14u, registers_.current.user.gprs.r14);
    EXPECT_EQ(65535u, registers_.current.spsr.value);
  }

  ValidateBanksUnmodifiedExcept(MODE_FIQ);
}

TEST_P(ArmLoadCPSRTest, ToIRQ) {
  ArmProgramStatusRegister next_cpsr;
  next_cpsr.mode = MODE_IRQ;
  ArmLoadCPSR(&registers_, next_cpsr);

  // Unmodified Registers
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);
  EXPECT_EQ(4u, registers_.current.user.gprs.r4);
  EXPECT_EQ(5u, registers_.current.user.gprs.r5);
  EXPECT_EQ(6u, registers_.current.user.gprs.r6);
  EXPECT_EQ(7u, registers_.current.user.gprs.r7);
  EXPECT_EQ(15u, registers_.current.user.gprs.r15);

  // Definitely Modified Registers
  EXPECT_EQ(MODE_IRQ, registers_.current.user.cpsr.mode);

  // Potentially Modified Registers
  if (MODE_IRQ != GetParam()) {
    if (MODE_FIQ == GetParam()) {
      EXPECT_EQ((MODE_FIQ << 4) | 8u, registers_.current.user.gprs.r8);
      EXPECT_EQ((MODE_FIQ << 4) | 9u, registers_.current.user.gprs.r9);
      EXPECT_EQ((MODE_FIQ << 4) | 10u, registers_.current.user.gprs.r10);
      EXPECT_EQ((MODE_FIQ << 4) | 11u, registers_.current.user.gprs.r11);
      EXPECT_EQ((MODE_FIQ << 4) | 12u, registers_.current.user.gprs.r12);
    } else {
      EXPECT_EQ(8u, registers_.current.user.gprs.r8);
      EXPECT_EQ(9u, registers_.current.user.gprs.r9);
      EXPECT_EQ(10u, registers_.current.user.gprs.r10);
      EXPECT_EQ(11u, registers_.current.user.gprs.r11);
      EXPECT_EQ(12u, registers_.current.user.gprs.r12);
    }
    EXPECT_EQ((MODE_IRQ << 4) | 13u, registers_.current.user.gprs.r13);
    EXPECT_EQ((MODE_IRQ << 4) | 14u, registers_.current.user.gprs.r14);
    EXPECT_EQ(MODE_IRQ << 4, registers_.current.spsr.value);
    ValidateNewlyBankedRegisters(MODE_IRQ);
  } else {
    EXPECT_EQ(13u, registers_.current.user.gprs.r13);
    EXPECT_EQ(14u, registers_.current.user.gprs.r14);
    EXPECT_EQ(65535u, registers_.current.spsr.value);
  }

  ValidateBanksUnmodifiedExcept(MODE_IRQ);
}

TEST_P(ArmLoadCPSRTest, ToSVC) {
  ArmProgramStatusRegister next_cpsr;
  next_cpsr.mode = MODE_SVC;
  ArmLoadCPSR(&registers_, next_cpsr);

  // Unmodified Registers
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);
  EXPECT_EQ(4u, registers_.current.user.gprs.r4);
  EXPECT_EQ(5u, registers_.current.user.gprs.r5);
  EXPECT_EQ(6u, registers_.current.user.gprs.r6);
  EXPECT_EQ(7u, registers_.current.user.gprs.r7);
  EXPECT_EQ(15u, registers_.current.user.gprs.r15);

  // Definitely Modified Registers
  EXPECT_EQ(MODE_SVC, registers_.current.user.cpsr.mode);

  // Potentially Modified Registers
  if (MODE_SVC != GetParam()) {
    if (MODE_FIQ == GetParam()) {
      EXPECT_EQ((MODE_FIQ << 4) | 8u, registers_.current.user.gprs.r8);
      EXPECT_EQ((MODE_FIQ << 4) | 9u, registers_.current.user.gprs.r9);
      EXPECT_EQ((MODE_FIQ << 4) | 10u, registers_.current.user.gprs.r10);
      EXPECT_EQ((MODE_FIQ << 4) | 11u, registers_.current.user.gprs.r11);
      EXPECT_EQ((MODE_FIQ << 4) | 12u, registers_.current.user.gprs.r12);
    } else {
      EXPECT_EQ(8u, registers_.current.user.gprs.r8);
      EXPECT_EQ(9u, registers_.current.user.gprs.r9);
      EXPECT_EQ(10u, registers_.current.user.gprs.r10);
      EXPECT_EQ(11u, registers_.current.user.gprs.r11);
      EXPECT_EQ(12u, registers_.current.user.gprs.r12);
    }
    EXPECT_EQ((MODE_SVC << 4) | 13u, registers_.current.user.gprs.r13);
    EXPECT_EQ((MODE_SVC << 4) | 14u, registers_.current.user.gprs.r14);
    EXPECT_EQ(MODE_SVC << 4, registers_.current.spsr.value);
    ValidateNewlyBankedRegisters(MODE_SVC);
  } else {
    EXPECT_EQ(13u, registers_.current.user.gprs.r13);
    EXPECT_EQ(14u, registers_.current.user.gprs.r14);
    EXPECT_EQ(65535u, registers_.current.spsr.value);
  }

  ValidateBanksUnmodifiedExcept(MODE_SVC);
}

TEST_P(ArmLoadCPSRTest, ToUND) {
  ArmProgramStatusRegister next_cpsr;
  next_cpsr.mode = MODE_UND;
  ArmLoadCPSR(&registers_, next_cpsr);

  // Unmodified Registers
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);
  EXPECT_EQ(4u, registers_.current.user.gprs.r4);
  EXPECT_EQ(5u, registers_.current.user.gprs.r5);
  EXPECT_EQ(6u, registers_.current.user.gprs.r6);
  EXPECT_EQ(7u, registers_.current.user.gprs.r7);
  EXPECT_EQ(15u, registers_.current.user.gprs.r15);

  // Definitely Modified Registers
  EXPECT_EQ(MODE_UND, registers_.current.user.cpsr.mode);

  // Potentially Modified Registers
  if (MODE_UND != GetParam()) {
    if (MODE_FIQ == GetParam()) {
      EXPECT_EQ((MODE_FIQ << 4) | 8u, registers_.current.user.gprs.r8);
      EXPECT_EQ((MODE_FIQ << 4) | 9u, registers_.current.user.gprs.r9);
      EXPECT_EQ((MODE_FIQ << 4) | 10u, registers_.current.user.gprs.r10);
      EXPECT_EQ((MODE_FIQ << 4) | 11u, registers_.current.user.gprs.r11);
      EXPECT_EQ((MODE_FIQ << 4) | 12u, registers_.current.user.gprs.r12);
    } else {
      EXPECT_EQ(8u, registers_.current.user.gprs.r8);
      EXPECT_EQ(9u, registers_.current.user.gprs.r9);
      EXPECT_EQ(10u, registers_.current.user.gprs.r10);
      EXPECT_EQ(11u, registers_.current.user.gprs.r11);
      EXPECT_EQ(12u, registers_.current.user.gprs.r12);
    }
    EXPECT_EQ((MODE_UND << 4) | 13u, registers_.current.user.gprs.r13);
    EXPECT_EQ((MODE_UND << 4) | 14u, registers_.current.user.gprs.r14);
    EXPECT_EQ(MODE_UND << 4, registers_.current.spsr.value);
    ValidateNewlyBankedRegisters(MODE_UND);
  } else {
    EXPECT_EQ(13u, registers_.current.user.gprs.r13);
    EXPECT_EQ(14u, registers_.current.user.gprs.r14);
    EXPECT_EQ(65535u, registers_.current.spsr.value);
  }

  ValidateBanksUnmodifiedExcept(MODE_UND);
}

INSTANTIATE_TEST_SUITE_P(ArmLoadCPSRModule, ArmLoadCPSRTest,
                         testing::Values(MODE_USR, MODE_SYS, MODE_ABT, MODE_FIQ,
                                         MODE_IRQ, MODE_SVC, MODE_UND));