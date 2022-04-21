extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/signed_data_transfer.h"
}

#include <cstring>
#include <vector>

#include "googletest/include/gtest/gtest.h"

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  return registers;
}

bool ArmAllRegistersAreZero(const ArmAllRegisters &regs) {
  auto zero = CreateArmAllRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmAllRegisters));
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

TEST_F(MemoryTest, ArmLDRH_IB) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 137u));
  registers.current.user.gprs.r0 = 8u;
  ArmLDRH_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRH_IB_Unaligned) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0xAABBu));
  registers.current.user.gprs.r0 = 8u;
  ArmLDRH_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 9u);
  EXPECT_EQ(0xBB0000AAu, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xAABBu, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRH_DB) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 137u));
  registers.current.user.gprs.r0 = 8u;
  registers.current.user.gprs.r1 = 16u;
  ArmLDRH_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRH_DB_Unaligned) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0xAABBu));
  registers.current.user.gprs.r0 = 8u;
  registers.current.user.gprs.r1 = 16u;
  ArmLDRH_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 7u);
  EXPECT_EQ(0xBB0000AAu, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xAABBu, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRH_DBW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 137u));
  registers.current.user.gprs.r1 = 10u;
  ArmLDRH_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRH_DBW_Unaligned) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0xAABBu));
  registers.current.user.gprs.r1 = 10u;
  ArmLDRH_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 1u);
  EXPECT_EQ(0xBB0000AAu, registers.current.user.gprs.r0);
  EXPECT_EQ(9u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xAABBu, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRH_DAW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 137u));
  registers.current.user.gprs.r1 = 8u;
  ArmLDRH_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(6u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRH_DAW_Unaligned) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0xAABBu));
  registers.current.user.gprs.r1 = 9u;
  ArmLDRH_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(0xBB0000AAu, registers.current.user.gprs.r0);
  EXPECT_EQ(7u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xAABBu, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRH_IBW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 137u));
  registers.current.user.gprs.r1 = 6u;
  ArmLDRH_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRH_IBW_Unaligned) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0xAABBu));
  registers.current.user.gprs.r1 = 6u;
  ArmLDRH_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 3u);
  EXPECT_EQ(0xBB0000AAu, registers.current.user.gprs.r0);
  EXPECT_EQ(9u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xAABBu, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRH_IAW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 137u));
  registers.current.user.gprs.r1 = 8u;
  ArmLDRH_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(10u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRH_IAW_Unaligned) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0xAABBu));
  registers.current.user.gprs.r1 = 9u;
  ArmLDRH_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(0xBB0000AAu, registers.current.user.gprs.r0);
  EXPECT_EQ(11u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xAABBu, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSB_IB) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store8S(memory_, 8u, -65));
  registers.current.user.gprs.r0_s = 8u;
  ArmLDRSB_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSB_DB) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store8S(memory_, 8u, -65));
  registers.current.user.gprs.r0_s = 8u;
  registers.current.user.gprs.r1 = 16u;
  ArmLDRSB_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSB_DBW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store8S(memory_, 8u, -65));
  registers.current.user.gprs.r1 = 10u;
  ArmLDRSB_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSB_DAW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store8S(memory_, 8u, -65));
  registers.current.user.gprs.r1 = 8u;
  ArmLDRSB_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(6u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSB_IBW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store8S(memory_, 8u, -65));
  registers.current.user.gprs.r1 = 6u;
  ArmLDRSB_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSB_IAW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store8S(memory_, 8u, -65));
  registers.current.user.gprs.r1 = 8u;
  ArmLDRSB_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(10u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_IB) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.current.user.gprs.r0_s = 8u;
  ArmLDRSH_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_IB_Unaligned) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.current.user.gprs.r0_s = 8u;
  ArmLDRSH_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 9u);
  EXPECT_EQ(-1, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_DB) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.current.user.gprs.r0_s = 8u;
  registers.current.user.gprs.r1 = 16u;
  ArmLDRSH_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_DB_Unaligned) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.current.user.gprs.r0_s = 8u;
  registers.current.user.gprs.r1 = 16u;
  ArmLDRSH_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 7u);
  EXPECT_EQ(-1, registers.current.user.gprs.r0_s);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_DBW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.current.user.gprs.r1 = 10u;
  ArmLDRSH_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_DBW_Unaligned) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.current.user.gprs.r1 = 10u;
  ArmLDRSH_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 1u);
  EXPECT_EQ(-1, registers.current.user.gprs.r0_s);
  EXPECT_EQ(9u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_DAW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.current.user.gprs.r1 = 8u;
  ArmLDRSH_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(6u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_DAW_Unaligned) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.current.user.gprs.r1 = 9u;
  ArmLDRSH_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-1, registers.current.user.gprs.r0_s);
  EXPECT_EQ(7u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_IBW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.current.user.gprs.r1 = 6u;
  ArmLDRSH_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_IBW_Unaligned) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.current.user.gprs.r1 = 6u;
  ArmLDRSH_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 3u);
  EXPECT_EQ(-1, registers.current.user.gprs.r0_s);
  EXPECT_EQ(9u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_IAW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.current.user.gprs.r1 = 8u;
  ArmLDRSH_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(10u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_IAW_Unaligned) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.current.user.gprs.r1 = 9u;
  ArmLDRSH_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-1, registers.current.user.gprs.r0_s);
  EXPECT_EQ(11u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmSTRH_IB) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 137u;
  ArmSTRH_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRH_IB_PC) {
  auto registers = CreateArmAllRegisters();

  ArmSTRH_IB(&registers, memory_, REGISTER_PC, REGISTER_R1, 8u);
  EXPECT_EQ(0u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(4, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRH_DB) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 16u;
  ArmSTRH_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRH_DB_PC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r1 = 16u;
  ArmSTRH_DB(&registers, memory_, REGISTER_PC, REGISTER_R1, 8u);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(4u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRH_DBW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 12u;
  ArmSTRH_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRH_DBW_PC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r1 = 12u;
  ArmSTRH_DBW(&registers, memory_, REGISTER_PC, REGISTER_R1, 4u);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(4u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRH_DAW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRH_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRH_DAW_PC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r1 = 8u;
  ArmSTRH_DAW(&registers, memory_, REGISTER_PC, REGISTER_R1, 4u);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(4u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRH_IBW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 4u;
  ArmSTRH_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRH_IBW_PC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r1 = 4u;
  ArmSTRH_IBW(&registers, memory_, REGISTER_PC, REGISTER_R1, 4u);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(4u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRH_IAW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRH_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRH_IAW_PC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r1 = 8u;
  ArmSTRH_IAW(&registers, memory_, REGISTER_PC, REGISTER_R1, 4u);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(4u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_IB) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -65;
  ArmSTRSB_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_IB_PC) {
  auto registers = CreateArmAllRegisters();

  ArmSTRSB_IB(&registers, memory_, REGISTER_PC, REGISTER_R1, 8u);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(4, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_DB) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 16u;
  ArmSTRSB_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_DB_PC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r1 = 16u;
  ArmSTRSB_DB(&registers, memory_, REGISTER_PC, REGISTER_R1, 8u);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(4, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_DBW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 12u;
  ArmSTRSB_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_DBW_PC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r1 = 12u;
  ArmSTRSB_DBW(&registers, memory_, REGISTER_PC, REGISTER_R1, 4u);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(4, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_DAW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRSB_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_DAW_PC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r1 = 8u;
  ArmSTRSB_DAW(&registers, memory_, REGISTER_PC, REGISTER_R1, 4u);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(4, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_IBW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 4u;
  ArmSTRSB_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_IBW_PC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r1 = 4u;
  ArmSTRSB_IBW(&registers, memory_, REGISTER_PC, REGISTER_R1, 4u);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(4, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_IAW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRSB_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_IAW_PC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r1 = 8u;
  ArmSTRSB_IAW(&registers, memory_, REGISTER_PC, REGISTER_R1, 4u);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(4, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_IB) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -65;
  ArmSTRSH_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_IB_PC) {
  auto registers = CreateArmAllRegisters();

  ArmSTRSH_IB(&registers, memory_, REGISTER_PC, REGISTER_R1, 8u);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(4, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_DB) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 16u;
  ArmSTRSH_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_DB_PC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r1 = 16u;
  ArmSTRSH_DB(&registers, memory_, REGISTER_PC, REGISTER_R1, 8u);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(4, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_DBW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 12u;
  ArmSTRSH_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_DBW_PC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r1 = 12u;
  ArmSTRSH_DBW(&registers, memory_, REGISTER_PC, REGISTER_R1, 4u);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(4, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_DAW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRSH_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_DAW_PC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r1 = 8u;
  ArmSTRSH_DAW(&registers, memory_, REGISTER_PC, REGISTER_R1, 4u);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(4, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_IBW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 4u;
  ArmSTRSH_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_IBW_PC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r1 = 4u;
  ArmSTRSH_IBW(&registers, memory_, REGISTER_PC, REGISTER_R1, 4u);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(4, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_IAW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRSH_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  registers.current.user.gprs.r0_s = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_IAW_PC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r1 = 8u;
  ArmSTRSH_IAW(&registers, memory_, REGISTER_PC, REGISTER_R1, 4u);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(4, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

class MemoryFailsTest : public testing::Test {
 public:
  void SetUp() override {
    memory_ = MemoryAllocate(nullptr, Load32LE, Load16LE, Load8, Store32LE,
                             Store16LE, Store8, nullptr);
    ASSERT_NE(nullptr, memory_);
  }

  void TearDown() override { MemoryFree(memory_); }

 protected:
  static bool Load32LE(const void *context, uint32_t address, uint32_t *value) {
    return false;
  }

  static bool Load16LE(const void *context, uint32_t address, uint16_t *value) {
    return false;
  }

  static bool Load8(const void *context, uint32_t address, uint8_t *value) {
    return false;
  }

  static bool Store32LE(void *context, uint32_t address, uint32_t value) {
    return false;
  }

  static bool Store16LE(void *context, uint32_t address, uint16_t value) {
    return false;
  }

  static bool Store8(void *context, uint32_t address, uint8_t value) {
    return false;
  }

  ArmAllRegisters CreateArmAllRegistersInMode() {
    ArmAllRegisters registers;
    memset(&registers, 0, sizeof(ArmAllRegisters));
    registers.current.user.cpsr.mode = MODE_USR;
    return registers;
  }

  bool ArmIsDataAbort(const ArmAllRegisters &regs) {
    return regs.current.user.cpsr.mode == MODE_ABT;
  }

  Memory *memory_;
};

TEST_F(MemoryFailsTest, ArmLDRH_IB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 8u;
  ArmLDRH_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(8u, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRH_DB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 8u;
  registers.current.user.gprs.r1 = 16u;
  ArmLDRH_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(8u, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRH_DBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 10u;
  ArmLDRH_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRH_DAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 8u;
  ArmLDRH_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(6u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRH_IBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 6u;
  ArmLDRH_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRH_IAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 8u;
  ArmLDRH_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(10u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRSB_IB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0_s = 8;
  ArmLDRSB_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(8, registers.current.user.gprs.r0_s);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRSB_DB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0_s = 8;
  registers.current.user.gprs.r1 = 16u;
  ArmLDRSB_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(8, registers.current.user.gprs.r0_s);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRSB_DBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 10u;
  ArmLDRSB_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRSB_DAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 8u;
  ArmLDRSB_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(6u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRSB_IBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 6u;
  ArmLDRSB_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRSB_IAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 8u;
  ArmLDRSB_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(10u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRSH_IB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0_s = 8;
  ArmLDRSH_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(8, registers.current.user.gprs.r0_s);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRSH_DB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0_s = 8;
  registers.current.user.gprs.r1 = 16u;
  ArmLDRSH_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(8, registers.current.user.gprs.r0_s);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRSH_DBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 10u;
  ArmLDRSH_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRSH_DAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 8u;
  ArmLDRSH_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(6u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRSH_IBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 6u;
  ArmLDRSH_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRSH_IAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 8u;
  ArmLDRSH_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(10u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRH_IB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 137u;
  ArmSTRH_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRH_DB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 16u;
  ArmSTRH_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRH_DBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 12u;
  ArmSTRH_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRH_DAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRH_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRH_IBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 4u;
  ArmSTRH_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRH_IAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRH_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRSB_IB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0_s = -65;
  ArmSTRSB_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRSB_DB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 16u;
  ArmSTRSB_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRSB_DBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 12u;
  ArmSTRSB_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRSB_DAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRSB_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRSB_IBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 4u;
  ArmSTRSB_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRSB_IAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRSB_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRSH_IB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0_s = -65;
  ArmSTRSH_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRSH_DB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 16u;
  ArmSTRSH_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRSH_DBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 12u;
  ArmSTRSH_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRSH_DAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRSH_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRSH_IBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 4u;
  ArmSTRSH_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRSH_IAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0_s = -65;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRSH_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.current.user.gprs.r0_s);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}