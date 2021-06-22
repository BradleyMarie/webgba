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