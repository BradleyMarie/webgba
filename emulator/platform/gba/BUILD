load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "platform",
    srcs = ["platform.c"],
    hdrs = ["platform.h"],
    deps = [
        "//emulator/cpu:interrupt_line",
        "//emulator/memory",
    ],
)

cc_test(
    name = "platform_test",
    srcs = ["platform_test.cc"],
    deps = [
        ":platform",
        "@com_google_googletest//:gtest_main",
    ],
)
