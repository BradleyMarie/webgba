load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "arm7tdmi",
    hdrs = ["arm7tdmi.h"],
    srcs = ["arm7tdmi.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":exceptions",
        ":registers",
        "//emulator/cpu/arm7tdmi/decoders/arm:execute",
        "//emulator/cpu/arm7tdmi/decoders/thumb:execute",
        "//emulator/memory:memory",
        "//emulator/platform:interrupt_line",
    ]
)

cc_test(
    name = "arm7tdmi_test",
    srcs = ["arm7tdmi_test.cc"],
    deps = [
        ":arm7tdmi",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "exceptions",
    hdrs = ["exceptions.h"],
    srcs = ["exceptions.c"],
    visibility = ["//emulator/cpu/arm7tdmi:__subpackages__"],
    deps = [
        ":registers",
    ],
)

cc_test(
    name = "exceptions_test",
    srcs = ["exceptions_test.cc"],
    deps = [
        ":exceptions",
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
    hdrs = ["memory.h"],
    srcs = ["memory.c"],
    deps = [
        "//emulator/memory:memory",
    ],
    visibility = ["//emulator/cpu/arm7tdmi:__subpackages__"],
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
    name = "registers",
    hdrs = ["registers.h"],
    srcs = ["registers.c"],
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