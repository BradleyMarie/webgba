extern "C" {
#include "emulator/timers/gba/timers.h"
}

#include "googletest/include/gtest/gtest.h"

#define TM0CNT_L_OFFSET 0x00u
#define TM0CNT_H_OFFSET 0x02u
#define TM1CNT_L_OFFSET 0x04u
#define TM1CNT_H_OFFSET 0x06u
#define TM2CNT_L_OFFSET 0x08u
#define TM2CNT_H_OFFSET 0x0Au
#define TM3CNT_L_OFFSET 0x0Cu
#define TM3CNT_H_OFFSET 0x0Eu

class TimersTest : public testing::Test {
 public:
  void SetUp() override {
    ASSERT_TRUE(GbaPlatformAllocate(&plat_, &plat_regs_, &rst_, &fiq_, &irq_));
    ASSERT_TRUE(GbaTimersAllocate(plat_, &timers_, &regs_));
  }

  void TearDown() override {
    GbaPlatformRelease(plat_);
    MemoryFree(plat_regs_);
    InterruptLineFree(rst_);
    InterruptLineFree(fiq_);
    InterruptLineFree(irq_);
    GbaTimersFree(timers_);
    MemoryFree(regs_);
  }

 protected:
  GbaPlatform *plat_;
  Memory *plat_regs_;
  InterruptLine *rst_;
  InterruptLine *fiq_;
  InterruptLine *irq_;
  GbaTimers *timers_;
  Memory *regs_;
};

TEST_F(TimersTest, GbaSpuRegistersLoad32LE) {
  uint32_t contents;
  EXPECT_TRUE(Store32LE(regs_, TM0CNT_L_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, TM0CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x11220000u, contents);
  EXPECT_TRUE(Store32LE(regs_, TM1CNT_L_OFFSET, 0xAABBCCDDu));
  EXPECT_TRUE(Load32LE(regs_, TM1CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xAABB0000u, contents);
  EXPECT_TRUE(Store32LE(regs_, TM2CNT_L_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, TM2CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x11220000u, contents);
  EXPECT_TRUE(Store32LE(regs_, TM3CNT_L_OFFSET, 0xAABBCCDDu));
  EXPECT_TRUE(Load32LE(regs_, TM3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xAABB0000u, contents);
}

TEST_F(TimersTest, GbaSpuRegistersLoad16LE) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, TM0CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x0u, contents);
  EXPECT_TRUE(Store16LE(regs_, TM0CNT_H_OFFSET, 0x3344u));
  EXPECT_TRUE(Load16LE(regs_, TM0CNT_H_OFFSET, &contents));
  EXPECT_EQ(0x3344u, contents);
  EXPECT_TRUE(Store16LE(regs_, TM1CNT_L_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, TM1CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x0u, contents);
  EXPECT_TRUE(Store16LE(regs_, TM1CNT_H_OFFSET, 0x3344u));
  EXPECT_TRUE(Load16LE(regs_, TM1CNT_H_OFFSET, &contents));
  EXPECT_EQ(0x3344u, contents);
  EXPECT_TRUE(Store16LE(regs_, TM2CNT_L_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, TM2CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x0u, contents);
  EXPECT_TRUE(Store16LE(regs_, TM2CNT_H_OFFSET, 0x3344u));
  EXPECT_TRUE(Load16LE(regs_, TM2CNT_H_OFFSET, &contents));
  EXPECT_EQ(0x3344u, contents);
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_L_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x0u, contents);
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_H_OFFSET, 0x3344u));
  EXPECT_TRUE(Load16LE(regs_, TM3CNT_H_OFFSET, &contents));
  EXPECT_EQ(0x3344u, contents);
}

TEST_F(TimersTest, GbaSpuRegistersLoad8) {
  uint8_t contents;
  EXPECT_TRUE(Store8(regs_, TM0CNT_L_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, TM0CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x0u, contents);
  EXPECT_TRUE(Store8(regs_, TM0CNT_L_OFFSET + 1u, 0x22u));
  EXPECT_TRUE(Load8(regs_, TM0CNT_L_OFFSET + 1u, &contents));
  EXPECT_EQ(0x0u, contents);
  EXPECT_TRUE(Store8(regs_, TM0CNT_H_OFFSET, 0x33u));
  EXPECT_TRUE(Load8(regs_, TM0CNT_H_OFFSET, &contents));
  EXPECT_EQ(0x33u, contents);
  EXPECT_TRUE(Store8(regs_, TM0CNT_H_OFFSET + 1u, 0x44u));
  EXPECT_TRUE(Load8(regs_, TM0CNT_H_OFFSET + 1u, &contents));
  EXPECT_EQ(0x44u, contents);
  EXPECT_TRUE(Store8(regs_, TM1CNT_L_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, TM1CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x0u, contents);
  EXPECT_TRUE(Store8(regs_, TM1CNT_L_OFFSET + 1u, 0x22u));
  EXPECT_TRUE(Load8(regs_, TM1CNT_L_OFFSET + 1u, &contents));
  EXPECT_EQ(0x0u, contents);
  EXPECT_TRUE(Store8(regs_, TM1CNT_H_OFFSET, 0x33u));
  EXPECT_TRUE(Load8(regs_, TM1CNT_H_OFFSET, &contents));
  EXPECT_EQ(0x33u, contents);
  EXPECT_TRUE(Store8(regs_, TM1CNT_H_OFFSET + 1u, 0x44u));
  EXPECT_TRUE(Load8(regs_, TM1CNT_H_OFFSET + 1u, &contents));
  EXPECT_EQ(0x44u, contents);
  EXPECT_TRUE(Store8(regs_, TM2CNT_L_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, TM2CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x0u, contents);
  EXPECT_TRUE(Store8(regs_, TM2CNT_L_OFFSET + 1u, 0x22u));
  EXPECT_TRUE(Load8(regs_, TM2CNT_L_OFFSET + 1u, &contents));
  EXPECT_EQ(0x0u, contents);
  EXPECT_TRUE(Store8(regs_, TM2CNT_H_OFFSET, 0x33u));
  EXPECT_TRUE(Load8(regs_, TM2CNT_H_OFFSET, &contents));
  EXPECT_EQ(0x33u, contents);
  EXPECT_TRUE(Store8(regs_, TM2CNT_H_OFFSET + 1u, 0x44u));
  EXPECT_TRUE(Load8(regs_, TM2CNT_H_OFFSET + 1u, &contents));
  EXPECT_EQ(0x44u, contents);
  EXPECT_TRUE(Store8(regs_, TM3CNT_L_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, TM3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x0u, contents);
  EXPECT_TRUE(Store8(regs_, TM3CNT_L_OFFSET + 1u, 0x22u));
  EXPECT_TRUE(Load8(regs_, TM3CNT_L_OFFSET + 1u, &contents));
  EXPECT_EQ(0x0u, contents);
  EXPECT_TRUE(Store8(regs_, TM3CNT_H_OFFSET, 0x33u));
  EXPECT_TRUE(Load8(regs_, TM3CNT_H_OFFSET, &contents));
  EXPECT_EQ(0x33u, contents);
  EXPECT_TRUE(Store8(regs_, TM3CNT_H_OFFSET + 1u, 0x44u));
  EXPECT_TRUE(Load8(regs_, TM3CNT_H_OFFSET + 1u, &contents));
  EXPECT_EQ(0x44u, contents);
}

TEST_F(TimersTest, OutOfBounds) {
  EXPECT_TRUE(Store32LE(regs_, TM3CNT_H_OFFSET + 2u, 0xFFFFFFFFu));
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_H_OFFSET + 2u, 0xFFFFu));
  EXPECT_TRUE(Store8(regs_, TM3CNT_H_OFFSET + 2u, 0xFFu));
  EXPECT_FALSE(Load32LE(regs_, TM3CNT_H_OFFSET + 2u, nullptr));
  EXPECT_FALSE(Load16LE(regs_, TM3CNT_H_OFFSET + 2u, nullptr));
  EXPECT_FALSE(Load8(regs_, TM3CNT_H_OFFSET + 2u, nullptr));
}