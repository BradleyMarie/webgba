extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/single_data_swap.h"
}

#include <strings.h>
#include <vector>

#include "googletest/include/gtest/gtest.h"

ArmGeneralPurposeRegisters CreateArmGeneralPurposeRegisters() {
  ArmGeneralPurposeRegisters registers;
  memset(&registers, 0, sizeof(ArmGeneralPurposeRegisters));
  return registers;
}

bool ArmGeneralPurposeRegistersAreZero(const ArmGeneralPurposeRegisters &regs) {
  auto zero = CreateArmGeneralPurposeRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmGeneralPurposeRegisters));
}

class MemoryTest : public testing::Test {
 public:
  void SetUp() override {
    for (char &c : memory_space_) {
      c = 0;
    }
    memory_ = MemoryAllocate(nullptr, Load32LE, Load16LE, Load8, Store32LE,
                             Store16LE, Store8, nullptr);
    ASSERT_NE(nullptr, memory_);
  }

  void TearDown() override { MemoryFree(memory_); }

 protected:
  static bool Load32LE(const void *context, uint32_t address, uint32_t *value) {
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

TEST_F(MemoryTest, ArmSWP) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  registers.r0 = 8u;
  registers.r1 = 0xCAFEBABE;
  ArmSWP(&registers, memory_, REGISTER_R2, REGISTER_R1, REGISTER_R0);
  EXPECT_EQ(8u, registers.r0);
  EXPECT_EQ(0xCAFEBABE, registers.r1);
  EXPECT_EQ(0xDEADC0DE, registers.r2);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xCAFEBABE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.r0 = 0u;
  registers.r1 = 0u;
  registers.r2 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSWPB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store8(nullptr, 8u, 16u));
  registers.r0 = 8u;
  registers.r1 = 32u;
  ArmSWP(&registers, memory_, REGISTER_R2, REGISTER_R1, REGISTER_R0);
  EXPECT_EQ(8u, registers.r0);
  EXPECT_EQ(32u, registers.r1);
  EXPECT_EQ(16u, registers.r2);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(32u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  registers.r0 = 0u;
  registers.r1 = 0u;
  registers.r2 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}