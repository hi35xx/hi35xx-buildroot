#
# image should be loaded at 0x01000000
#
TEXT_BASE = 0x40800000

PLATFORM_CPPFLAGS += -march=armv7-a -mno-unaligned-access -DTEXT_BASE=$(TEXT_BASE)
