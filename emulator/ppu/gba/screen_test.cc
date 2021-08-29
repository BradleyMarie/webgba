extern "C" {
#include "emulator/ppu/gba/screen.h"
}

#include "googletest/include/gtest/gtest.h"

class ScreenTest : public testing::Test {
 public:
  void SetUp() override {
    GbaPpuScreenClear(&screen_);
    GbaPpuScreenReloadContext(&screen_);
  }

  void TearDown() override { GbaPpuScreenDestroy(&screen_); }

 protected:
  GbaPpuScreen screen_;
};

TEST_F(ScreenTest, DrawPixel) {
  GbaPpuScreenDrawPixel(&screen_, 100u, 50u, 0x02345u, 1u);
  EXPECT_EQ(0x02345u, screen_.pixels[50u][100u]);
  EXPECT_EQ(UINT8_MAX - 1u, screen_.priorities[50u][100u]);
  GbaPpuScreenDrawPixel(&screen_, 100u, 50u, 0x0234u, 1u);
  EXPECT_EQ(0x02345u, screen_.pixels[50u][100u]);
  EXPECT_EQ(UINT8_MAX - 1u, screen_.priorities[50u][100u]);
  GbaPpuScreenDrawPixel(&screen_, 100u, 50u, 0x0234u, 0u);
  EXPECT_EQ(0x0234u, screen_.pixels[50u][100u]);
  EXPECT_EQ(UINT8_MAX, screen_.priorities[50u][100u]);
}

TEST_F(ScreenTest, CopyPixel) {
  GbaPpuScreenDrawPixel(&screen_, 100u, 50u, 0x0234u, 1u);
  GbaPpuScreenCopyPixel(&screen_, 100u, 50u, 101u, 51u, 2u);
  EXPECT_EQ(screen_.pixels[50u][100u], screen_.pixels[51u][101u]);
  EXPECT_EQ(UINT8_MAX - 2u, screen_.priorities[51u][101u]);
}