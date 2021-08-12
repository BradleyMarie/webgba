load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/ppu/gba:__subpackages__"])

cc_library(
    name = "vram",
    hdrs = ["vram.h"],
    srcs = ["vram.c"],
    deps = [
        "//emulator/memory:memory",
        "//emulator/ppu/gba:types",
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