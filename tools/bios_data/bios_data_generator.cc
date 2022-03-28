#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

namespace {

std::vector<unsigned char> ReadFile() {
  std::ifstream input(BIOS_FILE, std::ios::binary);
  std::vector<unsigned char> result(std::istreambuf_iterator<char>(input), {});
  return result;
}

}  // namespace

int main(int argc, char* argv[]) {
  std::vector<unsigned char> bytes = ReadFile();
  std::cout << "#ifndef _TOOLS_BIOS_DATA_DATA_" << std::endl;
  std::cout << "#define _TOOLS_BIOS_DATA_DATA_" << std::endl << std::endl;

  std::cout << "#include <stdint.h>" << std::endl << std::endl;

  std::cout << "static const uint32_t bios_size = " << bytes.size() << ";"
            << std::endl;
  std::cout << "static const unsigned char bios_data[" << bytes.size()
            << "] = {" << std::endl;
  for (unsigned char byte : bytes) {
    std::cout << "  " << (unsigned)byte << "u," << std::endl;
  }
  std::cout << "};" << std::endl << std::endl;

  std::cout << "#endif  // _TOOLS_BIOS_DATA_DATA_" << std::endl;

  return 0;
}