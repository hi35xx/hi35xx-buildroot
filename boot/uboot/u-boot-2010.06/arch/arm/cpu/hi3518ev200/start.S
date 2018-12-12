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
/*
 *************************************************************************
 *
 * Jump vector table as in table 3.1 in [1]
 *
 *************************************************************************
 */


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
	.fill 1024*4,1,0
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

_clr_remap_fmc_entry:
    .word   FMC_TEXT_ADRS + do_clr_remap - TEXT_BASE

/*
 * the actual reset code
 */

reset:
	/*
	 * set the cpu to SVC32 mode
	 */
	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0xd3
	msr	cpsr,r0

	/*
	 * we do sys-critical inits only at reboot,
	 * not when booting from ram!
	 */

	/*
	 * flush v4 I/D caches
	 */
	mov	r0, #0
	mcr	p15, 0, r0, c7, c7, 0	/* flush v3/v4 cache */
	mcr	p15, 0, r0, c8, c7, 0	/* flush v4 TLB */

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002300	/* clear bits 13, 9:8 (--V- --RS) */
	bic	r0, r0, #0x00000087	/* clear bits 7, 2:0 (B--- -CAM) */
	orr	r0, r0, #0x00000002	/* set bit 2 (A) Align */
	mcr	p15, 0, r0, c1, c0, 0

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

	ldr	sp, =STACK_TRAINING
#ifdef CONFIG_SVB_ENABLE
	bl	start_svb
#endif

	/* after ziju, we need ddr traning */
#ifdef CONFIG_DDR_TRAINING_V2
	ldr	r0, =REG_BASE_SCTL
	bl	start_ddr_training       /* DDR training */
#endif

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
	/* init serial and printf a string. */
	ldr	sp, =STACK_TRAINING
	bl      uart_early_init
	bl      msg_main_cpu_startup

	@if running not boot from fmc
	@we skipping boot_type checking.
	mov    r0, pc, lsr#24
	cmp    r0, #0x0
	bne    do_clr_remap

/* check_boot_type */
	ldr     r0, =SYS_CTRL_REG_BASE
	ldr     r0, [r0, #REG_SYSSTAT]
	mov     r6, r0, lsr#4
	and     r6, #0x1
	cmp     r6, #BOOT_FROM_FMC			@ [4]:0 fmc, 1 emmc */
	ldreq   pc, _clr_remap_fmc_entry

	@otherwise, [31]=1 means boot from bootrom, err
	beq	bug

do_clr_remap:
    /* do clear remap */
    ldr     r4, =SYS_CTRL_REG_BASE
	ldr 	r0, [r4, #REG_SC_CTRL]
	/* reg[0x20050000:8]:
	 * 0:	keep remap
	 * 1:	clear remap
	 */
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
	beq	relocate

	ldr     r0, _blank_zone_start
	ldr     r1, _TEXT_BASE
	sub     r0, r0, r1
	adrl    r1, _start
	add     r0, r0, r1
	mov     r1, #0          /* flags: 0->normal 1->pm */
	bl      init_registers
#ifdef CONFIG_SVB_ENABLE
	bl	start_svb
#endif

#ifdef CONFIG_DDR_TRAINING_V2
	ldr	r0, =REG_BASE_SCTL
	bl	start_ddr_training       /* DDR training */
#endif
/* check_boot_mode */
	ldr     r0, =SYS_CTRL_REG_BASE
	ldr     r0, [r0, #REG_SYSSTAT]
	mov     r6, r0, lsr#4
	and		r6, #0x1
	cmp     r6, #BOOT_FROM_FMC
	beq		relocate
#ifdef CONFIG_EMMC_SUPPORT
/* emmc_boot_relocate */
	ldr		r0, _TEXT_BASE
	ldr		r1, _armboot_start
	ldr		r2, _bss_start
	sub		r1, r2, r1
	bl		emmc_boot_read
	b		copy_abort_code
#endif

relocate:
	adrl	r0, _start		@ r0 stores current position of code
	ldr		r1, _TEXT_BASE	@ r1 stores where we will copy uboot to
	ldr		r2, _armboot_start
	ldr		r3, _bss_start
	sub		r2, r3, r2		@ r2 <- size of armboot
	bl		memcpy

copy_abort_code:
	adrl	r0, _start		@ r0 stores current position of code
	ldr		r1, =0x00000000
	mov		r2, #0x100
	bl		memcpy

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

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@       void memcpy(r1, r0, r2);
@
.align  2
memcpy:
	cmp     r0, r1		@ don't reloc during debug
	moveq   pc, lr
	add     r2, r0, r2
memcpy_loop:
	ldmia   r0!, {r3 - r10}
	stmia   r1!, {r3 - r10}
	cmp     r0, r2
	ble     memcpy_loop
	mov     pc, lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
.align  2
msg_main_cpu_startup:
	mov     r5, lr
	add     r0, pc, #4
	bl      uart_early_puts
	mov     pc, r5
L10:
#ifndef CONFIG_SUPPORT_CA_RELEASE
	.ascii "\r\n\r\nSystem startup\r\n\0"
#else
	.ascii "\r\n\r\n\r\n\0"
#endif

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


#include "lowlevel_init.S"

