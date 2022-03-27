load("@rules_cc//cc:defs.bzl", "cc_binary")

package(default_visibility = ["//visibility:private"])

cc_binary(
    name = "bios_data_generator",
    srcs = ["bios_data_generator.cc"],
    data = ["//third_party/bios/gba:gba_bios"],
    defines = ['BIOS_FILE=\\"third_party/bios/gba/gba_bios.bin\\"'],
)

genrule(
  name = "generate_data",
  srcs = ["//third_party/bios/gba:gba_bios"],
  outs = ["data.h"],
  cmd = "./$(location :bios_data_generator) > $@",
  tools = [":bios_data_generator"],
)

cc_library(
  name = "data",
  visibility = ["//emulator/memory/gba:__subpackages__"],
  hdrs = [":generate_data"],
)