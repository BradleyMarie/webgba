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
  if (argc < 3) {
    std::cout << "usage: array_generator <file> <array_name>" << std::endl;
    return EXIT_SUCCESS;
  }

  std::vector<unsigned char> bytes = ReadFile(argv[1]);
  std::cout << "#include <stdint.h>" << std::endl << std::endl;

  std::cout << "const uint32_t " << argv[2] << "_size = " << bytes.size() << ";"
            << std::endl;
  std::cout << "const unsigned char " << argv[2] << "_data[" << bytes.size()
            << "] = {" << std::endl;
  for (unsigned char byte : bytes) {
    std::cout << "  " << (unsigned)byte << "u," << std::endl;
  }
  std::cout << "};" << std::endl;

  return 0;
}