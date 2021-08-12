extern "C" {
#include "emulator/ppu/gba/vram/vram.h"
}

#include "googletest/include/gtest/gtest.h"

class VRamTest : public testing::Test {
 public:
  void SetUp() override {
    memset(&ppu_memory_, 0, sizeof(GbaPpuMemory));
    memory_ = VRamAllocate(&ppu_memory_);
    ASSERT_NE(nullptr, memory_);
  }

  void TearDown() override { MemoryFree(memory_); }

 protected:
  GbaPpuMemory ppu_memory_;
  Memory *memory_;
};

TEST_F(VRamTest, LoadStore32BG) {
  EXPECT_TRUE(Store32LE(memory_, 0x0u, 0x20304050u));
  uint32_t value;
  EXPECT_TRUE(Load32LE(memory_, 0x0u, &value));
  EXPECT_EQ(0x20304050u, value);
  EXPECT_TRUE(Load32LE(memory_, VRAM_BANK_SIZE, &value));
  EXPECT_EQ(0x20304050u, value);
}

TEST_F(VRamTest, LoadStore16BG) {
  EXPECT_TRUE(Store16LE(memory_, 0x0u, 0x2030u));
  uint16_t value;
  EXPECT_TRUE(Load16LE(memory_, 0x0u, &value));
  EXPECT_EQ(0x2030u, value);
  EXPECT_TRUE(Load16LE(memory_, VRAM_BANK_SIZE, &value));
  EXPECT_EQ(0x2030u, value);
}

TEST_F(VRamTest, LoadStore8BG) {
  EXPECT_TRUE(Store8(memory_, 0x0u, 0x20u));
  uint8_t value;
  EXPECT_TRUE(Load8(memory_, 0x0u, &value));
  EXPECT_EQ(0x20u, value);
  EXPECT_TRUE(Load8(memory_, 0x1u, &value));
  EXPECT_EQ(0x20u, value);
  EXPECT_TRUE(Load8(memory_, VRAM_BANK_SIZE, &value));
  EXPECT_EQ(0x20u, value);
  EXPECT_TRUE(Load8(memory_, VRAM_BANK_SIZE + 1u, &value));
  EXPECT_EQ(0x20u, value);

  EXPECT_TRUE(Store8(memory_, 0x3u, 0x30u));
  EXPECT_TRUE(Load8(memory_, 0x2u, &value));
  EXPECT_EQ(0x30u, value);
  EXPECT_TRUE(Load8(memory_, 0x3u, &value));
  EXPECT_EQ(0x30u, value);
  EXPECT_TRUE(Load8(memory_, VRAM_BANK_SIZE + 2u, &value));
  EXPECT_EQ(0x30u, value);
  EXPECT_TRUE(Load8(memory_, VRAM_BANK_SIZE + 3u, &value));
  EXPECT_EQ(0x30u, value);
}

TEST_F(VRamTest, LoadStore32OBJ) {
  EXPECT_TRUE(Store32LE(memory_, VRAM_BG_SIZE, 0x20304050u));
  uint32_t value;
  EXPECT_TRUE(Load32LE(memory_, VRAM_BG_SIZE, &value));
  EXPECT_EQ(0x20304050u, value);
  EXPECT_TRUE(Load32LE(memory_, VRAM_BG_SIZE + VRAM_OBJ_SIZE, &value));
  EXPECT_EQ(0x20304050u, value);
}

TEST_F(VRamTest, LoadStore16OBJ) {
  EXPECT_TRUE(Store16LE(memory_, VRAM_BG_SIZE, 0x2030u));
  uint16_t value;
  EXPECT_TRUE(Load16LE(memory_, VRAM_BG_SIZE, &value));
  EXPECT_EQ(0x2030u, value);
  EXPECT_TRUE(Load16LE(memory_, VRAM_BG_SIZE + VRAM_OBJ_SIZE, &value));
  EXPECT_EQ(0x2030u, value);
}

TEST_F(VRamTest, LoadStore8OBJ) {
  EXPECT_TRUE(Store8(memory_, VRAM_BG_SIZE, 0x20u));
  uint8_t value;
  EXPECT_TRUE(Load8(memory_, VRAM_BG_SIZE, &value));
  EXPECT_EQ(0x0u, value);
  EXPECT_TRUE(Load8(memory_, VRAM_BG_SIZE + 0x1u, &value));
  EXPECT_EQ(0x0u, value);
  EXPECT_TRUE(Load8(memory_, VRAM_BG_SIZE + VRAM_OBJ_SIZE, &value));
  EXPECT_EQ(0x0u, value);
  EXPECT_TRUE(Load8(memory_, VRAM_BG_SIZE + VRAM_OBJ_SIZE + 1u, &value));
  EXPECT_EQ(0x0u, value);

  EXPECT_TRUE(Store8(memory_, VRAM_BG_SIZE + 0x3u, 0x30u));
  EXPECT_TRUE(Load8(memory_, VRAM_BG_SIZE + 0x2u, &value));
  EXPECT_EQ(0x0u, value);
  EXPECT_TRUE(Load8(memory_, VRAM_BG_SIZE + 0x3u, &value));
  EXPECT_EQ(0x0u, value);
  EXPECT_TRUE(Load8(memory_, VRAM_BG_SIZE + VRAM_OBJ_SIZE + 2u, &value));
  EXPECT_EQ(0x0u, value);
  EXPECT_TRUE(Load8(memory_, VRAM_BG_SIZE + VRAM_OBJ_SIZE + 3u, &value));
  EXPECT_EQ(0x0u, value);
}