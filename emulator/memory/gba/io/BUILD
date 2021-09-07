load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/memory/gba:__subpackages__"])

cc_library(
    name = "io",
    srcs = ["io.c"],
    hdrs = ["io.h"],
    deps = [
        "//emulator/memory",
    ],
)

cc_test(
    name = "io_test",
    srcs = ["io_test.cc"],
    deps = [
        ":io",
        "//emulator/memory",
        "@com_google_googletest//:gtest_main",
    ],
)
