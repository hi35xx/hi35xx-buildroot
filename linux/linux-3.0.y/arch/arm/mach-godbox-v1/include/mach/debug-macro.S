.macro  addruart, rp, rv
mov     \rp,      #0x001F0000
orr     \rp, \rp, #0x0000b000
orr     \rv, \rp, #0xfe000000   @ virtual base
orr     \rp, \rp,  #0x10000000  @ physical base
.endm
#include <asm/hardware/debug-pl01x.S>
