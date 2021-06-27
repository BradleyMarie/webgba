extern "C" {
#include "emulator/cpu/arm7tdmi/decoders/arm/condition.h"
}

#define ARM_CONDITION_EQ 0u
#define ARM_CONDITION_NE 1u
#define ARM_CONDITION_CS 2u
#define ARM_CONDITION_CC 3u
#define ARM_CONDITION_MI 4u
#define ARM_CONDITION_PL 5u
#define ARM_CONDITION_VS 6u
#define ARM_CONDITION_VC 7u
#define ARM_CONDITION_HI 8u
#define ARM_CONDITION_LS 9u
#define ARM_CONDITION_GE 10u
#define ARM_CONDITION_LT 11u
#define ARM_CONDITION_GT 12u
#define ARM_CONDITION_LE 13u
#define ARM_CONDITION_AL 14u
#define ARM_CONDITION_NV 15u

#include "googletest/include/gtest/gtest.h"

class ArmConditionTest : public testing::TestWithParam<uint32_t> {
 public:
  void SetUp() override {
    cpsr_.zero = !!((GetParam() >> 0u) & 0x1u);
    cpsr_.negative = !!((GetParam() >> 1u) & 0x1u);
    cpsr_.carry = !!((GetParam() >> 2u) & 0x1u);
    cpsr_.overflow = !!((GetParam() >> 3u) & 0x1u);
  }

  uint32_t ConditionToInstruction(uint_fast8_t condition) {
    return condition << 28;
  }

 protected:
  ArmProgramStatusRegister cpsr_;
};

TEST_P(ArmConditionTest, EQ) {
  uint32_t instruction = ConditionToInstruction(ARM_CONDITION_EQ);
  EXPECT_EQ(cpsr_.zero, ArmInstructionShouldExecute(cpsr_, instruction));
}

TEST_P(ArmConditionTest, NE) {
  uint32_t instruction = ConditionToInstruction(ARM_CONDITION_NE);
  EXPECT_EQ(!cpsr_.zero, ArmInstructionShouldExecute(cpsr_, instruction));
}

TEST_P(ArmConditionTest, CS) {
  uint32_t instruction = ConditionToInstruction(ARM_CONDITION_CS);
  EXPECT_EQ(cpsr_.carry, ArmInstructionShouldExecute(cpsr_, instruction));
}

TEST_P(ArmConditionTest, CC) {
  uint32_t instruction = ConditionToInstruction(ARM_CONDITION_CC);
  EXPECT_EQ(!cpsr_.carry, ArmInstructionShouldExecute(cpsr_, instruction));
}

TEST_P(ArmConditionTest, VS) {
  uint32_t instruction = ConditionToInstruction(ARM_CONDITION_VS);
  EXPECT_EQ(cpsr_.overflow, ArmInstructionShouldExecute(cpsr_, instruction));
}

TEST_P(ArmConditionTest, VC) {
  uint32_t instruction = ConditionToInstruction(ARM_CONDITION_VC);
  EXPECT_EQ(!cpsr_.overflow, ArmInstructionShouldExecute(cpsr_, instruction));
}

TEST_P(ArmConditionTest, HI) {
  uint32_t instruction = ConditionToInstruction(ARM_CONDITION_HI);
  EXPECT_EQ(cpsr_.carry && !cpsr_.zero,
            ArmInstructionShouldExecute(cpsr_, instruction));
}

TEST_P(ArmConditionTest, LS) {
  uint32_t instruction = ConditionToInstruction(ARM_CONDITION_LS);
  EXPECT_EQ(!cpsr_.carry || cpsr_.zero,
            ArmInstructionShouldExecute(cpsr_, instruction));
}

TEST_P(ArmConditionTest, GE) {
  uint32_t instruction = ConditionToInstruction(ARM_CONDITION_GE);
  EXPECT_EQ(cpsr_.negative == cpsr_.overflow,
            ArmInstructionShouldExecute(cpsr_, instruction));
}

TEST_P(ArmConditionTest, LT) {
  uint32_t instruction = ConditionToInstruction(ARM_CONDITION_LT);
  EXPECT_EQ(cpsr_.negative != cpsr_.overflow,
            ArmInstructionShouldExecute(cpsr_, instruction));
}

TEST_P(ArmConditionTest, GT) {
  uint32_t instruction = ConditionToInstruction(ARM_CONDITION_GT);
  EXPECT_EQ(!cpsr_.zero && cpsr_.negative == cpsr_.overflow,
            ArmInstructionShouldExecute(cpsr_, instruction));
}

TEST_P(ArmConditionTest, LE) {
  uint32_t instruction = ConditionToInstruction(ARM_CONDITION_LE);
  EXPECT_EQ(cpsr_.zero || cpsr_.negative != cpsr_.overflow,
            ArmInstructionShouldExecute(cpsr_, instruction));
}

TEST_P(ArmConditionTest, AL) {
  uint32_t instruction = ConditionToInstruction(ARM_CONDITION_AL);
  EXPECT_TRUE(ArmInstructionShouldExecute(cpsr_, instruction));
}

TEST_P(ArmConditionTest, NV) {
  uint32_t instruction = ConditionToInstruction(ARM_CONDITION_NV);
  EXPECT_FALSE(ArmInstructionShouldExecute(cpsr_, instruction));
}

INSTANTIATE_TEST_SUITE_P(ArmConditionTestModule, ArmConditionTest,
                         testing::Values(0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u,
                                         10u, 11u, 12u, 13u, 14u, 15u));