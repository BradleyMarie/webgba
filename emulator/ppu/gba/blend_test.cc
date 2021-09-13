extern "C" {
#include "emulator/ppu/gba/blend.h"
}

#include "googletest/include/gtest/gtest.h"

class BlendTest : public testing::Test {
 public:
  void SetUp() override { GbaPpuBlendUnitReset(&blend_unit_); }

 protected:
  GbaPpuBlendUnit blend_unit_;
};

TEST_F(BlendTest, BackdropOnly) {
  GbaPpuBlendUnitSet(&blend_unit_, GBA_PPU_LAYER_BACKDROP, true, true, 0x7FFFu,
                     GBA_PPU_LAYER_PRIORITY_TRANSPARENT);
  EXPECT_EQ(0x7FFFu, GbaPpuBlendUnitNoBlend(&blend_unit_));
  EXPECT_EQ(0x7FFFu, GbaPpuBlendUnitBlend(&blend_unit_, 17u, 17u));
  EXPECT_EQ(0x7FFFu, GbaPpuBlendUnitBlackBlend(&blend_unit_, 17u));
  EXPECT_EQ(0x7FFFu, GbaPpuBlendUnitWhiteBlend(&blend_unit_, 17u));
}