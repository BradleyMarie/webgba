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
  GbaPpuBlendUnitAddBackground(&blend_unit_, true, true, 0x7FFFu,
                               GBA_PPU_LAYER_PRIORITY_BACKDROP);
  EXPECT_EQ(0x7FFFu, GbaPpuBlendUnitNoBlend(&blend_unit_));
  EXPECT_EQ(0x7FFFu, GbaPpuBlendUnitBlend(&blend_unit_, 17u, 17u));
  EXPECT_EQ(0x7FFFu, GbaPpuBlendUnitDarken(&blend_unit_, 17u, 17u, 0u));
  EXPECT_EQ(0x7FFFu, GbaPpuBlendUnitBrighten(&blend_unit_, 17u, 17u, 0u));
}