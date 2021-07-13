extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/single_data_swap.h"
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
    memory_no_loads_ =
        MemoryAllocate(nullptr, Load32LEFails, Load16LEFails, Load8Fails,
                       Store32LE, Store16LE, Store8, nullptr);
    ASSERT_NE(nullptr, memory_no_loads_);
    memory_no_stores_ =
        MemoryAllocate(nullptr, Load32LE, Load16LE, Load8, Store32LEFails,
                       Store16LEFails, Store8Fails, nullptr);
    ASSERT_NE(nullptr, memory_no_stores_);
  }

  void TearDown() override {
    MemoryFree(memory_);
    MemoryFree(memory_no_loads_);
    MemoryFree(memory_no_stores_);
  }

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

  static bool Load32LEFails(const void *context, uint32_t address,
                            uint32_t *value) {
    return false;
  }

  static bool Load16LEFails(const void *context, uint32_t address,
                            uint16_t *value) {
    return false;
  }

  static bool Load8Fails(const void *context, uint32_t address,
                         uint8_t *value) {
    return false;
  }

  static bool Store32LEFails(void *context, uint32_t address, uint32_t value) {
    return false;
  }

  static bool Store16LEFails(void *context, uint32_t address, uint16_t value) {
    return false;
  }

  static bool Store8Fails(void *context, uint32_t address, uint8_t value) {
    return false;
  }

  bool MemoryIsZero() {
    for (char c : memory_space_) {
      if (c != 0) {
        return false;
      }
    }
    return true;
  }

  bool ArmIsDataAbort(const ArmAllRegisters &regs) {
    return regs.current.user.cpsr.mode == MODE_ABT &&
           regs.current.user.gprs.pc == 0x10u;
  }

  static std::vector<char> memory_space_;
  Memory *memory_;
  Memory *memory_no_loads_;
  Memory *memory_no_stores_;
};

std::vector<char> MemoryTest::memory_space_(1024, 0);

TEST_F(MemoryTest, ArmSWP) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  registers.current.user.gprs.r0 = 8u;
  registers.current.user.gprs.r1 = 0xCAFEBABE;
  EXPECT_TRUE(
      ArmSWP(&registers, memory_, REGISTER_R2, REGISTER_R1, REGISTER_R0));
  EXPECT_EQ(8u, registers.current.user.gprs.r0);
  EXPECT_EQ(0xCAFEBABE, registers.current.user.gprs.r1);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r2);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xCAFEBABE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSWPLoadFails) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  registers.current.user.gprs.r0 = 8u;
  registers.current.user.gprs.r1 = 0xCAFEBABE;
  EXPECT_FALSE(ArmSWP(&registers, memory_no_loads_, REGISTER_R2, REGISTER_R1,
                      REGISTER_R0));
  EXPECT_EQ(8u, registers.current.user.gprs.r0);
  EXPECT_EQ(0xCAFEBABE, registers.current.user.gprs.r1);
  EXPECT_EQ(0u, registers.current.user.gprs.r2);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;

  EXPECT_TRUE(ArmIsDataAbort(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSWPStoreFails) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  registers.current.user.gprs.r0 = 8u;
  registers.current.user.gprs.r1 = 0xCAFEBABE;
  EXPECT_FALSE(ArmSWP(&registers, memory_no_stores_, REGISTER_R2, REGISTER_R1,
                      REGISTER_R0));
  EXPECT_EQ(8u, registers.current.user.gprs.r0);
  EXPECT_EQ(0xCAFEBABE, registers.current.user.gprs.r1);
  EXPECT_EQ(0u, registers.current.user.gprs.r2);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;

  EXPECT_TRUE(ArmIsDataAbort(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSWPB) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store8(nullptr, 8u, 16u));
  registers.current.user.gprs.r0 = 8u;
  registers.current.user.gprs.r1 = 32u;
  EXPECT_TRUE(
      ArmSWP(&registers, memory_, REGISTER_R2, REGISTER_R1, REGISTER_R0));
  EXPECT_EQ(8u, registers.current.user.gprs.r0);
  EXPECT_EQ(32u, registers.current.user.gprs.r1);
  EXPECT_EQ(16u, registers.current.user.gprs.r2);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(32u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSWPBLoadFails) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  ASSERT_TRUE(Store8(nullptr, 8u, 16u));
  registers.current.user.gprs.r0 = 8u;
  registers.current.user.gprs.r1 = 32u;
  EXPECT_FALSE(ArmSWP(&registers, memory_no_loads_, REGISTER_R2, REGISTER_R1,
                      REGISTER_R0));
  EXPECT_EQ(8u, registers.current.user.gprs.r0);
  EXPECT_EQ(32u, registers.current.user.gprs.r1);
  EXPECT_EQ(0u, registers.current.user.gprs.r2);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(16u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;

  EXPECT_TRUE(ArmIsDataAbort(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSWPBStoreFails) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  ASSERT_TRUE(Store8(nullptr, 8u, 16u));
  registers.current.user.gprs.r0 = 8u;
  registers.current.user.gprs.r1 = 32u;
  EXPECT_FALSE(ArmSWP(&registers, memory_no_stores_, REGISTER_R2, REGISTER_R1,
                      REGISTER_R0));
  EXPECT_EQ(8u, registers.current.user.gprs.r0);
  EXPECT_EQ(32u, registers.current.user.gprs.r1);
  EXPECT_EQ(0u, registers.current.user.gprs.r2);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(16u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;

  EXPECT_TRUE(ArmIsDataAbort(registers));
  EXPECT_TRUE(MemoryIsZero());
}