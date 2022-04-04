#include <gperftools/profiler.h>

#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

extern "C" {
#include "emulator/gba.h"
}

void NoOpAudioCallback(int16_t left, int16_t right) {}

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cout << "Usage: benchmark <num-frames> <rom>" << std::endl;
    return EXIT_SUCCESS;
  }

  int frame_count = 60u * 60u;  // 60 seconds * 60 frames per second.
  if (argc >= 3) {
    frame_count = std::atoi(argv[1u]);
    if (frame_count < 0) {
      std::cout << "ERROR: Negative frame count" << std::endl;
      return EXIT_FAILURE;
    }
  }

  std::ifstream file(argv[2u], std::ios::binary);
  if (file.fail()) {
    std::cout << "ERROR: Failed to open ROM file" << std::endl;
    return EXIT_FAILURE;
  }

  char c;
  std::vector<unsigned char> buffer;
  while (file.read(&c, sizeof(unsigned char))) {
    buffer.push_back(static_cast<unsigned char>(c));
  }

  if (buffer.empty()) {
    std::cout << "ERROR: Failed to read ROM file" << std::endl;
    return EXIT_FAILURE;
  }

  GbaEmulator *emulator;
  GamePad *gamepad;
  bool success =
      GbaEmulatorAllocate(buffer.data(), buffer.size(), &emulator, &gamepad);
  if (!success) {
    return EXIT_FAILURE;
  }

  std::cout << "Rendering " << frame_count << " frames." << std::endl;

  auto begin = std::chrono::steady_clock::now();

  ProfilerStart("benchmark.profile");
  for (int i = 0; i < frame_count; i++) {
    GbaEmulatorStep(emulator, /*framebuffer=*/0, /*scale_factor=*/1,
                    NoOpAudioCallback);
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