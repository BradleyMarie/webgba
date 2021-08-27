extern "C" {
#include "emulator/ppu/gba/oam/oam.h"
}

#include "googletest/include/gtest/gtest.h"

class OamTest : public testing::Test {
 public:
  void SetUp() override {
    memset(&oam_memory_, 0, sizeof(GbaPpuObjectAttributeMemory));
    memory_ = OamAllocate(&oam_memory_, nullptr, &reference_count_);
    ASSERT_NE(nullptr, memory_);
  }

  void TearDown() override { MemoryFree(memory_); }

 protected:
  GbaPpuObjectAttributeMemory oam_memory_;
  Memory *memory_;
  uint16_t reference_count_;
};

TEST_F(OamTest, LoadStore32Succeeds) {
  EXPECT_TRUE(Store32LE(memory_, 0x0u, 0x20304050u));
  uint32_t value;
  EXPECT_TRUE(Load32LE(memory_, 0x0u, &value));
  EXPECT_EQ(0x20304050u, value);
  EXPECT_TRUE(Load32LE(memory_, OAM_SIZE, &value));
  EXPECT_EQ(0x20304050u, value);
}

TEST_F(OamTest, LoadStore16Succeeds) {
  EXPECT_TRUE(Store16LE(memory_, 0x0u, 0x2030u));
  uint16_t value;
  EXPECT_TRUE(Load16LE(memory_, 0x0u, &value));
  EXPECT_EQ(0x2030u, value);
  EXPECT_TRUE(Load16LE(memory_, OAM_SIZE, &value));
  EXPECT_EQ(0x2030u, value);
}

TEST_F(OamTest, LoadStore8Succeeds) {
  EXPECT_TRUE(Store8(memory_, 0x0u, 0x20u));
  uint8_t value;
  EXPECT_TRUE(Load8(memory_, 0x0u, &value));
  EXPECT_EQ(0x0u, value);
  EXPECT_TRUE(Load8(memory_, OAM_SIZE, &value));
  EXPECT_EQ(0x0u, value);
}