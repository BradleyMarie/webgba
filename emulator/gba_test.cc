extern "C" {
#include "emulator/gba.h"
}

#include "googletest/include/gtest/gtest.h"

class GbaEmulatorTest : public testing::Test {
 public:
  void SetUp() override {
    static const char rom[100] = {};
    ASSERT_TRUE(GbaEmulatorAllocate(rom, 100u, &gba_, &gamepad_));
  }

  void TearDown() override {
    GbaEmulatorFree(gba_);
    GamePadFree(gamepad_);
  }

 protected:
  GbaEmulator *gba_;
  GamePad *gamepad_;
};

TEST_F(GbaEmulatorTest, EmptyTest) { GbaEmulatorStep(gba_, 0, nullptr); }