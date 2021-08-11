extern "C" {
#include "emulator/memory/gba/bad/bad.h"
}

#include "googletest/include/gtest/gtest.h"

class BadMemoryTest : public testing::Test {
 public:
  void SetUp() override {
    bios_ = BadMemoryAllocate();
    ASSERT_NE(nullptr, bios_);
  }

  void TearDown() override { MemoryFree(bios_); }

 protected:
  Memory* bios_;
};

TEST_F(BadMemoryTest, Load32LE) {
  uint32_t value;
  ASSERT_FALSE(Load32LE(bios_, 0u, &value));
}

TEST_F(BadMemoryTest, Load16LE) {
  uint16_t value;
  ASSERT_FALSE(Load16LE(bios_, 0u, &value));
}

TEST_F(BadMemoryTest, Load8) {
  uint8_t value;
  ASSERT_FALSE(Load8(bios_, 0u, &value));
}

TEST_F(BadMemoryTest, Store32LEBounds) {
  ASSERT_FALSE(Store32LE(bios_, 0u, 0u));
}

TEST_F(BadMemoryTest, Store16LEBounds) {
  ASSERT_FALSE(Store16LE(bios_, 0u, 0u));
}

TEST_F(BadMemoryTest, Store8Bounds) { ASSERT_FALSE(Store8(bios_, 0u, 0u)); }