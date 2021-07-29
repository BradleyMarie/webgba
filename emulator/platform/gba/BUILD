load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "interrupt_controller",
    hdrs = ["interrupt_controller.h"],
    srcs = ["interrupt_controller.c"],
    deps = [
        "//emulator/memory:memory",
        "//emulator/platform:interrupt_line",
    ],
)

cc_test(
    name = "interrupt_controller_test",
    srcs = ["interrupt_controller_test.cc"],
    deps = [
        ":interrupt_controller",
        "@com_google_googletest//:gtest_main",
    ],
)