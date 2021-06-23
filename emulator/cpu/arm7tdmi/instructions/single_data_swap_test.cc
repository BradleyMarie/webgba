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

ArmUserRegisters CreateArmUserRegisters() {
  ArmUserRegisters registers;
  memset(&registers, 0, sizeof(ArmUserRegisters));
  return registers;
}

bool ArmUserRegistersAreZero(const ArmUserRegisters &regs) {
  auto zero = CreateArmUserRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmUserRegisters));
}

ArmPrivilegedRegisters CreateArmPrivilegedRegisters() {
  ArmPrivilegedRegisters registers;
  memset(&registers, 0, sizeof(ArmPrivilegedRegisters));
  return registers;
}

bool ArmPrivilegedRegistersAreZero(const ArmPrivilegedRegisters &regs) {
  auto zero = CreateArmPrivilegedRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmPrivilegedRegisters));
}

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  return registers;
}

bool ArmAllRegistersAreZero(const ArmAllRegisters &regs) {
  auto zero = CreateArmAllRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmAllRegisters));
}

TEST(ArmMRS, Move) {
  auto registers = CreateArmUserRegisters();

  registers.cpsr.thumb = true;
  ArmMRS(&registers, REGISTER_R0);
  EXPECT_EQ(registers.gprs.r0, registers.cpsr.value);
  EXPECT_TRUE(registers.cpsr.thumb);

  registers.gprs.r0 = 0;
  registers.cpsr.thumb = false;

  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmMRSR, Move) {
  auto registers = CreateArmPrivilegedRegisters();

  registers.user.cpsr.mode = MODE_SVC;
  registers.spsr.mode = MODE_USR;
  ArmMRSR(&registers, REGISTER_R0);
  EXPECT_EQ(registers.user.gprs.r0, registers.spsr.value);
  EXPECT_EQ(MODE_SVC, registers.user.cpsr.mode);
  EXPECT_EQ(MODE_USR, registers.spsr.mode);

  registers.user.gprs.r0 = 0;
  registers.user.cpsr.mode = 0u;
  registers.spsr.mode = 0u;

  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmMSR_Immediate, FlagsOnly) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  ArmProgramStatusRegister next_status;
  memset(&next_status, 0, sizeof(ArmProgramStatusRegister));
  next_status.thumb = true;
  next_status.fiq_disable = true;
  next_status.irq_disable = true;
  next_status.overflow = true;
  next_status.carry = true;
  next_status.zero = true;
  next_status.negative = true;

  ArmMSR_Immediate(&registers, false, true, next_status.value);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.zero);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  registers.current.user.cpsr.zero = false;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.mode = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMSR_Immediate, FromUsr) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  ArmProgramStatusRegister next_status;
  memset(&next_status, 0, sizeof(ArmProgramStatusRegister));
  next_status.mode = MODE_SVC;
  next_status.thumb = true;
  next_status.fiq_disable = true;
  next_status.irq_disable = true;
  next_status.overflow = true;
  next_status.carry = true;
  next_status.zero = true;
  next_status.negative = true;

  ArmMSR_Immediate(&registers, true, true, next_status.value);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);
  EXPECT_TRUE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.zero);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.thumb = false;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  registers.current.user.cpsr.zero = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMSRR_Immediate, FlagsOnly) {
  auto registers = CreateArmPrivilegedRegisters();

  ArmProgramStatusRegister next_status;
  memset(&next_status, 0, sizeof(ArmProgramStatusRegister));
  next_status.mode = MODE_USR;
  next_status.thumb = true;
  next_status.fiq_disable = true;
  next_status.irq_disable = true;
  next_status.overflow = true;
  next_status.carry = true;
  next_status.zero = true;
  next_status.negative = true;

  ArmMSRR_Immediate(&registers, false, true, next_status.value);
  EXPECT_TRUE(registers.spsr.overflow);
  EXPECT_TRUE(registers.spsr.carry);
  EXPECT_TRUE(registers.spsr.zero);
  EXPECT_TRUE(registers.spsr.negative);

  registers.spsr.overflow = false;
  registers.spsr.carry = false;
  registers.spsr.zero = false;
  registers.spsr.negative = false;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmMSRR_Immediate, ControlOnly) {
  auto registers = CreateArmPrivilegedRegisters();

  ArmProgramStatusRegister next_status;
  memset(&next_status, 0, sizeof(ArmProgramStatusRegister));
  next_status.mode = MODE_USR;
  next_status.thumb = true;
  next_status.fiq_disable = true;
  next_status.irq_disable = true;
  next_status.overflow = true;
  next_status.carry = true;
  next_status.zero = true;
  next_status.negative = true;

  ArmMSRR_Immediate(&registers, true, false, next_status.value);
  EXPECT_EQ(MODE_USR, registers.spsr.mode);
  EXPECT_TRUE(registers.spsr.thumb);
  EXPECT_TRUE(registers.spsr.fiq_disable);
  EXPECT_TRUE(registers.spsr.irq_disable);

  registers.spsr.mode = 0u;
  registers.spsr.thumb = false;
  registers.spsr.fiq_disable = false;
  registers.spsr.irq_disable = false;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmMSRR_Immediate, Both) {
  auto registers = CreateArmPrivilegedRegisters();

  ArmProgramStatusRegister next_status;
  memset(&next_status, 0, sizeof(ArmProgramStatusRegister));
  next_status.mode = MODE_USR;
  next_status.thumb = true;
  next_status.fiq_disable = true;
  next_status.irq_disable = true;
  next_status.overflow = true;
  next_status.carry = true;
  next_status.zero = true;
  next_status.negative = true;

  ArmMSRR_Immediate(&registers, true, true, next_status.value);
  EXPECT_EQ(MODE_USR, registers.spsr.mode);
  EXPECT_TRUE(registers.spsr.thumb);
  EXPECT_TRUE(registers.spsr.fiq_disable);
  EXPECT_TRUE(registers.spsr.irq_disable);
  EXPECT_TRUE(registers.spsr.overflow);
  EXPECT_TRUE(registers.spsr.carry);
  EXPECT_TRUE(registers.spsr.zero);
  EXPECT_TRUE(registers.spsr.negative);

  registers.spsr.mode = 0u;
  registers.spsr.thumb = false;
  registers.spsr.fiq_disable = false;
  registers.spsr.irq_disable = false;
  registers.spsr.overflow = false;
  registers.spsr.carry = false;
  registers.spsr.zero = false;
  registers.spsr.negative = false;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmMSRR_Register, Both) {
  auto registers = CreateArmPrivilegedRegisters();

  ArmProgramStatusRegister next_status;
  memset(&next_status, 0, sizeof(ArmProgramStatusRegister));
  next_status.mode = MODE_USR;
  next_status.thumb = true;
  next_status.fiq_disable = true;
  next_status.irq_disable = true;
  next_status.overflow = true;
  next_status.carry = true;
  next_status.zero = true;
  next_status.negative = true;

  registers.user.gprs.r0 = next_status.value;
  ArmMSRR_Register(&registers, REGISTER_R0);
  EXPECT_EQ(next_status.value, registers.user.gprs.r0);
  EXPECT_EQ(MODE_USR, registers.spsr.mode);
  EXPECT_TRUE(registers.spsr.thumb);
  EXPECT_TRUE(registers.spsr.fiq_disable);
  EXPECT_TRUE(registers.spsr.irq_disable);
  EXPECT_TRUE(registers.spsr.overflow);
  EXPECT_TRUE(registers.spsr.carry);
  EXPECT_TRUE(registers.spsr.zero);
  EXPECT_TRUE(registers.spsr.negative);

  registers.user.gprs.r0 = 0u;
  registers.spsr.mode = 0u;
  registers.spsr.thumb = false;
  registers.spsr.fiq_disable = false;
  registers.spsr.irq_disable = false;
  registers.spsr.overflow = false;
  registers.spsr.carry = false;
  registers.spsr.zero = false;
  registers.spsr.negative = false;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
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

  ASSERT_TRUE(Store32LE(nullptr, 8u, 16u));
  registers.r0 = 8u;
  registers.r1 = 32u;
  ArmSWP(&registers, memory_, REGISTER_R2, REGISTER_R1, REGISTER_R0);
  EXPECT_EQ(8u, registers.r0);
  EXPECT_EQ(32u, registers.r1);
  EXPECT_EQ(16u, registers.r2);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(32u, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.r0 = 0u;
  registers.r1 = 0u;
  registers.r2 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}