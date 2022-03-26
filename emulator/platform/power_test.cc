extern "C" {
#include "emulator/platform/power.h"
}

#include "googletest/include/gtest/gtest.h"

class PowerTest : public testing::Test {
 public:
  void SetUp() override {
    expected_address_ = (void*)0x12345678u;
    power_ = PowerAllocate(expected_address_, PowerSetCallback, nullptr);
    ASSERT_NE(power_, nullptr);
  }

  void TearDown() override { PowerFree(power_); }

 protected:
  static void PowerSetCallback(void* context, PowerState state) {
    ASSERT_EQ(expected_address_, context);
    ASSERT_EQ(expected_state_, state);
  }

  static void* expected_address_;
  static PowerState expected_state_;
  Power* power_;
};

void* PowerTest::expected_address_;
PowerState PowerTest::expected_state_;

TEST_F(PowerTest, Run) {
  expected_state_ = POWER_STATE_RUN;
  PowerSet(power_, POWER_STATE_RUN);
}

TEST_F(PowerTest, Halt) {
  expected_state_ = POWER_STATE_HALT;
  PowerSet(power_, POWER_STATE_HALT);
}

TEST_F(PowerTest, Stop) {
  expected_state_ = POWER_STATE_STOP;
  PowerSet(power_, POWER_STATE_STOP);
}