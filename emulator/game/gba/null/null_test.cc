extern "C" {
#include "emulator/game/gba/null/null.h"
}

#include "googletest/include/gtest/gtest.h"

class NullTest : public testing::Test {
 public:
  void SetUp() override {
    memory_ = GbaNullSramAllocate();
    ASSERT_NE(memory_, nullptr);
  }

  void TearDown() override { MemoryFree(memory_); }

 protected:
  Memory *memory_;
};

TEST_F(NullTest, StoreLoad32LE) {
  Store32LE(memory_, 1024u, UINT32_MAX);

  uint32_t value;
  Load32LE(memory_, 0u, &value);
  EXPECT_EQ(0u, value);
}

TEST_F(NullTest, StoreLoad16LE) {
  Store16LE(memory_, 1024u, UINT16_MAX);

  uint16_t value;
  Load16LE(memory_, 0u, &value);
  EXPECT_EQ(0u, value);
}

TEST_F(NullTest, StoreLoad8LE) {
  Store8(memory_, 1024u, UINT8_MAX);

  uint8_t value;
  Load8(memory_, 0u, &value);
  EXPECT_EQ(0u, value);
}