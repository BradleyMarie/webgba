extern "C" {
#include "emulator/sound/gba/sound.h"
}

#include "googletest/include/gtest/gtest.h"

#define SOUND1CNT_L_OFFSET 0x00u
#define SOUND1CNT_H_OFFSET 0x02u
#define SOUND1CNT_X_OFFSET 0x04u
#define SOUND2CNT_L_OFFSET 0x08u
#define SOUND2CNT_H_OFFSET 0x0Cu
#define SOUND3CNT_L_OFFSET 0x10u
#define SOUND3CNT_H_OFFSET 0x12u
#define SOUND3CNT_X_OFFSET 0x14u
#define SOUND4CNT_L_OFFSET 0x18u
#define SOUND4CNT_H_OFFSET 0x1Cu
#define SOUNDCNT_L_OFFSET 0x20u
#define SOUNDCNT_H_OFFSET 0x22u
#define SOUNDCNT_X_OFFSET 0x24u
#define SOUNDBIAS_OFFSET 0x28u
#define WAVE_RAM0_L_OFFSET 0x30u
#define WAVE_RAM0_H_OFFSET 0x32u
#define WAVE_RAM1_L_OFFSET 0x34u
#define WAVE_RAM1_H_OFFSET 0x36u
#define WAVE_RAM2_L_OFFSET 0x38u
#define WAVE_RAM2_H_OFFSET 0x3Au
#define WAVE_RAM3_L_OFFSET 0x3Cu
#define WAVE_RAM3_H_OFFSET 0x3Eu
#define FIFO_A_OFFSET 0x40u
#define FIFO_B_OFFSET 0x44u

class SoundTest : public testing::Test {
 public:
  void SetUp() override {
    InterruptLine *irq =
        InterruptLineAllocate(nullptr, InterruptSetLevel, nullptr);
    ASSERT_NE(irq, nullptr);
    ASSERT_TRUE(GbaPlatformAllocate(irq, &platform_, &platform_registers_));
    ASSERT_TRUE(
        GbaDmaUnitAllocate(platform_, &dma_unit_, &dma_unit_registers_));
    ASSERT_TRUE(GbaSpuAllocate(dma_unit_, &spu_, &regs_));
  }

  void TearDown() override {
    GbaPlatformRelease(platform_);
    MemoryFree(platform_registers_);
    GbaDmaUnitRelease(dma_unit_);
    MemoryFree(dma_unit_registers_);
    GbaSpuRelease(spu_);
    MemoryFree(regs_);
  }

  static void InterruptSetLevel(void *context, bool raised) {
    // Do Nothing
  }

 protected:
  GbaPlatform *platform_;
  Memory *platform_registers_;
  GbaDmaUnit *dma_unit_;
  Memory *dma_unit_registers_;
  GbaSpu *spu_;
  Memory *regs_;
};

TEST_F(SoundTest, GbaSpuRegistersLoad32FifoFails) {
  uint32_t contents;
  EXPECT_FALSE(Load32LE(regs_, FIFO_A_OFFSET, &contents));
  EXPECT_FALSE(Load32LE(regs_, FIFO_B_OFFSET, &contents));
}

TEST_F(SoundTest, GbaSpuRegistersLoad32LEUnusedLow) {
  uint32_t contents;
  EXPECT_FALSE(Load32LE(regs_, 0x2Cu, &contents));
}

TEST_F(SoundTest, GbaSpuRegistersLoad32LEUnusedHigh) {
  uint32_t contents = 0xFFFFFFFFu;
  EXPECT_TRUE(Store16LE(regs_, SOUND4CNT_L_OFFSET, 16u));
  EXPECT_TRUE(Load32LE(regs_, SOUND4CNT_L_OFFSET, &contents));
  EXPECT_EQ(16u, contents);
}

TEST_F(SoundTest, GbaSpuRegistersLoad32LE) {
  uint32_t contents;
  EXPECT_TRUE(Store32LE(regs_, WAVE_RAM0_L_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, WAVE_RAM0_L_OFFSET, &contents));
  EXPECT_EQ(0x11223344u, contents);
}

TEST_F(SoundTest, GbaSpuRegistersLoad8Aligned) {
  uint8_t contents;
  EXPECT_TRUE(Store16LE(regs_, WAVE_RAM0_L_OFFSET, 0x1122u));
  EXPECT_TRUE(Load8(regs_, WAVE_RAM0_L_OFFSET, &contents));
  EXPECT_EQ(0x22, contents);
}

TEST_F(SoundTest, GbaSpuRegistersLoad8Unaligned) {
  uint8_t contents;
  EXPECT_TRUE(Store16LE(regs_, WAVE_RAM0_L_OFFSET, 0x1122u));
  EXPECT_TRUE(Load8(regs_, WAVE_RAM0_L_OFFSET + 1u, &contents));
  EXPECT_EQ(0x11, contents);
}

TEST_F(SoundTest, GbaSpuRegistersLoad8Fails) {
  uint8_t contents;
  EXPECT_FALSE(Load8(regs_, 0x2Cu, &contents));
}

TEST_F(SoundTest, GbaSpuRegistersStore32LEFifoSucceeds) {
  EXPECT_TRUE(Store32LE(regs_, FIFO_A_OFFSET, 0xFFFFFFFFu));
  EXPECT_TRUE(Store32LE(regs_, FIFO_B_OFFSET, 0xFFFFFFFFu));
}

TEST_F(SoundTest, GbaSpuRegistersStore16LE) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(regs_, WAVE_RAM0_L_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Load16LE(regs_, WAVE_RAM0_L_OFFSET, &contents));
  EXPECT_EQ(0xFFFFu, contents);
}

TEST_F(SoundTest, GbaSpuRegistersStore8Aligned) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(regs_, WAVE_RAM0_L_OFFSET, 0x1122u));
  EXPECT_TRUE(Store8(regs_, WAVE_RAM0_L_OFFSET, 0x33u));
  EXPECT_TRUE(Load16LE(regs_, WAVE_RAM0_L_OFFSET, &contents));
  EXPECT_EQ(0x1133u, contents);
}

TEST_F(SoundTest, GbaSpuRegistersStore8Unaligned) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(regs_, WAVE_RAM0_L_OFFSET, 0x1122u));
  EXPECT_TRUE(Store8(regs_, WAVE_RAM0_L_OFFSET + 1u, 0x33u));
  EXPECT_TRUE(Load16LE(regs_, WAVE_RAM0_L_OFFSET, &contents));
  EXPECT_EQ(0x3322u, contents);
}