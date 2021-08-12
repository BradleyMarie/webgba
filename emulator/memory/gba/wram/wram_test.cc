extern "C" {
#include "emulator/memory/gba/wram/wram.h"
}

#include "googletest/include/gtest/gtest.h"

#define WRAM_SIZE (256u * 1024u)

class WRamTest : public testing::Test {
 public:
  void SetUp() override {
    memory_ = WRamAllocate();
    ASSERT_NE(nullptr, memory_);
  }

  void TearDown() override { MemoryFree(memory_); }

 protected:
  Memory *memory_;
};

TEST_F(WRamTest, LoadStore8Succeeds) {
  EXPECT_TRUE(Store8(memory_, 0x0u, 0x20u));
  uint8_t value;
  EXPECT_TRUE(Load8(memory_, 0x0u, &value));
  EXPECT_EQ(0x20u, value);
  EXPECT_TRUE(Load8(memory_, WRAM_SIZE, &value));
  EXPECT_EQ(0x20u, value);
}

TEST_F(WRamTest, LoadStore16Succeeds) {
  EXPECT_TRUE(Store16LE(memory_, 0x0u, 0x2030u));
  uint16_t value;
  EXPECT_TRUE(Load16LE(memory_, 0x0u, &value));
  EXPECT_EQ(0x2030u, value);
  EXPECT_TRUE(Load16LE(memory_, WRAM_SIZE, &value));
  EXPECT_EQ(0x2030u, value);
}

TEST_F(WRamTest, LoadStore32Succeeds) {
  EXPECT_TRUE(Store32LE(memory_, 0x0u, 0x20304050u));
  uint32_t value;
  EXPECT_TRUE(Load32LE(memory_, 0x0u, &value));
  EXPECT_EQ(0x20304050u, value);
  EXPECT_TRUE(Load32LE(memory_, WRAM_SIZE, &value));
  EXPECT_EQ(0x20304050u, value);
}