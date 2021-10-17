extern "C" {
#include "emulator/timers/gba/timers.h"
}

#include "googletest/include/gtest/gtest.h"

#define IE_OFFSET 0x0u
#define IF_OFFSET 0x2u
#define IME_OFFSET 0x8u

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
    raised_ = false;
    InterruptLine *irq =
        InterruptLineAllocate(nullptr, InterruptSetLevel, nullptr);
    ASSERT_NE(irq, nullptr);
    ASSERT_TRUE(GbaPlatformAllocate(irq, &plat_, &plat_regs_));
    ASSERT_TRUE(
        GbaDmaUnitAllocate(plat_, &dma_unit_, &dma_unit_registers_));
    ASSERT_TRUE(GbaSpuAllocate(dma_unit_, &spu_, &spu_registers_));
    ASSERT_TRUE(GbaTimersAllocate(plat_, spu_, &timers_, &regs_));
    ASSERT_TRUE(Store16LE(plat_regs_, IE_OFFSET, 0xFFFFu));
    ASSERT_TRUE(Store16LE(plat_regs_, IF_OFFSET, 0xFFFFu));
    ASSERT_TRUE(Store16LE(plat_regs_, IME_OFFSET, 0xFFFFu));
  }

  void TearDown() override {
    GbaPlatformRelease(plat_);
    MemoryFree(plat_regs_);
    GbaDmaUnitRelease(dma_unit_);
    MemoryFree(dma_unit_registers_);
    GbaSpuRelease(spu_);
    MemoryFree(spu_registers_);
    GbaTimersFree(timers_);
    MemoryFree(regs_);
  }

 protected:
  static void InterruptSetLevel(void *context, bool raised) {
    raised_ = raised;
  }

  static bool raised_;

  GbaPlatform *plat_;
  Memory *plat_regs_;
  GbaDmaUnit *dma_unit_;
  Memory *dma_unit_registers_;
  GbaSpu *spu_;
  Memory *spu_registers_;
  GbaTimers *timers_;
  Memory *regs_;
};

bool TimersTest::raised_ = false;

TEST_F(TimersTest, GbaTimersRegistersLoad32LENotStarted) {
  uint32_t contents;
  EXPECT_TRUE(Store32LE(regs_, TM0CNT_L_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, TM0CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x11220000u, contents);
  EXPECT_TRUE(Store32LE(regs_, TM1CNT_L_OFFSET, 0x55667788u));
  EXPECT_TRUE(Load32LE(regs_, TM1CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x55660000u, contents);
  EXPECT_TRUE(Store32LE(regs_, TM2CNT_L_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, TM2CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x11220000u, contents);
  EXPECT_TRUE(Store32LE(regs_, TM3CNT_L_OFFSET, 0x55667788u));
  EXPECT_TRUE(Load32LE(regs_, TM3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x55660000u, contents);

  EXPECT_FALSE(raised_);
}

TEST_F(TimersTest, GbaTimersRegistersLoad32LEStarted) {
  uint32_t contents;
  EXPECT_TRUE(Store32LE(regs_, TM0CNT_L_OFFSET, 0xAABB3344u));
  EXPECT_TRUE(Load32LE(regs_, TM0CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xAABB3344u, contents);
  EXPECT_TRUE(Store32LE(regs_, TM1CNT_L_OFFSET, 0xAABB7788u));
  EXPECT_TRUE(Load32LE(regs_, TM1CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xAABB7788u, contents);
  EXPECT_TRUE(Store32LE(regs_, TM2CNT_L_OFFSET, 0xAABB3344u));
  EXPECT_TRUE(Load32LE(regs_, TM2CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xAABB3344u, contents);
  EXPECT_TRUE(Store32LE(regs_, TM3CNT_L_OFFSET, 0xAABB7788u));
  EXPECT_TRUE(Load32LE(regs_, TM3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xAABB7788u, contents);

  EXPECT_FALSE(raised_);
}

TEST_F(TimersTest, GbaTimersRegistersLoad16LENotStarted) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, TM0CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x0u, contents);
  EXPECT_TRUE(Store16LE(regs_, TM0CNT_H_OFFSET, 0xAABBu));
  EXPECT_TRUE(Load16LE(regs_, TM0CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAABBu, contents);
  EXPECT_TRUE(Load16LE(regs_, TM0CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x1122u, contents);
  EXPECT_TRUE(Store16LE(regs_, TM1CNT_L_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, TM1CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x0u, contents);
  EXPECT_TRUE(Store16LE(regs_, TM1CNT_H_OFFSET, 0xAABBu));
  EXPECT_TRUE(Load16LE(regs_, TM1CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAABBu, contents);
  EXPECT_TRUE(Load16LE(regs_, TM1CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x1122u, contents);
  EXPECT_TRUE(Store16LE(regs_, TM2CNT_L_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, TM2CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x0u, contents);
  EXPECT_TRUE(Store16LE(regs_, TM2CNT_H_OFFSET, 0xAABBu));
  EXPECT_TRUE(Load16LE(regs_, TM2CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAABBu, contents);
  EXPECT_TRUE(Load16LE(regs_, TM2CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x1122u, contents);
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_L_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x0u, contents);
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_H_OFFSET, 0xAABBu));
  EXPECT_TRUE(Load16LE(regs_, TM3CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAABBu, contents);
  EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x1122u, contents);

  EXPECT_FALSE(raised_);
}

TEST_F(TimersTest, GbaTimersRegistersLoad8) {
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
  EXPECT_TRUE(Store8(regs_, TM0CNT_H_OFFSET, 0xAAu));
  EXPECT_TRUE(Load8(regs_, TM0CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAAu, contents);
  EXPECT_TRUE(Load8(regs_, TM0CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x11u, contents);
  EXPECT_TRUE(Load8(regs_, TM0CNT_L_OFFSET + 1u, &contents));
  EXPECT_EQ(0x22u, contents);
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
  EXPECT_TRUE(Store8(regs_, TM1CNT_H_OFFSET, 0xAAu));
  EXPECT_TRUE(Load8(regs_, TM1CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAAu, contents);
  EXPECT_TRUE(Load8(regs_, TM1CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x11u, contents);
  EXPECT_TRUE(Load8(regs_, TM1CNT_L_OFFSET + 1u, &contents));
  EXPECT_EQ(0x22u, contents);
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
  EXPECT_TRUE(Store8(regs_, TM2CNT_H_OFFSET, 0xAAu));
  EXPECT_TRUE(Load8(regs_, TM2CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAAu, contents);
  EXPECT_TRUE(Load8(regs_, TM2CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x11u, contents);
  EXPECT_TRUE(Load8(regs_, TM2CNT_L_OFFSET + 1u, &contents));
  EXPECT_EQ(0x22u, contents);
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
  EXPECT_TRUE(Store8(regs_, TM3CNT_H_OFFSET, 0xAAu));
  EXPECT_TRUE(Load8(regs_, TM3CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAAu, contents);
  EXPECT_TRUE(Load8(regs_, TM3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x11u, contents);
  EXPECT_TRUE(Load8(regs_, TM3CNT_L_OFFSET + 1u, &contents));
  EXPECT_EQ(0x22u, contents);

  EXPECT_FALSE(raised_);
}

TEST_F(TimersTest, OutOfBounds) {
  EXPECT_FALSE(Store32LE(regs_, TM3CNT_H_OFFSET + 2u, 0xFFFFFFFFu));
  EXPECT_FALSE(Store16LE(regs_, TM3CNT_H_OFFSET + 2u, 0xFFFFu));
  EXPECT_FALSE(Store8(regs_, TM3CNT_H_OFFSET + 2u, 0xFFu));
  EXPECT_FALSE(Load32LE(regs_, TM3CNT_H_OFFSET + 2u, nullptr));
  EXPECT_FALSE(Load16LE(regs_, TM3CNT_H_OFFSET + 2u, nullptr));
  EXPECT_FALSE(Load8(regs_, TM3CNT_H_OFFSET + 2u, nullptr));

  EXPECT_FALSE(raised_);
}

TEST_F(TimersTest, OneCycleCounter) {
  EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Store16LE(regs_, TM0CNT_H_OFFSET, 0xC0u));
  EXPECT_FALSE(raised_);

  uint16_t contents;
  EXPECT_TRUE(Load16LE(regs_, TM0CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFFu, contents);

  EXPECT_TRUE(Store16LE(regs_, TM3CNT_L_OFFSET, 0u));  // No Op

  GbaTimersStep(timers_);
  EXPECT_TRUE(raised_);

  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &contents));
  EXPECT_EQ(1u << 3u, contents);

  EXPECT_TRUE(Load16LE(regs_, TM0CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFFu, contents);
}

TEST_F(TimersTest, SixtyFourCycleCounter) {
  EXPECT_TRUE(Store16LE(regs_, TM1CNT_L_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Store16LE(regs_, TM1CNT_H_OFFSET, 0xC1u));
  EXPECT_FALSE(raised_);

  for (uint16_t i = 0; i < 63u; i++) {
    EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0u));  // No Op
    uint16_t contents;
    EXPECT_TRUE(Load16LE(regs_, TM1CNT_L_OFFSET, &contents));
    EXPECT_EQ(0xFFFFu, contents);
    GbaTimersStep(timers_);
    EXPECT_FALSE(raised_);
  }

  EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0u));  // No Op

  GbaTimersStep(timers_);
  EXPECT_TRUE(raised_);

  uint16_t contents;
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &contents));
  EXPECT_EQ(1u << 4u, contents);

  EXPECT_TRUE(Load16LE(regs_, TM1CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFFu, contents);
}

TEST_F(TimersTest, TwoFiftySixCycleCounter) {
  EXPECT_TRUE(Store16LE(regs_, TM2CNT_L_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Store16LE(regs_, TM2CNT_H_OFFSET, 0xC2u));
  EXPECT_FALSE(raised_);

  for (uint16_t i = 0; i < 255u; i++) {
    EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0u));  // No Op
    GbaTimersStep(timers_);
    uint16_t contents;
    EXPECT_TRUE(Load16LE(regs_, TM2CNT_L_OFFSET, &contents));
    EXPECT_EQ(0xFFFFu, contents);
    EXPECT_FALSE(raised_);
  }

  EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0u));  // No Op

  GbaTimersStep(timers_);
  EXPECT_TRUE(raised_);

  uint16_t contents;
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &contents));
  EXPECT_EQ(1u << 5u, contents);

  EXPECT_TRUE(Load16LE(regs_, TM2CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFFu, contents);
}

TEST_F(TimersTest, FiveTwelveCycleCounter) {
  EXPECT_TRUE(Store16LE(regs_, TM2CNT_L_OFFSET, 0xFFFEu));
  EXPECT_TRUE(Store16LE(regs_, TM2CNT_H_OFFSET, 0xC2u));
  EXPECT_FALSE(raised_);

  for (uint16_t i = 0; i < 511u; i++) {
    EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0u));  // No Op
    uint16_t contents;
    EXPECT_TRUE(Load16LE(regs_, TM2CNT_L_OFFSET, &contents));
    EXPECT_EQ(0xFFFEu + i / 256u, contents);
    GbaTimersStep(timers_);
    EXPECT_FALSE(raised_);
  }

  EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0u));  // No Op

  GbaTimersStep(timers_);
  EXPECT_TRUE(raised_);

  uint16_t contents;
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &contents));
  EXPECT_EQ(1u << 5u, contents);

  EXPECT_TRUE(Load16LE(regs_, TM2CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFEu, contents);
}

TEST_F(TimersTest, TenTwentyFourCycleCounter) {
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_L_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_H_OFFSET, 0xC3u));
  EXPECT_FALSE(raised_);

  for (uint16_t i = 0; i < 1023u; i++) {
    EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0u));  // No Op
    uint16_t contents;
    EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
    EXPECT_EQ(0xFFFFu, contents);
    GbaTimersStep(timers_);
    EXPECT_FALSE(raised_);
  }

  EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0u));  // No Op

  GbaTimersStep(timers_);
  EXPECT_TRUE(raised_);

  uint16_t contents;
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &contents));
  EXPECT_EQ(1u << 6u, contents);

  EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFFu, contents);
}

TEST_F(TimersTest, ZeroCounter) {
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_L_OFFSET, 0u));
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_H_OFFSET, 0xC0u));
  EXPECT_FALSE(raised_);

  for (uint32_t i = 0; i < UINT16_MAX; i++) {
    EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0u));  // No Op
    uint16_t contents;
    EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
    EXPECT_EQ(i, contents);
    GbaTimersStep(timers_);
    EXPECT_FALSE(raised_);
  }

  EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0u));  // No Op

  GbaTimersStep(timers_);
  EXPECT_TRUE(raised_);

  uint16_t contents;
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &contents));
  EXPECT_EQ(1u << 6u, contents);

  EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
}

TEST_F(TimersTest, WriteWhileZeroTimerIsRunning) {
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_L_OFFSET, 0u));
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_H_OFFSET, 0xC0u));
  EXPECT_FALSE(raised_);

  for (uint32_t i = 0; i < UINT16_MAX; i++) {
    EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0u));  // No Op
    uint16_t contents;
    EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
    EXPECT_EQ(i, contents);
    GbaTimersStep(timers_);
    EXPECT_FALSE(raised_);
  }

  EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0u));  // No Op

  GbaTimersStep(timers_);
  EXPECT_TRUE(raised_);

  uint16_t contents;
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &contents));
  EXPECT_EQ(1u << 6u, contents);

  EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
}

TEST_F(TimersTest, Cascaded) {
  EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Store16LE(regs_, TM0CNT_H_OFFSET, 0x81u));
  EXPECT_TRUE(Store16LE(regs_, TM1CNT_L_OFFSET, 0xFFFEu));
  EXPECT_TRUE(Store16LE(regs_, TM1CNT_H_OFFSET, 0xC7));
  EXPECT_FALSE(raised_);

  for (uint32_t i = 0u; i < 127u; i++) {
    uint16_t contents;
    EXPECT_TRUE(Load16LE(regs_, TM0CNT_L_OFFSET, &contents));
    EXPECT_EQ(0xFFFFu, contents);
    EXPECT_TRUE(Load16LE(regs_, TM1CNT_L_OFFSET, &contents));
    EXPECT_EQ(0xFFFEu + i / 64u, contents);
    GbaTimersStep(timers_);
    EXPECT_FALSE(raised_);
  }

  GbaTimersStep(timers_);
  EXPECT_TRUE(raised_);

  uint16_t contents;
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &contents));
  EXPECT_EQ(1u << 4u, contents);

  EXPECT_TRUE(Load16LE(regs_, TM0CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFFu, contents);
  EXPECT_TRUE(Load16LE(regs_, TM1CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFEu, contents);
}

TEST_F(TimersTest, WriteWhileCascadedTimersAreRunning) {
  EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Store16LE(regs_, TM0CNT_H_OFFSET, 0x81u));
  EXPECT_TRUE(Store16LE(regs_, TM1CNT_L_OFFSET, 0xFFFEu));
  EXPECT_TRUE(Store16LE(regs_, TM1CNT_H_OFFSET, 0xC7));
  EXPECT_FALSE(raised_);

  for (uint32_t i = 0u; i < 127u; i++) {
    EXPECT_TRUE(Store16LE(regs_, TM3CNT_L_OFFSET, 0xFFFFu));  // No Op
    uint16_t contents;
    EXPECT_TRUE(Load16LE(regs_, TM0CNT_L_OFFSET, &contents));
    EXPECT_EQ(0xFFFFu, contents);
    EXPECT_TRUE(Load16LE(regs_, TM1CNT_L_OFFSET, &contents));
    EXPECT_EQ(0xFFFEu + i / 64u, contents);
    GbaTimersStep(timers_);
    EXPECT_FALSE(raised_);
  }

  EXPECT_TRUE(Store16LE(regs_, TM3CNT_L_OFFSET, 0u));  // No Op

  GbaTimersStep(timers_);
  EXPECT_TRUE(raised_);

  uint16_t contents;
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &contents));
  EXPECT_EQ(1u << 4u, contents);

  EXPECT_TRUE(Load16LE(regs_, TM0CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFFu, contents);
  EXPECT_TRUE(Load16LE(regs_, TM1CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFEu, contents);
}

TEST_F(TimersTest, Repeats) {
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_L_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_H_OFFSET, 0xC3u));
  EXPECT_FALSE(raised_);

  for (uint16_t i = 0; i < 1023u; i++) {
    uint16_t contents;
    EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
    EXPECT_EQ(0xFFFFu, contents);
    GbaTimersStep(timers_);
    EXPECT_FALSE(raised_);
  }

  GbaTimersStep(timers_);
  EXPECT_TRUE(raised_);

  uint16_t contents;
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &contents));
  EXPECT_EQ(1u << 6u, contents);

  EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFFu, contents);

  EXPECT_TRUE(Store16LE(plat_regs_, IF_OFFSET, 0xFFFFu));
  EXPECT_FALSE(raised_);

  for (uint16_t i = 0; i < 1023u; i++) {
    EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
    EXPECT_EQ(0xFFFFu, contents);
    GbaTimersStep(timers_);
    EXPECT_FALSE(raised_);
  }

  GbaTimersStep(timers_);
  EXPECT_TRUE(raised_);
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &contents));
  EXPECT_EQ(1u << 6u, contents);

  EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFFu, contents);
}

TEST_F(TimersTest, WriteWhileRepeatedTimerIsRunning) {
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_L_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_H_OFFSET, 0xC3u));
  EXPECT_FALSE(raised_);

  for (uint16_t i = 0; i < 1023u; i++) {
    EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0u));  // No Op
    uint16_t contents;
    EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
    EXPECT_EQ(0xFFFFu, contents);
    GbaTimersStep(timers_);
    EXPECT_FALSE(raised_);
  }

  EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0u));  // No Op

  GbaTimersStep(timers_);
  EXPECT_TRUE(raised_);

  uint16_t contents;
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &contents));
  EXPECT_EQ(1u << 6u, contents);

  EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFFu, contents);

  EXPECT_TRUE(Store16LE(plat_regs_, IF_OFFSET, 0xFFFFu));
  EXPECT_FALSE(raised_);

  for (uint16_t i = 0; i < 1023u; i++) {
    EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0u));  // No Op
    EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
    EXPECT_EQ(0xFFFFu, contents);
    GbaTimersStep(timers_);
    EXPECT_FALSE(raised_);
  }

  EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0u));  // No Op

  GbaTimersStep(timers_);
  EXPECT_TRUE(raised_);
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &contents));
  EXPECT_EQ(1u << 6u, contents);

  EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFFu, contents);
}

TEST_F(TimersTest, TwoSixtyFourCycleCounters) {
  EXPECT_TRUE(Store16LE(regs_, TM0CNT_L_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Store16LE(regs_, TM0CNT_H_OFFSET, 0xC1u));
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_L_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Store16LE(regs_, TM3CNT_H_OFFSET, 0xC1u));
  EXPECT_FALSE(raised_);

  for (uint16_t i = 0; i < 63u; i++) {
    EXPECT_TRUE(Store16LE(regs_, TM2CNT_L_OFFSET, 0u));  // No Op
    uint16_t contents;
    EXPECT_TRUE(Load16LE(regs_, TM0CNT_L_OFFSET, &contents));
    EXPECT_EQ(0xFFFFu, contents);
    EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
    EXPECT_EQ(0xFFFFu, contents);
    GbaTimersStep(timers_);
    EXPECT_FALSE(raised_);
  }

  EXPECT_TRUE(Store16LE(regs_, TM2CNT_L_OFFSET, 0u));  // No Op

  GbaTimersStep(timers_);
  EXPECT_TRUE(raised_);

  uint16_t contents;
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &contents));
  EXPECT_EQ((1u << 3u) | (1u << 6u), contents);

  EXPECT_TRUE(Load16LE(regs_, TM0CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFFu, contents);

  EXPECT_TRUE(Load16LE(regs_, TM3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFFu, contents);
}