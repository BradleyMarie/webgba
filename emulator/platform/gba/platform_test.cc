extern "C" {
#include "emulator/platform/gba/platform.h"
}

#include "googletest/include/gtest/gtest.h"

#define IE_OFFSET 0u
#define IF_OFFSET 2u
#define WAITCNT_OFFSET 4u
#define IME_OFFSET 8u

class PlatformTest : public testing::Test {
 public:
  void SetUp() override {
    ASSERT_TRUE(GbaPlatformAllocate(&platform_, &low_registers_,
                                    &high_registers_, &rst_, &fiq_, &irq_));
  }

  void TearDown() override {
    GbaPlatformRelease(platform_);
    MemoryFree(low_registers_);
    MemoryFree(high_registers_);
    InterruptLineFree(rst_);
    InterruptLineFree(fiq_);
    InterruptLineFree(irq_);
  }

  GbaPlatform *platform_;
  Memory *low_registers_;
  Memory *high_registers_;
  InterruptLine *rst_;
  InterruptLine *fiq_;
  InterruptLine *irq_;
};

TEST_F(PlatformTest, GbaPlatformInterruptMasterEnable) {
  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IME_OFFSET, &value));
  EXPECT_EQ(0u, value);
  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Load16LE(low_registers_, IME_OFFSET, &value));
  EXPECT_EQ(1u, value);
  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 0u));
  EXPECT_TRUE(Load16LE(low_registers_, IME_OFFSET, &value));
  EXPECT_EQ(0u, value);
}

TEST_F(PlatformTest, GbaPlatformInterruptEnable) {
  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IE_OFFSET, &value));
  EXPECT_EQ(0u, value);
  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Load16LE(low_registers_, IE_OFFSET, &value));
  EXPECT_EQ(0xFFFFu, value);
  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 0u));
  EXPECT_TRUE(Load16LE(low_registers_, IE_OFFSET, &value));
  EXPECT_EQ(0u, value);
}

TEST_F(PlatformTest, GbaPlatformInterruptAcknowledge) {
  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 0xFFFFu));

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
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(0x3FFFu, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 2u));

  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(0x3FFDu, value);
}

TEST_F(PlatformTest, GbaPlatformRaiseVBlankInterrupt) {
  GbaPlatformRaiseVBlankInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(1u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseVBlankInterruptReverse) {
  GbaPlatformRaiseVBlankInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(1u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseHBlankInterrupt) {
  GbaPlatformRaiseHBlankInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 2u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(2u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 2u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseHBlankInterruptReverse) {
  GbaPlatformRaiseHBlankInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 2u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(2u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 2u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseVBlankCountInterrupt) {
  GbaPlatformRaiseVBlankCountInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 4u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(4u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 4u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseVBlankCountInterruptReverse) {
  GbaPlatformRaiseVBlankCountInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 4u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(4u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 4u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer0Interrupt) {
  GbaPlatformRaiseTimer0Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 8u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(8u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 8u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer0InterruptReverse) {
  GbaPlatformRaiseTimer0Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 8u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(8u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 8u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer1Interrupt) {
  GbaPlatformRaiseTimer1Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 16u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(16u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 16u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer1InterruptReverse) {
  GbaPlatformRaiseTimer1Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 16u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(16u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 16u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer2Interrupt) {
  GbaPlatformRaiseTimer2Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 32u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(32u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 32u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer2InterruptReverse) {
  GbaPlatformRaiseTimer2Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 32u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(32u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 32u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer3Interrupt) {
  GbaPlatformRaiseTimer3Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 64u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(64u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 64u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseTimer3InterruptReverse) {
  GbaPlatformRaiseTimer3Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 64u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(64u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 64u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseSerialInterrupt) {
  GbaPlatformRaiseSerialInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 128u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(128u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 128u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseSerialInterruptReverse) {
  GbaPlatformRaiseSerialInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 128u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(128u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 128u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma0Interrupt) {
  GbaPlatformRaiseDma0Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 256u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(256u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 256u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma0InterruptReverse) {
  GbaPlatformRaiseDma0Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 256u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(256u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 256u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma1Interrupt) {
  GbaPlatformRaiseDma1Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 512u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(512u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 512u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma1InterruptReverse) {
  GbaPlatformRaiseDma1Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 512u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(512u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 512u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma2Interrupt) {
  GbaPlatformRaiseDma2Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 1024u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(1024u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 1024u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma2InterruptReverse) {
  GbaPlatformRaiseDma2Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 1024u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(1024u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 1024u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma3Interrupt) {
  GbaPlatformRaiseDma3Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 2048u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(2048u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 2048u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseDma3InterruptReverse) {
  GbaPlatformRaiseDma3Interrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 2048u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(2048u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 2048u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseKeypadInterrupt) {
  GbaPlatformRaiseKeypadInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 4096u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(4096u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 4096u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseKeypadInterruptReverse) {
  GbaPlatformRaiseKeypadInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 4096u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(4096u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 4096u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseCartridgeInterrupt) {
  GbaPlatformRaiseCartridgeInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 8192u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(8192u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 8192u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(PlatformTest, GbaPlatformRaiseCartridgeInterruptReverse) {
  GbaPlatformRaiseCartridgeInterrupt(platform_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IE_OFFSET, 8192u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  EXPECT_TRUE(Store16LE(low_registers_, IME_OFFSET, 1u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  uint16_t value;
  EXPECT_TRUE(Load16LE(low_registers_, IF_OFFSET, &value));
  EXPECT_EQ(8192u, value);

  EXPECT_TRUE(Store16LE(low_registers_, IF_OFFSET, 8192u));
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}