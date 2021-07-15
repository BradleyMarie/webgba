extern "C" {
#include "emulator/memory/gba/bios/bios.h"
}

#include "googletest/include/gtest/gtest.h"

class BiosTest : public testing::Test {
 public:
  void SetUp() override {
    bios_ = GBABiosAllocate();
    ASSERT_NE(nullptr, bios_);
  }

  void TearDown() override { MemoryFree(bios_); }

 protected:
  Memory* bios_;
};

TEST_F(BiosTest, Load32LE) {
  uint32_t value;
  ASSERT_TRUE(Load32LE(bios_, 0u, &value));
  EXPECT_EQ(0xEA00000Cu, value);
}

TEST_F(BiosTest, Load32LEBounds) {
  uint32_t value;
  ASSERT_FALSE(Load32LE(bios_, 0x4000u, &value));
  ASSERT_FALSE(Load32LE(bios_, 0x3FFFu, &value));
  ASSERT_FALSE(Load32LE(bios_, 0x3FFEu, &value));
  ASSERT_FALSE(Load32LE(bios_, 0x3FFDu, &value));
  ASSERT_TRUE(Load32LE(bios_, 0x3FFCu, &value));
}

TEST_F(BiosTest, Load16LE) {
  uint16_t value;
  ASSERT_TRUE(Load16LE(bios_, 0u, &value));
  EXPECT_EQ(0x000Cu, value);
}

TEST_F(BiosTest, Load16LEBounds) {
  uint16_t value;
  ASSERT_FALSE(Load16LE(bios_, 0x4000u, &value));
  ASSERT_FALSE(Load16LE(bios_, 0x3FFFu, &value));
  ASSERT_TRUE(Load16LE(bios_, 0x3FFEu, &value));
}

TEST_F(BiosTest, Load8) {
  uint8_t value;
  ASSERT_TRUE(Load8(bios_, 0u, &value));
  EXPECT_EQ(0x000Cu, value);
}

TEST_F(BiosTest, Load8Bounds) {
  uint8_t value;
  ASSERT_FALSE(Load8(bios_, 0x4000u, &value));
  ASSERT_TRUE(Load8(bios_, 0x3FFFu, &value));
}

TEST_F(BiosTest, Store32LEBounds) {
  ASSERT_FALSE(Store32LE(bios_, 0x4000u, 0u));
  ASSERT_FALSE(Store32LE(bios_, 0x3FFFu, 0u));
  ASSERT_FALSE(Store32LE(bios_, 0x3FFEu, 0u));
  ASSERT_FALSE(Store32LE(bios_, 0x3FFDu, 0u));
  ASSERT_FALSE(Store32LE(bios_, 0x3FFCu, 0u));
}

TEST_F(BiosTest, Store16LEBounds) {
  ASSERT_FALSE(Store16LE(bios_, 0x4000u, 0u));
  ASSERT_FALSE(Store16LE(bios_, 0x3FFFu, 0u));
  ASSERT_FALSE(Store16LE(bios_, 0x3FFEu, 0u));
}

TEST_F(BiosTest, Store8Bounds) {
  ASSERT_FALSE(Store8(bios_, 0x4000u, 0u));
  ASSERT_FALSE(Store8(bios_, 0x3FFFu, 0u));
}