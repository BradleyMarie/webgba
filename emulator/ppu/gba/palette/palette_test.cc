extern "C" {
#include "emulator/ppu/gba/palette/palette.h"
}

#include "googletest/include/gtest/gtest.h"

class PaletteTest : public testing::Test {
 public:
  void SetUp() override {
    memset(&ppu_memory_, 0, sizeof(GbaPpuMemory));
    memory_ = PaletteAllocate(&ppu_memory_);
    ASSERT_NE(nullptr, memory_);
  }

  void TearDown() override { MemoryFree(memory_); }

 protected:
  GbaPpuMemory ppu_memory_;
  Memory *memory_;
};

TEST_F(PaletteTest, LoadStore32Succeeds) {
  EXPECT_TRUE(Store32LE(memory_, 0x0u, 0x20304050u));
  uint32_t value;
  EXPECT_TRUE(Load32LE(memory_, 0x0u, &value));
  EXPECT_EQ(0x20304050u, value);
  EXPECT_TRUE(Load32LE(memory_, PALETTE_SIZE, &value));
  EXPECT_EQ(0x20304050u, value);
}

TEST_F(PaletteTest, LoadStore16Succeeds) {
  EXPECT_TRUE(Store16LE(memory_, 0x0u, 0x2030u));
  uint16_t value;
  EXPECT_TRUE(Load16LE(memory_, 0x0u, &value));
  EXPECT_EQ(0x2030u, value);
  EXPECT_TRUE(Load16LE(memory_, PALETTE_SIZE, &value));
  EXPECT_EQ(0x2030u, value);
}

TEST_F(PaletteTest, LoadStore8Succeeds) {
  EXPECT_TRUE(Store8(memory_, 0x0u, 0x20u));
  uint8_t value;
  EXPECT_TRUE(Load8(memory_, 0x0u, &value));
  EXPECT_EQ(0x20u, value);
  EXPECT_TRUE(Load8(memory_, 0x1u, &value));
  EXPECT_EQ(0x20u, value);
  EXPECT_TRUE(Load8(memory_, PALETTE_SIZE, &value));
  EXPECT_EQ(0x20u, value);
  EXPECT_TRUE(Load8(memory_, PALETTE_SIZE + 1u, &value));
  EXPECT_EQ(0x20u, value);

  EXPECT_TRUE(Store8(memory_, 0x3u, 0x30u));
  EXPECT_TRUE(Load8(memory_, 0x2u, &value));
  EXPECT_EQ(0x30u, value);
  EXPECT_TRUE(Load8(memory_, 0x3u, &value));
  EXPECT_EQ(0x30u, value);
  EXPECT_TRUE(Load8(memory_, PALETTE_SIZE + 2u, &value));
  EXPECT_EQ(0x30u, value);
  EXPECT_TRUE(Load8(memory_, PALETTE_SIZE + 3u, &value));
  EXPECT_EQ(0x30u, value);
}