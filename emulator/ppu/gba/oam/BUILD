load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/ppu/gba:__subpackages__"])

cc_library(
    name = "oam",
    srcs = ["oam.c"],
    hdrs = ["oam.h"],
    deps = [
        "//emulator/memory",
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
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
