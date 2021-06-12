load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/cpu/arm7tdmi:__subpackages__"])

cc_library(
    name = "multiply",
    hdrs = ["multiply.h"],
    deps = ["//emulator/cpu/arm7tdmi:arm7tdmi"],
)

cc_test(
    name = "multiply_test",
    srcs = ["multiply_test.cc"],
    deps = [
        ":multiply",
        "@com_google_googletest//:gtest_main",
    ],
)