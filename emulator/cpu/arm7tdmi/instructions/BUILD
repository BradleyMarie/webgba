load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/cpu/arm7tdmi:__subpackages__"])

cc_library(
    name = "block_data_transfer",
    hdrs = ["block_data_transfer.h"],
    srcs = ["block_data_transfer.c"],
    deps = [
        "//emulator/cpu/arm7tdmi:memory",
        "//emulator/cpu/arm7tdmi:registers",
    ],
)

cc_test(
    name = "block_data_transfer_test",
    srcs = ["block_data_transfer_test.cc"],
    deps = [
        ":block_data_transfer",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "branch_exchange",
    hdrs = ["branch_exchange.h"],
    deps = ["//emulator/cpu/arm7tdmi:registers"],
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
    deps = ["//emulator/cpu/arm7tdmi:registers"],
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
    name = "coprocessor_data_operation",
    hdrs = ["coprocessor_data_operation.h"],
    deps = ["//emulator/cpu/arm7tdmi:exceptions"],
)

cc_test(
    name = "coprocessor_data_operation_test",
    srcs = ["coprocessor_data_operation_test.cc"],
    deps = [
        ":coprocessor_data_operation",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "coprocessor_data_transfer",
    hdrs = ["coprocessor_data_transfer.h"],
    deps = ["//emulator/cpu/arm7tdmi:exceptions"],
)

cc_test(
    name = "coprocessor_data_transfer_test",
    srcs = ["coprocessor_data_transfer_test.cc"],
    deps = [
        ":coprocessor_data_transfer",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "coprocessor_register_transfer",
    hdrs = ["coprocessor_register_transfer.h"],
    deps = ["//emulator/cpu/arm7tdmi:exceptions"],
)

cc_test(
    name = "coprocessor_register_transfer_test",
    srcs = ["coprocessor_register_transfer_test.cc"],
    deps = [
        ":coprocessor_register_transfer",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "data_processing",
    hdrs = ["data_processing.h"],
    deps = [
        "//emulator/cpu/arm7tdmi:flags",
        "//emulator/cpu/arm7tdmi:registers",
    ],
)

cc_test(
    name = "data_processing_test",
    srcs = ["data_processing_test.cc"],
    deps = [
        ":data_processing",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "load_store_register_byte",
    hdrs = ["load_store_register_byte.h"],
    deps = [
        "//emulator/cpu/arm7tdmi:memory",
        "//emulator/cpu/arm7tdmi:registers",
    ],
)

cc_test(
    name = "load_store_register_byte_test",
    srcs = ["load_store_register_byte_test.cc"],
    deps = [
        ":load_store_register_byte",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "move_status_register",
    hdrs = ["move_status_register.h"],
    deps = ["//emulator/cpu/arm7tdmi:registers"],
)

cc_test(
    name = "move_status_register_test",
    srcs = ["move_status_register_test.cc"],
    deps = [
        ":move_status_register",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "multiply",
    hdrs = ["multiply.h"],
    deps = [
        "//emulator/cpu/arm7tdmi:flags",
        "//emulator/cpu/arm7tdmi:registers",
    ],
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
    deps = [
        "//emulator/cpu/arm7tdmi:flags",
        "//emulator/cpu/arm7tdmi:registers",
    ],
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
    name = "signed_data_transfer",
    hdrs = ["signed_data_transfer.h"],
    deps = [
        "//emulator/cpu/arm7tdmi:memory",
        "//emulator/cpu/arm7tdmi:registers",
    ],
)

cc_test(
    name = "signed_data_transfer_test",
    srcs = ["signed_data_transfer_test.cc"],
    deps = [
        ":signed_data_transfer",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "single_data_swap",
    hdrs = ["single_data_swap.h"],
    deps = [
        "//emulator/cpu/arm7tdmi:memory",
        "//emulator/cpu/arm7tdmi:registers",
    ],
)

cc_test(
    name = "single_data_swap_test",
    srcs = ["single_data_swap_test.cc"],
    deps = [
        ":single_data_swap",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "swi",
    hdrs = ["swi.h"],
    deps = ["//emulator/cpu/arm7tdmi:exceptions"],
)

cc_test(
    name = "swi_test",
    srcs = ["swi_test.cc"],
    deps = [
        ":swi",
        "@com_google_googletest//:gtest_main",
    ],
)