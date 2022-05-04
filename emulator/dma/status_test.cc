extern "C" {
#include "emulator/dma/status.h"
}

#include "googletest/include/gtest/gtest.h"

class DmaStatusTest : public testing::Test {
 public:
  void SetUp() override {
    expected_address_ = (void*)0x12345678u;
    dma_status_ =
        DmaStatusAllocate(expected_address_, DmaStatusSetCallback, nullptr);
    ASSERT_NE(dma_status_, nullptr);
  }

  void TearDown() override { DmaStatusFree(dma_status_); }

 protected:
  static void DmaStatusSetCallback(void* context, bool active) {
    ASSERT_EQ(expected_address_, context);
    ASSERT_EQ(expected_active_, active);
  }

  static void* expected_address_;
  static bool expected_active_;
  DmaStatus* dma_status_;
};

void* DmaStatusTest::expected_address_;
bool DmaStatusTest::expected_active_;

TEST_F(DmaStatusTest, Active) {
  expected_active_ = true;
  DmaStatusSet(dma_status_, true);
}

TEST_F(DmaStatusTest, Inactive) {
  expected_active_ = false;
  DmaStatusSet(dma_status_, false);
}