#include <asm/assembler.h>

.text

.align 5
.globl call_swsusp_resume

/*
 * it must be aligned by CACHE_LINE_SIZE.
 * it's using the prefetch buffer.
 */

call_swsusp_resume:
mcr	p15, 0, r0, c1, c0, 0	@ turning MMU on
mrc	p15, 0, r3, c0, c0, 0	@ invalidate cache line by reading ID
mov	r3, r3			@ a valueless dummy
mov	r3, r1			@ a valuable dummy
mov	pc, r3			@ jump. Good luck !
