load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "memory_bank",
    srcs = ["memory_bank.c"],
    hdrs = ["memory_bank.h"],
)

cc_test(
    name = "memory_bank_test",
    srcs = ["memory_bank_test.cc"],
    deps = [
        ":memory_bank",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "memory",
    srcs = ["memory.c"],
    hdrs = ["memory.h"],
)

cc_test(
    name = "memory_test",
    srcs = ["memory_test.cc"],
    deps = [
        ":memory",
        "@com_google_googletest//:gtest_main",
    ],
)
