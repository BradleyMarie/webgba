load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/game/gba:__subpackages__"])

cc_library(
    name = "null",
    srcs = ["null.c"],
    hdrs = ["null.h"],
    deps = [
        "//emulator/memory",
    ],
)

cc_test(
    name = "null_test",
    srcs = ["null_test.cc"],
    deps = [
        ":null",
        "@com_google_googletest//:gtest_main",
    ],
)
