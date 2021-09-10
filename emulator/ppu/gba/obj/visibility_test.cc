extern "C" {
#include "emulator/ppu/gba/obj/visibility.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

class ObjectVisibilityTest : public testing::Test {
 public:
  void SetUp() override {
    memset(&visibility_, 0, sizeof(GbaPpuObjectVisibility));
    memset(&internal_registers_, 0, sizeof(GbaPpuInternalRegisters));
    memset(&memory_, 0, sizeof(GbaPpuObjectAttributeMemory));
  }

 protected:
  GbaPpuObjectAttributeMemory memory_;
  GbaPpuInternalRegisters internal_registers_;
  GbaPpuObjectVisibility visibility_;
};

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawn) {
  GbaPpuObjectVisibilityDrawn(&memory_, 0u, &internal_registers_, &visibility_);
  GbaPpuObjectVisibilityDrawn(&memory_, 1u, &internal_registers_, &visibility_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x < 8u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
        EXPECT_EQ(1u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
  EXPECT_EQ(0, internal_registers_.object_coordinates[0u].x);
  EXPECT_EQ(0, internal_registers_.object_coordinates[0u].y);
  EXPECT_EQ(4u, internal_registers_.object_coordinates[0u].x_center);
  EXPECT_EQ(4u, internal_registers_.object_coordinates[0u].y_center);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].x_size);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawnNotVisible) {
  memory_.object_attributes[0u].flex_param_0 = true;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u, &internal_registers_, &visibility_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawnDouble) {
  memory_.object_attributes[0u].affine = true;
  memory_.object_attributes[0u].flex_param_0 = true;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u, &internal_registers_, &visibility_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x < 16u && y < 16u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
  EXPECT_EQ(0, internal_registers_.object_coordinates[0u].x);
  EXPECT_EQ(0, internal_registers_.object_coordinates[0u].y);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].x_center);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].y_center);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].x_size);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawnNegativeX) {
  memory_.object_attributes[0u].x_coordinate = -4;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u, &internal_registers_, &visibility_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x < 4u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawnPositiveX) {
  memory_.object_attributes[0u].x_coordinate = 236u;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u, &internal_registers_, &visibility_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x >= 236u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
  EXPECT_EQ(236, internal_registers_.object_coordinates[0u].x);
  EXPECT_EQ(0, internal_registers_.object_coordinates[0u].y);
  EXPECT_EQ(240u, internal_registers_.object_coordinates[0u].x_center);
  EXPECT_EQ(4u, internal_registers_.object_coordinates[0u].y_center);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].x_size);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityDrawnPositiveY) {
  memory_.object_attributes[0u].y_coordinate = 127u;
  memory_.object_attributes[0u].obj_size = 3u;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u, &internal_registers_, &visibility_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x < 64u && y >= 127u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
  EXPECT_EQ(0, internal_registers_.object_coordinates[0u].x);
  EXPECT_EQ(127, internal_registers_.object_coordinates[0u].y);
  EXPECT_EQ(32u, internal_registers_.object_coordinates[0u].x_center);
  EXPECT_EQ(159u, internal_registers_.object_coordinates[0u].y_center);
  EXPECT_EQ(64u, internal_registers_.object_coordinates[0u].x_size);
  EXPECT_EQ(64u, internal_registers_.object_coordinates[0u].y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHidden) {
  GbaPpuObjectVisibilityDrawn(&memory_, 0u, &internal_registers_, &visibility_);
  GbaPpuObjectVisibilityDrawn(&memory_, 1u, &internal_registers_, &visibility_);
  GbaPpuObjectVisibilityHidden(&memory_, &internal_registers_, 1u,
                               &visibility_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x < 8u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
  EXPECT_EQ(0, internal_registers_.object_coordinates[0u].x);
  EXPECT_EQ(0, internal_registers_.object_coordinates[0u].y);
  EXPECT_EQ(4u, internal_registers_.object_coordinates[0u].x_center);
  EXPECT_EQ(4u, internal_registers_.object_coordinates[0u].y_center);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].x_size);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHiddenNotVisible) {
  memory_.object_attributes[0u].flex_param_0 = true;
  memory_.object_attributes[1u].flex_param_0 = true;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u, &internal_registers_, &visibility_);
  GbaPpuObjectVisibilityDrawn(&memory_, 1u, &internal_registers_, &visibility_);
  GbaPpuObjectVisibilityHidden(&memory_, &internal_registers_, 1u,
                               &visibility_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHiddenDouble) {
  memory_.object_attributes[0u].affine = true;
  memory_.object_attributes[0u].flex_param_0 = true;
  memory_.object_attributes[1u].affine = true;
  memory_.object_attributes[1u].flex_param_0 = true;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u, &internal_registers_, &visibility_);
  GbaPpuObjectVisibilityDrawn(&memory_, 1u, &internal_registers_, &visibility_);
  GbaPpuObjectVisibilityHidden(&memory_, &internal_registers_, 1u,
                               &visibility_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x < 16u && y < 16u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
  EXPECT_EQ(0, internal_registers_.object_coordinates[0u].x);
  EXPECT_EQ(0, internal_registers_.object_coordinates[0u].y);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].x_center);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].y_center);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].x_size);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHiddenNegativeX) {
  memory_.object_attributes[0u].x_coordinate = -4;
  memory_.object_attributes[1u].x_coordinate = -4;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u, &internal_registers_, &visibility_);
  GbaPpuObjectVisibilityDrawn(&memory_, 1u, &internal_registers_, &visibility_);
  GbaPpuObjectVisibilityHidden(&memory_, &internal_registers_, 1u,
                               &visibility_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x < 4u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
  EXPECT_EQ(-4, internal_registers_.object_coordinates[0u].x);
  EXPECT_EQ(0, internal_registers_.object_coordinates[0u].y);
  EXPECT_EQ(0u, internal_registers_.object_coordinates[0u].x_center);
  EXPECT_EQ(4u, internal_registers_.object_coordinates[0u].y_center);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].x_size);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHiddenPositiveX) {
  memory_.object_attributes[0u].x_coordinate = 236u;
  memory_.object_attributes[0u].x_coordinate = 236u;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u, &internal_registers_, &visibility_);
  GbaPpuObjectVisibilityDrawn(&memory_, 1u, &internal_registers_, &visibility_);
  GbaPpuObjectVisibilityHidden(&memory_, &internal_registers_, 1u,
                               &visibility_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x >= 236u && y < 8u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
  EXPECT_EQ(236, internal_registers_.object_coordinates[0u].x);
  EXPECT_EQ(0, internal_registers_.object_coordinates[0u].y);
  EXPECT_EQ(240u, internal_registers_.object_coordinates[0u].x_center);
  EXPECT_EQ(4u, internal_registers_.object_coordinates[0u].y_center);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].x_size);
  EXPECT_EQ(8u, internal_registers_.object_coordinates[0u].y_size);
}

TEST_F(ObjectVisibilityTest, GbaPpuObjectVisibilityHiddenPositiveY) {
  memory_.object_attributes[0u].y_coordinate = 127u;
  memory_.object_attributes[0u].obj_size = 3u;
  memory_.object_attributes[1u].y_coordinate = 127u;
  memory_.object_attributes[1u].obj_size = 3u;
  GbaPpuObjectVisibilityDrawn(&memory_, 0u, &internal_registers_, &visibility_);
  GbaPpuObjectVisibilityDrawn(&memory_, 1u, &internal_registers_, &visibility_);
  GbaPpuObjectVisibilityHidden(&memory_, &internal_registers_, 1u,
                               &visibility_);
  for (uint_fast8_t x = 0; x < GBA_FULL_FRAME_WIDTH; x++) {
    for (uint_fast8_t y = 0; y < GBA_FULL_FRAME_HEIGHT; y++) {
      GbaPpuObjectSet set = GbaPpuObjectVisibilityGet(&visibility_, x, y);
      if (x < 64u && y >= 127u) {
        EXPECT_EQ(0u, GbaPpuObjectSetPop(&set));
      }
      EXPECT_TRUE(GbaPpuObjectSetEmpty(&set));
    }
  }
  EXPECT_EQ(0, internal_registers_.object_coordinates[0u].x);
  EXPECT_EQ(127, internal_registers_.object_coordinates[0u].y);
  EXPECT_EQ(32u, internal_registers_.object_coordinates[0u].x_center);
  EXPECT_EQ(159u, internal_registers_.object_coordinates[0u].y_center);
  EXPECT_EQ(64u, internal_registers_.object_coordinates[0u].x_size);
  EXPECT_EQ(64u, internal_registers_.object_coordinates[0u].y_size);
}