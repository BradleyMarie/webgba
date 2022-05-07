extern "C" {
#include "emulator/ppu/gba/software/screen.h"
}

#include "googletest/include/gtest/gtest.h"

class ScreenTest : public testing::Test {
 public:
  void SetUp() override { GbaPpuScreenReloadContext(&screen_); }

  void TearDown() override { GbaPpuScreenDestroy(&screen_); }

 protected:
  GbaPpuScreen screen_;
};

TEST_F(ScreenTest, Set) {
  GbaPpuScreenSet(&screen_, 100u, 50u, 0x02345u);
  EXPECT_EQ(0x02345u, screen_.pixels[50u][100u]);
}