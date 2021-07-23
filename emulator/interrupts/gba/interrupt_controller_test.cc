extern "C" {
#include "emulator/interrupts/gba/interrupt_controller.h"
}

#include "googletest/include/gtest/gtest.h"

class InterruptControllerTest : public testing::Test {
 public:
  void SetUp() override {
    ASSERT_TRUE(GbaInterruptControllerAllocate(&interrupt_controller_, &rst_,
                                               &fiq_, &irq_));
  }

  void TearDown() override {
    GbaInterruptControllerRelease(interrupt_controller_);
    InterruptLineFree(rst_);
    InterruptLineFree(fiq_);
    InterruptLineFree(irq_);
  }

  GbaInterruptController *interrupt_controller_;
  InterruptLine *rst_;
  InterruptLine *fiq_;
  InterruptLine *irq_;
};

TEST_F(InterruptControllerTest, GbaInterruptControllerInterruptMasterEnable) {
  EXPECT_EQ(0u, GbaInterruptControllerReadInterruptMasterEnable(
                    interrupt_controller_));
  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_,
                                                   0xFFFFu);
  EXPECT_EQ(1u, GbaInterruptControllerReadInterruptMasterEnable(
                    interrupt_controller_));
  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 0u);
  EXPECT_EQ(0u, GbaInterruptControllerReadInterruptMasterEnable(
                    interrupt_controller_));
}

TEST_F(InterruptControllerTest, GbaInterruptControllerInterruptEnable) {
  EXPECT_EQ(0u,
            GbaInterruptControllerReadInterruptEnable(interrupt_controller_));
  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 0xFFFFu);
  EXPECT_EQ(0xFFFFu,
            GbaInterruptControllerReadInterruptEnable(interrupt_controller_));
  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 0u);
  EXPECT_EQ(0u,
            GbaInterruptControllerReadInterruptEnable(interrupt_controller_));
}

TEST_F(InterruptControllerTest, GbaInterruptControllerInterruptAcknowledge) {
  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_,
                                                   0xFFFFu);
  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 0xFFFFu);

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

  EXPECT_EQ(0x3FFFu, GbaInterruptControllerReadInterruptRequestFlags(
                         interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 2u);

  EXPECT_EQ(0x3FFDu, GbaInterruptControllerReadInterruptRequestFlags(
                         interrupt_controller_));
}

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseVBlankInterrupt) {
  GbaInterruptControllerRaiseVBlankInterrupt(interrupt_controller_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(1u, GbaInterruptControllerReadInterruptRequestFlags(
                    interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 1u);
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

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(1u, GbaInterruptControllerReadInterruptRequestFlags(
                    interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseHBlankInterrupt) {
  GbaInterruptControllerRaiseHBlankInterrupt(interrupt_controller_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 2u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(2u, GbaInterruptControllerReadInterruptRequestFlags(
                    interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 2u);
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

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 2u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(2u, GbaInterruptControllerReadInterruptRequestFlags(
                    interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 2u);
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

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 4u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(4u, GbaInterruptControllerReadInterruptRequestFlags(
                    interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 4u);
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

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 4u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(4u, GbaInterruptControllerReadInterruptRequestFlags(
                    interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 4u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseTimer0Interrupt) {
  GbaInterruptControllerRaiseTimer0Interrupt(interrupt_controller_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 8u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(8u, GbaInterruptControllerReadInterruptRequestFlags(
                    interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 8u);
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

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 8u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(8u, GbaInterruptControllerReadInterruptRequestFlags(
                    interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 8u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseTimer1Interrupt) {
  GbaInterruptControllerRaiseTimer1Interrupt(interrupt_controller_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 16u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(16u, GbaInterruptControllerReadInterruptRequestFlags(
                     interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 16u);
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

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 16u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(16u, GbaInterruptControllerReadInterruptRequestFlags(
                     interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 16u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseTimer2Interrupt) {
  GbaInterruptControllerRaiseTimer2Interrupt(interrupt_controller_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 32u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(32u, GbaInterruptControllerReadInterruptRequestFlags(
                     interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 32u);
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

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 32u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(32u, GbaInterruptControllerReadInterruptRequestFlags(
                     interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 32u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseTimer3Interrupt) {
  GbaInterruptControllerRaiseTimer3Interrupt(interrupt_controller_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 64u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(64u, GbaInterruptControllerReadInterruptRequestFlags(
                     interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 64u);
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

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 64u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(64u, GbaInterruptControllerReadInterruptRequestFlags(
                     interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 64u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseSerialInterrupt) {
  GbaInterruptControllerRaiseSerialInterrupt(interrupt_controller_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 128u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(128u, GbaInterruptControllerReadInterruptRequestFlags(
                      interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 128u);
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

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 128u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(128u, GbaInterruptControllerReadInterruptRequestFlags(
                      interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 128u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseDma0Interrupt) {
  GbaInterruptControllerRaiseDma0Interrupt(interrupt_controller_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 256u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(256u, GbaInterruptControllerReadInterruptRequestFlags(
                      interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 256u);
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

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 256u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(256u, GbaInterruptControllerReadInterruptRequestFlags(
                      interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 256u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseDma1Interrupt) {
  GbaInterruptControllerRaiseDma1Interrupt(interrupt_controller_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 512u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(512u, GbaInterruptControllerReadInterruptRequestFlags(
                      interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 512u);
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

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 512u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(512u, GbaInterruptControllerReadInterruptRequestFlags(
                      interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 512u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseDma2Interrupt) {
  GbaInterruptControllerRaiseDma2Interrupt(interrupt_controller_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 1024u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(1024u, GbaInterruptControllerReadInterruptRequestFlags(
                       interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 1024u);
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

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 1024u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(1024u, GbaInterruptControllerReadInterruptRequestFlags(
                       interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 1024u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseDma3Interrupt) {
  GbaInterruptControllerRaiseDma3Interrupt(interrupt_controller_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 2048u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(2048u, GbaInterruptControllerReadInterruptRequestFlags(
                       interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 2048u);
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

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 2048u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(2048u, GbaInterruptControllerReadInterruptRequestFlags(
                       interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 2048u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseKeypadInterrupt) {
  GbaInterruptControllerRaiseKeypadInterrupt(interrupt_controller_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 4096u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(4096u, GbaInterruptControllerReadInterruptRequestFlags(
                       interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 4096u);
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

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 4096u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(4096u, GbaInterruptControllerReadInterruptRequestFlags(
                       interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 4096u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}

TEST_F(InterruptControllerTest, GbaInterruptControllerRaiseCartridgeInterrupt) {
  GbaInterruptControllerRaiseCartridgeInterrupt(interrupt_controller_);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 8192u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(8192u, GbaInterruptControllerReadInterruptRequestFlags(
                       interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 8192u);
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

  GbaInterruptControllerWriteInterruptEnable(interrupt_controller_, 8192u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));

  GbaInterruptControllerWriteInterruptMasterEnable(interrupt_controller_, 1u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_TRUE(InterruptLineIsRaised(irq_));

  EXPECT_EQ(8192u, GbaInterruptControllerReadInterruptRequestFlags(
                       interrupt_controller_));

  GbaInterruptControllerInterruptAcknowledge(interrupt_controller_, 8192u);
  EXPECT_FALSE(InterruptLineIsRaised(rst_));
  EXPECT_FALSE(InterruptLineIsRaised(fiq_));
  EXPECT_FALSE(InterruptLineIsRaised(irq_));
}