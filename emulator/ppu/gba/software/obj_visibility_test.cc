extern "C" {
#include "emulator/ppu/gba/software/obj_visibility.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

class ObjectVisibilityTest : public testing::Test {
 public:
  void SetUp() override {
    memset(&visibility_, 0, sizeof(GbaPpuObjectVisibility));
    memset(&memory_, 0, sizeof(GbaPpuObjectAttributeMemory));
  }

 protected:
  GbaPpuObjectAttributeMemory memory_;
  GbaPpuObjectVisibility visibility_;
};

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawn) {
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 0u);
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 1u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x < 8u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
        EXPECT_EQ(1u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(0, visibility_.object_coordinates[0u].true_x_start);
  EXPECT_EQ(0, visibility_.object_coordinates[0u].true_y_start);
  EXPECT_EQ(4, visibility_.object_coordinates[0u].true_x_center);
  EXPECT_EQ(4, visibility_.object_coordinates[0u].true_y_center);
  EXPECT_EQ(8u, visibility_.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(8u, visibility_.object_coordinates[0u].pixel_y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawnNotVisible) {
  memory_.object_attributes[0u].flex_param_0 = true;
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 0u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawnDouble) {
  memory_.object_attributes[0u].affine = true;
  memory_.object_attributes[0u].flex_param_0 = true;
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 0u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x < 16u && y < 16u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(0, visibility_.object_coordinates[0u].true_x_start);
  EXPECT_EQ(0, visibility_.object_coordinates[0u].true_y_start);
  EXPECT_EQ(8, visibility_.object_coordinates[0u].true_x_center);
  EXPECT_EQ(8, visibility_.object_coordinates[0u].true_y_center);
  EXPECT_EQ(8u, visibility_.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(8u, visibility_.object_coordinates[0u].pixel_y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawnNegativeX) {
  memory_.object_attributes[0u].x_coordinate = -4;
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 0u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x < 4u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawnPositiveX) {
  memory_.object_attributes[0u].x_coordinate = 236u;
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 0u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x >= 236u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(236, visibility_.object_coordinates[0u].true_x_start);
  EXPECT_EQ(0, visibility_.object_coordinates[0u].true_y_start);
  EXPECT_EQ(240, visibility_.object_coordinates[0u].true_x_center);
  EXPECT_EQ(4, visibility_.object_coordinates[0u].true_y_center);
  EXPECT_EQ(8u, visibility_.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(8u, visibility_.object_coordinates[0u].pixel_y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawnPositiveY) {
  memory_.object_attributes[0u].y_coordinate = 127u;
  memory_.object_attributes[0u].obj_size = 3u;
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 0u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x < 64u && y >= 127u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(0, visibility_.object_coordinates[0u].true_x_start);
  EXPECT_EQ(127, visibility_.object_coordinates[0u].true_y_start);
  EXPECT_EQ(32, visibility_.object_coordinates[0u].true_x_center);
  EXPECT_EQ(159, visibility_.object_coordinates[0u].true_y_center);
  EXPECT_EQ(64u, visibility_.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(64u, visibility_.object_coordinates[0u].pixel_y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHidden) {
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 0u);
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 1u);
  GbaPpuObjectVisibilityHidden(&visibility_, &memory_, 1u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x < 8u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(0, visibility_.object_coordinates[0u].true_x_start);
  EXPECT_EQ(0, visibility_.object_coordinates[0u].true_y_start);
  EXPECT_EQ(4, visibility_.object_coordinates[0u].true_x_center);
  EXPECT_EQ(4, visibility_.object_coordinates[0u].true_y_center);
  EXPECT_EQ(8u, visibility_.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(8u, visibility_.object_coordinates[0u].pixel_y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHiddenNotVisible) {
  memory_.object_attributes[0u].flex_param_0 = true;
  memory_.object_attributes[1u].flex_param_0 = true;
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 0u);
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 1u);
  GbaPpuObjectVisibilityHidden(&visibility_, &memory_, 1u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHiddenDouble) {
  memory_.object_attributes[0u].affine = true;
  memory_.object_attributes[0u].flex_param_0 = true;
  memory_.object_attributes[1u].affine = true;
  memory_.object_attributes[1u].flex_param_0 = true;
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 0u);
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 1u);
  GbaPpuObjectVisibilityHidden(&visibility_, &memory_, 1u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x < 16u && y < 16u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(0, visibility_.object_coordinates[0u].true_x_start);
  EXPECT_EQ(0, visibility_.object_coordinates[0u].true_y_start);
  EXPECT_EQ(8, visibility_.object_coordinates[0u].true_x_center);
  EXPECT_EQ(8, visibility_.object_coordinates[0u].true_y_center);
  EXPECT_EQ(8u, visibility_.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(8u, visibility_.object_coordinates[0u].pixel_y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHiddenNegativeX) {
  memory_.object_attributes[0u].x_coordinate = -4;
  memory_.object_attributes[1u].x_coordinate = -4;
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 0u);
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 1u);
  GbaPpuObjectVisibilityHidden(&visibility_, &memory_, 1u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x < 4u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(-4, visibility_.object_coordinates[0u].true_x_start);
  EXPECT_EQ(0, visibility_.object_coordinates[0u].true_y_start);
  EXPECT_EQ(0, visibility_.object_coordinates[0u].true_x_center);
  EXPECT_EQ(4, visibility_.object_coordinates[0u].true_y_center);
  EXPECT_EQ(8u, visibility_.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(8u, visibility_.object_coordinates[0u].pixel_y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHiddenPositiveX) {
  memory_.object_attributes[0u].x_coordinate = 236u;
  memory_.object_attributes[0u].x_coordinate = 236u;
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 0u);
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 1u);
  GbaPpuObjectVisibilityHidden(&visibility_, &memory_, 1u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x >= 236u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(236, visibility_.object_coordinates[0u].true_x_start);
  EXPECT_EQ(0, visibility_.object_coordinates[0u].true_y_start);
  EXPECT_EQ(240, visibility_.object_coordinates[0u].true_x_center);
  EXPECT_EQ(4, visibility_.object_coordinates[0u].true_y_center);
  EXPECT_EQ(8u, visibility_.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(8u, visibility_.object_coordinates[0u].pixel_y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHiddenPositiveY) {
  memory_.object_attributes[0u].y_coordinate = 127u;
  memory_.object_attributes[0u].obj_size = 3u;
  memory_.object_attributes[1u].y_coordinate = 127u;
  memory_.object_attributes[1u].obj_size = 3u;
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 0u);
  GbaPpuObjectVisibilityDrawn(&visibility_, &memory_, 1u);
  GbaPpuObjectVisibilityHidden(&visibility_, &memory_, 1u);
  for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x < 64u && y >= 127u) {
        EXPECT_EQ(0u, GbaPpuSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuSetEmpty(&set));
    }
  }

  EXPECT_EQ(0, visibility_.object_coordinates[0u].true_x_start);
  EXPECT_EQ(127, visibility_.object_coordinates[0u].true_y_start);
  EXPECT_EQ(32, visibility_.object_coordinates[0u].true_x_center);
  EXPECT_EQ(159, visibility_.object_coordinates[0u].true_y_center);
  EXPECT_EQ(64u, visibility_.object_coordinates[0u].pixel_x_size);
  EXPECT_EQ(64u, visibility_.object_coordinates[0u].pixel_y_size);
}