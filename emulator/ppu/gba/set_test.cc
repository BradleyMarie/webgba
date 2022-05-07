extern "C" {
#include "emulator/ppu/gba/set.h"
}

#include "googletest/include/gtest/gtest.h"

class SetTest : public testing::Test {
 public:
  void SetUp() override {
    set_.objects[0u] = 0ull;
    set_.objects[1u] = 0ull;
  }

 protected:
  GbaPpuSet set_;
};

TEST_F(SetTest, Empty) { EXPECT_TRUE(GbaPpuSetEmpty(&set_)); }

TEST_F(SetTest, AddLow) {
  GbaPpuSetAdd(&set_, 0u);
  EXPECT_FALSE(GbaPpuSetEmpty(&set_));
}

TEST_F(SetTest, AddHigh) {
  GbaPpuSetAdd(&set_, 64u);
  EXPECT_FALSE(GbaPpuSetEmpty(&set_));
}

TEST_F(SetTest, RemoveLowMissing) {
  GbaPpuSetRemove(&set_, 0u);
  EXPECT_TRUE(GbaPpuSetEmpty(&set_));
}

TEST_F(SetTest, RemoveHighMissing) {
  GbaPpuSetRemove(&set_, 64u);
  EXPECT_TRUE(GbaPpuSetEmpty(&set_));
}

TEST_F(SetTest, RemoveLowDoesNotDisturb) {
  GbaPpuSetAdd(&set_, 0u);
  GbaPpuSetRemove(&set_, 1u);
  EXPECT_FALSE(GbaPpuSetEmpty(&set_));
  EXPECT_EQ(0u, GbaPpuSetPop(&set_));
  EXPECT_TRUE(GbaPpuSetEmpty(&set_));
}

TEST_F(SetTest, RemoveHighDoesNotDisturb) {
  GbaPpuSetAdd(&set_, 64u);
  GbaPpuSetRemove(&set_, 65u);
  EXPECT_FALSE(GbaPpuSetEmpty(&set_));
  EXPECT_EQ(64u, GbaPpuSetPop(&set_));
  EXPECT_TRUE(GbaPpuSetEmpty(&set_));
}

TEST_F(SetTest, FillAndRemove) {
  EXPECT_TRUE(GbaPpuSetEmpty(&set_));
  for (uint_fast8_t i = 0; i < 128; i++) {
    GbaPpuSetAdd(&set_, i);
    EXPECT_FALSE(GbaPpuSetEmpty(&set_));
  }

  for (uint_fast8_t i = 0; i < 128; i++) {
    EXPECT_FALSE(GbaPpuSetEmpty(&set_));
    GbaPpuSetRemove(&set_, i);
  }
  EXPECT_TRUE(GbaPpuSetEmpty(&set_));
}

TEST_F(SetTest, FillAndPop) {
  EXPECT_TRUE(GbaPpuSetEmpty(&set_));
  for (uint_fast8_t i = 0; i < 128; i++) {
    GbaPpuSetAdd(&set_, i);
    EXPECT_FALSE(GbaPpuSetEmpty(&set_));
  }

  for (uint_fast8_t i = 0; i < 128; i++) {
    EXPECT_FALSE(GbaPpuSetEmpty(&set_));
    uint_fast8_t popped = GbaPpuSetPop(&set_);
    EXPECT_EQ(popped, i);
  }
  EXPECT_TRUE(GbaPpuSetEmpty(&set_));
}

TEST_F(SetTest, PopMultiple) {
  GbaPpuSetAdd(&set_, 127u);
  GbaPpuSetAdd(&set_, 39u);
  GbaPpuSetAdd(&set_, 67u);
  GbaPpuSetAdd(&set_, 5u);
  EXPECT_FALSE(GbaPpuSetEmpty(&set_));
  EXPECT_EQ(5u, GbaPpuSetPop(&set_));
  EXPECT_FALSE(GbaPpuSetEmpty(&set_));
  EXPECT_EQ(39u, GbaPpuSetPop(&set_));
  EXPECT_FALSE(GbaPpuSetEmpty(&set_));
  EXPECT_EQ(67u, GbaPpuSetPop(&set_));
  EXPECT_FALSE(GbaPpuSetEmpty(&set_));
  EXPECT_EQ(127u, GbaPpuSetPop(&set_));
  EXPECT_TRUE(GbaPpuSetEmpty(&set_));
}

TEST_F(SetTest, Intersection) {
  GbaPpuSetAdd(&set_, 1u);
  GbaPpuSetAdd(&set_, 2u);
  GbaPpuSetAdd(&set_, 64u);
  GbaPpuSetAdd(&set_, 65u);

  GbaPpuSet set1;
  set1.objects[0u] = 0u;
  set1.objects[1u] = 0u;
  GbaPpuSetAdd(&set1, 2u);
  GbaPpuSetAdd(&set1, 3u);
  GbaPpuSetAdd(&set1, 65u);
  GbaPpuSetAdd(&set1, 66u);

  GbaPpuSet intersection = GbaPpuSetIntersection(&set_, &set1);
  EXPECT_EQ(2u, GbaPpuSetPop(&intersection));
  EXPECT_FALSE(GbaPpuSetEmpty(&intersection));
  EXPECT_EQ(65u, GbaPpuSetPop(&intersection));
  EXPECT_TRUE(GbaPpuSetEmpty(&intersection));
}