#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace {

std::string MatchesMultiply(const std::bitset<32>& instruction) {
  if (instruction[4] != 1 || instruction[5] != 0 || instruction[6] != 0 ||
      instruction[7] != 1 || instruction[22] != 0 || instruction[23] != 0 ||
      instruction[24] != 0 || instruction[25] != 0 || instruction[26] != 0 ||
      instruction[27] != 0) {
    return std::string();
  }

  bool s = instruction[20];
  bool a = instruction[21];

  std::string opcode = a ? "MLA" : "MUL";
  if (s) {
    opcode += "S";
  }

  return opcode;
}

std::string MatchesMultiplyLong(const std::bitset<32>& instruction) {
  if (instruction[4] != 1 || instruction[5] != 0 || instruction[6] != 0 ||
      instruction[7] != 1 || instruction[23] != 1 || instruction[24] != 0 ||
      instruction[25] != 0 || instruction[26] != 0 || instruction[27] != 0) {
    return std::string();
  }

  bool s = instruction[20];
  bool a = instruction[21];
  bool u = instruction[22];

  std::string opcode;
  if (u) {
    if (a) {
      opcode = "UMLAL";
    } else {
      opcode = "UMULL";
    }
  } else {
    if (a) {
      opcode = "SMLAL";
    } else {
      opcode = "SMULL";
    }
  }

  if (s) {
    opcode += "S";
  }

  return opcode;
}

std::string MatchesBranchExchange(const std::bitset<32>& instruction) {
  // Ignores bits 8-19
  if (instruction[4] != 1 || instruction[5] != 0 || instruction[6] != 0 ||
      instruction[7] != 0 || instruction[20] != 0 || instruction[21] != 1 ||
      instruction[22] != 0 || instruction[23] != 0 || instruction[24] != 1 ||
      instruction[25] != 0 || instruction[26] != 0 || instruction[27] != 0) {
    return std::string();
  }

  return "BX";
}

std::string MatchesSingleDataSwap(const std::bitset<32>& instruction) {
  // Ignores bits 8-11
  if (instruction[4] != 1 || instruction[5] != 0 || instruction[6] != 0 ||
      instruction[7] != 1 || instruction[20] != 0 || instruction[21] != 0 ||
      instruction[23] != 0 || instruction[24] != 1 || instruction[25] != 0 ||
      instruction[26] != 0 || instruction[27] != 0) {
    return std::string();
  }

  bool b = instruction[22];

  return b ? "SWPB" : "SWP";
}

std::string MatchesHalfWordDataTransfer(const std::bitset<32>& instruction) {
  // Ignores bits 8-11
  if (instruction[4] != 1 || instruction[7] != 1 || instruction[25] != 0 ||
      instruction[26] != 0 || instruction[27] != 0) {
    return std::string();
  }

  bool h = instruction[5];
  bool s = instruction[6];
  bool l = instruction[20];
  bool w = instruction[21];
  bool i = instruction[22];
  bool u = instruction[23];
  bool p = instruction[24];

  std::string opcode;
  if (l) {
    if (s) {
      if (h) {
        opcode = "LDRSH";
      } else {
        opcode = "LDRSB";
      }
    } else {
      if (h) {
        opcode = "LDRH";
      } else {
        return std::string();
      }
    }
  } else {
    if (s) {
      return std::string();
    } else {
      if (h) {
        opcode = "STRH";
      } else {
        return std::string();
      }
    }
  }

  opcode += "_";

  opcode += u ? "I" : "D";
  opcode += p ? "B" : "A";
  if (w) {
    opcode += "W";
  }

  if (i) {
    opcode += "_I8";
  }

  return opcode;
}

std::string MatchesSingleDataTransfer(const std::bitset<32>& instruction) {
  if (instruction[26] != 1 || instruction[27] != 0) {
    return std::string();
  }

  bool l = instruction[20];
  bool w = instruction[21];
  bool b = instruction[22];
  bool u = instruction[23];
  bool p = instruction[24];
  bool i = instruction[25];

  std::string opcode = l ? "LDR" : "STR";
  if (b) {
    opcode += "B";
  }

  opcode += "_";

  opcode += u ? "I" : "D";
  opcode += p ? "B" : "A";
  if (w) {
    opcode += "W";
  }

  if (i) {
    opcode += "_I12";
  }

  return opcode;
}

std::string MatchesDataProcessing(const std::bitset<32>& instruction) {
  if (instruction[26] != 0 || instruction[27] != 0) {
    return std::string();
  }

  if (instruction[4] == 1 && instruction[7] == 1 && instruction[25] == 0) {
    return std::string();
  }

  bool s = instruction[20];

  std::bitset<4> arm_opcode;
  arm_opcode[0] = instruction[21];
  arm_opcode[1] = instruction[22];
  arm_opcode[2] = instruction[23];
  arm_opcode[3] = instruction[24];

  std::string opcode;
  switch (arm_opcode.to_ulong()) {
    case 0:
      opcode = "AND";
      break;
    case 1:
      opcode = "EOR";
      break;
    case 2:
      opcode = "SUB";
      break;
    case 3:
      opcode = "RSB";
      break;
    case 4:
      opcode = "ADD";
      break;
    case 5:
      opcode = "ADC";
      break;
    case 6:
      opcode = "SBC";
      break;
    case 7:
      opcode = "RSC";
      break;
    case 8:
      if (!s) {
        return std::string();
      }
      opcode = "TST";
      s = false;
      break;
    case 9:
      if (!s) {
        return std::string();
      }
      opcode = "TEQ";
      s = false;
      break;
    case 10:
      if (!s) {
        return std::string();
      }
      opcode = "CMP";
      s = false;
      break;
    case 11:
      if (!s) {
        return std::string();
      }
      opcode = "CMN";
      s = false;
      break;
    case 12:
      opcode = "ORR";
      break;
    case 13:
      opcode = "MOV";
      break;
    case 14:
      opcode = "BIC";
      break;
    case 15:
      opcode = "MVN";
      break;
    default:
      assert(false);
  }

  if (s) {
    opcode += "S";
  }

  bool i = instruction[25];

  if (i) {
    opcode += "_I32";
  }

  return opcode;
}

std::string MatchesBlockDataTransfer(const std::bitset<32>& instruction) {
  if (instruction[22] != 0 || instruction[25] != 0 || instruction[26] != 0 ||
      instruction[27] != 1) {
    return std::string();
  }

  bool l = instruction[20];
  bool w = instruction[21];
  bool u = instruction[23];
  bool p = instruction[24];

  std::string opcode = l ? "LDM" : "STM";
  opcode += u ? "I" : "D";
  opcode += p ? "B" : "A";
  if (w) {
    opcode += "_W";
  }

  return opcode;
}

std::string MatchesBranch(const std::bitset<32>& instruction) {
  if (instruction[25] != 1 || instruction[26] != 0 || instruction[27] != 1) {
    return std::string();
  }

  bool l = instruction[24];

  std::string opcode = l ? "BL" : "B";

  return opcode;
}

std::string MatchesCoprocessorDataTransfer(const std::bitset<32>& instruction) {
  if (instruction[25] != 0 || instruction[26] != 1 || instruction[27] != 1) {
    return std::string();
  }

  bool l = instruction[20];

  std::string opcode = l ? "LDC" : "STC";

  return opcode;
}

std::string MatchesCoprocessorDataOperation(
    const std::bitset<32>& instruction) {
  if (instruction[4] != 0 || instruction[24] != 0 || instruction[25] != 1 ||
      instruction[26] != 1 || instruction[27] != 1) {
    return std::string();
  }

  return "CDP";
}

std::string MatchesCoprocessorDataRegisterTransfer(
    const std::bitset<32>& instruction) {
  if (instruction[4] != 1 || instruction[24] != 0 || instruction[25] != 1 ||
      instruction[26] != 1 || instruction[27] != 1) {
    return std::string();
  }

  bool l = instruction[20];

  std::string opcode = l ? "MCR" : "MRC";

  return opcode;
}

std::string MatchesSoftwareInterrupt(const std::bitset<32>& instruction) {
  if (instruction[24] != 1 || instruction[25] != 1 || instruction[26] != 1 ||
      instruction[27] != 1) {
    return std::string();
  }

  return "SWI";
}

const std::vector<std::function<std::string(const std::bitset<32>&)>>
    g_matchers = {MatchesMultiply,
                  MatchesMultiplyLong,
                  MatchesBranchExchange,
                  MatchesSingleDataSwap,
                  MatchesHalfWordDataTransfer,
                  MatchesSingleDataTransfer,
                  MatchesDataProcessing,
                  MatchesBlockDataTransfer,
                  MatchesBranch,
                  MatchesCoprocessorDataTransfer,
                  MatchesCoprocessorDataOperation,
                  MatchesCoprocessorDataRegisterTransfer,
                  MatchesSoftwareInterrupt};

std::string MatchInstruction(const std::bitset<32>& instruction) {
  std::string match;
  for (const auto& entry : g_matchers) {
    std::string current_match = entry(instruction);
    if (current_match.empty()) {
      continue;
    }

    if (!match.empty()) {
      std::cout << "ERROR: Instruction " << instruction << " (0x" << std::hex
                << std::setw(8) << std::setfill('0') << instruction.to_ulong()
                << ") matched " << match << " and " << current_match << "."
                << std::endl;
      return std::string();
    }

    match = std::move(current_match);
  }

  if (match.empty()) {
    return "ARM_OPCODE_UNDEF";
  }

  return "ARM_OPCODE_" + match;
}

}  // namespace

int main(int argc, char* argv[]) {
  std::vector<std::string> opcodes;
  for (uint32_t index = 0; index < 4096; index++) {
    std::bitset<13> bits(index);
    std::bitset<32> instruction;
    instruction[4] = bits[0];
    instruction[5] = bits[1];
    instruction[6] = bits[2];
    instruction[7] = bits[3];
    instruction[20] = bits[4];
    instruction[21] = bits[5];
    instruction[22] = bits[6];
    instruction[23] = bits[7];
    instruction[24] = bits[8];
    instruction[25] = bits[9];
    instruction[26] = bits[10];
    instruction[27] = bits[11];
    std::string opcode = MatchInstruction(instruction);
    if (opcode.empty()) {
      return -1;
    }
    std::replace(opcode.begin(), opcode.end(), '_', '=');
    opcodes.push_back(opcode);
  }

  std::set<std::string> sorted_opcodes;
  sorted_opcodes.insert(opcodes.begin(), opcodes.end());

  if (UINT8_MAX < sorted_opcodes.size()) {
    std::cout << "ERROR: Cannot represent opcodes in a uint8_t" << std::endl;
    return -1;
  }

  std::map<std::string, uint32_t> opcode_number;
  sorted_opcodes.erase("ARM_OPCODE_UNDEF");
  opcode_number["ARM_OPCODE_UNDEF"] = 0;

  std::cout << "#include <assert.h>" << std::endl;
  std::cout << "#include <stdint.h>" << std::endl << std::endl;

  std::cout << "typedef enum {" << std::endl;
  std::cout << "  ARM_OPCODE_UNDEF = 0u," << std::endl;

  uint32_t value = 1;
  for (const auto& entry : sorted_opcodes) {
    opcode_number[entry] = value;
    std::string opcode = entry;
    std::replace(opcode.begin(), opcode.end(), '=', '_');
    std::cout << "  " << opcode << " = " << value++ << "u," << std::endl;
  }

  std::cout << "} ArmOpcode;" << std::endl << std::endl;

  std::cout
      << "static inline ArmOpcode ArmDecodeOperation(uint32_t instruction) {"
      << std::endl;
  std::cout << "  static const uint8_t opcode_table[4096] = {" << std::endl;
  for (const auto& entry : opcodes) {
    std::cout << "    " << opcode_number.at(entry) << "u," << std::endl;
  }
  std::cout << "  };" << std::endl << std::endl;

  std::cout << "  uint_fast16_t opcode_index_low = (instruction << 24u) >> 28u;"
            << std::endl;
  std::cout << "  uint_fast16_t opcode_index_hi = (instruction << 4u) >> 24u;"
            << std::endl;
  std::cout
      << "  uint_fast16_t opcode_index = opcode_index_low | opcode_index_hi;"
      << std::endl;

  std::cout << "  assert(opcode_index < 4096);" << std::endl << std::endl;

  std::cout << "  return (ArmOpcode)opcode_table[opcode_index];" << std::endl;
  std::cout << "}" << std::endl;

  return 0;
}