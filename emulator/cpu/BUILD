load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "interrupt_line",
    srcs = ["interrupt_line.c"],
    hdrs = ["interrupt_line.h"],
)

cc_test(
    name = "interrupt_line_test",
    srcs = ["interrupt_line_test.cc"],
    deps = [
        ":interrupt_line",
        "@com_google_googletest//:gtest_main",
    ],
)
