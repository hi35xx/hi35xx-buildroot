#include <mach/platform.h>

.macro  addruart, rp, rv ,tmp
mov     \rp,	  #0x00080000
orr     \rv, \rp, #0xFE000000
orr     \rv, \rv, #0x00100000	@ virtual addr
orr     \rp, \rp, #0x20000000	@ physical addr
.endm

#include <asm/hardware/debug-pl01x.S>
