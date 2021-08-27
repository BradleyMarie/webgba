load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/ppu/gba:__subpackages__"])

cc_library(
    name = "palette",
    hdrs = ["palette.h"],
    srcs = ["palette.c"],
    deps = [
        "//emulator/memory:memory",
        "//emulator/ppu/gba:memory",
    ],
)

cc_test(
    name = "palette_test",
    srcs = ["palette_test.cc"],
    deps = [
        ":palette",
        "@com_google_googletest//:gtest_main",
    ],
)