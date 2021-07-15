load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/memory/gba:__subpackages__"])

cc_library(
    name = "iram",
    hdrs = ["iram.h"],
    srcs = ["iram.c"],
    deps = [
        "//emulator/memory:memory",
    ],
)

cc_test(
    name = "iram_test",
    srcs = ["iram_test.cc"],
    deps = [
        ":iram",
        "@com_google_googletest//:gtest_main",
    ],
)