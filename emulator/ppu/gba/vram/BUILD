load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/ppu/gba:__subpackages__"])

cc_library(
    name = "vram",
    srcs = ["vram.c"],
    hdrs = ["vram.h"],
    deps = [
        "//emulator/memory",
        "//emulator/ppu/gba:memory",
    ],
)

cc_test(
    name = "vram_test",
    srcs = ["vram_test.cc"],
    deps = [
        ":vram",
        "@com_google_googletest//:gtest_main",
    ],
)
