                .macro  addruart, rp, rv
                mov     \rp,      #0x00080000
		ldr	\rv,      =0xFE280000	@ virtual base
                orr     \rp, \rp, #0x20000000   @ physical base
                .endm

#include <asm/hardware/debug-pl01x.S>
