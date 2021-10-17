load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "timers",
    srcs = ["timers.c"],
    hdrs = ["timers.h"],
    deps = [
        "//emulator/memory",
        "//emulator/platform/gba:platform",
        "//emulator/sound/gba:sound",
    ],
)

cc_test(
    name = "timers_test",
    srcs = ["timers_test.cc"],
    deps = [
        ":timers",
        "@com_google_googletest//:gtest_main",
    ],
)
