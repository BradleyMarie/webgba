load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "platform",
    hdrs = ["platform.h"],
    srcs = ["platform.c"],
    deps = [
        "//emulator/memory:memory",
        "//emulator/platform:interrupt_line",
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