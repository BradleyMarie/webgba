extern "C" {
#include "emulator/memory/gba/io/io.h"
}

#include "googletest/include/gtest/gtest.h"

class IoMemoryTest : public testing::Test {
 public:
  void SetUp() override {
    banks_[0u] =
        MemoryAllocate(banks_ + 0u, Load32LEFunc, Load16LEFunc, Load8Func,
                       Store32LEFunc, Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, banks_[0u]);
    banks_[1u] =
        MemoryAllocate(banks_ + 1u, Load32LEFunc, Load16LEFunc, Load8Func,
                       Store32LEFunc, Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, banks_[1u]);
    banks_[2u] =
        MemoryAllocate(banks_ + 2u, Load32LEFunc, Load16LEFunc, Load8Func,
                       Store32LEFunc, Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, banks_[2u]);
    banks_[3u] =
        MemoryAllocate(banks_ + 3u, Load32LEFunc, Load16LEFunc, Load8Func,
                       Store32LEFunc, Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, banks_[3u]);
    banks_[4u] =
        MemoryAllocate(banks_ + 4u, Load32LEFunc, Load16LEFunc, Load8Func,
                       Store32LEFunc, Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, banks_[4u]);
    banks_[5u] =
        MemoryAllocate(banks_ + 5u, Load32LEFunc, Load16LEFunc, Load8Func,
                       Store32LEFunc, Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, banks_[5u]);
    io_ = IoMemoryAllocate(banks_[0u], banks_[1u], banks_[2u], banks_[3u],
                           banks_[4u], banks_[5u]);
    ASSERT_NE(nullptr, io_);
  }

  void TearDown() override {
    // banks_ are owned by io_ and do not need to be freed
    MemoryFree(io_);
  }

  static size_t BankTriggered(const void *context) {
    for (size_t i = 0u; i < 6u; i++) {
      if (banks_ + i == context) {
        return i;
      }
    }
    return (size_t)-1;
  }

  static bool Load32LEFunc(const void *context, uint32_t address,
                           uint32_t *value) {
    EXPECT_EQ(expected_bank_, BankTriggered(context));
    EXPECT_EQ(expected_address_, address);
    *value = expected32_;
    return expected_response_;
  }

  static bool Load16LEFunc(const void *context, uint32_t address,
                           uint16_t *value) {
    EXPECT_EQ(expected_bank_, BankTriggered(context));
    EXPECT_EQ(expected_address_, address);
    *value = expected16_;
    return expected_response_;
  }

  static bool Load8Func(const void *context, uint32_t address, uint8_t *value) {
    EXPECT_EQ(expected_bank_, BankTriggered(context));
    EXPECT_EQ(expected_address_, address);
    *value = expected8_;
    return expected_response_;
  }

  static bool Store32LEFunc(void *context, uint32_t address, uint32_t value) {
    EXPECT_EQ(expected_bank_, BankTriggered(context));
    EXPECT_EQ(expected_address_, address);
    EXPECT_EQ(expected32_, value);
    return expected_response_;
  }

  static bool Store16LEFunc(void *context, uint32_t address, uint16_t value) {
    EXPECT_EQ(expected_bank_, BankTriggered(context));
    EXPECT_EQ(expected_address_, address);
    EXPECT_EQ(expected16_, value);
    return expected_response_;
  }

  static bool Store8Func(void *context, uint32_t address, uint8_t value) {
    EXPECT_EQ(expected_bank_, BankTriggered(context));
    EXPECT_EQ(expected_address_, address);
    EXPECT_EQ(expected8_, value);
    return expected_response_;
  }

  void TestAddress(uint32_t address, size_t expected_bank,
                   uint32_t expected_address, uint32_t value32,
                   uint16_t value16, uint8_t value8) {
    expected_bank_ = expected_bank;
    expected_address_ = expected_address;

    expected32_ = value32;
    uint32_t actual32;

    expected_response_ = true;
    EXPECT_TRUE(Load32LE(io_, address, &actual32));
    EXPECT_EQ(expected32_, actual32);
    EXPECT_TRUE(Store32LE(io_, address, value32));

    expected_response_ = false;
    EXPECT_FALSE(Load32LE(io_, address, &actual32));
    EXPECT_FALSE(Store32LE(io_, address, value32));

    expected16_ = value16;
    uint16_t actual16;

    expected_response_ = true;
    EXPECT_TRUE(Load16LE(io_, address, &actual16));
    EXPECT_EQ(expected16_, actual16);
    EXPECT_TRUE(Store16LE(io_, address, value16));

    expected_response_ = false;
    EXPECT_FALSE(Load16LE(io_, address, &actual16));
    EXPECT_FALSE(Store16LE(io_, address, value16));

    expected8_ = value8;
    uint8_t actual8;

    expected_response_ = true;
    EXPECT_TRUE(Load8(io_, address, &actual8));
    EXPECT_EQ(expected8_, actual8);
    EXPECT_TRUE(Store8(io_, address, value8));

    expected_response_ = false;
    EXPECT_FALSE(Load8(io_, address, &actual8));
    EXPECT_FALSE(Store8(io_, address, value8));
  }

 protected:
  static Memory *banks_[6u];
  static Memory *io_;

  static size_t expected_bank_;
  static uint32_t expected_address_;
  static uint32_t expected32_;
  static uint16_t expected16_;
  static uint8_t expected8_;
  static bool expected_response_;
};

Memory *IoMemoryTest::banks_[6u];
Memory *IoMemoryTest::io_;
size_t IoMemoryTest::expected_bank_;
uint32_t IoMemoryTest::expected_address_;
uint32_t IoMemoryTest::expected32_;
uint16_t IoMemoryTest::expected16_;
uint8_t IoMemoryTest::expected8_;
bool IoMemoryTest::expected_response_;

TEST_F(IoMemoryTest, PpuBank) {
  for (uint32_t addr = 0u; addr < 0x060u; addr++) {
    TestAddress(addr, 0u, addr, 0x12345678u, 0x4321u, 0xABu);
  }
}

TEST_F(IoMemoryTest, SoundBank) {
  for (uint32_t addr = 0x60u; addr < 0x0B0u; addr++) {
    TestAddress(addr, 1u, addr - 0x60u, 0x12345678u, 0x4321u, 0xABu);
  }
}

TEST_F(IoMemoryTest, DmaBank) {
  for (uint32_t addr = 0xB0u; addr < 0x100u; addr++) {
    TestAddress(addr, 2u, addr - 0xB0u, 0x12345678u, 0x4321u, 0xABu);
  }
}

TEST_F(IoMemoryTest, TimerBank) {
  for (uint32_t addr = 0x100u; addr < 0x120u; addr++) {
    TestAddress(addr, 3u, addr - 0x100u, 0x12345678u, 0x4321u, 0xABu);
  }
}

TEST_F(IoMemoryTest, PeripheralsBank) {
  for (uint32_t addr = 0x120u; addr < 0x200u; addr++) {
    TestAddress(addr, 4u, addr - 0x120u, 0x12345678u, 0x4321u, 0xABu);
  }
}

TEST_F(IoMemoryTest, PlatformBank) {
  for (uint32_t addr = 0x200u; addr < 0x100000u; addr++) {
    TestAddress(addr, 5u, addr - 0x200u, 0x12345678u, 0x4321u, 0xABu);
  }
}