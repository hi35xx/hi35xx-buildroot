#include <mach/platform.h>
#include <mach/hardware.h>
#include <asm/memory.h>
#include <linux/linkage.h>

#define __virt_to_phys(x)   ((x) - PAGE_OFFSET + PLAT_PHYS_OFFSET)
#define __phys_to_virt(x)   ((x) - PLAT_PHYS_OFFSET + PAGE_OFFSET)

/* General CPU constants */
#define MODE_USR        0x10
#define MODE_FIQ        0x11
#define MODE_IRQ        0x12
#define MODE_SVC        0x13
#define MODE_ABT        0x17
#define MODE_UND        0x1B
#define MODE_SYS        0x1F
#define I_Bit           0x80
#define F_Bit           0x40

#define DM_CA9_SCU      0x10220000
#define CA9_SCU_ICD	0x1000       @ GIC Distributor offset from SCU_BASE
#define CA9_SCU_ICC	0x100        @ GIC CPU Interface offset from SCU_BASE

@@ ************************************************************************@@
@@                                                                         @@
@@                                                                         @@
@@ General Interupt Controller (GIC) - register offsets                    @@
@@ ====================================================                    @@
@@ ICD register offsets with respect to the GIC Distributor base address   @@
@@ ICC register offsets with respect to the GIC CPU Interface base address @@
@@                                                                         @@
@@ ************************************************************************@@

#define ICC_ICR   0x0   @ ICC control (banked in Security Extns)
#define ICC_PMR   0x4   @ interrupt priority mask
#define ICC_BPR   0x8   @ binary point (banked in Security Extns)
#define ICC_ABPR  0x1C  @ aliased bianry point (Security Extns)

#define ICD_DCR   0x0   @ ICD control (banked in Security Extns)
#define ICD_ICTR  0x4   @ type information (RO)
#define ICD_ISR   0x80  @ interrupt security registers
#define ICD_ISER  0x100 @ set-enable registers
#define ICD_IPR   0x400 @ priority registers
#define ICD_IPTR  0x800 @ processor target registers
#define ICD_ICFR  0xC00 @ interrupt configuration registers

	.macro	mode_save, rd

	@ save current mode r0~r12, r14, so we can use them
	STMIA	\rd!, {r4 - r12, sp, lr}

	@ Save banked ARM registers
	MRS	r4, CPSR
	MRS	r5, SPSR

	@ save the current CPSR and SPSR
	STMIA	\rd!, {r4 - r5}

	CPS	#MODE_SYS
	STMIA	\rd!, {sp, lr}

	@ switch to Abort mode
	CPS	#MODE_ABT
	MRS	r4, SPSR
	STMIA	\rd!, {r4, sp, lr}

	@ switch to Undefined mode
	CPS	#MODE_UND
	MRS	r4, SPSR
	STMIA	\rd!, {r4, sp, lr}

	CPS	#MODE_IRQ
	MRS	r4, SPSR
	STMIA	\rd!, {r4, sp, lr}

	CPS	#MODE_FIQ
	MRS	r4, SPSR
	STMIA	\rd!, {r4, r8 - r12, sp, LR}

	CPS	#MODE_SVC

	@ CSSELR ? Cache Size Selection Register
	MRC	p15, 2, r3, c0, c0, 0

	@ ACTLR - Auxiliary Control Register
	MRC	p15, 0, r4, c1, c0, 1

	@ SCTLR - System Control Register
	MRC	p15, 0, r5, c1, c0, 0

	@ CPACR - Coprocessor Access Control Register
	MRC	p15, 0, r6, c1, c0, 2

	STMIA	\rd!, {r3, r4, R5, r6}

	.endm

	.macro	gic_save, rd

	LDR	r1, =hi_scu_virtbase
	LDR	r1, [r1]
	MOVW	r6, #CA9_SCU_ICC
	ADD	r1, r1, r6
	LDR	r2, [r1, #0x4]@interrupt priority mask
	LDR	r3, [r1, #0x8]@interrupt binary point (grouping)
	LDR	r4, [r1, #0x0]@CPU Interface control
	STMIA	\rd!, {r2-r4}

	@ Set priority mask to the lowest value to block interrupts
	MOV	r2, #0x00
	STR	r2, [r1, #0x4]	@ interrupt priority mask

	LDR	r1, =hi_scu_virtbase
	LDR	r1, [r1]
	MOVW	r6, #CA9_SCU_ICD
	ADD	r1, r1, r6
	LDR	r2, [r1, #0x4]	@ read feature fields
	UBFX	r2, r2, #0, #5		@ extract ITLinesNumber
	ADD	r2, r2, #1
	MOV	r3, r2

gicloop1:
	LDR	r5, [r1, #0x100]	@ interrupt set-enable
	STR	r5, [\rd], #4
	ADD	r1, r1, #4
	SUBS	r3, r3, #1
	BNE	gicloop1

	LDR	r1, =hi_scu_virtbase
	LDR	r1, [r1]
	MOVW	r6, #CA9_SCU_ICD
	ADD	r1, r1, r6
	LSL	r3, r2, #3

gicloop2:
	LDR	r4, [r1, #0x400]	@ interrupt priority
	LDR	r5, [r1, #0x800]	@ interrupt processor targets
	STM	\rd!, {r4, r5}
	ADD	r1, r1, #4
	SUBS	r3, r3, #1
	BNE	gicloop2

	LDR	r1, =hi_scu_virtbase
	LDR	r1, [r1]
	MOVW	r6, #CA9_SCU_ICD
	ADD	r1, r1, r6
	LSL	r3, r2, #1

gicloop3:
	LDR	r4, [r1, #0xc00]	@ interrupt configuration
	STR	r4, [\rd], #4
	ADD	r1, r1, #4
	SUBS	r3, r3, #1
	BNE	gicloop3

	LDR	r1, =hi_scu_virtbase
	LDR	r1, [r1]
	MOVW	r6, #CA9_SCU_ICD
	ADD	r1, r1, r6
	LDR	r2, [r1, #0x0]	@ Distributor control
	STR	r2, [\rd], #4

	.endm

	.macro	vfp_save, rd

	@ FPSID,MVFR0 and MVFR1 don't get serialized/saved (Read Only).
	MRC	p15, 0, r9, c1, c0, 2    @ CPACR allows CP10 and CP11 access
	ORR	r2, r9, #0xF00000
	MCR	p15, 0, r2, c1, c0, 2
	ISB
	MRC	p15, 0, r2, c1, c0, 2
	AND	r2, r2, #0xF00000
	CMP	r2, #0xF00000
	BEQ	do_fpu_save
	MOVS	r2, #1
	STR     r2, [r12, #(REG_SC_GEN2)]
	B	exit_fpu_save

do_fpu_save:

	@ Save configuration registers and enable.
	MRS	r10, FPEXC
	STR	r10, [\rd], #4        @ Save the FPEXC

	@ Enable FPU access to save/restore the other registers.
	LDR	r2, =0x40000000
	MSR	FPEXC, r2
	MRS	r2, FPSCR
	STR	r2, [\rd], #4         @ Save the FPSCR

	@ Store the VFP-D16 registers.
	VSTM	\rd!, {D0 - D15}

exit_fpu_save:
	MCR	p15, 0, r9, c1, c0, 2   @ Restore the original CPACR value.

	.endm

	.macro	mmu_save, rd

	@ read PID register
	MRC	p15, 0, r2, c13, c0, 0
	STR	r2, [r12, #(REG_SC_GEN3)]

	@ read TTB register
	MRC	p15, 0, r5, c2, c0, 0     @ TTBR0
	MRC	p15, 0, r6, c2, c0, 1     @ TTBR1
	MRC	p15, 0, r7, c2, c0, 2     @ TTBCR
	STMIA	\rd!, {r5 - r7}

	MRC	p15, 0, r4, c3, c0, 0     @ DACR
	MRC	p15, 0, r5, c7, c4, 0     @ PAR
	MRC	p15, 0, r6, c10, c2, 0    @ PRRR
	MRC	p15, 0, r7, c10, c2, 1    @ NMRR
	STMIA	\rd!, {r4 - r7}

	MRC	p15, 0, r3, c12, c0, 0    @ VBAR
	MRC	p15, 0, r4, c13, c0, 1    @ CONTEXTIDR
	MRC	p15, 0, r5, c13, c0, 2    @ TPIDRURW
	MRC	p15, 0, r6, c13, c0, 3    @ TPIDRURO
	MRC	p15, 0, r7, c13, c0, 4    @ TPIDRPRW
	STMIA	\rd!, {r3 - r7}

	.endm

	.macro	ddrtrain_save, rd

	@ save ddr train data
	MOV	r4, #0
	LDR	r5, =PAGE_OFFSET

save_loop:
	LDR	r1, [r5]
	STR	r1, [\rd]
	ADD	\rd, \rd, #4
	ADD	r5, r5, #4
	ADD	r4, r4, #4
	CMP	r4, #32
	BNE     save_loop

	.endm

	.macro	mode_restore, rd

	LDMDB	\rd!, {r3, r4, R5, r6}

	@ Restore CPACR. This must be done after configuring FPU
	MCR	p15, 0, r6, c1, c0, 2
	ISB

	@ SCTLR - System Control Register
	MCR	p15, 0, r5, c1, c0, 0
	ISB

	@ ACTLR - Auxiliary Control Register
	MCR	p15, 0, r4, c1, c0, 1
	ISB

	@ CSSELR ? Cache Size Selection Register
	MCR	p15, 2, r3, c0, c0, 0

	CPS	#MODE_FIQ                @ switch to FIQ mode
	LDMDB	\rd!, {r4, r8 - r12, sp, lr}
	MSR	SPSR_fsxc, r4            @ restore the current SPSR

	CPS	#MODE_IRQ                @ switch to IRQ mode
	LDMDB	\rd!, {r4, sp, lr}
	MSR	SPSR_fsxc, r4            @ restore the current SPSR

	CPS	#MODE_UND                @ switch to Undefined mode
	LDMDB	\rd!, {r4, sp, lr}
	MSR	SPSR_fsxc,r4             @ restore the current SPSR

	CPS	#MODE_ABT                @ switch to Abort mode
	LDMDB	\rd!, {r4, sp, lr}
	MSR	SPSR_fsxc,r4             @ restore the current SPSR

	CPS	#MODE_SYS                @ switch to System mode
	LDMDB	\rd!, {sp, lr}

	LDMDB	\rd!, {r4 - r5}
	MSR	CPSR_fsxc, r4
	MSR	SPSR_fsxc, r5

	CPS	#MODE_SVC                @ switch back to Supervisor mode
	LDMDB	\rd!, {r4 - r12, sp, lr}

	.endm

	.macro	gic_restore, rd

	LDR	r1, =DM_CA9_SCU
	MOVW	r6, #CA9_SCU_ICC
	ADD	r1, r1, r6
	LDMIA	\rd!, {r2 - r4}
	STR	r2, [r1, #0x4]   @ interrupt priority mask
	STR	r3, [r1, #0x8]   @ interrupt binary point (grouping)
	STR	r4, [r1, #0x0]   @ CPU Interface control

	LDR	r1, =DM_CA9_SCU
	MOVW	r6, #CA9_SCU_ICD
	ADD	r1, r1, r6

	LDR	r2, [r1, #0x4]  @ read feature fields
	UBFX	r2, r2, #0, #5      @ extract ITLinesNumber
	ADD	r2, r2, #1
	MOV	r3, r2

loop1_gic:
	LDR	r5, [\rd], #4
	STR	r5, [r1, #0x100]  @ interrupt set-enable
	ADD	r1, r1, #4
	SUBS	r3, r3, #1
	BNE	loop1_gic

	LDR	r1, =DM_CA9_SCU
	MOVW	r6, #CA9_SCU_ICD
	ADD	r1, r1, r6
	LSL	r3, r2, #3

loop2_gic:
	LDM	\rd!, {r4, r5}
	STR	r4, [r1, #0x400]   @ interrupt priority
	STR	r5, [r1, #0x800]  @ interrupt processor targets
	ADD	r1, r1, #4
	SUBS	r3, r3, #1
	BNE	loop2_gic

	LDR	r1, =DM_CA9_SCU
	MOVW	r6, #CA9_SCU_ICD
	ADD	r1, r1, r6
	LSL	r3, r2, #1

loop3_gic:
	LDR	r4, [\rd], #4
	STR	r4, [r1, #0xc00]  @ interrupt configuration
	ADD	r1, r1, #4
	SUBS	r3, r3, #1
	BNE	loop3_gic

	LDR	r1, =DM_CA9_SCU
	MOVW	r6, #CA9_SCU_ICD
	ADD	r1, r1, r6
	LDR	r2, [\rd], #4
	STR	r2, [r1, #0x0]   @ Distributor control

	.endm

	.macro	vfp_restore, rd

	LDR     r2, [r12, #(REG_SC_GEN2)]
	CMP	r2, #1
	BEQ	exit_fpu_restore

	@ Modify CPACR to allow CP10 and CP11 access
	MRC	p15, 0, r2, c1, c0, 2
	ORR	r2, r2, #0x00F00000
	MCR	p15, 0, r2, c1, c0, 2

	@ Enable FPU access to save/restore the rest of registers.
	LDR	r2, =0x40000000
	MSR	FPEXC, r2

	@ Recover FPEXC and FPSCR. These will be restored later.
	LDM	\rd!, {r9, r10}

	@ Restore the VFP-D16 registers.
	VLDM	\rd!, {D0 - D15}
complete_fpu_restore:

	@ Restore configuration registers and enable.
	@ Restore FPSCR _before_ FPEXC since FPEXC could
	@ disable FPU and make setting FPSCR unpredictable.
	MSR	   FPSCR, r10
	MSR	   FPEXC, r9	@ Restore FPEXC after FPSCR

exit_fpu_restore:

	.endm

	.macro	mmu_restore, rd

	@ resume PID register
	LDR	r3, [r12, #(REG_SC_GEN3)]

	@resume control register,enable mmu
	MCR     p15, 0, r3, c13, c0, 0

	@ resume TTB register
	LDMIA	r0!, {r5 - r7}
	MCR	p15, 0, r5, c2, c0, 0      @ TTBR0
	MCR	p15, 0, r6, c2, c0, 1      @ TTBR1
	MCR	p15, 0, r7, c2, c0, 2      @ TTBCR

	LDMIA	r0!, {r4 - r7}
	MCR	p15, 0, r4, c3, c0, 0      @ DACR
	MCR	p15, 0, r5, c7, c4, 0      @ PAR
	MCR	p15, 0, r6, c10, c2, 0     @ PRRR
	MCR	p15, 0, r7, c10, c2, 1     @ NMRR

	LDMIA	r0!, {r3 - r7}
	MCR	p15, 0, r3, c12, c0, 0     @ VBAR
	MCR	p15, 0, r4, c13, c0, 1     @ CONTEXTIDR
	MCR	p15, 0, r5, c13, c0, 2     @ TPIDRURW
	MCR	p15, 0, r6, c13, c0, 3     @ TPIDRURO
	MCR	p15, 0, r7, c13, c0, 4    @ TPIDRPRW

	.endm

	.macro	ddrtrain_restore, rd

	@ resume_ddrtrain
	LDR	r5, =0x80000000
	MOV	r4, #0

resume_loop:
	LDR	r1, [\rd]
	STR	r1, [r5]
	ADD	\rd, \rd, #4
	ADD	r5, r5, #4
	ADD	r4, r4, #4
	CMP	r4, #32
	BNE     resume_loop

	.endm

/*
 *-------------------------------------------------------------------------
 *   Function: hilpm_godpsleep
 *
 *   this function is the low level interface when deep sleep.
 *
 */
ENTRY (hi_pm_sleep)
store_current:
	LDR	r0, hi_pm_ddrbase

	mode_save r0

	@ save mode_save end addr to REG_SC_GEN4 register
	LDR  	r12, =hi_sc_virtbase
	LDR     r12, [r12]
	STR	r0, [r12, #(REG_SC_GEN4)]

store_resume:
	LDR	r5, =(__virt_to_phys(resume_code))
	STR	r5, [r12, #(REG_SC_GEN9)]

	LDR	r4, hi_pm_ddrbase
	ADD  	r0, r4,#0x100

	gic_save r0
	vfp_save r0
	mmu_save r0
	ddrtrain_save r0

disable_mmu:

	LDR	r9, =(__virt_to_phys(enable_mcu))
	BL	v7_flush_kern_cache_all
	BL	invalid_cache

	@ read control register to r1
	MRC	p15, 0, r1, c1, c0, 0
	STR	r1, [r12, #(REG_SC_GEN5)]

	@ write control register to disable the mmu
	MRC	p15, 0, r1, c1, c0, 0
	MOV	r1, #0
	MCR	p15, 0, r1, c1, c0, 0
	BX	r9
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP

enable_mcu:
	LDR	r1, =0x600bf000
	LDR	r2, [r1]
	ORR 	r2, #0x1
	STR	r2, [r1]

	B	.
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP

resume_code:

	@ write domain access to get the domain access right
	LDR	r7, =0xFFFFFFFF
	MCR	p15, 0, r7, c3, c0, 0

	@ write control register to disable the mmu
	MRC	p15, 0, r1, c1, c0, 0
	MOV	r1, #0
	MCR	p15, 0, r1, c1, c0, 0

	LDR	r12, =REG_BASE_SCTL

	LDR	r4, hi_pm_phybase
	ADD  	r0, r4, #0x100

	gic_restore r0
	vfp_restore r0
	mmu_restore r0
	ddrtrain_restore r0

	LDR	r9,  =resume_virt

	@ restore control register,enable mmu
	LDR	r3, [r12, #(REG_SC_GEN5)]
	MCR	p15, 0, r3, c1, c0, 0

	BX	r9

	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP

resume_virt:

	LDR  	r12, =hi_sc_virtbase
	LDR     r12, [r12]
	LDR	r0,  [r12, #(REG_SC_GEN4)]

	mode_restore r0

	@ go back to the call point
	MOV	pc, lr

invalid_cache:

	STMFD sp!, {r0 - r12, lr}

	/*
	 * Invalidate L1 I/D
	 */
	MOV	r0, #0                  @ set up for MCR
	MCR	p15, 0, r0, c8, c7, 0   @ invalidate TLBs
	MCR	p15, 0, r0, c7, c5, 0   @ invalidate icache

	/* Invalidate L1 D-cache */
	/* select L1 data cache */
	MCR     p15, 2, r0, c0, c0, 0
	/* Read Current Cache Size Identification Register */
	MRC     p15, 1, r3, c0, c0, 0
	LDR	r1, =0x1ff
	AND     r3, r1, r3, LSR #13             /* r3 = (numBer of sets -1 */
	MOV     r0, #0
way_loop:
	MOV     r1, #0                          /* r1->set counter */
line_loop:
	MOV     r2, r0, LSL #30

	/* r2->set/way cache-op format */
	ORR     r2, r1, LSL #5

	/* Invalidate line descriBed by r2 */
	MCR     p15, 0, r2, c7, c6, 2

	/* Increment set counter */
	ADD     r1, r1, #1

	/* Check if the last set is reached... */
	CMP     r1, r3

	/* if not, continue the set_loop */
	BLE     line_loop

	/* else, Increment way counter */
	ADD     r0, r0, #1

	/* Check if the last way is reached... */
	CMP     r0, #4

	/* if not, continue the way_loop */
	BLT     way_loop
	MOV 	pc, lr
	LDMFD   sp!, {r0 - r12, pc}

.global hi_pm_ddrbase
hi_pm_ddrbase:
.word hi_pm_ddrbase

.global hi_pm_phybase
hi_pm_phybase:
.word hi_pm_phybase

