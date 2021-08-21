load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/memory/gba:__subpackages__"])

cc_library(
    name = "io",
    hdrs = ["io.h"],
    srcs = ["io.c"],
    deps = [
        "//emulator/memory:memory",
    ],
)

cc_test(
    name = "io_test",
    srcs = ["io_test.cc"],
    deps = [
        ":io",
        "//emulator/memory:memory",
        "@com_google_googletest//:gtest_main",
    ],
)