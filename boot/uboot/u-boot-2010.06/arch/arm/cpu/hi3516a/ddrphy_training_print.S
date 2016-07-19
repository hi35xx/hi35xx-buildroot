#include <config.h>
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@  void ddr_training_print(int type, int num);
@
@
.align	2
.global	ddr_training_print
.type	ddr_training_print, %function
ddr_training_print:
	mov     r5, lr
	mov     r6, r1
	cmp	r0, #7
	movge	r0, #7	@ if type >=7, set to 7, WRONG type, r0 = 1 ~ 7
	cmp	r0, #1
	movlt	r0, #7	@ if type < 1, set to 7
	sub	r0, #1	@ r0 = 0 ~ 6
	lsl	r0, r0, #2	@ 4Bytes per error
	add     r1, pc, #0x20
	add	r0, r1
	bl      uart_early_puts
	mov     r0, r6
	bl      uart_early_put_hex
	mov     r0, #'\r'
	bl      uart_early_putc
	mov     r0, #'\n'
	bl      uart_early_putc
	mov     pc, r5


.align	2
SW_WL_ERR:
	.ascii "SWL\0"
.align	2
SW_GATE_ERR:
	.ascii "SGA\0"
.align	2
HW_GATE_ERR:
	.ascii "HGA\0"
.align	2
HW_RDET_ERR:
	.ascii "HRD\0"
.align	2
SW_WDET_ERR:
	.ascii "SWD\0"
.align	2
SW_RDEL_ERR:
	.ascii "SRD\0"
.align	2
WRONG:
	.ascii "ERR\0"
