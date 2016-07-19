
#define ARM(x...)   x

#define THUMB(x...)

/*
 * dcache_line_size - get the minimum D-cache line size from the CTR register
 * on ARMv7.
 */
.macro  dcache_line_size, reg, tmp
	mrc     p15, 0, \tmp, c0, c0, 1         @ read ctr
	lsr     \tmp, \tmp, #16
	and     \tmp, \tmp, #0xf                @ cache line size encoding
	mov     \reg, #4                        @ bytes per word
	mov     \reg, \reg, lsl \tmp            @ actual cache line size
.endm

	.align 5
.global dcache_flush_all
dcache_flush_all:
	stmfd   r13!, {r0 - r7, r9 - r12, r14}

	dmb			@ ensure ordering with previous memory accesses
	mrc	p15, 1, r0, c0, c0, 1	@ read clidr
	ands	r3, r0, #0x7000000	@ extract loc from clidr
	mov	r3, r3, lsr #23		@ left align loc bit field
	beq	finished		@ if loc is 0, then no need to clean
	mov	r10, #0			@ start clean at cache level 0
loop1:
	add	r2, r10, r10, lsr #1	@ work out 3x current cache level
	mov	r1, r0, lsr r2		@ extract cache type bits from clidr
	and	r1, r1, #7	@ mask of the bits for current cache only
	cmp	r1, #2			@ see what cache we have at this level
	blt	skip			@ skip if no cache, or just i-cache
	mcr	p15, 2, r10, c0, c0, 0	@ select current cache level in cssr
	isb				@ isb to sych the new cssr&csidr
	mrc	p15, 1, r1, c0, c0, 0	@ read the new csidr
	and	r2, r1, #7		@ extract the length of the cache lines
	add	r2, r2, #4		@ add 4 (line length offset)
	ldr	r4, =0x3ff
	ands	r4, r4, r1, lsr #3	@ find maximum number on the way size
	clz	r5, r4		@ find bit position of way size increment
	ldr	r7, =0x7fff
	ands	r7, r7, r1, lsr #13	@ extract max number of the index size
loop2:
	mov	r9, r4			@ create working copy of max way size
loop3:
 ARM(	orr	r11, r10, r9, lsl r5 )	@ factor way and cache number into r11
 THUMB(	lsl	r6, r9, r5	     )
 THUMB(	orr	r11, r10, r6	     )	@ factor way and cache number into r11
 ARM(	orr	r11, r11, r7, lsl r2 )	@ factor index number into r11
 THUMB(	lsl	r6, r7, r2	     )
 THUMB(	orr	r11, r11, r6	     )	@ factor index number into r11
	mcr	p15, 0, r11, c7, c14, 2	@ clean & invalidate by set/way
	subs	r9, r9, #1		@ decrement the way
	bge	loop3
	subs	r7, r7, #1		@ decrement the index
	bge	loop2
skip:
	add	r10, r10, #2		@ increment cache number
	cmp	r3, r10
	bgt	loop1
finished:
	mov	r10, #0			@ swith back to cache level 0
	mcr	p15, 2, r10, c0, c0, 0	@ select current cache level in cssr
	dsb
	isb
@	mov	pc, lr
	ldmfd   r13!, {r0 - r7, r9 - r12, pc}


	.align 5
.global dcache_inv_all
dcache_inv_all:
	stmfd   r13!, {r0 - r7, r9 - r12, r14}
	dmb			@ ensure ordering with previous memory accesses
	mrc	p15, 1, r0, c0, c0, 1	@ read clidr
	ands	r3, r0, #0x7000000	@ extract loc from clidr
	mov	r3, r3, lsr #23		@ left align loc bit field
	beq	finished		@ if loc is 0, then no need to clean
	mov	r10, #0			@ start clean at cache level 0
loop1_1:
	add	r2, r10, r10, lsr #1	@ work out 3x current cache level
	mov	r1, r0, lsr r2		@ extract cache type bits from clidr
	and	r1, r1, #7	@ mask of the bits for current cache only
	cmp	r1, #2			@ see what cache we have at this level
	blt	skip			@ skip if no cache, or just i-cache
	mcr	p15, 2, r10, c0, c0, 0	@ select current cache level in cssr
	isb				@ isb to sych the new cssr&csidr
	mrc	p15, 1, r1, c0, c0, 0	@ read the new csidr
	and	r2, r1, #7		@ extract the length of the cache lines
	add	r2, r2, #4		@ add 4 (line length offset)
	ldr	r4, =0x3ff
	ands	r4, r4, r1, lsr #3	@ find maximum number on the way size
	clz	r5, r4		@ find bit position of way size increment
	ldr	r7, =0x7fff
	ands	r7, r7, r1, lsr #13	@ extract max number of the index size
loop2_1:
	mov	r9, r4			@ create working copy of max way size
loop3_1:
 ARM(	orr	r11, r10, r9, lsl r5 )	@ factor way and cache number into r11
 THUMB(	lsl	r6, r9, r5	     )
 THUMB(	orr	r11, r10, r6	     )	@ factor way and cache number into r11
 ARM(	orr	r11, r11, r7, lsl r2 )	@ factor index number into r11
 THUMB(	lsl	r6, r7, r2	     )
 THUMB(	orr	r11, r11, r6	     )	@ factor index number into r11
	mcr	p15, 0, r11, c7, c6, 2	@ invalidate by set/way
	subs	r9, r9, #1		@ decrement the way
	bge	loop3
	subs	r7, r7, #1		@ decrement the index
	bge	loop2
skip_1:
	add	r10, r10, #2		@ increment cache number
	cmp	r3, r10
	bgt	loop1
finished_1:
	mov	r10, #0			@ swith back to cache level 0
	mcr	p15, 2, r10, c0, c0, 0	@ select current cache level in cssr
	dsb
	isb
	ldmfd   r13!, {r0 - r7, r9 - r12, pc}
	@mov	pc, lr

/*
 *      dcache_inv_range(start,end)
 *
 *      Invalidate the data cache within the specified region; we will
 *      be performing a DMA operation in this region and we want to
 *      purge old data in the cache.
 *
 *      - start   - virtual start address of region
 *      - end     - virtual end address of region
 */
	.align 5
.global dcache_inv_range
dcache_inv_range:
	stmfd   r13!, {r0 - r3, r14}
	dcache_line_size r2, r3
	sub     r3, r2, #1
	tst     r0, r3
	bic     r0, r0, r3

	mcrne   p15, 0, r0, c7, c14, 1          @ clean & invalidate D / U line

	tst     r1, r3
	bic     r1, r1, r3
	mcrne   p15, 0, r1, c7, c14, 1          @ clean & invalidate D / U line
1:
	mcr     p15, 0, r0, c7, c6, 1           @ invalidate D / U line
	add     r0, r0, r2
	cmp     r0, r1
	blo     1b
	dsb
	ldmfd   r13!, {r0 - r3, pc}
	@mov     pc, lr

/*
 *      dcache_clean_range(start,end)
 *      - start   - virtual start address of region
 *      - end     - virtual end address of region
 */
	.align 5
.global dcache_clean_range
dcache_clean_range:
	stmfd   r13!, {r0 - r3, r14}
	dcache_line_size r2, r3
	sub     r3, r2, #1
	bic     r0, r0, r3
1:
	mcr     p15, 0, r0, c7, c10, 1          @ clean D / U line
	add     r0, r0, r2
	cmp     r0, r1
	blo     1b
	dsb
	ldmfd   r13!, {r0 - r3, pc}
	@mov     pc, lr

