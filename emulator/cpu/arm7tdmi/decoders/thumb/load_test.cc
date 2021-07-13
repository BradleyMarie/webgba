extern "C" {
#include "emulator/cpu/arm7tdmi/decoders/thumb/load.h"
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

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  return registers;
}

bool ArmAllRegistersAreZero(const ArmAllRegisters &regs) {
  auto zero = CreateArmAllRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmAllRegisters));
}

TEST_F(MemoryTest, LoadAligned) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.pc = 48u;
  ASSERT_TRUE(Store32LE(nullptr, 152u, 0xAABBCCDDu));
  EXPECT_TRUE(ThumbLDR_PC_IB(&registers, memory_, REGISTER_R0, 104u));
  EXPECT_EQ(0xAABBCCDDu, registers.current.user.gprs.r0);
  EXPECT_EQ(48u, registers.current.user.gprs.pc);

  registers.current.user.gprs.pc = 0u;
  registers.current.user.gprs.r0 = 0;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, LoadUnaligned) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.pc = 50u;
  ASSERT_TRUE(Store32LE(nullptr, 152u, 0xAABBCCDDu));
  EXPECT_TRUE(ThumbLDR_PC_IB(&registers, memory_, REGISTER_R0, 104u));
  EXPECT_EQ(0xAABBCCDDu, registers.current.user.gprs.r0);
  EXPECT_EQ(50u, registers.current.user.gprs.pc);

  registers.current.user.gprs.pc = 0u;
  registers.current.user.gprs.r0 = 0;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}