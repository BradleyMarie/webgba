load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "arm7tdmi",
    srcs = ["arm7tdmi.c"],
    hdrs = ["arm7tdmi.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":exceptions",
        ":registers",
        "//emulator/cpu:interrupt_line",
        "//emulator/cpu/arm7tdmi/decoders/arm:execute",
        "//emulator/cpu/arm7tdmi/decoders/thumb:execute",
        "//emulator/memory",
        "//util:macros",
    ],
)

cc_test(
    name = "arm7tdmi_test",
    srcs = ["arm7tdmi_test.cc"],
    deps = [
        ":arm7tdmi",
        ":program_counter",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "exceptions",
    srcs = ["exceptions.c"],
    hdrs = ["exceptions.h"],
    visibility = ["//emulator/cpu/arm7tdmi:__subpackages__"],
    deps = [
        ":program_counter",
        ":registers",
    ],
)

cc_test(
    name = "exceptions_test",
    srcs = ["exceptions_test.cc"],
    deps = [
        ":exceptions",
        ":program_counter",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "flags",
    hdrs = ["flags.h"],
    visibility = ["//emulator/cpu/arm7tdmi:__subpackages__"],
)

cc_library(
    name = "memory",
    srcs = ["memory.c"],
    hdrs = ["memory.h"],
    visibility = ["//emulator/cpu/arm7tdmi:__subpackages__"],
    deps = [
        "//emulator/memory",
    ],
)

cc_test(
    name = "memory_test",
    srcs = ["memory_test.cc"],
    deps = [
        ":memory",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "program_counter",
    hdrs = ["program_counter.h"],
    visibility = ["//emulator/cpu/arm7tdmi:__subpackages__"],
    deps = [
        ":registers",
    ],
)

cc_test(
    name = "program_counter_test",
    srcs = ["program_counter_test.cc"],
    deps = [
        ":program_counter",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "registers",
    srcs = ["registers.c"],
    hdrs = ["registers.h"],
    visibility = ["//emulator/cpu/arm7tdmi:__subpackages__"],
)

cc_test(
    name = "registers_test",
    srcs = ["registers_test.cc"],
    deps = [
        ":registers",
        "@com_google_googletest//:gtest_main",
    ],
)
