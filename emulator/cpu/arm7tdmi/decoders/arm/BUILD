load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/cpu/arm7tdmi/decoders:__subpackages__"])

cc_library(
    name = "branch_link",
    hdrs = ["branch_link.h"],
    deps = ["//emulator/cpu/arm7tdmi:arm7tdmi"],
)

cc_test(
    name = "branch_link_test",
    srcs = ["branch_link_test.cc"],
    deps = [
        ":branch_link",
        "@com_google_googletest//:gtest_main",
    ],
)

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
        ":branch_link",
        ":condition",
        ":opcode",
        ":operand",
        "//emulator/cpu/arm7tdmi/instructions:block_data_transfer",
        "//emulator/cpu/arm7tdmi/instructions:branch_exchange",
        "//emulator/cpu/arm7tdmi/instructions:branch",
        "//emulator/cpu/arm7tdmi/instructions:coprocessor_data_operation",
        "//emulator/cpu/arm7tdmi/instructions:coprocessor_data_transfer",
        "//emulator/cpu/arm7tdmi/instructions:coprocessor_register_transfer",
        "//emulator/cpu/arm7tdmi/instructions:data_processing",
        "//emulator/cpu/arm7tdmi/instructions:load_store_register_byte",
        "//emulator/cpu/arm7tdmi/instructions:move_status_register",
        "//emulator/cpu/arm7tdmi/instructions:multiply",
        "//emulator/cpu/arm7tdmi/instructions:multiply_long",
        "//emulator/cpu/arm7tdmi/instructions:signed_data_transfer",
        "//emulator/cpu/arm7tdmi/instructions:single_data_swap",
        "//emulator/cpu/arm7tdmi/instructions:swi",
        "//emulator/memory:memory",
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