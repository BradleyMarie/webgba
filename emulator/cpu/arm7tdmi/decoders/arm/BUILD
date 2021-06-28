load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/cpu/arm7tdmi/decoders:__subpackages__"])

cc_library(
    name = "condition",
    hdrs = ["condition.h"],
    deps = ["//emulator/cpu/arm7tdmi:arm7tdmi"],
)

cc_test(
    name = "condition_test",
    srcs = ["condition_test.cc"],
    deps = [
        ":condition",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "operation",
    hdrs = ["operation.h"],
)