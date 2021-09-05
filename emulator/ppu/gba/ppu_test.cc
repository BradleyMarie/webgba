extern "C" {
#include "emulator/ppu/gba/ppu.h"
}

#include "googletest/include/gtest/gtest.h"

#define DISPCNT_OFFSET 0x00u
#define DISPSTAT_OFFSET 0x04u

class PpuTest : public testing::Test {
 public:
  void SetUp() override {
    ASSERT_TRUE(GbaPlatformAllocate(&plat_, &plat_regs_, &rst_, &fiq_, &irq_));
    ASSERT_TRUE(GbaPpuAllocate(plat_, &ppu_, &pram_, &vram_, &oam_, &regs_));
  }

  void TearDown() override {
    GbaPlatformRelease(plat_);
    MemoryFree(plat_regs_);
    InterruptLineFree(rst_);
    InterruptLineFree(fiq_);
    InterruptLineFree(irq_);
    GbaPpuFree(ppu_);
    MemoryFree(pram_);
    MemoryFree(vram_);
    MemoryFree(oam_);
    MemoryFree(regs_);
  }

 protected:
  GbaPlatform *plat_;
  Memory *plat_regs_;
  InterruptLine *rst_;
  InterruptLine *fiq_;
  InterruptLine *irq_;
  GbaPpu *ppu_;
  Memory *pram_;
  Memory *vram_;
  Memory *oam_;
  Memory *regs_;
};

TEST_F(PpuTest, InitialRegisterState) {
  uint16_t contents;
  EXPECT_TRUE(Load16LE(regs_, DISPCNT_OFFSET, &contents));
  EXPECT_EQ(0x80u, contents);
  EXPECT_TRUE(Load16LE(regs_, DISPSTAT_OFFSET, &contents));
  EXPECT_EQ(0x4u, contents);
}