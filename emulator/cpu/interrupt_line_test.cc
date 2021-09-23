extern "C" {
#include "emulator/cpu/interrupt_line.h"
}

#include "googletest/include/gtest/gtest.h"

class InterruptLineTest : public testing::Test {
 public:
  void SetUp() override {
    expected_address_ = (void*)0x12345678u;
    interrupt_line_ = InterruptLineAllocate(expected_address_,
                                            InterruptLineSetCallback, nullptr);
    ASSERT_NE(interrupt_line_, nullptr);
  }

  void TearDown() override { InterruptLineFree(interrupt_line_); }

 protected:
  static void InterruptLineSetCallback(void* context, bool raised) {
    ASSERT_EQ(expected_address_, context);
    ASSERT_EQ(expected_raised_, raised);
  }

  static void* expected_address_;
  static bool expected_raised_;
  InterruptLine* interrupt_line_;
};

void* InterruptLineTest::expected_address_;
bool InterruptLineTest::expected_raised_;

TEST_F(InterruptLineTest, Raised) {
  expected_raised_ = true;
  InterruptLineSetLevel(interrupt_line_, true);
}

TEST_F(InterruptLineTest, Lowered) {
  expected_raised_ = false;
  InterruptLineSetLevel(interrupt_line_, false);
}