extern "C" {
#include "emulator/ppu/gba/screen.h"
}

#include "googletest/include/gtest/gtest.h"

class ScreenTest : public testing::Test {
 public:
  void SetUp() override { GbaPpuScreenReloadContext(&screen_); }

  void TearDown() override { GbaPpuScreenDestroy(&screen_); }

 protected:
  GbaPpuScreen screen_;
};

TEST_F(ScreenTest, SetPixel) {
  GbaPpuScreenSetPixel(&screen_, 100u, 50u, 0x0234u);
  EXPECT_EQ(0x0234u << 1u, screen_.pixels[50u][100u]);
}

TEST_F(ScreenTest, CopyPixel) {
  GbaPpuScreenSetPixel(&screen_, 100u, 50u, 0x0234u);
  GbaPpuScreenCopyPixel(&screen_, 100u, 50u, 101u, 51u);
  EXPECT_EQ(screen_.pixels[50u][100u], screen_.pixels[51u][101u]);
}