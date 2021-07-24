extern "C" {
#include "emulator/ppu/gba/ppu.h"
}

#include "googletest/include/gtest/gtest.h"

class PpuTest : public testing::Test {
 public:
  void SetUp() override {
    ASSERT_TRUE(GbaInterruptControllerAllocate(&ic_, &rst_, &fiq_, &irq_));
    ASSERT_TRUE(GbaPpuAllocate(ic_, &ppu_, &pram_, &vram_, &oam_, &regs_));
  }

  void TearDown() override {
    GbaInterruptControllerRelease(ic_);
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
  GbaInterruptController *ic_;
  InterruptLine *rst_;
  InterruptLine *fiq_;
  InterruptLine *irq_;
  GbaPpu *ppu_;
  Memory *pram_;
  Memory *vram_;
  Memory *oam_;
  Memory *regs_;
};

TEST_F(PpuTest, PRamLoad32LE) {}