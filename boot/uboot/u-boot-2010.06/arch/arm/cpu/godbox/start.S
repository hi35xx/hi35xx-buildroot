#include <config.h>
#include <version.h>

.globl _start
_start:
	b reset 
	b .
	b .
	b .
	b .
	b .
	b .
	b .

_pad:			.word 0x12345678 /* now 16*4=64 */

__blank_zone_start:
	.fill 1024*8,1,0
__blank_zone_end:

.balignl 16,0xdeadbeef
__start:		.word _start
/*************************************************************************
 *
 * Startup Code (reset vector)
 *
 * do important init only if we don't start from memory!
 * setup Memory and board specific bits prior to relocation.
 * relocate armboot to ram
 * setup stack
 *
 *************************************************************************/

.globl _blank_zone_start
_blank_zone_start:
	.word __blank_zone_start


.globl _blank_zone_end
_blank_zone_end:
	.word __blank_zone_end

_TEXT_BASE:
	.word	TEXT_BASE

.globl _armboot_start
_armboot_start:
	.word _start

/*
 * These are defined in the board-specific linker script.
 */
.globl _bss_start
_bss_start:
	.word __bss_start

.globl _bss_end
_bss_end:
	.word _end

.globl _relocate
_relocate:
	.word relocate
#ifdef CONFIG_USE_IRQ
/* IRQ stack memory (calculated at run-time) */
.globl IRQ_STACK_START
IRQ_STACK_START:
	.word	0x0badc0de

/* IRQ stack memory (calculated at run-time) */
.globl FIQ_STACK_START
FIQ_STACK_START:
	.word 0x0badc0de
#endif

_clr_remap_nand_entry:
    .word   NAND_TEXT_ADRS  + do_clr_remap - TEXT_BASE 

_clr_remap_spi_entry:
    .word   SF_TEXT_ADRS    + do_clr_remap - TEXT_BASE

_emmc_entry:
    .word  other_boot 
/*
 * the actual reset code
 */

reset:
	/*
	 *  delay wait mcu switch BUS finish
	 *  prevent mcu and A9 compete BUS conflict
	 */
	ldr	r0, =0x100
delay_wait_mcu:
	sub	r0, #1
	cmp	r0, #0
	bne	delay_wait_mcu
 
	/*
	 *  read and write system register REG_SC_GEN1
         *  judge BUS mux whether or not OK
	 */
	ldr	r0, =REG_BASE_SCTL
	mov	r1, #0
wait_mux_finish:
	add	r1, r1, #1
	str	r1, [r0, #REG_SC_GEN1]
	ldr	r2, [r0, #REG_SC_GEN1]
	cmp	r1, r2
	bne	wait_mux_finish 

	/*check if standby net transmit or not */
	ldr     r1, [r0, #REG_SC_GEN10]
	ldr     r2, =0x1211beef        /* magic for net transmit mode   */
	cmp     r1, r2
	bne     bootstrap_check       /*go to  ziju flow*/	
wait_mcu_sync_finish:
	ldr     r1, [r0, #REG_SC_GEN11]
	ldr     r2, =0x4523beef        /* magic for mcu synchronize flag */
	cmp     r1, r2
	bne     wait_mcu_sync_finish 
	mov     r1, #0                  /* clear standby  flag */
	str     r1, [r0, #REG_SC_GEN10]
	str     r1, [r0, #REG_SC_GEN11]

bootstrap_check:
	
	/*
	 *  read system register REG_SC_GEN1
	 *  check if ziju flag
	 */	
	ldr	r1, [r0, #REG_SC_GEN14]
	ldr	r2, =0x7a696a75          /* magic for "ziju" */
	cmp	r1, r2
	bne	normal_start_flow
	mov	r1, sp                   /* save sp */
	str	r1, [r0, #REG_SC_GEN14]  /* clear ziju flag */
	
	/* init PLL/DDRC/pin mux/... */
	ldr	r0, _blank_zone_start 
	ldr	r1, _TEXT_BASE
	sub	r0, r0, r1
	add	r0, r0, #RAM_START_ADRS
	mov	r1, #0x0                 /* flags: 0->normal 1->pm */ 
	bl	init_registers           /* init PLL/DDRC/... */

	ldr	r0, =REG_BASE_SCTL
    	ldr	r1, [r0, #REG_SC_GEN14]
	mov	sp, r1		         /* restore sp */
    	ldr	r1, [r0, #REG_SC_GEN15]
	mov	pc, r1                   /* return to bootrom */
	nop
	nop
	nop
	nop
	nop
	nop	
	nop
	nop
	b	.                        /* bug here */

normal_start_flow:	
 
	/*
	 * set the cpu to SVC32 mode
	 */
	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0xd3
	msr	cpsr,r0

	/*
	 * Invalidate L1 I/D
	 */
	mov	r0, #0			/* set up for MCR */
	mcr	p15, 0, r0, c8, c7, 0	/* invalidate TLBs */
	mcr	p15, 0, r0, c7, c5, 0	/* invalidate icache */

	/* Invalidate L1 D-cache */
	mcr     p15, 2, r0, c0, c0, 0	/* select L1 data cache */
	mrc     p15, 1, r3, c0, c0, 0	/* Read Current Cache Size Identification Register */
	ldr	r1, =0x1ff
	and     r3, r1, r3, LSR #13	/* r3 = (number of sets -1) */
	mov     r0, #0
way_loop:
	mov     r1, #0			/* r1->set counter */
line_loop:
	mov     r2, r0, LSL #30
	orr     r2, r1, LSL #5		/* r2->set/way cache-op format */
	mcr     p15, 0, r2, c7, c6, 2	/* Invalidate line described by r2 */
	add     r1, r1, #1		/* Increment set counter */
	cmp     r1, r3			/* Check if the last set is reached... */
	ble     line_loop		/* if not, continue the set_loop */
	add     r0, r0, #1		/* else, Increment way counter */
	cmp     r0, #4			/* Check if the last way is reached... */
	blt     way_loop		/* if not, continue the way_loop */
	
	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000	/* clear bits 13 (--V-) */
	bic	r0, r0, #0x00000007	/* clear bits 2:0 (-CAM) */
	orr	r0, r0, #0x00000002	/* set bit 1 (--A-) Align */
	orr	r0, r0, #0x00000800	/* set bit 12 (Z---) BTB */
	mcr	p15, 0, r0, c1, c0, 0

	/* check_boot_type */
	ldr	r0, =REG_BASE_PERI_CTRL 
	ldr     r0, [r0, #REG_START_MODE]
	mov     r6, r0, lsr#9	
	and     r6, #0x3
	
	/* 
	 * REG_SC_GEN14: save start addr 
	 * REG_SC_GEN15: save run flag
	 */
	ldr	r4, =REG_BASE_SCTL      

	cmp	r6, #BOOT_FROM_NAND
	bne	check_from_spi
	ldr	r2, =NAND_TEXT_ADRS
	str	r2, [r4, #REG_SC_GEN14] 
	mov     r2, #BOOT_FROM_NAND	
	str     r2, [r4, #REG_SC_GEN15]  
	b	check_start_mode

check_from_spi:
        cmp	r6, #BOOT_FROM_SPI
	bne	check_from_emmc
	ldr	r2, =SF_TEXT_ADRS
	str	r2, [r4, #REG_SC_GEN14]
	mov	r2, #BOOT_FROM_SPI
	str     r2, [r4, #REG_SC_GEN15] 	
	b       check_start_mode

check_from_emmc:
	cmp     r6, #BOOT_FROM_EMMC
	bne     check_from_nor
        @ldr     r2, _TEXT_BASE 
        mov     r2, #(0xc00 + CONFIG_MMC_BOOT_ADDR)
        str     r2, [r4, #REG_SC_GEN14]
        mov     r2, #BOOT_FROM_EMMC
        str     r2, [r4, #REG_SC_GEN15]
        b       check_start_mode

check_from_nor:
	b	.   /* boot from nor flash is not suport */

check_start_mode:

    	/* Check if I need jump to flash */
    	mov	r0, pc, lsr#28
	cmp	r0, #0x0
    	bne	check_ddr_run    
	
	cmp	r6, #BOOT_FROM_NAND
        ldreq	pc, _clr_remap_nand_entry
	
	cmp     r6, #BOOT_FROM_SPI
        ldreq	pc, _clr_remap_spi_entry
	
	/* boot from emmc, I need jump after ddr_init,not here!*/
	cmp	r6, #BOOT_FROM_EMMC
	beq	do_clr_remap
	b	.          /*bug here*/

check_ddr_run:
	cmp	r0, #8
	bne	do_clr_remap
	mov     r2, #BOOT_FROM_DDR	
	str     r2, [r4, #REG_SC_GEN15] 
	
do_clr_remap: 
	ldr 	r0, [r4, #REG_SC_CTRL]

    	/* Set clear remap bit */
    	orr 	r0, #(1<<8)
    	str 	r0, [r4, #REG_SC_CTRL]
	
	/* enable I-Cache now */
 	mrc	p15, 0, r0, c1, c0, 0
        orr	r0, r0, #0x00001000     /* set bit 12 (I) I-Cache */
        mcr	p15, 0, r0, c1, c0, 0
	
	/* check if pmoc resume */
	ldr	r1, [r4, #REG_SC_GEN0]
	ldr	r2, =0x66031013		/* magic for pm */
	cmp	r1, r2
	bne	normal_flag
	mov	r1, #0		        /* clear pm flag */
	str	r1, [r4, #REG_SC_GEN0]

	/* disable mcu */
	ldr	r0, =0x600bf000	
	ldr	r1, [r0]
        bic	r1, r1, #0x1
        str	r1, [r0]

	/* disable timer0 */
	ldr	r0, =REG_BASE_TIMER01
	mov	r1, #0
	str	r1, [r0, #REG_TIMER_CONTROL]

	/* disable WDG_RST */
	mov	r1, #0
	str	r1, [r4, #REG_SC_WDG_RST_CTRL]

	ldr	r0, _blank_zone_start 
	ldr	r1, _TEXT_BASE
	sub	r0, r0, r1
	ldr	r1, [r4, #REG_SC_GEN14]
	add	r0, r0, r1
	mov	r1, #1	        /* flags: 0->normal 1->pm */   
	bl	init_registers

    	ldr     r4, =REG_BASE_SCTL
	ldr	r0, [r4, #REG_SC_GEN9]
	mov	pc, r0
	nop
	nop
	nop
	nop
	nop
	nop	
	nop
	nop
	b	.                  /* bug here */
	
	/* Check if I'm running in ddr */
normal_flag:
	ldr	r0, [r4, #REG_SC_GEN15]
	cmp	r0, #BOOT_FROM_DDR
	beq	relocate
	ldr	r0, _blank_zone_start 
	ldr	r1, _TEXT_BASE
	sub	r0, r0, r1
	ldr	r1, [r4, #REG_SC_GEN14]
	add	r0, r0, r1
	mov	r1, #0x0      /* flags: 0->normal 1->pm */    
	bl	init_registers 

check_boot_emmc:
	ldr	r4, =REG_BASE_SCTL
	ldr	r0, [r4, #REG_SC_GEN15]
	cmp	r0, #BOOT_FROM_EMMC
	bne	other_boot

emmc_boot:
	mov	sp, #0x200
	bl	emmc_init
	ldr	r0, _TEXT_BASE
	ldr	r1, _bss_start
	sub	r1, r1, r0
	bl	emmc_read
	bl	emmc_deinit
	ldr	r4, =REG_BASE_SCTL
	ldr	r0, _TEXT_BASE
	str	r0, [r4, #REG_SC_GEN14] /* change emmc start addr to _TEXT_BASE */
	ldr	pc, _emmc_entry

other_boot:	
    	ldr     r4, =REG_BASE_SCTL
	ldr	r0, [r4, #REG_SC_GEN14]
	b	copy_abort_code

relocate:				@ relocate U-Boot to RAM
	mov	r2, pc
	sub	r2, r2, #8
	ldr	r1, _armboot_start
	ldr	r0, _relocate
	sub	r1, r0, r1
	sub	r0, r2, r1
	str	r0, [r4, #REG_SC_GEN14]
	/* copy abnormity code to 0 addr start */
copy_abort_code:
        ldr     r1, =0
        ldr     r2, =0x100
	add     r2, r0, r2
1:
        ldmia   r0!, {r3 - r10}
        stmia   r1!, {r3 - r10}
        cmp     r0, r2
        ble     1b
	/* relocate Boot to DDR */
    	ldr     r4, =REG_BASE_SCTL
	ldr	r0, [r4, #REG_SC_GEN14]
	/*now, r0 stores __reset offset from where we get started*/
	ldr	r1, _TEXT_BASE		/* r1 stores where we will copy uboot to */

	/* compare source and target address, if equal no copy to target address */
	cmp	r0, r1
	beq	stack_setup             



	ldr r2, _armboot_start
	ldr r3, _bss_start
        sub	r2, r3, r2      /* r2 <- size of armboot */
        add	r2, r0, r2      /* r2 <- source end address */

	/* copy code to ddr _TEXT_BASE start place */
copy_loop:				/* copy 32 bytes at a time */
	ldmia	r0!, {r3 - r10}		/* copy from source address [r0] */
	stmia	r1!, {r3 - r10}		/* copy to   target address [r1] */
	cmp	r0, r2			/* until source end addreee [r2] */
	ble	copy_loop

	/* Set up the stack */
stack_setup:
	ldr	r0, _TEXT_BASE		@ upper 128 KiB: relocated uboot
	sub	r0, r0, #CONFIG_SYS_MALLOC_LEN @ malloc area
	sub	r0, r0, #CONFIG_SYS_GBL_DATA_SIZE @ bdinfo
#ifdef CONFIG_USE_IRQ
	sub	r0, r0, #(CONFIG_STACKSIZE_IRQ + CONFIG_STACKSIZE_FIQ)
#endif
	sub	sp, r0, #12		@ leave 3 words for abort-stack
	and	sp, sp, #~7		@ 8 byte alinged for (ldr/str)d

	/* Clear BSS (if any). Is below tx (watch load addr - need space) */
clear_bss:
	ldr r0, _bss_start      @ find start of bss segment
	ldr r1, _bss_end        @ stop here
	mov r2, #0x00000000     @ clear value
clbss_l:
	str r2, [r0]        @ clear BSS location
	cmp r0, r1          @ are we at the end yet
	add r0, r0, #4      @ increment clear index pointer
	bne clbss_l         @ keep clearing till at end

	ldr pc, _start_armboot  @ jump to C code

_start_armboot: .word start_armboot

