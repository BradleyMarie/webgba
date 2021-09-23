extern "C" {
#include "emulator/ppu/gba/ppu.h"
}

#include "googletest/include/gtest/gtest.h"

#define DISPCNT_OFFSET 0x00u
#define DISPSTAT_OFFSET 0x04u

class PpuTest : public testing::Test {
 public:
  void SetUp() override {
    raised_ = false;
    InterruptLine *irq =
        InterruptLineAllocate(nullptr, InterruptSetLevel, nullptr);
    ASSERT_NE(irq, nullptr);
    ASSERT_TRUE(GbaPlatformAllocate(irq, &plat_, &plat_regs_));
    ASSERT_TRUE(GbaDmaUnitAllocate(plat_, &dma_unit_, &dma_unit_regs_));
    ASSERT_TRUE(
        GbaPpuAllocate(dma_unit_, plat_, &ppu_, &pram_, &vram_, &oam_, &regs_));
  }

  void TearDown() override {
    GbaDmaUnitRelease(dma_unit_);
    MemoryFree(dma_unit_regs_);
    GbaPlatformRelease(plat_);
    MemoryFree(plat_regs_);
    GbaPpuFree(ppu_);
    MemoryFree(pram_);
    MemoryFree(vram_);
    MemoryFree(oam_);
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
  Memory *dma_unit_regs_;
  GbaPpu *ppu_;
  Memory *pram_;
  Memory *vram_;
  Memory *oam_;
  Memory *regs_;
};

bool PpuTest::raised_ = false;

TEST_F(PpuTest, InitialRegisterState) {
  uint16_t contents;
  EXPECT_TRUE(Load16LE(regs_, DISPCNT_OFFSET, &contents));
  EXPECT_EQ(0x80u, contents);
  EXPECT_TRUE(Load16LE(regs_, DISPSTAT_OFFSET, &contents));
  EXPECT_EQ(0x4u, contents);
}