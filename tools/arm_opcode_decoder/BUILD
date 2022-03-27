load("@rules_cc//cc:defs.bzl", "cc_binary")

package(default_visibility = ["//visibility:private"])

cc_binary(
    name = "arm_opcode_decoder_generator",
    srcs = ["arm_opcode_decoder_generator.cc"],
)

genrule(
  name = "generate_decoder",
  outs = ["decoder.h"],
  cmd = "./$(location :arm_opcode_decoder_generator) > $@",
  tools = [":arm_opcode_decoder_generator"],
)

cc_library(
  name = "decoder",
  visibility = ["//emulator/cpu/arm7tdmi/decoders/arm:__subpackages__"],
  hdrs = [":generate_decoder"],
)