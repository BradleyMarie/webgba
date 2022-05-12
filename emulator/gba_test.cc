extern "C" {
#include "emulator/gba.h"
}

#include "googletest/include/gtest/gtest.h"

class GbaEmulatorTest : public testing::Test {
 public:
  void SetUp() override {
    static const unsigned char rom[100] = {};
    ASSERT_TRUE(GbaEmulatorAllocate(rom, 100u, &gba_, &gamepad_));
  }

  void TearDown() override {
    GbaEmulatorFree(gba_);
    GamePadFree(gamepad_);
  }

  static void AudioCallback(int16_t left, int16_t right) {}

 protected:
  GbaEmulator *gba_;
  GamePad *gamepad_;
  uint8_t fbo_contents_;
};

TEST_F(GbaEmulatorTest, EmptyTest) {
  GbaEmulatorStep(gba_, /*fbo=*/0, /*width=*/1, /*height=*/1, AudioCallback,
                  &fbo_contents_);
}