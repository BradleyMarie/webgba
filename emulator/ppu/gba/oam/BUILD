load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/ppu/gba:__subpackages__"])

cc_library(
    name = "oam",
    hdrs = ["oam.h"],
    srcs = ["oam.c"],
    deps = [
        "//emulator/memory:memory",
        "//emulator/ppu/gba:memory",
    ],
)

cc_test(
    name = "oam_test",
    srcs = ["oam_test.cc"],
    deps = [
        ":oam",
        "@com_google_googletest//:gtest_main",
    ],
)