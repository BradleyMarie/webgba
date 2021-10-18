#include <gperftools/profiler.h>

#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

extern "C" {
#include "emulator/gba.h"
}

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cout << "Usage: benchmark <num-frames> <rom>" << std::endl;
    return 0;
  }

  int frame_count = 60u * 60u;  // 60 seconds * 60 frames per second.
  if (argc >= 3) {
    frame_count = std::atoi(argv[1u]);
    if (frame_count < 0) {
      std::cout << "ERROR: Negative frame count" << std::endl;
      return -1;
    }
  }

  std::ifstream file(argv[2u], std::ios::binary);
  if (file.fail()) {
    std::cout << "ERROR: Failed to open ROM file" << std::endl;
    return -1;
  }

  char c;
  std::vector<char> buffer;
  while (file.read(&c, sizeof(char))) {
    buffer.push_back(c);
  }

  if (buffer.empty()) {
    std::cout << "ERROR: Failed to read ROM file" << std::endl;
    return -1;
  }

  GbaEmulator *emulator;
  GamePad *gamepad;
  bool success =
      GbaEmulatorAllocate(buffer.data(), buffer.size(), &emulator, &gamepad);
  if (!success) {
    return false;
  }

  std::cout << "Rendering " << frame_count << " frames." << std::endl;

  auto begin = std::chrono::steady_clock::now();

  ProfilerStart("benchmark.profile");
  for (int i = 0; i < frame_count; i++) {
    for (uint32_t i = 0u; i < 280896u; i++) {
      GbaEmulatorStep(emulator);
    }
  }
  ProfilerStop();

  auto end = std::chrono::steady_clock::now();

  auto time_elapsed_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
          .count();

  std::cout << "Rendered " << frame_count << " frames in " << time_elapsed_ms
            << " ms" << std::endl;

  GamePadFree(gamepad);
  GbaEmulatorFree(emulator);

  return EXIT_SUCCESS;
}