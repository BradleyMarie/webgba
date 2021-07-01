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
    name = "execute",
    hdrs = ["execute.h"],
    deps = [
        ":condition",
        ":opcode",
        ":operand",
        "//emulator/cpu/arm7tdmi/instructions:branch",
        "//emulator/cpu/arm7tdmi/instructions:data_processing",
        "//emulator:memory",
    ],
)

cc_test(
    name = "execute_test",
    srcs = ["execute_test.cc"],
    deps = [
        ":execute",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "opcode",
    hdrs = ["opcode.h"],
)

cc_library(
    name = "operand",
    hdrs = ["operand.h"],
    deps = ["//emulator/cpu/arm7tdmi:arm7tdmi"],
)

cc_test(
    name = "operand_test",
    srcs = ["operand_test.cc"],
    deps = [
        ":operand",
        "@com_google_googletest//:gtest_main",
    ],
)