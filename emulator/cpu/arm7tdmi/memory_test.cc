extern "C" {
#include "emulator/cpu/arm7tdmi/memory.h"
}

#include <cstring>
#include <vector>

#include "googletest/include/gtest/gtest.h"

class MemoryTest : public testing::Test {
 public:
  void SetUp() override {
    for (char &c : memory_space_) {
      c = 0;
    }
    memory_ = MemoryAllocate(nullptr, Load32LEWithRotation, Load16LE, Load8,
                             Store32LE, Store16LE, Store8, nullptr);
    ASSERT_NE(nullptr, memory_);
  }

  void TearDown() override { MemoryFree(memory_); }

 protected:
  static bool Load32LEWithRotation(const void *context, uint32_t address,
                                   uint32_t *value) {
    if (address + sizeof(uint32_t) - 1 >= memory_space_.size()) {
      return false;
    }
    char *data = memory_space_.data() + address;
    *value = *reinterpret_cast<uint32_t *>(data);
    return true;
  }

  static bool Load16LE(const void *context, uint32_t address, uint16_t *value) {
    if (address + sizeof(uint16_t) - 1 >= memory_space_.size()) {
      return false;
    }
    char *data = memory_space_.data() + address;
    *value = *reinterpret_cast<uint16_t *>(data);
    return true;
  }

  static bool Load8(const void *context, uint32_t address, uint8_t *value) {
    if (address > memory_space_.size()) {
      return false;
    }
    *value = memory_space_[address];
    return true;
  }

  static bool Store32LE(void *context, uint32_t address, uint32_t value) {
    if (address + sizeof(uint32_t) - 1 >= memory_space_.size()) {
      return false;
    }
    char *data = memory_space_.data() + address;
    *reinterpret_cast<uint32_t *>(data) = value;
    return true;
  }

  static bool Store16LE(void *context, uint32_t address, uint16_t value) {
    if (address + sizeof(uint16_t) - 1 >= memory_space_.size()) {
      return false;
    }
    char *data = memory_space_.data() + address;
    *reinterpret_cast<uint16_t *>(data) = value;
    return true;
  }

  static bool Store8(void *context, uint32_t address, uint8_t value) {
    if (address >= memory_space_.size()) {
      return false;
    }
    memory_space_[address] = value;
    return true;
  }

  bool MemoryIsZero() {
    for (char c : memory_space_) {
      if (c != 0) {
        return false;
      }
    }
    return true;
  }

  static std::vector<char> memory_space_;
  Memory *memory_;
};

std::vector<char> MemoryTest::memory_space_(1024, 0);

TEST_F(MemoryTest, ArmLoad32LEWithRotation_0) {
  ASSERT_TRUE(Store32LE(nullptr, 0u, 0xAABBCCDDu));
  uint32_t value;
  ASSERT_TRUE(ArmLoad32LEWithRotation(memory_, 0u, &value));
  EXPECT_EQ(0xAABBCCDDu, value);
}

TEST_F(MemoryTest, ArmLoad32LEWithRotation_1) {
  ASSERT_TRUE(Store32LE(nullptr, 0u, 0xAABBCCDDu));
  uint32_t value;
  ASSERT_TRUE(ArmLoad32LEWithRotation(memory_, 1u, &value));
  EXPECT_EQ(0xDDAABBCCu, value);
}

TEST_F(MemoryTest, ArmLoad32LEWithRotation_2) {
  ASSERT_TRUE(Store32LE(nullptr, 0u, 0xAABBCCDDu));
  uint32_t value;
  ASSERT_TRUE(ArmLoad32LEWithRotation(memory_, 2u, &value));
  EXPECT_EQ(0xCCDDAABBu, value);
}

TEST_F(MemoryTest, ArmLoad32LEWithRotation_3) {
  ASSERT_TRUE(Store32LE(nullptr, 0u, 0xAABBCCDDu));
  uint32_t value;
  ASSERT_TRUE(ArmLoad32LEWithRotation(memory_, 3u, &value));
  EXPECT_EQ(0xBBCCDDAAu, value);
}

TEST_F(MemoryTest, ArmLoad32LE_0) {
  ASSERT_TRUE(Store32LE(nullptr, 0u, 0xAABBCCDDu));
  uint32_t value;
  ASSERT_TRUE(ArmLoad32LE(memory_, 0u, &value));
  EXPECT_EQ(0xAABBCCDDu, value);
}

TEST_F(MemoryTest, ArmLoad32LE_1) {
  ASSERT_TRUE(Store32LE(nullptr, 0u, 0xAABBCCDDu));
  uint32_t value;
  ASSERT_TRUE(ArmLoad32LE(memory_, 1u, &value));
  EXPECT_EQ(0xAABBCCDDu, value);
}

TEST_F(MemoryTest, ArmLoad32LE_2) {
  ASSERT_TRUE(Store32LE(nullptr, 0u, 0xAABBCCDDu));
  uint32_t value;
  ASSERT_TRUE(ArmLoad32LE(memory_, 2u, &value));
  EXPECT_EQ(0xAABBCCDDu, value);
}

TEST_F(MemoryTest, ArmLoad32LE_3) {
  ASSERT_TRUE(Store32LE(nullptr, 0u, 0xAABBCCDDu));
  uint32_t value;
  ASSERT_TRUE(ArmLoad32LE(memory_, 3u, &value));
  EXPECT_EQ(0xAABBCCDDu, value);
}

TEST_F(MemoryTest, ArmLoad16LEWithRotation_0) {
  ASSERT_TRUE(Store16LE(nullptr, 0u, 0xAABBu));
  uint32_t value;
  ASSERT_TRUE(ArmLoad16LEWithRotation(memory_, 0u, &value));
  EXPECT_EQ(0xAABBu, value);
}

TEST_F(MemoryTest, ArmLoad16LEWithRotation_1) {
  ASSERT_TRUE(Store16LE(nullptr, 0u, 0xAABBu));
  uint32_t value;
  ASSERT_TRUE(ArmLoad16LEWithRotation(memory_, 1u, &value));
  EXPECT_EQ(0xBB0000AAu, value);
}

TEST_F(MemoryTest, ArmLoad32SLEWithRotation_0) {
  ASSERT_TRUE(Store32SLE(memory_, 0u, (int32_t)0xAABBCCDD));
  int32_t value;
  ASSERT_TRUE(ArmLoad32SLEWithRotation(memory_, 0u, &value));
  EXPECT_EQ((int32_t)0xAABBCCDD, value);
}

TEST_F(MemoryTest, ArmLoad32SLEWithRotation_1) {
  ASSERT_TRUE(Store32SLE(memory_, 0u, (int32_t)0xAABBCCDD));
  int32_t value;
  ASSERT_TRUE(ArmLoad32SLEWithRotation(memory_, 1u, &value));
  EXPECT_EQ((int32_t)0xDDAABBCCl, value);
}

TEST_F(MemoryTest, ArmLoad32SLEWithRotation_2) {
  ASSERT_TRUE(Store32SLE(memory_, 0u, (int32_t)0xAABBCCDD));
  int32_t value;
  ASSERT_TRUE(ArmLoad32SLEWithRotation(memory_, 2u, &value));
  EXPECT_EQ((int32_t)0xCCDDAABB, value);
}

TEST_F(MemoryTest, ArmLoad32SLEWithRotation_3) {
  ASSERT_TRUE(Store32SLE(memory_, 0u, (int32_t)0xAABBCCDD));
  int32_t value;
  ASSERT_TRUE(ArmLoad32SLEWithRotation(memory_, 3u, &value));
  EXPECT_EQ((int32_t)0xBBCCDDAA, value);
}

TEST_F(MemoryTest, ArmLoad32SLE_0) {
  ASSERT_TRUE(Store32SLE(memory_, 0u, (int32_t)0xAABBCCDD));
  int32_t value;
  ASSERT_TRUE(ArmLoad32SLE(memory_, 0u, &value));
  EXPECT_EQ((int32_t)0xAABBCCDD, value);
}

TEST_F(MemoryTest, ArmLoad32SLE_1) {
  ASSERT_TRUE(Store32SLE(memory_, 0u, (int32_t)0xAABBCCDD));
  int32_t value;
  ASSERT_TRUE(ArmLoad32SLE(memory_, 1u, &value));
  EXPECT_EQ((int32_t)0xAABBCCDD, value);
}

TEST_F(MemoryTest, ArmLoad32SLE_2) {
  ASSERT_TRUE(Store32SLE(memory_, 0u, (int32_t)0xAABBCCDD));
  int32_t value;
  ASSERT_TRUE(ArmLoad32SLE(memory_, 2u, &value));
  EXPECT_EQ((int32_t)0xAABBCCDD, value);
}

TEST_F(MemoryTest, ArmLoad32SLE_3) {
  ASSERT_TRUE(Store32SLE(memory_, 0u, (int32_t)0xAABBCCDD));
  int32_t value;
  ASSERT_TRUE(ArmLoad32SLE(memory_, 3u, &value));
  EXPECT_EQ((int32_t)0xAABBCCDD, value);
}

TEST_F(MemoryTest, ArmLoad16SLE_0) {
  ASSERT_TRUE(Store16SLE(memory_, 0u, (int16_t)0xAABB));
  int32_t value;
  ASSERT_TRUE(ArmLoad16SLEWithRotation(memory_, 0u, &value));
  EXPECT_EQ((int16_t)0xAABB, value);
}

TEST_F(MemoryTest, ArmLoad16SLE_1) {
  ASSERT_TRUE(Store16SLE(memory_, 0u, (int16_t)0xAABB));
  int32_t value;
  ASSERT_TRUE(ArmLoad16SLEWithRotation(memory_, 1u, &value));
  EXPECT_EQ((int8_t)0xAA, value);
}

TEST_F(MemoryTest, ArmStore32LE_0) {
  ASSERT_TRUE(ArmStore32LE(memory_, 0u, 0xAABBCCDDu));
  uint32_t value;
  ASSERT_TRUE(Load32LE(memory_, 0u, &value));
  EXPECT_EQ(0xAABBCCDDu, value);
}

TEST_F(MemoryTest, ArmStore32LE_1) {
  ASSERT_TRUE(ArmStore32LE(memory_, 1u, 0xAABBCCDDu));
  uint32_t value;
  ASSERT_TRUE(Load32LE(memory_, 0u, &value));
  EXPECT_EQ(0xAABBCCDDu, value);
}

TEST_F(MemoryTest, ArmStore32LE_2) {
  ASSERT_TRUE(ArmStore32LE(memory_, 2u, 0xAABBCCDDu));
  uint32_t value;
  ASSERT_TRUE(Load32LE(memory_, 0u, &value));
  EXPECT_EQ(0xAABBCCDDu, value);
}

TEST_F(MemoryTest, ArmStore32LE_3) {
  ASSERT_TRUE(ArmStore32LE(memory_, 3u, 0xAABBCCDDu));
  uint32_t value;
  ASSERT_TRUE(Load32LE(memory_, 0u, &value));
  EXPECT_EQ(0xAABBCCDDu, value);
}

TEST_F(MemoryTest, ArmStore16LE_0) {
  ASSERT_TRUE(ArmStore16LE(memory_, 0u, 0xAABBu));
  uint16_t value;
  ASSERT_TRUE(Load16LE(nullptr, 0u, &value));
  EXPECT_EQ(0xAABBu, value);
}

TEST_F(MemoryTest, ArmStore16LE_1) {
  ASSERT_TRUE(ArmStore16LE(memory_, 1u, 0xAABBu));
  uint16_t value;
  ASSERT_TRUE(Load16LE(nullptr, 0u, &value));
  EXPECT_EQ(0xAABBu, value);
}

TEST_F(MemoryTest, ArmStore32SLE_0) {
  ASSERT_TRUE(ArmStore32SLE(memory_, 0u, (int32_t)0xAABBCCDD));
  int32_t value;
  ASSERT_TRUE(Load32SLE(memory_, 0u, &value));
  EXPECT_EQ((int32_t)0xAABBCCDD, value);
}

TEST_F(MemoryTest, ArmStore32SLE_1) {
  ASSERT_TRUE(ArmStore32SLE(memory_, 1u, (int32_t)0xAABBCCDD));
  int32_t value;
  ASSERT_TRUE(Load32SLE(memory_, 0u, &value));
  EXPECT_EQ((int32_t)0xAABBCCDD, value);
}

TEST_F(MemoryTest, ArmStore32SLE_2) {
  ASSERT_TRUE(ArmStore32SLE(memory_, 2u, (int32_t)0xAABBCCDD));
  int32_t value;
  ASSERT_TRUE(Load32SLE(memory_, 0u, &value));
  EXPECT_EQ((int32_t)0xAABBCCDD, value);
}

TEST_F(MemoryTest, ArmStore32SLE_3) {
  ASSERT_TRUE(ArmStore32SLE(memory_, 3u, (int32_t)0xAABBCCDD));
  int32_t value;
  ASSERT_TRUE(Load32SLE(memory_, 0u, &value));
  EXPECT_EQ((int32_t)0xAABBCCDD, value);
}

TEST_F(MemoryTest, ArmStore16SLE_0) {
  ASSERT_TRUE(ArmStore16SLE(memory_, 0u, (int16_t)0xAABB));
  int16_t value;
  ASSERT_TRUE(Load16SLE(memory_, 0u, &value));
  EXPECT_EQ((int16_t)0xAABB, value);
}

TEST_F(MemoryTest, ArmStore16SLE_1) {
  ASSERT_TRUE(ArmStore16SLE(memory_, 1u, (int16_t)0xAABB));
  int16_t value;
  ASSERT_TRUE(Load16SLE(memory_, 0u, &value));
  EXPECT_EQ((int16_t)0xAABB, value);
}