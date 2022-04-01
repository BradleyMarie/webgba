load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "sdlarch",
    srcs = [
        "glad.h",
        "glad.c",
        "khrplatform.h",
        "libretro.h",
        "sdlarch.c",
    ],
    data = ["//front_end/libretro:webgba.so"],
    linkopts = ["-lSDL2"]
)