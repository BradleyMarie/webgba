load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "branch_link",
    hdrs = ["branch_link.h"],
    deps = ["//emulator/cpu/arm7tdmi:registers"],
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
    deps = ["//emulator/cpu/arm7tdmi:registers"],
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
    visibility = ["//emulator/cpu/arm7tdmi:__pkg__"],
    deps = [
        ":branch_link",
        ":condition",
        ":load",
        ":opcode",
        ":operand",
        ":shift",
        "//emulator/cpu/arm7tdmi:exceptions",
        "//emulator/cpu/arm7tdmi/instructions:block_data_transfer",
        "//emulator/cpu/arm7tdmi/instructions:branch_exchange",
        "//emulator/cpu/arm7tdmi/instructions:branch",
        "//emulator/cpu/arm7tdmi/instructions:data_processing",
        "//emulator/cpu/arm7tdmi/instructions:load_store_register_byte",
        "//emulator/cpu/arm7tdmi/instructions:multiply",
        "//emulator/cpu/arm7tdmi/instructions:signed_data_transfer",
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
    name = "load",
    hdrs = ["load.h"],
    deps = [
        "//emulator/cpu/arm7tdmi:exceptions",
        "//emulator/cpu/arm7tdmi:memory",
        "//emulator/cpu/arm7tdmi:registers",
    ],
)

cc_test(
    name = "load_test",
    srcs = ["load_test.cc"],
    deps = [
        ":load",
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
    deps = ["//emulator/cpu/arm7tdmi:registers"],
)

cc_test(
    name = "operand_test",
    srcs = ["operand_test.cc"],
    deps = [
        ":operand",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "shift",
    hdrs = ["shift.h"],
    deps = [
        "//emulator/cpu/arm7tdmi:flags",
        "//emulator/cpu/arm7tdmi:registers",
    ],
)

cc_test(
    name = "shift_test",
    srcs = ["shift_test.cc"],
    deps = [
        ":shift",
        "@com_google_googletest//:gtest_main",
    ],
)