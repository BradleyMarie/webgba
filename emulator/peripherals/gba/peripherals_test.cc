extern "C" {
#include "emulator/peripherals/gba/peripherals.h"
}

#include "googletest/include/gtest/gtest.h"

#define IE_OFFSET 0x0u
#define IF_OFFSET 0x2u
#define IME_OFFSET 0x8u

class PeripheralsTest : public testing::Test {
 public:
  void SetUp() override {
    ASSERT_TRUE(GbaPlatformAllocate(&plat_, &plat_regs_, &rst_, &fiq_, &irq_));
    ASSERT_TRUE(
        GbaPeripheralsAllocate(plat_, &peripherals_, &gamepad_, &regs_));

    ASSERT_TRUE(Store16LE(plat_regs_, IE_OFFSET, 0xFFFFu));
    ASSERT_TRUE(Store16LE(plat_regs_, IF_OFFSET, 0xFFFFu));
    ASSERT_TRUE(Store16LE(plat_regs_, IME_OFFSET, 0xFFFFu));
  }

  void TearDown() override {
    GbaPlatformRelease(plat_);
    MemoryFree(plat_regs_);
    InterruptLineFree(rst_);
    InterruptLineFree(fiq_);
    InterruptLineFree(irq_);
    GbaPeripheralsFree(peripherals_);
    GamePadFree(gamepad_);
    MemoryFree(regs_);
  }

 protected:
  GbaPlatform *plat_;
  Memory *plat_regs_;
  InterruptLine *rst_;
  InterruptLine *fiq_;
  InterruptLine *irq_;
  GbaPeripherals *peripherals_;
  GamePad *gamepad_;
  Memory *regs_;
};