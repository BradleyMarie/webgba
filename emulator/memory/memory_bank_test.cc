extern "C" {
#include "emulator/memory/memory_bank.h"
}

#include "googletest/include/gtest/gtest.h"

class MemoryBankTest : public testing::Test {
 public:
  void SetUp() override {
    memory_bank_ = MemoryBankAllocate(1024u, 2u, WriteCallback);
    ASSERT_NE(memory_bank_, nullptr);
  }

  void TearDown() override { MemoryBankFree(memory_bank_); }

 protected:
  static void WriteCallback(MemoryBank *memory_bank, uint32_t address,
                            uint32_t value) {
    EXPECT_EQ(memory_bank_, memory_bank);
    EXPECT_EQ(expected_address_, address);
    EXPECT_EQ(expected_value_, value);
  }

  static MemoryBank *memory_bank_;
  static uint32_t expected_address_;
  static uint32_t expected_value_;
};

MemoryBank *MemoryBankTest::memory_bank_;
uint32_t MemoryBankTest::expected_address_;
uint32_t MemoryBankTest::expected_value_;

TEST_F(MemoryBankTest, Load32LE) {
  expected_value_ = UINT32_MAX;
  expected_address_ = 0u;
  MemoryBankStore32LE(memory_bank_, 1024u, UINT32_MAX);

  uint32_t value;
  MemoryBankLoad32LE(memory_bank_, 0u, &value);
  EXPECT_EQ(expected_value_, value);
}

TEST_F(MemoryBankTest, Load16LE) {
  expected_value_ = UINT16_MAX;
  expected_address_ = 0u;
  MemoryBankStore16LE(memory_bank_, 1024u, UINT16_MAX);

  uint16_t value;
  MemoryBankLoad16LE(memory_bank_, 0u, &value);
  EXPECT_EQ(expected_value_, value);
}

TEST_F(MemoryBankTest, Load8LE) {
  expected_value_ = UINT8_MAX;
  expected_address_ = 0u;
  MemoryBankStore8(memory_bank_, 1024u, UINT8_MAX);

  uint8_t value;
  MemoryBankLoad8(memory_bank_, 0u, &value);
  EXPECT_EQ(expected_value_, value);
}

TEST_F(MemoryBankTest, IgnoreWrites) {
  MemoryBankIgnoreWrites(memory_bank_);

  expected_value_ = UINT32_MAX;
  expected_address_ = 0u;
  MemoryBankStore32LE(memory_bank_, 1024u, UINT32_MAX);

  uint32_t value;
  MemoryBankLoad32LE(memory_bank_, 0u, &value);
  EXPECT_EQ(0u, value);
}

TEST_F(MemoryBankTest, ChangeBank) {
  expected_value_ = UINT32_MAX;
  expected_address_ = 0u;
  MemoryBankStore32LE(memory_bank_, 1024u, UINT32_MAX);

  MemoryBankChangeBank(memory_bank_, 1u);

  uint32_t value;
  MemoryBankLoad32LE(memory_bank_, 0u, &value);
  EXPECT_EQ(0u, value);

  expected_value_ = 1337u;
  expected_address_ = 0u;
  MemoryBankStore32LE(memory_bank_, 1024u, 1337u);

  MemoryBankLoad32LE(memory_bank_, 0u, &value);
  EXPECT_EQ(1337u, value);

  MemoryBankChangeBank(memory_bank_, 0u);

  MemoryBankLoad32LE(memory_bank_, 0u, &value);
  EXPECT_EQ(UINT32_MAX, value);
}