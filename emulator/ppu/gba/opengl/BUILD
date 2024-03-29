load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "bg_affine",
    srcs = ["bg_affine.c"],
    hdrs = ["bg_affine.h"],
    linkopts = ["-lGL"],
    deps = [
        ":uniform_locations",
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "bg_bitmap_mode3",
    srcs = ["bg_bitmap_mode3.c"],
    hdrs = ["bg_bitmap_mode3.h"],
    linkopts = ["-lGL"],
    deps = [
        ":texture_bindings",
        ":uniform_locations",
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "bg_bitmap_mode4",
    srcs = ["bg_bitmap_mode4.c"],
    hdrs = ["bg_bitmap_mode4.h"],
    linkopts = ["-lGL"],
    deps = [
        ":texture_bindings",
        ":uniform_locations",
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "bg_bitmap_mode5",
    srcs = ["bg_bitmap_mode5.c"],
    hdrs = ["bg_bitmap_mode5.h"],
    linkopts = ["-lGL"],
    deps = [
        ":texture_bindings",
        ":uniform_locations",
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "bg_control",
    srcs = ["bg_control.c"],
    hdrs = ["bg_control.h"],
    linkopts = ["-lGL"],
    deps = [
        ":uniform_locations",
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "bg_scrolling",
    srcs = ["bg_scrolling.c"],
    hdrs = ["bg_scrolling.h"],
    linkopts = ["-lGL"],
    deps = [
        ":uniform_locations",
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "blend",
    srcs = ["blend.c"],
    hdrs = ["blend.h"],
    linkopts = ["-lGL"],
    deps = [
        ":uniform_locations",
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "objects",
    srcs = ["objects.c"],
    hdrs = ["objects.h"],
    linkopts = ["-lGL"],
    deps = [
        ":texture_bindings",
        ":uniform_locations",
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
        "//emulator/ppu/gba:set",
    ],
)

cc_library(
    name = "palette",
    srcs = ["palette.c"],
    hdrs = ["palette.h"],
    linkopts = ["-lGL"],
    deps = [
        ":texture_bindings",
        ":uniform_locations",
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
    ],
)

cc_library(
    name = "programs",
    srcs = ["programs.c"],
    hdrs = ["programs.h"],
    linkopts = ["-lGL"],
    deps = [
        ":shader_fragment",
        ":shader_vertex",
        ":uniform_locations",
        "//emulator/ppu/gba:dirty",
    ],
)

cc_library(
    name = "render",
    srcs = ["render.c"],
    hdrs = ["render.h"],
    visibility = ["//emulator/ppu:__subpackages__"],
    deps = [
        ":bg_affine",
        ":bg_bitmap_mode3",
        ":bg_bitmap_mode4",
        ":bg_bitmap_mode5",
        ":bg_control",
        ":bg_scrolling",
        ":blend",
        ":objects",
        ":palette",
        ":programs",
        ":tilemap",
        ":tiles",
        ":window",
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
        "//emulator:screen",
    ],
)

genrule(
  name = "shader_fragment_generate",
  srcs = [":shader.frag"],
  outs = ["shader_fragment.h"],
  cmd = "./$(location //tools/string_generator:string_generator) $(location :shader.frag) fragment_shader_source > $@",
  tools = ["//tools/string_generator:string_generator"],
)

cc_library(
  name = "shader_fragment",
  hdrs = [":shader_fragment_generate"],
)

genrule(
  name = "shader_vertex_generate",
  srcs = [":shader.vert"],
  outs = ["shader_vertex.h"],
  cmd = "./$(location //tools/string_generator:string_generator) $(location :shader.vert) vertex_shader_source > $@",
  tools = ["//tools/string_generator:string_generator"],
)

cc_library(
  name = "shader_vertex",
  hdrs = [":shader_vertex_generate"],
)

cc_library(
    name = "texture_bindings",
    hdrs = ["texture_bindings.h"],
)

cc_library(
    name = "tilemap",
    srcs = ["tilemap.c"],
    hdrs = ["tilemap.h"],
    linkopts = ["-lGL"],
    deps = [
        ":texture_bindings",
        ":uniform_locations",
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
    ],
)

cc_library(
    name = "tiles",
    srcs = ["tiles.c"],
    hdrs = ["tiles.h"],
    linkopts = ["-lGL"],
    deps = [
        ":texture_bindings",
        ":uniform_locations",
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
    ],
)

cc_library(
    name = "uniform_locations",
    srcs = ["uniform_locations.c"],
    hdrs = ["uniform_locations.h"],
    linkopts = ["-lGL"],
)

cc_library(
    name = "window",
    srcs = ["window.c"],
    hdrs = ["window.h"],
    linkopts = ["-lGL"],
    deps = [
        ":uniform_locations",
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:registers",
    ],
)