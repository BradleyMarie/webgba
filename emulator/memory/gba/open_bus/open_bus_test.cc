extern "C" {
#include "emulator/memory/gba/open_bus/open_bus.h"
}

#include "googletest/include/gtest/gtest.h"

class OpenBusTest : public testing::Test {
 public:
  void SetUp() override {
    expected_context_ = (void *)0x12345678u;
    expected_address_ = 0xABCDEF12u;
    expected_32_ = 0x13723747u;
    expected_16_ = 0x1532u;
    expected_8_ = 0xF1u;
    Memory *memory = MemoryAllocate(
        expected_context_, Load32LEStatic, Load16LEStatic, Load8Static,
        Store32LEStatic, Store16LEStatic, Store8Static, FreeStatic);
    ASSERT_NE(memory, nullptr);
    memory_ = OpenBusAllocate(memory);
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

void *OpenBusTest::expected_context_;
uint32_t OpenBusTest::expected_address_;
uint32_t OpenBusTest::expected_32_;
uint16_t OpenBusTest::expected_16_;
uint8_t OpenBusTest::expected_8_;
bool OpenBusTest::expected_response_;

TEST_F(OpenBusTest, Load32LE) {
  uint32_t value;
  expected_response_ = true;
  EXPECT_TRUE(Load32LE(memory_, expected_address_, &value));
  EXPECT_EQ(expected_32_, value);
  expected_response_ = false;
  EXPECT_TRUE(Load32LE(memory_, expected_address_, &value));
  EXPECT_EQ(0u, value);
}

TEST_F(OpenBusTest, Load16LE) {
  uint16_t value;
  expected_response_ = true;
  EXPECT_TRUE(Load16LE(memory_, expected_address_, &value));
  EXPECT_EQ(expected_16_, value);
  expected_response_ = false;
  EXPECT_TRUE(Load16LE(memory_, expected_address_, &value));
  EXPECT_EQ(0u, value);
}

TEST_F(OpenBusTest, Load8LE) {
  uint8_t value;
  expected_response_ = true;
  EXPECT_TRUE(Load8(memory_, expected_address_, &value));
  EXPECT_EQ(expected_8_, value);
  expected_response_ = false;
  EXPECT_TRUE(Load8(memory_, expected_address_, &value));
  EXPECT_EQ(0u, value);
}

TEST_F(OpenBusTest, Store32LE) {
  expected_response_ = true;
  EXPECT_TRUE(Store32LE(memory_, expected_address_, expected_32_));
  expected_response_ = false;
  EXPECT_TRUE(Store32LE(memory_, expected_address_, expected_32_));
}

TEST_F(OpenBusTest, Store16LE) {
  expected_response_ = true;
  EXPECT_TRUE(Store16LE(memory_, expected_address_, expected_16_));
  expected_response_ = false;
  EXPECT_TRUE(Store16LE(memory_, expected_address_, expected_16_));
}

TEST_F(OpenBusTest, Store8) {
  expected_response_ = true;
  EXPECT_TRUE(Store8(memory_, expected_address_, expected_8_));
  expected_response_ = false;
  EXPECT_TRUE(Store8(memory_, expected_address_, expected_8_));
}