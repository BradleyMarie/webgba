extern "C" {
#include "emulator/ppu/gba/object_set.h"
}

#include "googletest/include/gtest/gtest.h"

class ObjectSetTest : public testing::Test {
 public:
  void SetUp() override {
    set_.objects[0u] = 0ull;
    set_.objects[1u] = 0ull;
  }

 protected:
  GbaPpuObjectSet set_;
};

TEST_F(ObjectSetTest, Empty) { EXPECT_TRUE(GbaPpuObjectSetEmpty(&set_)); }

TEST_F(ObjectSetTest, AddLow) {
  GbaPpuObjectSetAdd(&set_, 0u);
  EXPECT_FALSE(GbaPpuObjectSetEmpty(&set_));
}

TEST_F(ObjectSetTest, AddHigh) {
  GbaPpuObjectSetAdd(&set_, 64u);
  EXPECT_FALSE(GbaPpuObjectSetEmpty(&set_));
}

TEST_F(ObjectSetTest, RemoveLowMissing) {
  GbaPpuObjectSetRemove(&set_, 0u);
  EXPECT_TRUE(GbaPpuObjectSetEmpty(&set_));
}

TEST_F(ObjectSetTest, RemoveHighMissing) {
  GbaPpuObjectSetRemove(&set_, 64u);
  EXPECT_TRUE(GbaPpuObjectSetEmpty(&set_));
}

TEST_F(ObjectSetTest, RemoveLowDoesNotDisturb) {
  GbaPpuObjectSetAdd(&set_, 0u);
  GbaPpuObjectSetRemove(&set_, 1u);
  EXPECT_FALSE(GbaPpuObjectSetEmpty(&set_));
  EXPECT_EQ(0u, GbaPpuObjectSetPop(&set_));
  EXPECT_TRUE(GbaPpuObjectSetEmpty(&set_));
}

TEST_F(ObjectSetTest, RemoveHighDoesNotDisturb) {
  GbaPpuObjectSetAdd(&set_, 64u);
  GbaPpuObjectSetRemove(&set_, 65u);
  EXPECT_FALSE(GbaPpuObjectSetEmpty(&set_));
  EXPECT_EQ(64u, GbaPpuObjectSetPop(&set_));
  EXPECT_TRUE(GbaPpuObjectSetEmpty(&set_));
}

TEST_F(ObjectSetTest, PopMultiple) {
  GbaPpuObjectSetAdd(&set_, 127u);
  GbaPpuObjectSetAdd(&set_, 39u);
  GbaPpuObjectSetAdd(&set_, 67u);
  GbaPpuObjectSetAdd(&set_, 5u);
  EXPECT_FALSE(GbaPpuObjectSetEmpty(&set_));
  EXPECT_EQ(5u, GbaPpuObjectSetPop(&set_));
  EXPECT_FALSE(GbaPpuObjectSetEmpty(&set_));
  EXPECT_EQ(39u, GbaPpuObjectSetPop(&set_));
  EXPECT_FALSE(GbaPpuObjectSetEmpty(&set_));
  EXPECT_EQ(67u, GbaPpuObjectSetPop(&set_));
  EXPECT_FALSE(GbaPpuObjectSetEmpty(&set_));
  EXPECT_EQ(127u, GbaPpuObjectSetPop(&set_));
  EXPECT_TRUE(GbaPpuObjectSetEmpty(&set_));
}

TEST_F(ObjectSetTest, Intersection) {
  GbaPpuObjectSetAdd(&set_, 1u);
  GbaPpuObjectSetAdd(&set_, 2u);
  GbaPpuObjectSetAdd(&set_, 64u);
  GbaPpuObjectSetAdd(&set_, 65u);

  GbaPpuObjectSet set1;
  set1.objects[0u] = 0u;
  set1.objects[1u] = 0u;
  GbaPpuObjectSetAdd(&set1, 2u);
  GbaPpuObjectSetAdd(&set1, 3u);
  GbaPpuObjectSetAdd(&set1, 65u);
  GbaPpuObjectSetAdd(&set1, 66u);

  GbaPpuObjectSet intersection = GbaPpuObjectSetIntersection(&set_, &set1);
  EXPECT_EQ(2u, GbaPpuObjectSetPop(&intersection));
  EXPECT_FALSE(GbaPpuObjectSetEmpty(&intersection));
  EXPECT_EQ(65u, GbaPpuObjectSetPop(&intersection));
  EXPECT_TRUE(GbaPpuObjectSetEmpty(&intersection));
}