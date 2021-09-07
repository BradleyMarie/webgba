load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/memory/gba:__subpackages__"])

cc_library(
    name = "wram",
    srcs = ["wram.c"],
    hdrs = ["wram.h"],
    deps = [
        "//emulator/memory",
    ],
)

cc_test(
    name = "wram_test",
    srcs = ["wram_test.cc"],
    deps = [
        ":wram",
        "@com_google_googletest//:gtest_main",
    ],
)
