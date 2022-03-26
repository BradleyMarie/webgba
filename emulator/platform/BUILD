load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "power",
    srcs = ["power.c"],
    hdrs = ["power.h"],
)

cc_test(
    name = "power_test",
    srcs = ["power_test.cc"],
    deps = [
        ":power",
        "@com_google_googletest//:gtest_main",
    ],
)
