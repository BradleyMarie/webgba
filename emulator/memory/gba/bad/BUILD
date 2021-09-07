load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/memory/gba:__subpackages__"])

cc_library(
    name = "bad",
    srcs = ["bad.c"],
    hdrs = ["bad.h"],
    deps = [
        "//emulator/memory",
    ],
)

cc_test(
    name = "bad_test",
    srcs = ["bad_test.cc"],
    deps = [
        ":bad",
        "@com_google_googletest//:gtest_main",
    ],
)
