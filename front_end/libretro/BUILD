load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "webgba.so",
    srcs = ["webgba.c"],
    visibility = ["//third_party/sdlarch:__subpackages__"],
    linkopts = [
        "-Wl,-lGL,--version-script",
        "$(location :webgba.lds)",
    ],
    linkshared = 1,
    deps = [
        ":webgba.lds",
        "//emulator:gba",
        "//third_party/libretro",
    ],
)
