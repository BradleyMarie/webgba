load("@rules_cc//cc:defs.bzl", "cc_binary")

package(default_visibility = ["//visibility:private"])

cc_binary(
    name = "thumb_opcode_decoder_generator",
    srcs = ["thumb_opcode_decoder_generator.cc"],
)

genrule(
  name = "generate_decoder",
  outs = ["decoder.h"],
  cmd = "./$(location :thumb_opcode_decoder_generator) > $@",
  tools = [":thumb_opcode_decoder_generator"],
)

cc_library(
  name = "decoder",
  visibility = ["//emulator/cpu/arm7tdmi/decoders/thumb:__subpackages__"],
  hdrs = [":generate_decoder"],
)