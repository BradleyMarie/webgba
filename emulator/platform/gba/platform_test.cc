extern "C" {
#include "emulator/platform/gba/platform.h"
}

#include "googletest/include/gtest/gtest.h"

#define IE_OFFSET 0u
#define IF_OFFSET 2u
#define WAITCNT_OFFSET 4u
#define IME_OFFSET 8u
#define POSTFLG_OFFSET 0x100u
#define HALTCNT_OFFSET 0x101u

class PlatformTest : public testing::Test {
 public:
  void SetUp() override {
    ASSERT_TRUE(
        GbaPlatformAllocate(&platform_, &registers_, &rst_, &fiq_, &irq_));
  }

  void TearDown() override {
    GbaPlatformRelease(platform_);
    MemoryFree(registers_);
    InterruptLineFree(rst_);
    InterruptLineFree(fiq_);
    InterruptLineFree(irq_);
  }

  GbaPlatform *platform_;
  Memory *registers_;
  InterruptLine *rst_;
  InterruptLine *fiq_;
  InterruptLine *irq_;
};

TEST_F(PlatformTest, GbaPlatformInterruptMasterEnable) {
  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IME_OFFSET, &value));
  EXPECT_EQ(0u, value);
  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Load16LE(registers_, IME_OFFSET, &value));
  EXPECT_EQ(1u, value);
  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 0u));
  EXPECT_TRUE(Load16LE(registers_, IME_OFFSET, &value));
  EXPECT_EQ(0u, value);
}

TEST_F(PlatformTest, GbaPlatformInterruptEnable) {
  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IE_OFFSET, &value));
  EXPECT_EQ(0u, value);
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Load16LE(registers_, IE_OFFSET, &value));
  EXPECT_EQ(0xFFFFu, value);
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 0u));
  EXPECT_TRUE(Load16LE(registers_, IE_OFFSET, &value));
  EXPECT_EQ(0u, value);
}

TEST_F(PlatformTest, GbaPlatformInterruptAcknowledge) {
  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 0xFFFFu));

  GbaPlatformRaiseVBlankInterrupt(platform_);
  GbaPlatformRaiseHBlankInterrupt(platform_);
  GbaPlatformRaiseVBlankCountInterrupt(platform_);
  GbaPlatformRaiseTimer0Interrupt(platform_);
  GbaPlatformRaiseTimer1Interrupt(platform_);
  GbaPlatformRaiseTimer2Interrupt(platform_);
  GbaPlatformRaiseTimer3Interrupt(platform_);
  GbaPlatformRaiseSerialInterrupt(platform_);
  GbaPlatformRaiseDma0Interrupt(platform_);
  GbaPlatformRaiseDma1Interrupt(platform_);
  GbaPlatformRaiseDma2Interrupt(platform_);
  GbaPlatformRaiseDma3Interrupt(platform_);
  GbaPlatformRaiseKeypadInterrupt(platform_);
  GbaPlatformRaiseCartridgeInterrupt(platform_);

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(0x3FFFu, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 2u));

  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(0x3FFDu, value);
}

TEST_F(PlatformTest, GbaPlatformRaiseVBlankInterrupt) {
  GbaPlatformRaiseVBlankInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(1u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseVBlankInterruptReverse) {
  GbaPlatformRaiseVBlankInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(1u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseVBlankInterruptWakes) {
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store8(registers_, HALTCNT_OFFSET, 0u));
  EXPECT_EQ(GBA_POWER_STATE_HALT, GbaPlatformPowerState(platform_));

  GbaPlatformRaiseVBlankInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(GBA_POWER_STATE_RUN, GbaPlatformPowerState(platform_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(1u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseHBlankInterrupt) {
  GbaPlatformRaiseHBlankInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 2u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(2u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 2u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseHBlankInterruptReverse) {
  GbaPlatformRaiseHBlankInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 2u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(2u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 2u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseHBlankInterruptWakes) {
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 2u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store8(registers_, HALTCNT_OFFSET, 0u));
  EXPECT_EQ(GBA_POWER_STATE_HALT, GbaPlatformPowerState(platform_));

  GbaPlatformRaiseHBlankInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(GBA_POWER_STATE_RUN, GbaPlatformPowerState(platform_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(2u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 2u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseVBlankCountInterrupt) {
  GbaPlatformRaiseVBlankCountInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 4u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(4u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 4u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseVBlankCountInterruptReverse) {
  GbaPlatformRaiseVBlankCountInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 4u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(4u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 4u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseVBlankCountInterruptWakes) {
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 4u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store8(registers_, HALTCNT_OFFSET, 0u));
  EXPECT_EQ(GBA_POWER_STATE_HALT, GbaPlatformPowerState(platform_));

  GbaPlatformRaiseVBlankCountInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(GBA_POWER_STATE_RUN, GbaPlatformPowerState(platform_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(4u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 4u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer0Interrupt) {
  GbaPlatformRaiseTimer0Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 8u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(8u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 8u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer0InterruptReverse) {
  GbaPlatformRaiseTimer0Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 8u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(8u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 8u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer0InterruptWakes) {
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 8u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store8(registers_, HALTCNT_OFFSET, 0u));
  EXPECT_EQ(GBA_POWER_STATE_HALT, GbaPlatformPowerState(platform_));

  GbaPlatformRaiseTimer0Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(GBA_POWER_STATE_RUN, GbaPlatformPowerState(platform_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(8u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 8u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer1Interrupt) {
  GbaPlatformRaiseTimer1Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 16u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(16u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 16u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer1InterruptReverse) {
  GbaPlatformRaiseTimer1Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 16u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(16u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 16u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer1InterruptWakes) {
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 16u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store8(registers_, HALTCNT_OFFSET, 0u));
  EXPECT_EQ(GBA_POWER_STATE_HALT, GbaPlatformPowerState(platform_));

  GbaPlatformRaiseTimer1Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(GBA_POWER_STATE_RUN, GbaPlatformPowerState(platform_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(16u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 16u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer2Interrupt) {
  GbaPlatformRaiseTimer2Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 32u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(32u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 32u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer2InterruptReverse) {
  GbaPlatformRaiseTimer2Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 32u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(32u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 32u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer2InterruptWakes) {
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 32u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store8(registers_, HALTCNT_OFFSET, 0u));
  EXPECT_EQ(GBA_POWER_STATE_HALT, GbaPlatformPowerState(platform_));

  GbaPlatformRaiseTimer2Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(GBA_POWER_STATE_RUN, GbaPlatformPowerState(platform_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(32u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 32u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer3Interrupt) {
  GbaPlatformRaiseTimer3Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 64u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(64u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 64u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer3InterruptReverse) {
  GbaPlatformRaiseTimer3Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 64u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(64u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 64u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer3InterruptWakes) {
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 64u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store8(registers_, HALTCNT_OFFSET, 0u));
  EXPECT_EQ(GBA_POWER_STATE_HALT, GbaPlatformPowerState(platform_));

  GbaPlatformRaiseTimer3Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(GBA_POWER_STATE_RUN, GbaPlatformPowerState(platform_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(64u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 64u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseSerialInterrupt) {
  GbaPlatformRaiseSerialInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 128u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(128u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 128u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseSerialInterruptReverse) {
  GbaPlatformRaiseSerialInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 128u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(128u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 128u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseSerialInterruptWakes) {
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 128u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store8(registers_, HALTCNT_OFFSET, 0x80u));
  EXPECT_EQ(GBA_POWER_STATE_STOP, GbaPlatformPowerState(platform_));

  GbaPlatformRaiseSerialInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(GBA_POWER_STATE_RUN, GbaPlatformPowerState(platform_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(128u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 128u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma0Interrupt) {
  GbaPlatformRaiseDma0Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 256u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(256u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 256u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma0InterruptReverse) {
  GbaPlatformRaiseDma0Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 256u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(256u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 256u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma0InterruptWakes) {
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 256u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store8(registers_, HALTCNT_OFFSET, 0u));
  EXPECT_EQ(GBA_POWER_STATE_HALT, GbaPlatformPowerState(platform_));

  GbaPlatformRaiseDma0Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(GBA_POWER_STATE_RUN, GbaPlatformPowerState(platform_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(256u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 256u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma1Interrupt) {
  GbaPlatformRaiseDma1Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 512u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(512u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 512u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma1InterruptReverse) {
  GbaPlatformRaiseDma1Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 512u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(512u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 512u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma1InterruptWakes) {
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 512u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store8(registers_, HALTCNT_OFFSET, 0u));
  EXPECT_EQ(GBA_POWER_STATE_HALT, GbaPlatformPowerState(platform_));

  GbaPlatformRaiseDma1Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(GBA_POWER_STATE_RUN, GbaPlatformPowerState(platform_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(512u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 512u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma2Interrupt) {
  GbaPlatformRaiseDma2Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 1024u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(1024u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 1024u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma2InterruptReverse) {
  GbaPlatformRaiseDma2Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 1024u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(1024u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 1024u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma2InterruptWakes) {
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 1024u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store8(registers_, HALTCNT_OFFSET, 0u));
  EXPECT_EQ(GBA_POWER_STATE_HALT, GbaPlatformPowerState(platform_));

  GbaPlatformRaiseDma2Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(GBA_POWER_STATE_RUN, GbaPlatformPowerState(platform_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(1024u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 1024u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma3Interrupt) {
  GbaPlatformRaiseDma3Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 2048u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(2048u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 2048u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma3InterruptReverse) {
  GbaPlatformRaiseDma3Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 2048u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(2048u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 2048u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma3InterruptWakes) {
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 2048u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store8(registers_, HALTCNT_OFFSET, 0u));
  EXPECT_EQ(GBA_POWER_STATE_HALT, GbaPlatformPowerState(platform_));

  GbaPlatformRaiseDma3Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(GBA_POWER_STATE_RUN, GbaPlatformPowerState(platform_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(2048u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 2048u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseKeypadInterrupt) {
  GbaPlatformRaiseKeypadInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 4096u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(4096u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 4096u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseKeypadInterruptReverse) {
  GbaPlatformRaiseKeypadInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 4096u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(4096u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 4096u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseKeypadInterruptWakes) {
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 4096u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store8(registers_, HALTCNT_OFFSET, 0x80u));
  EXPECT_EQ(GBA_POWER_STATE_STOP, GbaPlatformPowerState(platform_));

  GbaPlatformRaiseKeypadInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(GBA_POWER_STATE_RUN, GbaPlatformPowerState(platform_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(4096u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 4096u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseCartridgeInterrupt) {
  GbaPlatformRaiseCartridgeInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 8192u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(8192u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 8192u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseCartridgeInterruptReverse) {
  GbaPlatformRaiseCartridgeInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 8192u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(8192u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 8192u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseCartridgeInterruptWakes) {
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 8192u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store8(registers_, HALTCNT_OFFSET, 0x80u));
  EXPECT_EQ(GBA_POWER_STATE_STOP, GbaPlatformPowerState(platform_));

  GbaPlatformRaiseCartridgeInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(GBA_POWER_STATE_RUN, GbaPlatformPowerState(platform_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(8192u, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 8192u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformSramWaitStateCycles) {
  EXPECT_EQ(4u, GbaPlatformSramWaitStateCycles(platform_));
  EXPECT_TRUE(Store16LE(registers_, WAITCNT_OFFSET, 0x0001u));
  EXPECT_EQ(3u, GbaPlatformSramWaitStateCycles(platform_));
  EXPECT_TRUE(Store16LE(registers_, WAITCNT_OFFSET, 0x0002u));
  EXPECT_EQ(2u, GbaPlatformSramWaitStateCycles(platform_));
  EXPECT_TRUE(Store16LE(registers_, WAITCNT_OFFSET, 0x0003u));
  EXPECT_EQ(8u, GbaPlatformSramWaitStateCycles(platform_));
}

TEST_F(PlatformTest, GbaPlatformRom0FirstAccessWaitCycles) {
  EXPECT_EQ(4u, GbaPlatformRom0FirstAccessWaitCycles(platform_));
  EXPECT_TRUE(Store16LE(registers_, WAITCNT_OFFSET, 0x0004u));
  EXPECT_EQ(3u, GbaPlatformRom0FirstAccessWaitCycles(platform_));
  EXPECT_TRUE(Store16LE(registers_, WAITCNT_OFFSET, 0x0008u));
  EXPECT_EQ(2u, GbaPlatformRom0FirstAccessWaitCycles(platform_));
  EXPECT_TRUE(Store16LE(registers_, WAITCNT_OFFSET, 0x000Cu));
  EXPECT_EQ(8u, GbaPlatformRom0FirstAccessWaitCycles(platform_));
}

TEST_F(PlatformTest, GbaPlatformRom0SecondAccessWaitCycles) {
  EXPECT_EQ(2u, GbaPlatformRom0SecondAccessWaitCycles(platform_));
  EXPECT_TRUE(Store16LE(registers_, WAITCNT_OFFSET, 0x0010u));
  EXPECT_EQ(1u, GbaPlatformRom0SecondAccessWaitCycles(platform_));
}

TEST_F(PlatformTest, GbaPlatformRom1FirstAccessWaitCycles) {
  EXPECT_EQ(4u, GbaPlatformRom1FirstAccessWaitCycles(platform_));
  EXPECT_TRUE(Store16LE(registers_, WAITCNT_OFFSET, 0x0020u));
  EXPECT_EQ(3u, GbaPlatformRom1FirstAccessWaitCycles(platform_));
  EXPECT_TRUE(Store16LE(registers_, WAITCNT_OFFSET, 0x0040u));
  EXPECT_EQ(2u, GbaPlatformRom1FirstAccessWaitCycles(platform_));
  EXPECT_TRUE(Store16LE(registers_, WAITCNT_OFFSET, 0x0060u));
  EXPECT_EQ(8u, GbaPlatformRom1FirstAccessWaitCycles(platform_));
}

TEST_F(PlatformTest, GbaPlatformRom1SecondAccessWaitCycles) {
  EXPECT_EQ(4u, GbaPlatformRom1SecondAccessWaitCycles(platform_));
  EXPECT_TRUE(Store16LE(registers_, WAITCNT_OFFSET, 0x0080u));
  EXPECT_EQ(1u, GbaPlatformRom1SecondAccessWaitCycles(platform_));
}

TEST_F(PlatformTest, GbaPlatformRom2FirstAccessWaitCycles) {
  EXPECT_EQ(4u, GbaPlatformRom2FirstAccessWaitCycles(platform_));
  EXPECT_TRUE(Store16LE(registers_, WAITCNT_OFFSET, 0x0100u));
  EXPECT_EQ(3u, GbaPlatformRom2FirstAccessWaitCycles(platform_));
  EXPECT_TRUE(Store16LE(registers_, WAITCNT_OFFSET, 0x0200u));
  EXPECT_EQ(2u, GbaPlatformRom2FirstAccessWaitCycles(platform_));
  EXPECT_TRUE(Store16LE(registers_, WAITCNT_OFFSET, 0x0300u));
  EXPECT_EQ(8u, GbaPlatformRom2FirstAccessWaitCycles(platform_));
}

TEST_F(PlatformTest, GbaPlatformRom2SecondAccessWaitCycles) {
  EXPECT_EQ(8u, GbaPlatformRom2SecondAccessWaitCycles(platform_));
  EXPECT_TRUE(Store16LE(registers_, WAITCNT_OFFSET, 0x0400u));
  EXPECT_EQ(1u, GbaPlatformRom2SecondAccessWaitCycles(platform_));
}

TEST_F(PlatformTest, GbaPlatformRomPrefetch) {
  EXPECT_FALSE(GbaPlatformRomPrefetch(platform_));
  EXPECT_TRUE(Store16LE(registers_, WAITCNT_OFFSET, 0x4000u));
  EXPECT_TRUE(GbaPlatformRomPrefetch(platform_));
}

TEST_F(PlatformTest, PostFlag) {
  EXPECT_TRUE(Store8(registers_, POSTFLG_OFFSET, 0xDDu));

  uint8_t value8;
  EXPECT_TRUE(Load8(registers_, POSTFLG_OFFSET, &value8));
  EXPECT_EQ(0xDDu, value8);
}

TEST_F(PlatformTest, InternalMemoryControl) {
  uint32_t imc_start = 0x4000800u - 0x4000200u;
  uint32_t imc_spacing = 0x10000u;
  uint32_t num_imc = (0x5000000u - 0x4000800u) / imc_spacing;
  for (uint32_t i = 0; i < num_imc; i++) {
    uint32_t base_address = imc_start + i * imc_spacing;
    EXPECT_TRUE(Store32LE(registers_, base_address, 0xAABBCCDDu));

    uint32_t value32;
    EXPECT_TRUE(Load32LE(registers_, base_address, &value32));
    EXPECT_EQ(0xAABBCCDD, value32);

    EXPECT_TRUE(Store32LE(registers_, base_address, 0u));
    EXPECT_TRUE(Load32LE(registers_, base_address, &value32));
    EXPECT_EQ(0u, value32);

    EXPECT_TRUE(Store16LE(registers_, base_address, 0xAABBu));
    EXPECT_TRUE(Store16LE(registers_, base_address + 2u, 0xCCDDu));

    uint16_t value16;
    EXPECT_TRUE(Load16LE(registers_, base_address, &value16));
    EXPECT_EQ(0xAABBu, value16);
    EXPECT_TRUE(Load16LE(registers_, base_address + 2u, &value16));
    EXPECT_EQ(0xCCDDu, value16);

    EXPECT_TRUE(Store32LE(registers_, base_address, 0u));
    EXPECT_TRUE(Load32LE(registers_, base_address, &value32));
    EXPECT_EQ(0u, value32);

    EXPECT_TRUE(Store16LE(registers_, base_address, 0xAABBu));
    EXPECT_TRUE(Store16LE(registers_, base_address + 2u, 0xCCDDu));

    EXPECT_TRUE(Store8(registers_, base_address, 0xAAu));
    EXPECT_TRUE(Store8(registers_, base_address + 1u, 0xBBu));
    EXPECT_TRUE(Store8(registers_, base_address + 2u, 0xCCu));
    EXPECT_TRUE(Store8(registers_, base_address + 3u, 0xDDu));

    uint8_t value8;
    EXPECT_TRUE(Load8(registers_, base_address, &value8));
    EXPECT_EQ(0xAAu, value8);
    EXPECT_TRUE(Load8(registers_, base_address + 1u, &value8));
    EXPECT_EQ(0xBBu, value8);
    EXPECT_TRUE(Load8(registers_, base_address + 2u, &value8));
    EXPECT_EQ(0xCCu, value8);
    EXPECT_TRUE(Load8(registers_, base_address + 3u, &value8));
    EXPECT_EQ(0xDDu, value8);

    EXPECT_FALSE(Load32LE(registers_, base_address + 4u, &value32));
    EXPECT_FALSE(Load16LE(registers_, base_address + 4u, &value16));
    EXPECT_FALSE(Load8(registers_, base_address + 4u, &value8));
  }
}