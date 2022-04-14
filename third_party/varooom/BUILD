load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//front_end:__subpackages__"])

genrule(
  name = "generate_game",
  srcs = ["varooom_3d.gba"],
  outs = ["game.c"],
  cmd = "./$(location //tools/array_generator:array_generator) $(locations :varooom_3d.gba) game > $@",
  tools = ["//tools/array_generator:array_generator"],
)

cc_library(
    name = "game",
    hdrs = [
        "game.h"
    ],
    srcs = [
        ":generate_game"
    ],
)