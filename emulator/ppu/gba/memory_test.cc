extern "C" {
#include "emulator/ppu/gba/memory.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

class ObjectVisibilityTest : public testing::Test {
 public:
  void SetUp() override {
    memset(&memory_, 0, sizeof(GbaPpuObjectAttributeMemory));
  }

 protected:
  GbaPpuObjectAttributeMemory memory_;
};

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawn) {
  GbaPpuObjectVisibilityDrawn(&memory_, 0u);
  GbaPpuObjectVisibilityDrawn(&memory_, 1u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&memory_, x, y);
      if (x < 8u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
        EXPECT_EQ(1u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(0, memory_.internal.object_coordinates[0u].true_x_start);
  EXPECT_EQ(0, memory_.internal.object_coordinates[0u].true_y_start);
  EXPECT_EQ(4, memory_.internal.object_coordinates[0u].true_x_center);
  EXPECT_EQ(4, memory_.internal.object_coordinates[0u].true_y_center);
  EXPECT_EQ(8u, memory_.internal.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(8u, memory_.internal.object_coordinates[0u].pixel_y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawnNotVisible) {
  memory_.object_attributes[0u].flex_param_0 = true;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&memory_, x, y);
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawnDouble) {
  memory_.object_attributes[0u].affine = true;
  memory_.object_attributes[0u].flex_param_0 = true;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&memory_, x, y);
      if (x < 16u && y < 16u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(0, memory_.internal.object_coordinates[0u].true_x_start);
  EXPECT_EQ(0, memory_.internal.object_coordinates[0u].true_y_start);
  EXPECT_EQ(8, memory_.internal.object_coordinates[0u].true_x_center);
  EXPECT_EQ(8, memory_.internal.object_coordinates[0u].true_y_center);
  EXPECT_EQ(8u, memory_.internal.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(8u, memory_.internal.object_coordinates[0u].pixel_y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawnNegativeX) {
  memory_.object_attributes[0u].x_coordinate = -4;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&memory_, x, y);
      if (x < 4u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawnPositiveX) {
  memory_.object_attributes[0u].x_coordinate = 236u;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&memory_, x, y);
      if (x >= 236u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(236, memory_.internal.object_coordinates[0u].true_x_start);
  EXPECT_EQ(0, memory_.internal.object_coordinates[0u].true_y_start);
  EXPECT_EQ(240, memory_.internal.object_coordinates[0u].true_x_center);
  EXPECT_EQ(4, memory_.internal.object_coordinates[0u].true_y_center);
  EXPECT_EQ(8u, memory_.internal.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(8u, memory_.internal.object_coordinates[0u].pixel_y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawnPositiveY) {
  memory_.object_attributes[0u].y_coordinate = 127u;
  memory_.object_attributes[0u].obj_size = 3u;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&memory_, x, y);
      if (x < 64u && y >= 127u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(0, memory_.internal.object_coordinates[0u].true_x_start);
  EXPECT_EQ(127, memory_.internal.object_coordinates[0u].true_y_start);
  EXPECT_EQ(32, memory_.internal.object_coordinates[0u].true_x_center);
  EXPECT_EQ(159, memory_.internal.object_coordinates[0u].true_y_center);
  EXPECT_EQ(64u, memory_.internal.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(64u, memory_.internal.object_coordinates[0u].pixel_y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHidden) {
  GbaPpuObjectVisibilityDrawn(&memory_, 0u);
  GbaPpuObjectVisibilityDrawn(&memory_, 1u);
  GbaPpuObjectVisibilityHidden(&memory_, 1u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&memory_, x, y);
      if (x < 8u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(0, memory_.internal.object_coordinates[0u].true_x_start);
  EXPECT_EQ(0, memory_.internal.object_coordinates[0u].true_y_start);
  EXPECT_EQ(4, memory_.internal.object_coordinates[0u].true_x_center);
  EXPECT_EQ(4, memory_.internal.object_coordinates[0u].true_y_center);
  EXPECT_EQ(8u, memory_.internal.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(8u, memory_.internal.object_coordinates[0u].pixel_y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHiddenNotVisible) {
  memory_.object_attributes[0u].flex_param_0 = true;
  memory_.object_attributes[1u].flex_param_0 = true;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u);
  GbaPpuObjectVisibilityDrawn(&memory_, 1u);
  GbaPpuObjectVisibilityHidden(&memory_, 1u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&memory_, x, y);
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHiddenDouble) {
  memory_.object_attributes[0u].affine = true;
  memory_.object_attributes[0u].flex_param_0 = true;
  memory_.object_attributes[1u].affine = true;
  memory_.object_attributes[1u].flex_param_0 = true;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u);
  GbaPpuObjectVisibilityDrawn(&memory_, 1u);
  GbaPpuObjectVisibilityHidden(&memory_, 1u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&memory_, x, y);
      if (x < 16u && y < 16u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(0, memory_.internal.object_coordinates[0u].true_x_start);
  EXPECT_EQ(0, memory_.internal.object_coordinates[0u].true_y_start);
  EXPECT_EQ(8, memory_.internal.object_coordinates[0u].true_x_center);
  EXPECT_EQ(8, memory_.internal.object_coordinates[0u].true_y_center);
  EXPECT_EQ(8u, memory_.internal.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(8u, memory_.internal.object_coordinates[0u].pixel_y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHiddenNegativeX) {
  memory_.object_attributes[0u].x_coordinate = -4;
  memory_.object_attributes[1u].x_coordinate = -4;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u);
  GbaPpuObjectVisibilityDrawn(&memory_, 1u);
  GbaPpuObjectVisibilityHidden(&memory_, 1u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&memory_, x, y);
      if (x < 4u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(-4, memory_.internal.object_coordinates[0u].true_x_start);
  EXPECT_EQ(0, memory_.internal.object_coordinates[0u].true_y_start);
  EXPECT_EQ(0, memory_.internal.object_coordinates[0u].true_x_center);
  EXPECT_EQ(4, memory_.internal.object_coordinates[0u].true_y_center);
  EXPECT_EQ(8u, memory_.internal.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(8u, memory_.internal.object_coordinates[0u].pixel_y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHiddenPositiveX) {
  memory_.object_attributes[0u].x_coordinate = 236u;
  memory_.object_attributes[0u].x_coordinate = 236u;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u);
  GbaPpuObjectVisibilityDrawn(&memory_, 1u);
  GbaPpuObjectVisibilityHidden(&memory_, 1u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&memory_, x, y);
      if (x >= 236u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(236, memory_.internal.object_coordinates[0u].true_x_start);
  EXPECT_EQ(0, memory_.internal.object_coordinates[0u].true_y_start);
  EXPECT_EQ(240, memory_.internal.object_coordinates[0u].true_x_center);
  EXPECT_EQ(4, memory_.internal.object_coordinates[0u].true_y_center);
  EXPECT_EQ(8u, memory_.internal.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(8u, memory_.internal.object_coordinates[0u].pixel_y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHiddenPositiveY) {
  memory_.object_attributes[0u].y_coordinate = 127u;
  memory_.object_attributes[0u].obj_size = 3u;
  memory_.object_attributes[1u].y_coordinate = 127u;
  memory_.object_attributes[1u].obj_size = 3u;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u);
  GbaPpuObjectVisibilityDrawn(&memory_, 1u);
  GbaPpuObjectVisibilityHidden(&memory_, 1u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&memory_, x, y);
      if (x < 64u && y >= 127u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(0, memory_.internal.object_coordinates[0u].true_x_start);
  EXPECT_EQ(127, memory_.internal.object_coordinates[0u].true_y_start);
  EXPECT_EQ(32, memory_.internal.object_coordinates[0u].true_x_center);
  EXPECT_EQ(159, memory_.internal.object_coordinates[0u].true_y_center);
  EXPECT_EQ(64u, memory_.internal.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(64u, memory_.internal.object_coordinates[0u].pixel_y_size);
}