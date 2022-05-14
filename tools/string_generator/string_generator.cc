#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "usage: string_generator <file> <string_name>" << std::endl;
    return EXIT_SUCCESS;
  }

  std::cout << "static const char * " << argv[2] << " =" << std::endl;

  std::ifstream input(argv[1]);
  std::string line;
  while (std::getline(input, line)) {
    if (line.find('\"') != std::string::npos) {
      std::cerr << "ERROR: input file must not contain any double quotes"
                << std::endl;
      return -1;
    }

    std::cout << "\"" << line << "\\n\"" << std::endl;
  }
  std::cout << "\"\\n\";" << std::endl;

  return 0;
}