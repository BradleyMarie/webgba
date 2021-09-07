extern "C" {
#include "emulator/ppu/gba/object_state.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

class ObjectStateTest : public testing::Test {
 public:
  void SetUp() override {
    memset(&state_, 0, sizeof(GbaPpuObjectState));
    memset(&memory_, 0, sizeof(GbaPpuObjectAttributeMemory));
  }

 protected:
  GbaPpuObjectAttributeMemory memory_;
  GbaPpuObjectState state_;
};

TEST_F(ObjectStateTest, GbaPpuObjectStateAdd) {
  GbaPpuObjectStateAdd(&memory_, 0u, &state_);
  GbaPpuObjectStateAdd(&memory_, 1u, &state_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectStateGetObjects(&state_, x, y);
      if (x < 8u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
        EXPECT_EQ(1u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}

TEST_F(ObjectStateTest, GbaPpuObjectStateAddNotVisible) {
  memory_.object_attributes[0u].flex_param_1 = true;
  GbaPpuObjectStateAdd(&memory_, 0u, &state_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectStateGetObjects(&state_, x, y);
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}

TEST_F(ObjectStateTest, GbaPpuObjectStateAddDouble) {
  memory_.object_attributes[0u].affine = true;
  memory_.object_attributes[0u].flex_param_1 = true;
  GbaPpuObjectStateAdd(&memory_, 0u, &state_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectStateGetObjects(&state_, x, y);
      if (x < 16u && y < 16u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}

TEST_F(ObjectStateTest, GbaPpuObjectStateAddNegativeX) {
  memory_.object_attributes[0u].x_coordinate = -4;
  GbaPpuObjectStateAdd(&memory_, 0u, &state_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectStateGetObjects(&state_, x, y);
      if (x < 4u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}

TEST_F(ObjectStateTest, GbaPpuObjectStateAddNegativeY) {
  memory_.object_attributes[0u].y_coordinate = -4;
  GbaPpuObjectStateAdd(&memory_, 0u, &state_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectStateGetObjects(&state_, x, y);
      if (x < 8u && y < 4u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}

TEST_F(ObjectStateTest, GbaPpuObjectStateAddPositiveX) {
  memory_.object_attributes[0u].x_coordinate = 236u;
  GbaPpuObjectStateAdd(&memory_, 0u, &state_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectStateGetObjects(&state_, x, y);
      if (x >= 236u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}

TEST_F(ObjectStateTest, GbaPpuObjectStateAddPositiveY) {
  memory_.object_attributes[0u].y_coordinate = 127u;
  memory_.object_attributes[0u].obj_size = 3u;
  GbaPpuObjectStateAdd(&memory_, 0u, &state_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectStateGetObjects(&state_, x, y);
      if (x < 64u && y >= 127u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}

TEST_F(ObjectStateTest, GbaPpuObjectStateClear) {
  GbaPpuObjectStateAdd(&memory_, 0u, &state_);
  GbaPpuObjectStateAdd(&memory_, 1u, &state_);
  GbaPpuObjectStateClear(&memory_, 1u, &state_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectStateGetObjects(&state_, x, y);
      if (x < 8u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}

TEST_F(ObjectStateTest, GbaPpuObjectStateClearNotVisible) {
  memory_.object_attributes[0u].flex_param_1 = true;
  memory_.object_attributes[1u].flex_param_1 = true;
  GbaPpuObjectStateAdd(&memory_, 0u, &state_);
  GbaPpuObjectStateAdd(&memory_, 1u, &state_);
  GbaPpuObjectStateClear(&memory_, 1u, &state_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectStateGetObjects(&state_, x, y);
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}

TEST_F(ObjectStateTest, GbaPpuObjectStateClearDouble) {
  memory_.object_attributes[0u].affine = true;
  memory_.object_attributes[0u].flex_param_1 = true;
  memory_.object_attributes[1u].affine = true;
  memory_.object_attributes[1u].flex_param_1 = true;
  GbaPpuObjectStateAdd(&memory_, 0u, &state_);
  GbaPpuObjectStateAdd(&memory_, 1u, &state_);
  GbaPpuObjectStateClear(&memory_, 1u, &state_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectStateGetObjects(&state_, x, y);
      if (x < 16u && y < 16u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}

TEST_F(ObjectStateTest, GbaPpuObjectStateClearNegativeX) {
  memory_.object_attributes[0u].x_coordinate = -4;
  memory_.object_attributes[1u].x_coordinate = -4;
  GbaPpuObjectStateAdd(&memory_, 0u, &state_);
  GbaPpuObjectStateAdd(&memory_, 1u, &state_);
  GbaPpuObjectStateClear(&memory_, 1u, &state_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectStateGetObjects(&state_, x, y);
      if (x < 4u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}

TEST_F(ObjectStateTest, GbaPpuObjectStateClearNegativeY) {
  memory_.object_attributes[0u].y_coordinate = -4;
  memory_.object_attributes[1u].y_coordinate = -4;
  GbaPpuObjectStateAdd(&memory_, 0u, &state_);
  GbaPpuObjectStateAdd(&memory_, 1u, &state_);
  GbaPpuObjectStateClear(&memory_, 1u, &state_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectStateGetObjects(&state_, x, y);
      if (x < 8u && y < 4u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}

TEST_F(ObjectStateTest, GbaPpuObjectStateClearPositiveX) {
  memory_.object_attributes[0u].x_coordinate = 236u;
  memory_.object_attributes[0u].x_coordinate = 236u;
  GbaPpuObjectStateAdd(&memory_, 0u, &state_);
  GbaPpuObjectStateAdd(&memory_, 1u, &state_);
  GbaPpuObjectStateClear(&memory_, 1u, &state_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectStateGetObjects(&state_, x, y);
      if (x >= 236u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}

TEST_F(ObjectStateTest, GbaPpuObjectStateClearPositiveY) {
  memory_.object_attributes[0u].y_coordinate = 127u;
  memory_.object_attributes[0u].obj_size = 3u;
  memory_.object_attributes[1u].y_coordinate = 127u;
  memory_.object_attributes[1u].obj_size = 3u;
  GbaPpuObjectStateAdd(&memory_, 0u, &state_);
  GbaPpuObjectStateAdd(&memory_, 1u, &state_);
  GbaPpuObjectStateClear(&memory_, 1u, &state_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectStateGetObjects(&state_, x, y);
      if (x < 64u && y >= 127u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}