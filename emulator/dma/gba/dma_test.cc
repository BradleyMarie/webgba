extern "C" {
#include "emulator/dma/gba/dma.h"
}

#include "googletest/include/gtest/gtest.h"

#define IE_OFFSET 0x0u
#define IF_OFFSET 0x2u
#define IME_OFFSET 0x8u

#define DMA0SAD_OFFSET 0x00u
#define DMA0DAD_OFFSET 0x04u
#define DMA0CNT_L_OFFSET 0x08u
#define DMA0CNT_H_OFFSET 0x0Au
#define DMA1SAD_OFFSET 0x0Cu
#define DMA1DAD_OFFSET 0x10u
#define DMA1CNT_L_OFFSET 0x14u
#define DMA1CNT_H_OFFSET 0x16u
#define DMA2SAD_OFFSET 0x18u
#define DMA2DAD_OFFSET 0x1Cu
#define DMA2CNT_L_OFFSET 0x20u
#define DMA2CNT_H_OFFSET 0x22u
#define DMA3SAD_OFFSET 0x24u
#define DMA3DAD_OFFSET 0x28u
#define DMA3CNT_L_OFFSET 0x2Cu
#define DMA3CNT_H_OFFSET 0x2Eu

#define GBA_DMA_ADDR_INCREMENT 0u
#define GBA_DMA_ADDR_DECREMENT 1u
#define GBA_DMA_ADDR_FIXED 2u
#define GBA_DMA_ADDR_INCREMENT_RELOAD 3u

#define GBA_DMA_IMMEDIATE 0u
#define GBA_DMA_VBLANK 1u
#define GBA_DMA_HBLANK 2u
#define GBA_DMA_SPECIAL 3u

class DmaUnitTest : public testing::Test {
 public:
  void SetUp() override {
    ASSERT_TRUE(GbaPlatformAllocate(&plat_, &plat_regs_, &rst_, &fiq_, &irq_));
    ASSERT_TRUE(GbaDmaUnitAllocate(plat_, &dma_unit_, &regs_));
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
    GbaDmaUnitFree(dma_unit_);
    MemoryFree(regs_);
  }

 protected:
  GbaPlatform *plat_;
  Memory *plat_regs_;
  InterruptLine *rst_;
  InterruptLine *fiq_;
  InterruptLine *irq_;
  GbaDmaUnit *dma_unit_;
  Memory *regs_;
};

TEST_F(DmaUnitTest, GbaDmaUnitRegistersLoad16LE) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(regs_, DMA0SAD_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA0SAD_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA0SAD_OFFSET + 2u, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA0SAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA0DAD_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA0DAD_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA0DAD_OFFSET + 2u, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA0DAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA0CNT_L_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA0CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA0CNT_H_OFFSET, 0xAABBu));
  EXPECT_TRUE(Load16LE(regs_, DMA0CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAABBu, contents);
  EXPECT_TRUE(Store16LE(regs_, DMA1SAD_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA1SAD_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA1SAD_OFFSET + 2u, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA1SAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA1DAD_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA1DAD_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA1DAD_OFFSET + 2u, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA1DAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA1CNT_L_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA1CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA1CNT_H_OFFSET, 0xAABBu));
  EXPECT_TRUE(Load16LE(regs_, DMA1CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAABBu, contents);
  EXPECT_TRUE(Store16LE(regs_, DMA2SAD_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA2SAD_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA2SAD_OFFSET + 2u, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA2SAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA2DAD_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA2DAD_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA2DAD_OFFSET + 2u, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA2DAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA2CNT_L_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA2CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA2CNT_H_OFFSET, 0xAABBu));
  EXPECT_TRUE(Load16LE(regs_, DMA2CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAABBu, contents);
  EXPECT_TRUE(Store16LE(regs_, DMA3SAD_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA3SAD_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA3SAD_OFFSET + 2u, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA3SAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA3DAD_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA3DAD_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA3DAD_OFFSET + 2u, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA3DAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA3CNT_L_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA3CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA3CNT_H_OFFSET, 0xAABBu));
  EXPECT_TRUE(Load16LE(regs_, DMA3CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAABBu, contents);

  // Out of Bounds
  EXPECT_TRUE(Store16LE(regs_, DMA3CNT_H_OFFSET + 2u, 0xAABBu));
  EXPECT_FALSE(Load16LE(regs_, DMA3CNT_H_OFFSET + 2u, &contents));
}

TEST_F(DmaUnitTest, GbaDmaUnitRegistersLoad8) {
  uint8_t contents;
  EXPECT_TRUE(Store8(regs_, DMA0SAD_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA0SAD_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0SAD_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA0SAD_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0SAD_OFFSET + 2u, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA0SAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0SAD_OFFSET + 3u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA0SAD_OFFSET + 3u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0DAD_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA0DAD_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0DAD_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA0DAD_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0DAD_OFFSET + 2u, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA0DAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0DAD_OFFSET + 3u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA0DAD_OFFSET + 3u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0CNT_L_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA0CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0CNT_L_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA0CNT_L_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0CNT_H_OFFSET, 0xAAu));
  EXPECT_TRUE(Load8(regs_, DMA0CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAAu, contents);
  EXPECT_TRUE(Store8(regs_, DMA0CNT_H_OFFSET + 1u, 0xBBu));
  EXPECT_TRUE(Load8(regs_, DMA0CNT_H_OFFSET + 1u, &contents));
  EXPECT_EQ(0xBBu, contents);
  EXPECT_TRUE(Store8(regs_, DMA1SAD_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA1SAD_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1SAD_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA1SAD_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1SAD_OFFSET + 2u, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA1SAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1SAD_OFFSET + 3u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA1SAD_OFFSET + 3u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1DAD_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA1DAD_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1DAD_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA1DAD_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1DAD_OFFSET + 2u, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA1DAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1DAD_OFFSET + 3u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA1DAD_OFFSET + 3u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1CNT_L_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA1CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1CNT_L_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA1CNT_L_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1CNT_H_OFFSET, 0xAAu));
  EXPECT_TRUE(Load8(regs_, DMA1CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAAu, contents);
  EXPECT_TRUE(Store8(regs_, DMA1CNT_H_OFFSET + 1u, 0xBBu));
  EXPECT_TRUE(Load8(regs_, DMA1CNT_H_OFFSET + 1u, &contents));
  EXPECT_EQ(0xBBu, contents);
  EXPECT_TRUE(Store8(regs_, DMA2SAD_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA2SAD_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2SAD_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA2SAD_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2SAD_OFFSET + 2u, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA2SAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2SAD_OFFSET + 3u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA2SAD_OFFSET + 3u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2DAD_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA2DAD_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2DAD_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA2DAD_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2DAD_OFFSET + 2u, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA2DAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2DAD_OFFSET + 3u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA2DAD_OFFSET + 3u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2CNT_L_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA2CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2CNT_L_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA2CNT_L_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2CNT_H_OFFSET, 0xAAu));
  EXPECT_TRUE(Load8(regs_, DMA2CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAAu, contents);
  EXPECT_TRUE(Store8(regs_, DMA2CNT_H_OFFSET + 1u, 0xBBu));
  EXPECT_TRUE(Load8(regs_, DMA2CNT_H_OFFSET + 1u, &contents));
  EXPECT_EQ(0xBBu, contents);
  EXPECT_TRUE(Store8(regs_, DMA3SAD_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA3SAD_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3SAD_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA3SAD_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3SAD_OFFSET + 2u, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA3SAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3SAD_OFFSET + 3u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA3SAD_OFFSET + 3u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3DAD_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA3DAD_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3DAD_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA3DAD_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3DAD_OFFSET + 2u, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA3DAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3DAD_OFFSET + 3u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA3DAD_OFFSET + 3u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3CNT_L_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA3CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3CNT_L_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA3CNT_L_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3CNT_H_OFFSET, 0xAAu));
  EXPECT_TRUE(Load8(regs_, DMA3CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAAu, contents);
  EXPECT_TRUE(Store8(regs_, DMA3CNT_H_OFFSET + 1u, 0xBBu));
  EXPECT_TRUE(Load8(regs_, DMA3CNT_H_OFFSET + 1u, &contents));
  EXPECT_EQ(0xBBu, contents);

  // Out of Bounds
  EXPECT_TRUE(Store8(regs_, DMA3CNT_H_OFFSET + 2u, 0xAAu));
  EXPECT_FALSE(Load8(regs_, DMA3CNT_H_OFFSET + 2u, &contents));
}

TEST_F(DmaUnitTest, GbaDmaUnitRegistersLoad32LE) {
  uint32_t contents;
  EXPECT_TRUE(Store32LE(regs_, DMA0SAD_OFFSET, 0x11223344u));
  EXPECT_FALSE(Load32LE(regs_, DMA0SAD_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA0DAD_OFFSET, 0x11233442u));
  EXPECT_FALSE(Load32LE(regs_, DMA0DAD_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA0CNT_L_OFFSET, 0x11223344u));
  EXPECT_FALSE(Load32LE(regs_, DMA0CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA1SAD_OFFSET, 0x11223344u));
  EXPECT_FALSE(Load32LE(regs_, DMA1SAD_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA1DAD_OFFSET, 0x11233442u));
  EXPECT_FALSE(Load32LE(regs_, DMA1DAD_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA1CNT_L_OFFSET, 0x11223344u));
  EXPECT_FALSE(Load32LE(regs_, DMA1CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA2SAD_OFFSET, 0x11223344u));
  EXPECT_FALSE(Load32LE(regs_, DMA2SAD_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA2DAD_OFFSET, 0x11233442u));
  EXPECT_FALSE(Load32LE(regs_, DMA2DAD_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA2CNT_L_OFFSET, 0x11223344u));
  EXPECT_FALSE(Load32LE(regs_, DMA2CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA3SAD_OFFSET, 0x11223344u));
  EXPECT_FALSE(Load32LE(regs_, DMA3SAD_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA3DAD_OFFSET, 0x11233442u));
  EXPECT_FALSE(Load32LE(regs_, DMA3DAD_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA3CNT_L_OFFSET, 0x11223344u));
  EXPECT_FALSE(Load32LE(regs_, DMA3CNT_L_OFFSET, &contents));

  // Out of Bounds
  EXPECT_TRUE(Store32LE(regs_, DMA3CNT_L_OFFSET + 4u, 0xAABBCCDDu));
  EXPECT_FALSE(Load32LE(regs_, DMA3CNT_L_OFFSET + 4u, &contents));
}