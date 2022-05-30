load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "bg_affine",
    srcs = ["bg_affine.c"],
    hdrs = ["bg_affine.h"],
    linkopts = ["-lGL"],
    deps = [
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
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "control",
    srcs = ["control.c"],
    hdrs = ["control.h"],
    linkopts = ["-lGL"],
    deps = [
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "mosaic",
    srcs = ["mosaic.c"],
    hdrs = ["mosaic.h"],
    linkopts = ["-lGL"],
    deps = [
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "obj_attributes",
    srcs = ["obj_attributes.c"],
    hdrs = ["obj_attributes.h"],
    linkopts = ["-lGL"],
    deps = [
        ":texture_bindings",
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
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
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
        ":control",
        ":mosaic",
        ":obj_attributes",
        ":palette",
        ":shader_fragment",
        ":shader_vertex",
        ":tilemap",
        ":tiles",
        ":window",
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:draw_manager",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

genrule(
  name = "shader_fragment_generate",
  srcs = [":shader_fragment.glsl"],
  outs = ["shader_fragment.h"],
  cmd = "./$(location //tools/string_generator:string_generator) $(location :shader_fragment.glsl) fragment_shader_source > $@",
  tools = ["//tools/string_generator:string_generator"],
)

cc_library(
  name = "shader_fragment",
  hdrs = [":shader_fragment_generate"],
)

genrule(
  name = "shader_vertex_generate",
  srcs = [":shader_vertex.glsl"],
  outs = ["shader_vertex.h"],
  cmd = "./$(location //tools/string_generator:string_generator) $(location :shader_vertex.glsl) vertex_shader_source > $@",
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
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
    ],
)

cc_library(
    name = "window",
    srcs = ["window.c"],
    hdrs = ["window.h"],
    linkopts = ["-lGL"],
    deps = [
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:registers",
    ],
)