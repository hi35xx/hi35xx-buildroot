                .macro  addruart, rp, rv
                mov     \rp,      #0x00080000
                orr     \rv, \rp, #0xfe000000   @ virtual base
                orr     \rp, \rp, #0x20000000  @ physical base
                .endm

#include <asm/hardware/debug-pl01x.S>
