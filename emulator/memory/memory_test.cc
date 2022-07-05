extern "C" {
#include "emulator/memory/memory.h"
}

#include "googletest/include/gtest/gtest.h"

class MemoryTest : public testing::Test {
 public:
  void SetUp() override {
    expected_context_ = (void *)0x12345678u;
    expected_address_ = 0xABCDEF12u;
    expected_32_ = 0x13723747u;
    expected_16_ = 0x1532u;
    expected_8_ = 0xF1u;
    memory_ = MemoryAllocate(expected_context_, Load32LEStatic, Load16LEStatic,
                             Load8Static, Store32LEStatic, Store16LEStatic,
                             Store8Static, FreeStatic);
    ASSERT_NE(memory_, nullptr);
  }

  void TearDown() override { MemoryFree(memory_); }

 protected:
  static bool Load32LEStatic(const void *context, uint32_t address,
                             uint32_t *value) {
    EXPECT_EQ(expected_context_, context);
    EXPECT_EQ(expected_address_, address);
    *value = expected_32_;
    return expected_response_;
  }

  static bool Load16LEStatic(const void *context, uint32_t address,
                             uint16_t *value) {
    EXPECT_EQ(expected_context_, context);
    EXPECT_EQ(expected_address_, address);
    *value = expected_16_;
    return expected_response_;
  }

  static bool Load8Static(const void *context, uint32_t address,
                          uint8_t *value) {
    EXPECT_EQ(expected_context_, context);
    EXPECT_EQ(expected_address_, address);
    *value = expected_8_;
    return expected_response_;
  }

  static bool Store32LEStatic(void *context, uint32_t address, uint32_t value) {
    EXPECT_EQ(expected_context_, context);
    EXPECT_EQ(expected_address_, address);
    EXPECT_EQ(expected_32_, value);
    return expected_response_;
  }

  static bool Store16LEStatic(void *context, uint32_t address, uint16_t value) {
    EXPECT_EQ(expected_context_, context);
    EXPECT_EQ(expected_address_, address);
    EXPECT_EQ(expected_16_, value);
    return expected_response_;
  }

  static bool Store8Static(void *context, uint32_t address, uint8_t value) {
    EXPECT_EQ(expected_context_, context);
    EXPECT_EQ(expected_address_, address);
    EXPECT_EQ(expected_8_, value);
    return expected_response_;
  }

  static void FreeStatic(void *context) {
    EXPECT_EQ(expected_context_, context);
  }

  static void *expected_context_;
  static uint32_t expected_address_;
  static uint32_t expected_32_;
  static uint16_t expected_16_;
  static uint8_t expected_8_;
  static bool expected_response_;
  Memory *memory_;
};

void *MemoryTest::expected_context_;
uint32_t MemoryTest::expected_address_;
uint32_t MemoryTest::expected_32_;
uint16_t MemoryTest::expected_16_;
uint8_t MemoryTest::expected_8_;
bool MemoryTest::expected_response_;

TEST_F(MemoryTest, Load32LE) {
  uint32_t value;
  expected_response_ = true;
  EXPECT_TRUE(Load32LE(memory_, expected_address_, &value));
  EXPECT_EQ(expected_32_, value);
  expected_response_ = false;
  EXPECT_FALSE(Load32LE(memory_, expected_address_, &value));
}

TEST_F(MemoryTest, Load16LE) {
  uint16_t value;
  expected_response_ = true;
  EXPECT_TRUE(Load16LE(memory_, expected_address_, &value));
  EXPECT_EQ(expected_16_, value);
  expected_response_ = false;
  EXPECT_FALSE(Load16LE(memory_, expected_address_, &value));
}

TEST_F(MemoryTest, Load8LE) {
  uint8_t value;
  expected_response_ = true;
  EXPECT_TRUE(Load8(memory_, expected_address_, &value));
  EXPECT_EQ(expected_8_, value);
  expected_response_ = false;
  EXPECT_FALSE(Load8(memory_, expected_address_, &value));
}

TEST_F(MemoryTest, Store32LE) {
  expected_response_ = true;
  EXPECT_TRUE(Store32LE(memory_, expected_address_, expected_32_));
  expected_response_ = false;
  EXPECT_FALSE(Store32LE(memory_, expected_address_, expected_32_));
}

TEST_F(MemoryTest, Store16LE) {
  expected_response_ = true;
  EXPECT_TRUE(Store16LE(memory_, expected_address_, expected_16_));
  expected_response_ = false;
  EXPECT_FALSE(Store16LE(memory_, expected_address_, expected_16_));
}

TEST_F(MemoryTest, Store8) {
  expected_response_ = true;
  EXPECT_TRUE(Store8(memory_, expected_address_, expected_8_));
  expected_response_ = false;
  EXPECT_FALSE(Store8(memory_, expected_address_, expected_8_));
}

class MemoryWithBankTest : public testing::Test {
 public:
  void SetUp() override {
    expected_context_ = (void *)0x12345678u;
    MemoryBank *memory_bank = MemoryBankAllocate(1024u, 1u, nullptr);
    ASSERT_NE(memory_bank, nullptr);
    memory_ = MemoryAllocateWithBanks(
        expected_context_, &memory_bank, 1u, Load32LEStatic, Load16LEStatic,
        Load8Static, Store32LEStatic, Store16LEStatic, Store8Static,
        FreeStatic);
    ASSERT_NE(memory_, nullptr);
  }

  void TearDown() override { MemoryFree(memory_); }

 protected:
  static bool Load32LEStatic(const void *context, uint32_t address,
                             uint32_t *value) {
    EXPECT_FALSE(true);
    return false;
  }

  static bool Load16LEStatic(const void *context, uint32_t address,
                             uint16_t *value) {
    EXPECT_FALSE(true);
    return false;
  }

  static bool Load8Static(const void *context, uint32_t address,
                          uint8_t *value) {
    EXPECT_FALSE(true);
    return false;
  }

  static bool Store32LEStatic(void *context, uint32_t address, uint32_t value) {
    EXPECT_FALSE(true);
    return false;
  }

  static bool Store16LEStatic(void *context, uint32_t address, uint16_t value) {
    EXPECT_FALSE(true);
    return false;
  }

  static bool Store8Static(void *context, uint32_t address, uint8_t value) {
    EXPECT_FALSE(true);
    return false;
  }

  static void FreeStatic(void *context) {
    EXPECT_EQ(expected_context_, context);
  }

  static void *expected_context_;
  Memory *memory_;
};

void *MemoryWithBankTest::expected_context_;

TEST_F(MemoryWithBankTest, LoadStore32LE) {
  uint32_t value;
  EXPECT_TRUE(Load32LE(memory_, 0xDEADBEEFu, &value));
  EXPECT_EQ(0u, value);
  EXPECT_TRUE(Store32LE(memory_, 0xDEADBEEFu, 0xCAFEBABEu));
  EXPECT_TRUE(Load32LE(memory_, 0xDEADBEEFu, &value));
  EXPECT_EQ(0xCAFEBABEu, value);
}

TEST_F(MemoryWithBankTest, LoadStore16LE) {
  uint16_t value;
  EXPECT_TRUE(Load16LE(memory_, 0xDEADBEEFu, &value));
  EXPECT_EQ(0u, value);
  EXPECT_TRUE(Store32LE(memory_, 0xDEADBEEFu, 1337u));
  EXPECT_TRUE(Load16LE(memory_, 0xDEADBEEFu, &value));
  EXPECT_EQ(1337u, value);
}

TEST_F(MemoryWithBankTest, LoadStore8) {
  uint8_t value;
  EXPECT_TRUE(Load8(memory_, 0xDEADBEEFu, &value));
  EXPECT_EQ(0u, value);
  EXPECT_TRUE(Load8(memory_, 0xDEADBEEFu, &value));
  EXPECT_TRUE(Store8(memory_, 0xDEADBEEFu, 128u));
  EXPECT_TRUE(Load8(memory_, 0xDEADBEEFu, &value));
  EXPECT_EQ(128u, value);
}