package(default_visibility = ["//emulator/cpu/memory:__subpackages__"])

cc_binary(
    name = "gba_bios_memory",
    srcs = ["bios_memory.cc"],
    data = ["//third_party/bios/gba:gba_bios"],
    defines = ['BIOS_FILE=\\"third_party/bios/gba/gba_bios.bin\\"'],
)
