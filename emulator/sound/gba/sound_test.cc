extern "C" {
#include "emulator/sound/gba/sound.h"
}

#include "googletest/include/gtest/gtest.h"

class SoundTest : public testing::Test {
 public:
  void SetUp() override { ASSERT_TRUE(GbaSpuAllocate(&spu_, &regs_)); }

  void TearDown() override {
    GbaSpuFree(spu_);
    MemoryFree(regs_);
  }

 protected:
  GbaSpu *spu_;
  Memory *regs_;
};

TEST_F(SoundTest, EmptyTest) {}