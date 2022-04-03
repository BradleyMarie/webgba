#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

namespace {

std::vector<unsigned char> ReadFile(const char* filename) {
  std::ifstream input(filename, std::ios::binary);
  std::vector<unsigned char> result(std::istreambuf_iterator<char>(input), {});
  return result;
}

}  // namespace

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout <<"usage: array_generator <file>" << std::endl;
    return EXIT_SUCCESS;
  }

  std::vector<unsigned char> bytes = ReadFile(argv[1]);
  std::cout << "#ifndef _TOOLS_ARRAY_GENERATOR_" << std::endl;
  std::cout << "#define _TOOLS_ARRAY_GENERATOR_" << std::endl << std::endl;

  std::cout << "#include <stdint.h>" << std::endl << std::endl;

  std::cout << "static const uint32_t array_size = " << bytes.size() << ";"
            << std::endl;
  std::cout << "static const unsigned char array_data[" << bytes.size()
            << "] = {" << std::endl;
  for (unsigned char byte : bytes) {
    std::cout << "  " << (unsigned)byte << "u," << std::endl;
  }
  std::cout << "};" << std::endl << std::endl;

  std::cout << "#endif  // _TOOLS_ARRAY_GENERATOR_" << std::endl;

  return 0;
}