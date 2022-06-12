load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/ppu/gba:__subpackages__"])

cc_library(
    name = "io",
    srcs = ["io.c"],
    hdrs = ["io.h"],
    deps = [
        "//emulator/ppu/gba:dirty",
        "//emulator/memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_test(
    name = "io_test",
    srcs = ["io_test.cc"],
    deps = [
        ":io",
        "@com_google_googletest//:gtest_main",
    ],
)
