extern "C" {
#include "emulator/interrupts/gba/interrupt_controller.h"
}

#include "googletest/include/gtest/gtest.h"

#define IE_OFFSET 0u
#define IF_OFFSET 2u
#define WAITCNT_OFFSET 4u
#define IME_OFFSET 8u

class InterruptControllerTest : public testing::Test {
 public:
  void SetUp() override {
    ASSERT_TRUE(GbaInterruptControllerAllocate(
        &interrupt_controller_, &registers_, &rst_, &fiq_, &irq_));
  }

  void TearDown() override {
    GbaInterruptControllerRelease(interrupt_controller_);
    MemoryFree(registers_);
    InterruptLineFree(rst_);
    InterruptLineFree(fiq_);
    InterruptLineFree(irq_);
  }

  GbaInterruptController *interrupt_controller_;
  Memory *registers_;
  InterruptLine *rst_;
  InterruptLine *fiq_;
  InterruptLine *irq_;
};

TEST_F(InterruptControllerTest, GbaInterruptControllerInterruptMasterEnable) {
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

TEST_F(InterruptControllerTest, GbaInterruptControllerInterruptEnable) {
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

TEST_F(InterruptControllerTest, GbaInterruptControllerInterruptAcknowledge) {
  EXPECT_TRUE(Store16LE(registers_, IME_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Store16LE(registers_, IE_OFFSET, 0xFFFFu));

  GbaInterruptControllerRaiseVBlankInterrupt(interrupt_controller_);
  GbaInterruptControllerRaiseHBlankInterrupt(interrupt_controller_);
  GbaInterruptControllerRaiseVBlankCountInterrupt(interrupt_controller_);
  GbaInterruptControllerRaiseTimer0Interrupt(interrupt_controller_);
  GbaInterruptControllerRaiseTimer1Interrupt(interrupt_controller_);
  GbaInterruptControllerRaiseTimer2Interrupt(interrupt_controller_);
  GbaInterruptControllerRaiseTimer3Interrupt(interrupt_controller_);
  GbaInterruptControllerRaiseSerialInterrupt(interrupt_controller_);
  GbaInterruptControllerRaiseDma0Interrupt(interrupt_controller_);
  GbaInterruptControllerRaiseDma1Interrupt(interrupt_controller_);
  GbaInterruptControllerRaiseDma2Interrupt(interrupt_controller_);
  GbaInterruptControllerRaiseDma3Interrupt(interrupt_controller_);
  GbaInterruptControllerRaiseKeypadInterrupt(interrupt_controller_);
  GbaInterruptControllerRaiseCartridgeInterrupt(interrupt_controller_);

  uint16_t value;
  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(0x3FFFu, value);

  EXPECT_TRUE(Store16LE(registers_, IF_OFFSET, 2u));

  EXPECT_TRUE(Load16LE(registers_, IF_OFFSET, &value));
  EXPECT_EQ(0x3FFDu, value);
}

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseVBlankInterrupt) {
  GbaInterruptControllerRaiseVBlankInterrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest,
       GbaInterruptControllerRaiseVBlankInterruptReverse) {
  GbaInterruptControllerRaiseVBlankInterrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseHBlankInterrupt) {
  GbaInterruptControllerRaiseHBlankInterrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest,
       GbaInterruptControllerRaiseHBlankInterruptReverse) {
  GbaInterruptControllerRaiseHBlankInterrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest,
       GbaInterruptControllerRaiseVBlankCountInterrupt) {
  GbaInterruptControllerRaiseVBlankCountInterrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest,
       GbaInterruptControllerRaiseVBlankCountInterruptReverse) {
  GbaInterruptControllerRaiseVBlankCountInterrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseTimer0Interrupt) {
  GbaInterruptControllerRaiseTimer0Interrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest,
       GbaInterruptControllerRaiseTimer0InterruptReverse) {
  GbaInterruptControllerRaiseTimer0Interrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseTimer1Interrupt) {
  GbaInterruptControllerRaiseTimer1Interrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest,
       GbaInterruptControllerRaiseTimer1InterruptReverse) {
  GbaInterruptControllerRaiseTimer1Interrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseTimer2Interrupt) {
  GbaInterruptControllerRaiseTimer2Interrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest,
       GbaInterruptControllerRaiseTimer2InterruptReverse) {
  GbaInterruptControllerRaiseTimer2Interrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseTimer3Interrupt) {
  GbaInterruptControllerRaiseTimer3Interrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest,
       GbaInterruptControllerRaiseTimer3InterruptReverse) {
  GbaInterruptControllerRaiseTimer3Interrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseSerialInterrupt) {
  GbaInterruptControllerRaiseSerialInterrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest,
       GbaInterruptControllerRaiseSerialInterruptReverse) {
  GbaInterruptControllerRaiseSerialInterrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseDma0Interrupt) {
  GbaInterruptControllerRaiseDma0Interrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest,
       GbaInterruptControllerRaiseDma0InterruptReverse) {
  GbaInterruptControllerRaiseDma0Interrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseDma1Interrupt) {
  GbaInterruptControllerRaiseDma1Interrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest,
       GbaInterruptControllerRaiseDma1InterruptReverse) {
  GbaInterruptControllerRaiseDma1Interrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseDma2Interrupt) {
  GbaInterruptControllerRaiseDma2Interrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest,
       GbaInterruptControllerRaiseDma2InterruptReverse) {
  GbaInterruptControllerRaiseDma2Interrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseDma3Interrupt) {
  GbaInterruptControllerRaiseDma3Interrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest,
       GbaInterruptControllerRaiseDma3InterruptReverse) {
  GbaInterruptControllerRaiseDma3Interrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseKeypadInterrupt) {
  GbaInterruptControllerRaiseKeypadInterrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest,
       GbaInterruptControllerRaiseKeypadInterruptReverse) {
  GbaInterruptControllerRaiseKeypadInterrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseCartridgeInterrupt) {
  GbaInterruptControllerRaiseCartridgeInterrupt(interrupt_controller_);
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

TEST_F(InterruptControllerTest,
       GbaInterruptControllerRaiseCartridgeInterruptReverse) {
  GbaInterruptControllerRaiseCartridgeInterrupt(interrupt_controller_);
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