#
# image should be loaded at 0x01000000
#
#TEXT_BASE = 0x80800000
TEXT_BASE = 0x88400000

PLATFORM_CPPFLAGS += -march=armv7-a -mno-unaligned-access -DTEXT_BASE=$(TEXT_BASE)
