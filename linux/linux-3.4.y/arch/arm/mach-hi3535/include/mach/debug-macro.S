#include <mach/platform.h>

.macro  addruart, rp, rv ,tmp
mov     \rp,      #0x00080000
orr     \rv, \rp, #0xfe000000
orr     \rv, \rv, #0x00100000  @ virtual base
orr     \rp, \rp, #0x20000000  @ physical base
.endm

#include <asm/hardware/debug-pl01x.S>
