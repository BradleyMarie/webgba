extern "C" {
#include "emulator/gba.h"
}

#include "googletest/include/gtest/gtest.h"

class GbaEmulatorTest : public testing::Test {
 public:
  void SetUp() override {
    static const unsigned char rom[100] = {};
    ASSERT_TRUE(GbaEmulatorAllocate(rom, 100u, &gba_, &gamepad_));
    screen_ = ScreenAllocate();
    ASSERT_TRUE(screen_);
  }

  void TearDown() override {
    GbaEmulatorFree(gba_);
    GamePadFree(gamepad_);
    ScreenFree(screen_);
  }

  static void AudioCallback(int16_t left, int16_t right) {}

 protected:
  GbaEmulator *gba_;
  GamePad *gamepad_;
  Screen *screen_;
};

TEST_F(GbaEmulatorTest, EmptyTest) {
  GbaEmulatorStep(gba_, screen_, AudioCallback);
}