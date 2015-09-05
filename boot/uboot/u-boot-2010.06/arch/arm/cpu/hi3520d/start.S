/*
 * armboot - Startup Code for OMAP3530/ARM Cortex CPU-core
 *
 * Copyright (c) 2004	Texas Instruments <r-woodruff2@ti.com>
 *
 * Copyright (c) 2001	Marius Gröger <mag@sysgo.de>
 * Copyright (c) 2002	Alex Züpke <azu@sysgo.de>
 * Copyright (c) 2002	Gary Jennejohn <garyj@denx.de>
 * Copyright (c) 2003	Richard Woodruff <r-woodruff2@ti.com>
 * Copyright (c) 2003	Kshitij <kshitij@ti.com>
 * Copyright (c) 2006-2008 Syed Mohammed Khasim <x0khasim@ti.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <version.h>

.globl _start
_start: b	reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

_undefined_instruction: .word undefined_instruction
_software_interrupt:	.word software_interrupt
_prefetch_abort:	.word prefetch_abort
_data_abort:		.word data_abort
_not_used:		.word not_used
_irq:			.word irq
_fiq:			.word fiq
_pad:			.word 0x12345678 /* now 16*4=64 */

__blank_zone_start:
#ifdef ENABLE_HI3520D_BLANK
.fill REG_INFO_BLANK_SIZE,1,0
#endif
#ifdef ENABLE_HI3515A_BLANK
.fill REG_INFO_BLANK_SIZE,1,0
#endif
__blank_zone_end:

.globl _blank_zone_start
_blank_zone_start:
.word __blank_zone_start

.globl _blank_zone_end
_blank_zone_end:
.word __blank_zone_end


	.balignl 16,0xdeadbeef

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

_clr_remap_spi_entry:
    .word   SF_TEXT_ADRS + do_clr_remap - TEXT_BASE
_clr_remap_ddr_entry:
    .word   MEM_BASE_DDR + do_clr_remap - TEXT_BASE

#ifdef CONFIG_DDR_GATE_TRAINING
_gatetraining_offset:
	.word	ddrphy_train_route - _start
#endif
/* hi DDR Training v200 */
#ifdef CONFIG_DDR_TRAINING_V200
_ddrtraining_offset:
	.word   ddrt_entry - _start
.globl _ddrtraining_flag
_ddrtraining_flag:
	@.word 0
	.word DDRT_CHECK_VALUE
_blank_zone_offset:
	.word   __blank_zone_start - _start
.globl _ddrtraining_flag_offset
_ddrtraining_flag_offset:
	.word _ddrtraining_flag - _start
#endif

/*
 * the actual reset code
 */

reset:
	/*
	 *  read system register REG_SC_GEN2
         *  check if ziju flag
	 */
	ldr	r0, =SYS_CTRL_REG_BASE
    	ldr	r1, [r0, #REG_SC_GEN2]
	ldr	r2, =0x7a696a75          /* magic for "ziju" */
	cmp	r1, r2
	bne	normal_start_flow
	mov	r1, sp                   /* save sp */
	str	r1, [r0, #REG_SC_GEN2]  /* clear ziju flag */

	/* init PLL/DDRC/pin mux/... */
	ldr	r0, _blank_zone_start
	ldr	r1, _TEXT_BASE
	sub	r0, r0, r1
	ldr	r1, =RAM_START_ADRS
	add	r0, r0, r1
	mov	r1, #0x0                 /* flags: 0->normal 1->pm */
	bl	init_registers           /* init PLL/DDRC/... */

	ldr	r0, =SYS_CTRL_REG_BASE
    	ldr	r1, [r0, #REG_SC_GEN2]
	mov	sp, r1		         /* restore sp */
    	ldr	r1, [r0, #REG_SC_GEN3]
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
	mov	r0, #0			@ set up for MCR
	mcr	p15, 0, r0, c8, c7, 0	@ invalidate TLBs
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate icache

    	/* Invalidate L1 D-cache */
	mcr     p15, 2, r0, c0, c0, 0	/* select L1 data cache */
    	mrc     p15, 1, r3, c0, c0, 0   /* Read Current Cache Size
						Identification Register */
    	ldr     r1, =0x1ff
    	and     r3, r1, r3, LSR #13     /* r3 = (number of sets -1 */
   	mov     r0, #0

way_loop:
    	mov     r1, #0                  /* r1->set counter */
line_loop:
    	mov     r2, r0, LSL #30
    	orr     r2, r1, LSL #5          /* r2->set/way cache-op format */
    	mcr     p15, 0, r2, c7, c6, 2   /* Invalidate line described by r2 */
    	add     r1, r1, #1              /* Increment set counter */
   	cmp     r1, r3                  /* Check if the last set is reached */
    	ble     line_loop               /* if not, continue the set_loop */
    	add     r0, r0, #1              /* else, Increment way counter */
    	cmp     r0, #4                  /* Check if the last way is reached */
    	blt     way_loop                /* if not, continue the way_loop */

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000	@ clear bits 13 (--V-)
	bic	r0, r0, #0x00000007	@ clear bits 2:0 (-CAM)
	orr	r0, r0, #0x00000002	@ set bit 1 (--A-) Align
	orr	r0, r0, #0x00000800	@ set bit 11 (Z---) BTB
	mcr	p15, 0, r0, c1, c0, 0

       	@if running not boot from  spi/ddr ram,
	@we skipping boot_type checking.
    	mov    r0, pc, lsr#24
    	cmp    r0, #0x0
    	bne    do_clr_remap

	ldr     pc, _clr_remap_spi_entry

do_clr_remap:
        /* do clear remap */
    	ldr     r4, =SYS_CTRL_REG_BASE
	ldr 	r0, [r4, #REG_SC_CTRL]

    	@Set clear remap bit.
    	orr 	r0, #(1<<8)
    	str 	r0, [r4, #REG_SC_CTRL]

	@enable I-Cache now
 	mrc p15, 0, r0, c1, c0, 0
        orr r0, r0, #0x00001000 /* set bit 12 (I) I-Cache */
        mcr p15, 0, r0, c1, c0, 0

	@Check wether I'm running in dynamic mem bank
	mov r0, pc, lsr#28
	cmp r0, #8
	bleq    relocate

	ldr     r0, _blank_zone_start
#if defined (ENABLE_HI3520D_BLANK) && defined (ENABLE_HI3515A_BLANK)
	ldr	r2, =REG_CHIP_ID_BASE
	ldr	r1, [r2]
	and	r1, r1, #3
	cmp	r1, #3
	beq	chip_20d
	mov	r3, #REG_INFO_BLANK_SIZE
	add	r0, r0, r3, lsl#0
chip_20d:
#endif
	ldr     r1, _TEXT_BASE
	sub     r0, r0, r1
	adrl    r1, _start
	add     r0, r0, r1
	mov     r1, #0          /* flags: 0->normal 1->pm */
	bl      init_registers
#ifdef CONFIG_DDR_GATE_TRAINING
	/* gate training macro */
	.macro do_gate_training
	ldr	r3, =SF_TEXT_ADRS
	ldr	r4, _gatetraining_offset
	add	r3, r3, r4
	ldr     sp, =DDRT_SP
	mov	lr, pc
	mov	pc, r3
	.endm

gate_training_start:
	/* gate training or not */
	ldr	r1, =DDRT_OPTION_REG2
	ldr	r2, [r1]
	and	r2, r2, #0x1
	cmp	r2, #1
	bne	gate_training_start_end

	do_gate_training

	ldr     r1, =DDRT_OPTION_REG2
	cmp     r0, #1
	bne     gate_training_success
	mov     r2, #0xf
	str     r2, [r1]
	b	gate_training_start_end
gate_training_success:
	mov     r2, #0xa
	str     r2, [r1]
gate_training_start_end:
#endif

#ifdef CONFIG_DDR_TRAINING_V200
	ldr     r0, =SYS_CTRL_REG_BASE
	ldr     r0, [r0, #REG_SYSSTAT]
	mov     r6, r0, lsr#4
	and     r6, #0x3
	cmp     r6, #0
	beq     ddr_training
	b       ddr_training_end

ddr_training:
	ldr     r0, _ddrtraining_flag
	ldr     r1, =DDRT_CHECK_VALUE
	cmp     r0, r1
	bne     ddr_training_end

	ldr     r0, _blank_zone_offset
	ldr     r3, =SF_TEXT_ADRS
	ldr     r4, _ddrtraining_offset
	add     r3, r3, r4
	ldr     sp, =DDRT_SP
	mov     lr, pc
	mov     pc, r3

ddr_training_end:
#endif

#ifndef CONFIG_SKIP_RELOCATE_UBOOT

relocate:				@ relocate U-Boot to RAM
	adrl	r0, _start		@ r0 <- current position of code
	ldr	r1, _TEXT_BASE		@ test if we run from flash or RAM
	cmp	r0, r1			@ don't reloc during debug
	beq	stack_setup

	ldr	r2, _armboot_start
	ldr	r3, _bss_start
	sub	r2, r3, r2		@ r2 <- size of armboot
	add	r2, r0, r2		@ r2 <- source end address

copy_loop:				@ copy 32 bytes at a time
	ldmia	r0!, {r3 - r10}		@ copy from source address [r0]
	stmia	r1!, {r3 - r10}		@ copy to   target address [r1]
	cmp	r0, r2			@ until source end addreee [r2]
	ble	copy_loop
#endif	/* CONFIG_SKIP_RELOCATE_UBOOT */

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
	ldr	r0, _bss_start		@ find start of bss segment
	ldr	r1, _bss_end		@ stop here
	mov	r2, #0x0		@ clear value
clbss_l:
	str	r2, [r0]		@ clear BSS location
	cmp	r0, r1			@ are we at the end yet
	add	r0, r0, #4		@ increment clear index pointer
	bne	clbss_l			@ keep clearing till at end

	ldr	pc, _start_armboot	@ jump to C code

_start_armboot: .word start_armboot

bug:

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	b	.			/* bug here */

/*
 *************************************************************************
 *
 * Interrupt handling
 *
 *************************************************************************
 */
@
@ IRQ stack frame.
@
#define S_FRAME_SIZE	72

#define S_OLD_R0	68
#define S_PSR		64
#define S_PC		60
#define S_LR		56
#define S_SP		52

#define S_IP		48
#define S_FP		44
#define S_R10		40
#define S_R9		36
#define S_R8		32
#define S_R7		28
#define S_R6		24
#define S_R5		20
#define S_R4		16
#define S_R3		12
#define S_R2		8
#define S_R1		4
#define S_R0		0

#define MODE_SVC 0x13
#define I_BIT	 0x80

/*
 * use bad_save_user_regs for abort/prefetch/undef/swi ...
 * use irq_save_user_regs / irq_restore_user_regs for IRQ/FIQ handling
 */

	.macro	bad_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE		@ carve out a frame on current
						@ user stack
	stmia	sp, {r0 - r12}			@ Save user registers (now in
						@ svc mode) r0-r12

	ldr	r2, _armboot_start
	sub	r2, r2, #(CONFIG_SYS_MALLOC_LEN)
	@ set base 2 words into abort
	sub	r2, r2, #(CONFIG_SYS_GBL_DATA_SIZE + 8)
						@ stack
	ldmia	r2, {r2 - r3}			@ get values for "aborted" pc
						@ and cpsr (into parm regs)
	add	r0, sp, #S_FRAME_SIZE		@ grab pointer to old stack

	add	r5, sp, #S_SP
	mov	r1, lr
	stmia	r5, {r0 - r3}			@ save sp_SVC, lr_SVC, pc, cpsr
	mov	r0, sp				@ save current stack into r0
						@ (param register)
	.endm

	.macro	irq_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			@ Calling r0-r12
	add	r8, sp, #S_PC			@ !! R8 NEEDS to be saved !!
						@ a reserved stack spot would
						@ be good.
	stmdb	r8, {sp, lr}^			@ Calling SP, LR
	str	lr, [r8, #0]			@ Save calling PC
	mrs	r6, spsr
	str	r6, [r8, #4]			@ Save CPSR
	str	r0, [r8, #8]			@ Save OLD_R0
	mov	r0, sp
	.endm

	.macro	irq_restore_user_regs
	ldmia	sp, {r0 - lr}^			@ Calling r0 - lr
	mov	r0, r0
	ldr	lr, [sp, #S_PC]			@ Get PC
	add	sp, sp, #S_FRAME_SIZE
	subs	pc, lr, #4			@ return & move spsr_svc into
						@ cpsr
	.endm

	.macro get_bad_stack
	ldr	r13, _armboot_start		@ setup our mode stack (enter
						@ in banked mode)
	@ move past malloc pool
	sub	r13, r13, #(CONFIG_SYS_MALLOC_LEN)
	@ move to reserved a couple
	sub	r13, r13, #(CONFIG_SYS_GBL_DATA_SIZE + 8)
						@ spots for abort stack

	str	lr, [r13]			@ save caller lr in position 0
						@ of saved stack
	mrs	lr, spsr			@ get the spsr
	str	lr, [r13, #4]			@ save spsr in position 1 of
						@ saved stack

	mov	r13, #MODE_SVC			@ prepare SVC-Mode
	@ msr	spsr_c, r13
	msr	spsr, r13			@ switch modes, make sure
						@ moves will execute
	mov	lr, pc				@ capture return pc
	movs	pc, lr				@ jump to next instruction &
						@ switch modes.
	.endm

	.macro get_bad_stack_swi
	sub	r13, r13, #4			@ space on current stack for
						@ scratch reg.
	str	r0, [r13]			@ save R0's value.
	ldr	r0, _armboot_start		@ get data regions start
	@ move past malloc pool
	sub	r0, r0, #(CONFIG_SYS_MALLOC_LEN)
	@ move past gbl and a couple
	sub	r0, r0, #(CONFIG_SYS_GBL_DATA_SIZE + 8)
						@ spots for abort stack
	str	lr, [r0]			@ save caller lr in position 0
						@ of saved stack
	mrs	r0, spsr			@ get the spsr
	str	lr, [r0, #4]			@ save spsr in position 1 of
						@ saved stack
	ldr	r0, [r13]			@ restore r0
	add	r13, r13, #4			@ pop stack entry
	.endm

	.macro get_irq_stack			@ setup IRQ stack
	ldr	sp, IRQ_STACK_START
	.endm

	.macro get_fiq_stack			@ setup FIQ stack
	ldr	sp, FIQ_STACK_START
	.endm

/*
 * exception handlers
 */
	.align	5
undefined_instruction:
	get_bad_stack
	bad_save_user_regs
	bl	do_undefined_instruction

	.align	5
software_interrupt:
	get_bad_stack_swi
	bad_save_user_regs
	bl	do_software_interrupt

	.align	5
prefetch_abort:
	get_bad_stack
	bad_save_user_regs
	bl	do_prefetch_abort

	.align	5
data_abort:
	get_bad_stack
	bad_save_user_regs
	bl	do_data_abort

	.align	5
not_used:
	get_bad_stack
	bad_save_user_regs
	bl	do_not_used

#ifdef CONFIG_USE_IRQ

	.align	5
irq:
	get_irq_stack
	irq_save_user_regs
	bl	do_irq
	irq_restore_user_regs

	.align	5
fiq:
	get_fiq_stack
	/* someone ought to write a more effective fiq_save_user_regs */
	irq_save_user_regs
	bl	do_fiq
	irq_restore_user_regs

#else

	.align	5
irq:
	get_bad_stack
	bad_save_user_regs
	bl	do_irq

	.align	5
fiq:
	get_bad_stack
	bad_save_user_regs
	bl	do_fiq

#endif

/*
 *	v7_flush_dcache_all()
 *
 *	Flush the whole D-cache.
 *
 *	Corrupted registers: r0-r5, r7, r9-r11
 *
 *	- mm	- mm_struct describing address space
 */
	.align 5
.global v7_flush_dcache_all
v7_flush_dcache_all:
	stmfd	r13!, {r0 - r5, r7, r9 - r12, r14}

	mov	r7, r0				@ take a backup of device type
	cmp	r0, #0x3			@ check if the device type is
						@ GP
	moveq r12, #0x1				@ set up to invalide L2
smi:	.word 0x01600070			@ Call SMI monitor (smieq)
	cmp	r7, #0x3			@ compare again in case its
						@ lost
	beq	finished_inval			@ if GP device, inval done
						@ above

	mrc	p15, 1, r0, c0, c0, 1		@ read clidr
	ands	r3, r0, #0x7000000		@ extract loc from clidr
	mov	r3, r3, lsr #23			@ left align loc bit field
	beq	finished_inval			@ if loc is 0, then no need to
						@ clean
	mov	r10, #0				@ start clean at cache level 0
inval_loop1:
	add	r2, r10, r10, lsr #1		@ work out 3x current cache
						@ level
	mov	r1, r0, lsr r2			@ extract cache type bits from
						@ clidr
	and	r1, r1, #7			@ mask of the bits for current
						@ cache only
	cmp	r1, #2				@ see what cache we have at
						@ this level
	blt	skip_inval			@ skip if no cache, or just
						@ i-cache
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level
						@ in cssr
	mov	r2, #0				@ operand for mcr SBZ
	mcr	p15, 0, r2, c7, c5, 4		@ flush prefetch buffer to
						@ sych the new cssr&csidr,
						@ with armv7 this is 'isb',
						@ but we compile with armv5
	mrc	p15, 1, r1, c0, c0, 0		@ read the new csidr
	and	r2, r1, #7			@ extract the length of the
						@ cache lines
	add	r2, r2, #4			@ add 4 (line length offset)
	ldr	r4, =0x3ff
	ands	r4, r4, r1, lsr #3		@ find maximum number on the
						@ way size
	clz	r5, r4				@ find bit position of way
						@ size increment
	ldr	r7, =0x7fff
	ands	r7, r7, r1, lsr #13		@ extract max number of the
						@ index size
inval_loop2:
	mov	r9, r4				@ create working copy of max
						@ way size
inval_loop3:
	orr	r11, r10, r9, lsl r5		@ factor way and cache number
						@ into r11
	orr	r11, r11, r7, lsl r2		@ factor index number into r11
	mcr	p15, 0, r11, c7, c6, 2		@ invalidate by set/way
	subs	r9, r9, #1			@ decrement the way
	bge	inval_loop3
	subs	r7, r7, #1			@ decrement the index
	bge	inval_loop2
skip_inval:
	add	r10, r10, #2			@ increment cache number
	cmp	r3, r10
	bgt	inval_loop1
finished_inval:
	mov	r10, #0				@ swith back to cache level 0
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level
						@ in cssr
	mcr	p15, 0, r10, c7, c5, 4		@ flush prefetch buffer,
						@ with armv7 this is 'isb',
						@ but we compile with armv5

	ldmfd	r13!, {r0 - r5, r7, r9 - r12, pc}

#include "lowlevel_init.S"
#include "ddrphy_train_route.S"
