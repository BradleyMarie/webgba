load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//front_end:__subpackages__"])

cc_library(
    name = "libsamplerate",
    defines = [
        'PACKAGE=\\"libsamplerate\\"',
        'VERSION=\\"0.22.0\\"',
        'ENABLE_SINC_FAST_CONVERTER',
        'ENABLE_SINC_MEDIUM_CONVERTER',
        'ENABLE_SINC_BEST_CONVERTER',
    ],
    hdrs = ["samplerate.h"],
    srcs = [
        "common.h",
        "fastest_coeffs.h",
        "high_qual_coeffs.h",
        "mid_qual_coeffs.h",
        "samplerate.c",
        "src_linear.c",
        "src_sinc.c",
        "src_zoh.c",
    ],
)