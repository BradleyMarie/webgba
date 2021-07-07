extern "C" {
#include "emulator/cpu/arm7tdmi/decoders/thumb/execute.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

class ExecuteTest : public testing::TestWithParam<uint16_t> {
 public:
  void SetUp() override {
    for (char &c : memory_space_) {
      c = 0;
    }
    memory_ = MemoryAllocate(nullptr, Load32LE, Load16LE, Load8, Store32LE,
                             Store16LE, Store8, nullptr);
    ASSERT_NE(nullptr, memory_);

    memset(&registers_, 0, sizeof(ArmAllRegisters));
    registers_.current.user.cpsr.mode = MODE_SVC;
    registers_.current.spsr.mode = MODE_USR;
    registers_.current.user.gprs.pc = 0x108;
    registers_.current.user.gprs.sp = 0x200;
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

  // Assumes little-endian hex string
  bool RunInstruction(std::string instruction_hex) {
    uint16_t instruction = ToInstruction(instruction_hex);
    bool result = ThumbInstructionExecute(instruction, &registers_, memory_);
    return result;
  }

  static std::vector<char> memory_space_;
  ArmAllRegisters registers_;
  Memory *memory_;

 private:
  uint16_t ToInstruction(std::string instruction_hex) {
    if (instruction_hex[0u] == '0' && instruction_hex[1u] == 'x') {
      instruction_hex.erase(0u, 2u);
    }

    assert(instruction_hex.size() == 4u);
    uint16_t instruction;
    for (size_t i = 0u; i < instruction_hex.size(); i += 2u) {
      std::string hex_byte;
      hex_byte += instruction_hex[i];
      hex_byte += instruction_hex[i + 1u];
      unsigned long byte = std::stoul(hex_byte, nullptr, 16u);
      instruction += byte << (i * 4u);
    }

    return instruction;
  }
};

std::vector<char> ExecuteTest::memory_space_(1024u, 0);