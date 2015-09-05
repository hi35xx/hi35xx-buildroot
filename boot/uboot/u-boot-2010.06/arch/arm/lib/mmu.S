#include <asm/sizes.h>

#define P15_CTRL_RR	(1<<14)			/* cache replace method bit */
#define P15_CTRL_IC	(1<<12)			/* I-cache enable bit */
#define P15_CTRL_DC	(1<<2)			/* D-cache enable bit */
#define P15_CTRL_MMU	(1<<0)			/* MMU enable bit */

/* A:permit |Domain|Not Use|Not Use*/
#define MMU_SEC_DESC	( (3<<10) | (0<<5) | (1<<4) | (2<<0) )
#define MMU_SEC_CB	( (1<<3) | (1<<2) )	/* cached, write back */
#define MMU_SEC_NCB	~((1<<3) | (1<<2))	/* no cached, no writebuf */


/* r0: page directory address, must align to 16KB */
.global mmu_pagedir_init
mmu_pagedir_init:

	/*
	 * init page dir param for  setction address
	 */
	add	r3, r0, #SZ_16K
	ldr	r2, =MMU_SEC_DESC	/* param */
1:
	str	r2, [r0], #4		/* section 1:1 mapping */
	add	r2, r2, #SZ_1M		/* setction base address */
	teq	r0, r3
	bne	1b

	mov	pc,lr

/*
 * r0: page directory address, must align to 16KB
 * r1: start of cached MEM, must align to 1MB
 * r2: size of cached MEM, must align to 1MB
 */
.global mmu_pagedir_cached_range
mmu_pagedir_cached_range:

	/*
	 * init page dir param for cached , writebuffer
	 */
	add	r1, r0, r1, lsr #18	/* r1 = r0 + r1>>18 the start index addr of map mem */
	add	r2, r1, r2, lsr #18	/* the end index addr of map mem */

1:
	ldr	r0, [r1]
	orr	r0, #MMU_SEC_CB
	str	r0, [r1], #4		/*store and index addr ++ */

	cmp	r1, r2
	blt	1b

	mov	pc,lr

/*
 * r0: page directory address, must align to 16KB
 * r1: start of cached MEM, must align to 1MB
 * r2: size of cached MEM, must align to 1MB
 */
.global mmu_pagedir_nocached_range
mmu_pagedir_nocached_range:
clean_loop:

	/*
	 * init page dir param for no cache ,no writebuffer
	 */
	mrc	p15, 0, r15, c7, c14, 3
	bne	clean_loop		/* test, clean and invalidate D-cache */

	mov	r3, #0
	mcr	p15, 0, r3, c8, c7, 0	/* flush TLBs */

	add	r1, r0, r1, lsr #18	/* the start section index of map mem */
	add	r2, r1, r2, lsr #18	/* the end section index of map mem */

1:
	ldr	r0, [r1]
	and	r0, #MMU_SEC_NCB	/* the setcion is no cache,no writebuf */
	str	r0, [r1], #4

	cmp	r1, r2
	blt	1b

	mov	pc,lr

.global flush_cache_off
flush_cache_off:

	/*
	 * invalidate(flush) TLB
	 */
	mrc	p15, 0, r0, c1, c0, 0	/* read control reg >> r0 */
	mcr	p15, 0, r0, c1, c0, 0	/* write r0 >> control reg */

	mov	r0, #0
	mcr	p15, 0, r0, c8, c7, 0	/* flush TLBs */

	mov	pc,lr

/* r0: page directory address, must align to 16KB */
.global mmu_startup
mmu_startup:

	/*
	 * enable mmu
	 */
	stmdb	sp!, {r0, lr}
	bl	flush_cache_off		/* r0,lr >> stack */
	ldmia	sp!, {r0, lr}

	mrc	p15, 0, r3, c1, c0, 0	/* read control reg */
	bic	r3, r3, #P15_CTRL_RR	/* cache replace method */
	orr	r3, r3, #P15_CTRL_MMU	/* mmu enable bit */

	mov	r2, #0
	mov	r1, #-1

	mcr	p15, 0, r0, c2, c0, 0	/* write page table pointer to Base  Reg */
	mcr	p15, 0, r1, c3, c0, 0	/* write domain access control reg */
	mcr	p15, 0, r3, c1, c0, 0	/* enable mmu */
	mcr	p15, 0, r2, c8, c7, 0	/* flush TLBs */

	mov	pc,lr

.global mmu_turnoff
mmu_turnoff:

	/*
	 * disable d-cache, mmu
	 */
	mrc	p15, 0, r3, c1, c0, 0	/* read control reg */
	bic	r3, r3, #P15_CTRL_DC	/* disable d-cache bit */
	bic	r3, r3, #P15_CTRL_MMU	/* disable mmu bit */
	mcr	p15, 0, r3, c1, c0, 0	/* load control register */

	mov	pc,lr

.global dcache_stop
dcache_stop:

	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #P15_CTRL_DC
	mcr	p15, 0, r0, c1, c0, 0

	mov	pc,lr

.global dcache_start
dcache_start:
	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #P15_CTRL_DC
	mcr	p15, 0, r0, c1, c0, 0

	mov	pc,lr

.global dcache_stop_noflush
dcache_stop_noflush:

	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #P15_CTRL_DC
	mcr	p15, 0, r0, c1, c0, 0

	mov	pc,lr

.global dcache_flush_all
dcache_flush_all:
	mov	r0, #0			/* set up for MCR */
	mcr	p15, 0, r0, c8, c7, 0	/* invalidate TLBs */
	mcr	p15, 0, r0, c7, c5, 0	/* invalidate icache */

    	/* Invalidate L1 D-cache */
	mcr     p15, 2, r0, c0, c0, 0	/* select L1 data cache */

	/* Read Current Cache Size Identification Register */
    	mrc     p15, 1, r3, c0, c0, 0
    	ldr    r1, =0x1ff
    	and     r3, r1, r3, LSR #13	/* r3 = number of sets -1 */
   	mov     r0, #0
way_loop:
    	mov     r1, #0			/* r1->set counter */
line_loop:
    	mov     r2, r0, LSL #30
    	orr     r2, r1, LSL #5		/* r2->set/way cache-op format */

	/* Clean and Invalidate line described by r2 */
    	mcr     p15, 0, r2, c7, c14, 2
    	add     r1, r1, #1		/* Increment set counter */
   	cmp     r1, r3			/* Check if the last set is reached... */
    	ble     line_loop		/* if not, continue the set_loop */
    	add     r0, r0, #1		/* else, Increment way counter */
    	cmp     r0, #4			/* Check if the last way is reached... */
    	blt     way_loop		/* if not, continue the way_loop */

	mov	pc,lr

.global dcache_inv_all
dcache_inv_all:
	mov	r0, #0			/* set up for MCR */
	mcr	p15, 0, r0, c8, c7, 0	/* invalidate TLBs */
	mcr	p15, 0, r0, c7, c5, 0	/* invalidate icache */

	/* Invalidate L1 D-cache */
	mcr     p15, 2, r0, c0, c0, 0	/* select L1 data cache*/

	/* Read Current Cache Size Identification Register */
    	mrc     p15, 1, r3, c0, c0, 0
    	ldr     r1, =0x1ff
    	and     r3, r1, r3, LSR #13	/* r3 = number of sets -1 */
   	mov     r0, #0
way_lp:
    	mov     r1, #0			/* r1->set counter */
line_lp:
    	mov     r2, r0, LSL #30
    	orr     r2, r1, LSL #5		/* r2->set/way cache-op format */
    	mcr     p15, 0, r2, c7, c6, 2	/* Invalidate line described by r2 */
    	add     r1, r1, #1		/* Increment set counter */
   	cmp     r1, r3			/* Check if the last set is reached... */
    	ble     line_lp			/* if not, continue the set_loop */
    	add     r0, r0, #1		/* else, Increment way counter */
    	cmp     r0, #4			/* Check if the last way is reached... */
    	blt     way_lp			/* if not, continue the way_loop */

	mov	pc,lr

