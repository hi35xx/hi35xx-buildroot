/*
 *  Copyright (c) 2014 Hisilicon Limited.
 *  Copyright (c) 2014 Linaro Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/linkage.h>
#include <linux/init.h>
#include <asm/assembler.h>

	__CPUINIT

ENTRY(hix5hd2_secondary_startup)
	bl	v7_invalidate_l1
	b	secondary_startup
ENDPROC(hix5hd2_secondary_startup)


ENTRY(hi3519_secondary_startup)
	/* Set A17 acinactive to 1, bit 8 is acinactive */
	bl      hi_pmc_clear_a17_ac

	bl	cci_enable_port_for_self

	/* config l2ctl */
	mrc     p15, 1, r0, c9, c0, 2
	mov     r2, #0x100000
	orr     r1, r1, r2
	mcr     p15, 1, r0, c9, c0, 2

	/*
	 * set SMP bit ACTLR register for A17 slave core
	 */
	mrc     p15, 0, r0, c1, c0, 1
	orr     r0, #(1 << 6)
	mcr     p15, 0, r0, c1, c0, 1

	bl      v7_invalidate_l1

	b       secondary_startup
ENDPROC(hi3519_secondary_startup)

ENTRY(hi3519_cpu_resume)
	safe_svcmode_maskall    r1

	/* Set A17 acinactive to 0, bit 8 is acinactive */
	bl      hi_pmc_clear_a17_ac

	bl	cci_enable_port_for_self

        /* config l2ctl, just follow the datasheet why?*/
        mrc     p15, 1, r0, c9, c0, 2
        mov     r2, #0x100000
        orr     r1, r1, r2
        mcr     p15, 1, r0, c9, c0, 2

        /*
         * set SMP bit ACTLR register for A17 slave core
         */
        mrc     p15, 0, r0, c1, c0, 1
        orr     r0, #(1 << 6)
        mcr     p15, 0, r0, c1, c0, 1

        b       cpu_resume

ENDPROC(hi3519_cpu_resume)

ENTRY(hi3516av200_secondary_startup)
	/* Set A17 acinactive to 1, bit 8 is acinactive */
	bl      hi_pmc_clear_a17_ac

	bl	cci_enable_port_for_self

	/* config l2ctl */
	mrc     p15, 1, r0, c9, c0, 2
	mov     r2, #0x100000
	orr     r1, r1, r2
	mcr     p15, 1, r0, c9, c0, 2

	/*
	 * set SMP bit ACTLR register for A17 slave core
	 */
	mrc     p15, 0, r0, c1, c0, 1
	orr     r0, #(1 << 6)
	mcr     p15, 0, r0, c1, c0, 1

	bl      v7_invalidate_l1

	b       secondary_startup
ENDPROC(hi3516av200_secondary_startup)

ENTRY(hi3516av200_cpu_resume)
	safe_svcmode_maskall    r1

	/* Set A17 acinactive to 0, bit 8 is acinactive */
	bl      hi_pmc_clear_a17_ac

	bl	cci_enable_port_for_self

        /* config l2ctl, just follow the datasheet why?*/
        mrc     p15, 1, r0, c9, c0, 2
        mov     r2, #0x100000
        orr     r1, r1, r2
        mcr     p15, 1, r0, c9, c0, 2

        /*
         * set SMP bit ACTLR register for A17 slave core
         */
        mrc     p15, 0, r0, c1, c0, 1
        orr     r0, #(1 << 6)
        mcr     p15, 0, r0, c1, c0, 1

        b       cpu_resume

ENDPROC(hi3516av200_cpu_resume)

ENTRY(hi3559_secondary_startup)
	/* Set A17 acinactive to 1, bit 8 is acinactive */
	bl      hi_pmc_clear_a17_ac

	bl	cci_enable_port_for_self

	/* config l2ctl */
	mrc     p15, 1, r0, c9, c0, 2
	mov     r2, #0x100000
	orr     r1, r1, r2
	mcr     p15, 1, r0, c9, c0, 2

	/*
	 * set SMP bit ACTLR register for A17 slave core
	 */
	mrc     p15, 0, r0, c1, c0, 1
	orr     r0, #(1 << 6)
	mcr     p15, 0, r0, c1, c0, 1

	bl      v7_invalidate_l1

	b       secondary_startup
ENDPROC(hi3559_secondary_startup)

ENTRY(hi3559_cpu_resume)
	safe_svcmode_maskall    r1

	/* Set A17 acinactive to 0, bit 8 is acinactive */
	bl      hi_pmc_clear_a17_ac

	bl	cci_enable_port_for_self

        /* config l2ctl, just follow the datasheet why?*/
        mrc     p15, 1, r0, c9, c0, 2
        mov     r2, #0x100000
        orr     r1, r1, r2
        mcr     p15, 1, r0, c9, c0, 2

        /*
         * set SMP bit ACTLR register for A17 slave core
         */
        mrc     p15, 0, r0, c1, c0, 1
        orr     r0, #(1 << 6)
        mcr     p15, 0, r0, c1, c0, 1

        b       cpu_resume

ENDPROC(hi3559_cpu_resume)

ENTRY(hi3536c_secondary_startup)
    /* set the cpu to SVC32 mode */
    mrs r0, cpsr
    bic r0, r0, #0x1f  /* r0 = ((~0x1F) & r0) */
    orr r0, r0, #0xd3  /* r0 = (0xd3 | r0)    */
    msr cpsr, r0

    mrc p15, 0, r0, c0, c0, 5
    and r0, r0, #15
    adr r4, 1f
    ldmia   r4, {r5, r6}
    sub r4, r4, r5
    add r6, r6, r4
pen_36c:    ldr r7, [r6]
    cmp r7, r0
    bne pen_36c

    /*
     * we've been released from the holding pen: secondary_stack
     * should now contain the SVC stack for this core
     */
    b   secondary_startup

1:  .long   .
    .long   pen_release
ENDPROC(hi3536c_secondary_startup)
