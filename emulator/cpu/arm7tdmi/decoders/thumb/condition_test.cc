extern "C" {
#include "emulator/cpu/arm7tdmi/decoders/thumb/condition.h"
}

#define THUMB_CONDITION_EQ 0u
#define THUMB_CONDITION_NE 1u
#define THUMB_CONDITION_CS 2u
#define THUMB_CONDITION_CC 3u
#define THUMB_CONDITION_MI 4u
#define THUMB_CONDITION_PL 5u
#define THUMB_CONDITION_VS 6u
#define THUMB_CONDITION_VC 7u
#define THUMB_CONDITION_HI 8u
#define THUMB_CONDITION_LS 9u
#define THUMB_CONDITION_GE 10u
#define THUMB_CONDITION_LT 11u
#define THUMB_CONDITION_GT 12u
#define THUMB_CONDITION_LE 13u

#include "googletest/include/gtest/gtest.h"

class ThumbConditionTest : public testing::TestWithParam<uint32_t> {
 public:
  void SetUp() override {
    cpsr_.zero = !!((GetParam() >> 0u) & 0x1u);
    cpsr_.negative = !!((GetParam() >> 1u) & 0x1u);
    cpsr_.carry = !!((GetParam() >> 2u) & 0x1u);
    cpsr_.overflow = !!((GetParam() >> 3u) & 0x1u);
  }

 protected:
  ArmProgramStatusRegister cpsr_;
};

TEST_P(ThumbConditionTest, EQ) {
  EXPECT_EQ(cpsr_.zero, ThumbShouldBranch(cpsr_, THUMB_CONDITION_EQ));
}

TEST_P(ThumbConditionTest, NE) {
  EXPECT_EQ(!cpsr_.zero, ThumbShouldBranch(cpsr_, THUMB_CONDITION_NE));
}

TEST_P(ThumbConditionTest, CS) {
  EXPECT_EQ(cpsr_.carry, ThumbShouldBranch(cpsr_, THUMB_CONDITION_CS));
}

TEST_P(ThumbConditionTest, CC) {
  EXPECT_EQ(!cpsr_.carry, ThumbShouldBranch(cpsr_, THUMB_CONDITION_CC));
}

TEST_P(ThumbConditionTest, VS) {
  EXPECT_EQ(cpsr_.overflow, ThumbShouldBranch(cpsr_, THUMB_CONDITION_VS));
}

TEST_P(ThumbConditionTest, VC) {
  EXPECT_EQ(!cpsr_.overflow, ThumbShouldBranch(cpsr_, THUMB_CONDITION_VC));
}

TEST_P(ThumbConditionTest, HI) {
  EXPECT_EQ(cpsr_.carry && !cpsr_.zero,
            ThumbShouldBranch(cpsr_, THUMB_CONDITION_HI));
}

TEST_P(ThumbConditionTest, LS) {
  EXPECT_EQ(!cpsr_.carry || cpsr_.zero,
            ThumbShouldBranch(cpsr_, THUMB_CONDITION_LS));
}

TEST_P(ThumbConditionTest, GE) {
  EXPECT_EQ(cpsr_.negative == cpsr_.overflow,
            ThumbShouldBranch(cpsr_, THUMB_CONDITION_GE));
}

TEST_P(ThumbConditionTest, LT) {
  EXPECT_EQ(cpsr_.negative != cpsr_.overflow,
            ThumbShouldBranch(cpsr_, THUMB_CONDITION_LT));
}

TEST_P(ThumbConditionTest, GT) {
  EXPECT_EQ(!cpsr_.zero && cpsr_.negative == cpsr_.overflow,
            ThumbShouldBranch(cpsr_, THUMB_CONDITION_GT));
}

TEST_P(ThumbConditionTest, LE) {
  EXPECT_EQ(cpsr_.zero || cpsr_.negative != cpsr_.overflow,
            ThumbShouldBranch(cpsr_, THUMB_CONDITION_LE));
}

INSTANTIATE_TEST_SUITE_P(ThumbConditionTestModule, ThumbConditionTest,
                         testing::Values(0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u,
                                         10u, 11u, 12u, 13u));