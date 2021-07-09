load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "arm7tdmi",
    hdrs = ["arm7tdmi.h"],
    srcs = ["arm7tdmi.c"],
    visibility = ["//visibility:public"],
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
    name = "execute",
    hdrs = ["execute.h"],
    srcs = ["execute.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":arm7tdmi",
        "//emulator/cpu/arm7tdmi/decoders/arm:execute",
        "//emulator/cpu/arm7tdmi/decoders/thumb:execute",
        "//emulator/memory:memory",
    ],
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