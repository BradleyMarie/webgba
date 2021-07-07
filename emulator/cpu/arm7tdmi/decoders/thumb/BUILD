load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/cpu/arm7tdmi/decoders:__subpackages__"])

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