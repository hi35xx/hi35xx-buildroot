
#include <mach/hardware.h>
#include <mach/platform.h>


.globl tcm_slow_reset


tcm_slow_reset:
	mov	r11, r0
	mov	r12, r1
	adrl    r0, tcm_start
	mov	r1, r11
	mov     r2, #0x80
	add     r2, r0, r2
copy_tcm:
	ldmia   r0!, {r3 - r10}
	stmia   r1!, {r3 - r10}
	cmp     r0, r2
	ble     copy_tcm
	mov     pc, r11
tcm_start:
	mov     r0, r12
	ldr     r1, [r0]
	bic     r1, r1, #0x00000007
	orr     r1, r1, #0x00000002
	str     r1, [r0]
slow_mode:
	ldr     r1, [r0]
	bic     r1, r1, #0xffffff87
	lsr     r1, #3
	cmp     r1, #2
	bne     slow_mode
	mov	r1, #0xf
	str	r1, [r0, #0x4]

