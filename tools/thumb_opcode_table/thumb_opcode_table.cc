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

std::string MatchesBranchExchange(const std::bitset<16>& instruction) {
  if (instruction[15] != 0u || instruction[14] != 1u || instruction[13] != 0u ||
      instruction[12] != 0u || instruction[11] != 0u || instruction[10] != 1u ||
      instruction[9] != 1u || instruction[8] != 1u || instruction[7] != 0u) {
    return std::string();
  }

  return "BX";
}

std::string MatchesConditionalBranch(const std::bitset<16>& instruction) {
  if (instruction[15] != 1u || instruction[14] != 1u || instruction[13] != 0u ||
      instruction[12] != 1u) {
    return std::string();
  }


  std::bitset<2> cond;
  cond[0] = instruction[8];
  cond[1] = instruction[9];
  cond[2] = instruction[10];
  cond[3] = instruction[11];
  switch (cond.to_ulong()) {
    case 14u: // Always
      return std::string();
    case 15u: // Never
      return std::string();
  }

  return "B_COND";
}

std::string MatchesUnconditionalBranch(const std::bitset<16>& instruction) {
  if (instruction[15] != 1u || instruction[14] != 1u || instruction[13] != 1u) {
    return std::string();
  }

  std::bitset<2> h;
  h[0] = instruction[11];
  h[1] = instruction[12];

  switch (h.to_ulong()) {
    case 0u:
      return "B";
    case 1u:
      return "BL_HI";
    case 2u:
      return "BL_LO";
    case 3u:
      break;
    default:
      assert(false);
  }

  return std::string();
}

std::string MatchesAddOrSubtractRegister(const std::bitset<16>& instruction) {
  if (instruction[15] != 0u || instruction[14] != 0u || instruction[13] != 0u ||
      instruction[12] != 1u || instruction[11] != 1u || instruction[10] != 0u) {
    return std::string();
  }

  bool sub = (instruction[9] == 1u);

  std::string opcode;
  if (sub) {
    opcode = "SUBS";
  } else {
    opcode = "ADDS";
  }

  return opcode;
}

std::string MatchesAddOrSubtractImmediate3(const std::bitset<16>& instruction) {
  if (instruction[15] != 0u || instruction[14] != 0u || instruction[13] != 0u ||
      instruction[12] != 1u || instruction[11] != 1u || instruction[10] != 1u) {
    return std::string();
  }

  bool sub = (instruction[9] == 1u);

  std::string opcode;
  if (sub) {
    opcode = "SUBS";
  } else {
    opcode = "ADDS";
  }

  opcode += "_@3";

  return opcode;
}

std::string MatchesAddSubMovCmpImmediate8(const std::bitset<16>& instruction) {
  if (instruction[15] != 0u || instruction[14] != 0u || instruction[13] != 1u) {
    return std::string();
  }

  std::bitset<2> op;
  op[0] = instruction[11];
  op[1] = instruction[12];

  std::string opcode;
  switch (op.to_ulong()) {
    case 0u:
      opcode = "MOVS";
      break;
    case 1u:
      opcode = "CMP";
      break;
    case 2u:
      opcode = "ADDS";
      break;
    default:
      assert(false);
      return std::string();
    case 3u:
      opcode = "SUBS";
      break;
  }

  opcode += "_@8";

  return opcode;
}

std::string MatchesShiftImmediate5(const std::bitset<16>& instruction) {
  if (instruction[15] != 0u || instruction[14] != 0u || instruction[13] != 0u) {
    return std::string();
  }

  std::bitset<2> op;
  op[0] = instruction[11];
  op[1] = instruction[12];

  std::string opcode;
  switch (op.to_ulong()) {
    case 0u:
      opcode = "LSLS";
      break;
    case 1u:
      opcode = "LSRS";
      break;
    case 2u:
      opcode = "ASRS";
      break;
    default:
      assert(false);
    case 3u:
      return std::string();
  }

  opcode += "_@5";

  return opcode;
}

std::string MatchesDataProcessing(const std::bitset<16>& instruction) {
  if (instruction[15] != 0u || instruction[14] != 1u || instruction[13] != 0u ||
      instruction[12] != 0u || instruction[11] != 0u || instruction[10] != 0u) {
    return std::string();
  }

  std::bitset<4> op;
  op[0] = instruction[6];
  op[1] = instruction[7];
  op[2] = instruction[8];
  op[3] = instruction[9];

  std::string opcode;
  switch (op.to_ulong()) {
    case 0u:
      opcode = "ANDS";
      break;
    case 1u:
      opcode = "EORS";
      break;
    case 2u:
      opcode = "LSLS";
      break;
    case 3u:
      opcode = "LSRS";
      break;
    case 4u:
      opcode = "ASRS";
      break;
    case 5u:
      opcode = "ADCS";
      break;
    case 6u:
      opcode = "SBCS";
      break;
    case 7u:
      opcode = "RORS";
      break;
    case 8u:
      opcode = "TST";
      break;
    case 9u:
      opcode = "NEGS";
      break;
    case 10u:
      opcode = "CMP";
      break;
    case 11u:
      opcode = "CMN";
      break;
    case 12u:
      opcode = "ORRS";
      break;
    case 13u:
      opcode = "MULS";
      break;
    case 14u:
      opcode = "BICS";
      break;
    default:
      assert(false);
    case 15u:
      opcode = "MVNS";
      break;
  }

  return opcode;
}

std::string MatchesAddToStackOrPC(const std::bitset<16>& instruction) {
  if (instruction[15] != 1u || instruction[14] != 0u || instruction[13] != 1u ||
      instruction[12] != 0u) {
    return std::string();
  }

  bool sp = (instruction[11] == 1u);

  std::string opcode;
  if (sp) {
    opcode += "ADD_SP";
  } else {
    opcode += "ADD_PC";
  }

  return opcode;
}

std::string MatchesAddOrSubStackImmediate7(const std::bitset<16>& instruction) {
  if (instruction[15] != 1u || instruction[14] != 0u || instruction[13] != 1u ||
      instruction[12] != 1u || instruction[11] != 0u || instruction[10] != 0u ||
      instruction[9] != 0u || instruction[8] != 0u) {
    return std::string();
  }

  bool sub = (instruction[7] == 1u);

  std::string opcode;
  if (sub) {
    opcode += "SUB_SP_I7";
  } else {
    opcode += "ADD_PC_I7";
  }

  return opcode;
}

std::string MatchesMovAddCmpAllRegs(const std::bitset<16>& instruction) {
  if (instruction[15] != 0u || instruction[14] != 1u || instruction[13] != 0u ||
      instruction[12] != 0u || instruction[11] != 0u || instruction[10] != 1u) {
    return std::string();
  }

  std::bitset<2> op;
  op[0] = instruction[8];
  op[1] = instruction[9];

  std::string opcode;
  switch (op.to_ulong()) {
    case 0u:
      opcode = "ADD";
      break;
    case 1u:
      opcode = "CMP";
      break;
    case 2u:
      opcode = "MOV";
      break;
    default:
      assert(false);
    case 3u:
      return std::string();
  }

  opcode += "_ANY_REGISTER";

  return opcode;
}

std::string MatchesLoadStoreOffset5(const std::bitset<16>& instruction) {
  std::bitset<5> op;
  op[0] = instruction[11];
  op[1] = instruction[12];
  op[2] = instruction[13];
  op[3] = instruction[14];
  op[4] = instruction[15];

  std::string opcode;
  switch (op.to_ulong()) {
    case 12u:
      opcode = "STR";
      break;
    case 13u:
      opcode = "LDR";
      break;
    case 14u:
      opcode = "STRB";
      break;
    case 15u:
      opcode = "LDRB";
      break;
    case 16u:
      opcode = "STRH";
      break;
    case 17u:
      opcode = "LDRH";
      break;
    default:
      return std::string();
  }

  opcode += "_@5";

  return opcode;
}

std::string MatchesLoadStoreRegister(const std::bitset<16>& instruction) {
  std::bitset<7> op;
  op[0] = instruction[9];
  op[1] = instruction[10];
  op[2] = instruction[11];
  op[3] = instruction[12];
  op[4] = instruction[13];
  op[5] = instruction[14];
  op[6] = instruction[15];

  std::string opcode;
  switch (op.to_ulong()) {
    case 40u:
      opcode = "STR";
      break;
    case 41u:
      opcode = "STRH";
      break;
    case 42u:
      opcode = "STRB";
      break;
    case 43u:
      opcode = "LDRSB";
      break;
    case 44u:
      opcode = "LDR";
      break;
    case 45u:
      opcode = "LDRH";
      break;
    case 46u:
      opcode = "LDRB";
      break;
    case 47u:
      opcode = "LDRSH";
      break;
    default:
      return std::string();
  }

  return opcode;
}

std::string MatchesLdrPcOffset(const std::bitset<16>& instruction) {
  if (instruction[15] != 0u || instruction[14] != 1u || instruction[13] != 0u ||
      instruction[12] != 0u || instruction[11] != 1u) {
    return std::string();
  }

  return "LDR_PC_OFFSET_@8";
}

std::string MatchesLdrStrSpOffset(const std::bitset<16>& instruction) {
  if (instruction[15] != 1u || instruction[14] != 0u || instruction[13] != 0u ||
      instruction[12] != 1u) {
    return std::string();
  }

  bool l = (instruction[11] == 1u);

  std::string opcode;
  if (l) {
    opcode += "LDR";
  } else {
    opcode += "STR";
  }

  opcode += "_SP_OFFSET_@8";

  return opcode;
}

std::string MatchesLdmiaStmia(const std::bitset<16>& instruction) {
  if (instruction[15] != 1u || instruction[14] != 1u || instruction[13] != 0u ||
      instruction[12] != 0u) {
    return std::string();
  }

  bool l = (instruction[11] == 1u);

  std::string opcode;
  if (l) {
    opcode += "LDMIA";
  } else {
    opcode += "STMIA";
  }

  return opcode;
}

std::string MatchesPushPop(const std::bitset<16>& instruction) {
  if (instruction[15] != 1u || instruction[14] != 0u || instruction[13] != 1u ||
      instruction[12] != 1u || instruction[10] != 1u || instruction[9] != 0u) {
    return std::string();
  }

  bool l = (instruction[11] == 1u);

  std::string opcode;
  if (l) {
    opcode += "POP";
  } else {
    opcode += "PUSH";
  }

  return opcode;
}

std::string MatchesSWI(const std::bitset<16>& instruction) {
  if (instruction[15] != 1u || instruction[14] != 1u || instruction[13] != 0u ||
      instruction[12] != 1u || instruction[11] != 1u || instruction[10] != 1u ||
      instruction[9] != 1u || instruction[8] != 1u) {
    return std::string();
  }

  return "SWI";
}

const std::vector<std::function<std::string(const std::bitset<16>&)>>
    g_matchers = {MatchesBranchExchange,
                  MatchesConditionalBranch,
                  MatchesUnconditionalBranch,
                  MatchesAddOrSubtractRegister,
                  MatchesAddOrSubtractImmediate3,
                  MatchesAddSubMovCmpImmediate8,
                  MatchesShiftImmediate5,
                  MatchesDataProcessing,
                  MatchesAddToStackOrPC,
                  MatchesAddOrSubStackImmediate7,
                  MatchesMovAddCmpAllRegs,
                  MatchesLoadStoreOffset5,
                  MatchesLoadStoreRegister,
                  MatchesLdrPcOffset,
                  MatchesLdrStrSpOffset,
                  MatchesLdmiaStmia,
                  MatchesPushPop,
                  MatchesSWI};

std::string MatchInstruction(const std::bitset<16>& instruction) {
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
    return "THUMB_OPCODE_UNDEF";
  }

  return "THUMB_OPCODE_" + match;
}

}  // namespace

int main(int argc, char* argv[]) {
  std::vector<std::string> opcodes;
  for (uint32_t index = 0; index < 1024; index++) {
    std::bitset<10> bits(index);
    std::bitset<16> instruction;
    instruction[6] = bits[0];
    instruction[7] = bits[1];
    instruction[8] = bits[2];
    instruction[9] = bits[3];
    instruction[10] = bits[4];
    instruction[11] = bits[5];
    instruction[12] = bits[6];
    instruction[13] = bits[7];
    instruction[14] = bits[8];
    instruction[15] = bits[9];
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
  sorted_opcodes.erase("THUMB=OPCODE=UNDEF");

  std::cout << "#include <assert.h>" << std::endl;
  std::cout << "#include <stdint.h>" << std::endl << std::endl;

  std::cout << "typedef enum {" << std::endl;

  uint32_t value = 0u;
  for (const auto& entry : sorted_opcodes) {
    opcode_number[entry] = value;
    std::string opcode = entry;
    std::replace(opcode.begin(), opcode.end(), '=', '_');
    std::replace(opcode.begin(), opcode.end(), '@', 'I');
    std::cout << "  " << opcode << " = " << value++ << "u," << std::endl;
  }
  std::cout << "  THUMB_OPCODE_UNDEF = " << value << "u," << std::endl;
  opcode_number["THUMB=OPCODE=UNDEF"] = value++;

  std::cout << "} ThumbOpcode;" << std::endl << std::endl;

  std::cout
      << "static inline ThumbOpcode ThumbDecodeOpcode(uint16_t instruction) {"
      << std::endl;
  std::cout << "  static const uint8_t opcode_table[1024] = {" << std::endl;
  for (const auto& entry : opcodes) {
    std::cout << "    " << opcode_number.at(entry) << "u," << std::endl;
  }
  std::cout << "  };" << std::endl << std::endl;

  std::cout << "  uint_fast8_t opcode_index = instruction >> 6u;" << std::endl;
  std::cout << "  assert(opcode_index <= 1024);" << std::endl << std::endl;

  std::cout << "  return (ThumbOpcode)opcode_table[opcode_index];" << std::endl;
  std::cout << "}" << std::endl;

  return 0;
}