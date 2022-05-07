extern "C" {
#include "emulator/ppu/gba/software/blend.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

class BlendTest : public testing::Test {
 public:
  void SetUp() override {
    GbaPpuBlendUnitReset(&blend_unit_);
    memset(&registers_, 0, sizeof(GbaPpuRegisters));
  }

 protected:
  GbaPpuBlendUnit blend_unit_;
  GbaPpuRegisters registers_;
};

TEST_F(BlendTest, BackdropOnlyNoBlend) {
  registers_.bldcnt.a_obj = true;
  GbaPpuBlendUnitAddBackdrop(&blend_unit_, &registers_, 0x7FFFu);
  EXPECT_EQ(0x7FFFu, GbaPpuBlendUnitNoBlend(&blend_unit_));
}

TEST_F(BlendTest, BackdropOnlyAddative) {
  registers_.bldcnt.a_obj = true;
  registers_.bldcnt.mode = 1u;
  registers_.bldalpha.eva = 17u;
  registers_.bldalpha.evb = 17u;
  GbaPpuBlendUnitAddBackdrop(&blend_unit_, &registers_, 0x7FFFu);
  EXPECT_EQ(0x7FFFu, GbaPpuBlendUnitBlend(&blend_unit_, &registers_));
}

TEST_F(BlendTest, BackdropOnlyBrighten) {
  registers_.bldcnt.a_obj = true;
  registers_.bldcnt.mode = 2u;
  registers_.bldy.evy = 0u;
  GbaPpuBlendUnitAddBackdrop(&blend_unit_, &registers_, 0x7FFFu);
  EXPECT_EQ(0x7FFFu, GbaPpuBlendUnitBlend(&blend_unit_, &registers_));
}

TEST_F(BlendTest, BackdropOnlyDarken) {
  registers_.bldcnt.a_obj = true;
  registers_.bldcnt.mode = 3u;
  registers_.bldy.evy = 0u;
  GbaPpuBlendUnitAddBackdrop(&blend_unit_, &registers_, 0x7FFFu);
  EXPECT_EQ(0x7FFFu, GbaPpuBlendUnitBlend(&blend_unit_, &registers_));
}