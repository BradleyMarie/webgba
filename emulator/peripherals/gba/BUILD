load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "peripherals",
    srcs = ["peripherals.c"],
    hdrs = ["peripherals.h"],
    deps = [
        "//emulator/memory",
        "//emulator/peripherals:gamepad",
        "//emulator/platform/gba:platform",
    ],
)

cc_test(
    name = "peripherals_test",
    srcs = ["peripherals_test.cc"],
    deps = [
        ":peripherals",
        "@com_google_googletest//:gtest_main",
    ],
)
