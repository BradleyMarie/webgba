load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/cpu/arm7tdmi:__subpackages__"])

cc_library(
    name = "branch_exchange",
    hdrs = ["branch_exchange.h"],
    deps = ["//emulator/cpu/arm7tdmi:arm7tdmi"],
)

cc_test(
    name = "branch_exchange_test",
    srcs = ["branch_exchange_test.cc"],
    deps = [
        ":branch_exchange",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "branch",
    hdrs = ["branch.h"],
    deps = ["//emulator/cpu/arm7tdmi:arm7tdmi"],
)

cc_test(
    name = "branch_test",
    srcs = ["branch_test.cc"],
    deps = [
        ":branch",
        "@com_google_googletest//:gtest_main",
    ],
)

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

cc_library(
    name = "multiply_long",
    hdrs = ["multiply_long.h"],
    deps = ["//emulator/cpu/arm7tdmi:arm7tdmi"],
)

cc_test(
    name = "multiply_long_test",
    srcs = ["multiply_long_test.cc"],
    deps = [
        ":multiply_long",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "swi",
    hdrs = ["swi.h"],
    deps = ["//emulator/cpu/arm7tdmi:arm7tdmi"],
)

cc_test(
    name = "swi_test",
    srcs = ["swi_test.cc"],
    deps = [
        ":swi",
        "@com_google_googletest//:gtest_main",
    ],
)