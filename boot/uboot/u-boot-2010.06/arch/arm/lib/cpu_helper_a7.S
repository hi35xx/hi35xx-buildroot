#include <asm/memory.h>

        @ Aliases for mode encodings - do not change
        .equ MODE_USR, 0x10
        .equ MODE_FIQ, 0x11
        .equ MODE_IRQ, 0x12
        .equ MODE_SVC, 0x13
        .equ MODE_MON, 0x16  @ A-profile (Security Extensions) only
        .equ MODE_ABT, 0x17
        .equ MODE_UND, 0x1B
        .equ MODE_SYS, 0x1F
        .equ MODE_HYP, 0x1A

        .equ TTBCR_EAE, (1<<31)    @ Are we using LPAE?

        .equ PFR0_THUMB_EE_SUPPORT, (1<<12)

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global restore_performance_monitors
restore_performance_monitors:
        .func
        push    {r4-r6, r8-r10, lr}
                                        @ NOTE: all counters disabled
					@ by PMCR<0> == 0 on reset

        ldr    r8,[r0]                  @ r8 = PMCR
        add    r1,r0,#20                @ r1 now points to saved PMOVSR
        ldr    r9,[r1]                  @ r9 = PMOVSR

        mvn    r2,#0                    @ generate Register of all 1's
        mcr    p15,0,r2,c9,c14,2        @ disable all counter related interrupts
        mcr    p15,0,r2,c9,c12,3        @ clear all overflow flags
        isb

        ubfx   r12,r8,#11,#5            @ extract # of event counters, N (0-31)
        tst    r12, r12
        beq    20f

        add    r1,r0,#32                @ r1 now points to the 1st saved
					@ event counter
	@@ Restore counters
	mov    r6,#0
10:
        mcr    p15,0,r6,c9,c12,5        @ PMon: select CounterN
        isb
        ldm    r1!, {r3,r4}             @ Read saved data
        mcr    p15,0,r3,c9,c13,1        @ PMon: restore Event Type Register
        mcr    p15,0,r4,c9,c13,2        @ PMon: restore Event Counter Register
        add    r6,r6,#1                 @ increment index
        @ cmps   r6,r12
        cmp    r6,r12
        bne    10b

20:
        tst    r9, #0x80000000          @ check for cycle count overflow flag
        beq    40f
        mcr    p15,0,r2,c9,c13,0        @ set Cycle Counter to all 1's
        isb
        mov    r3, #1
        mcr    p15,0,r3,c9,c12,0        @ set the PMCR global enable bit
        mov    r3, #0x80000000
        mcr    p15,0,r3,c9,c12,1        @ enable the Cycle Counter
        isb

30:
        mrc    p15,0,r4,c9,c12,3        @ check cycle count overflow now set
        movs   r4,r4                    @ test bit<31>
        bpl    30b
        mcr    p15,0,r3,c9,c12,2        @ disable the Cycle Counter

40:
	mov    r1, #0
	mcr    p15,0,r1,c9,c12,0        @ clear the PMCR global enable bit
	isb

	@@ Restore left regs but PMCR
	add    r1,r0,#4                 @ r1 now points to the PMSELR
	ldm    r1!,{r3,r4}
	mcr    p15,0,r3,c9,c12,5        @ PMon: Event Counter Selection Reg
	mcr    p15,0,r4,c9,c12,1        @ PMon: Count Enable Set Reg
	ldm    r1!, {r3,r4}
	mcr    p15,0,r4,c9,c13,0        @ PMon: Cycle Counter Register
	ldm    r1!,{r3,r4}
	mcr    p15,0,r3,c9,c14,2        @ PMon: Interrupt Enable Clear Reg
	mcr    p15,0,r4,c9,c14,1        @ PMon: Interrupt Enable Set Reg
	ldr    r3,[r1]
	isb
	ldr    r0,[r0]
	mcr    p15,0,r0,c9,c12,0        @ restore the PM Control Register
	isb

	pop    {r4-r6, r8-r10, pc}
.endfunc


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	.global restore_banked_registers
restore_banked_registers:
	.func
        mrs    r2, CPSR         @ save current mode
        and    r3, r2, #0x1f    @ If we are in HYP mode then use the virt.
        cmp    r3, #MODE_HYP    @ instructions to restore the banked registers
        beq    rest_in_hyp      @ without changing the mode

        cps    #MODE_SYS        @ switch to System mode
        ldr    sp,[r0],#4       @ restore the User SP
        ldr    lr,[r0],#4       @ restore the User LR
        cps    #MODE_ABT        @ switch to Abort mode
        ldr    sp,[r0],#4       @ restore the current SP
        ldm    r0!,{r3,lr}      @ restore the current LR
        msr    SPSR_fsxc,r3     @ restore the current SPSR
        cps    #MODE_UND        @ switch to Undefined mode
        ldr    sp,[r0],#4       @ restore the current SP
        ldm    r0!,{r3,lr}      @ restore the current LR
        msr    SPSR_fsxc,r3     @ restore the current SPSR
        cps    #MODE_IRQ        @ switch to IRQ mode
        ldr    sp,[r0],#4       @ restore the current SP
        ldm    r0!,{r3,lr}      @ restore the current LR
        msr    SPSR_fsxc,r3     @ restore the current SPSR
        cps    #MODE_FIQ        @ switch to FIQ mode
        ldr    sp,[r0],#4       @ restore the current SP
        ldm    r0!,{r8-r12,lr}  @ restore the current r8-r12,LR
        msr    SPSR_fsxc,r4     @ restore the current SPSR
        msr    CPSR_cxsf, r2    @ switch back to original mode

        LDR     SP, [r0], #4                    @ restore the current SP
        LDM     r0!, {r3, r4-r12, LR}           @ restore the current r4-r12,LR
        MSR     SPSR_fsxc, r3                   @ restore the current SPSR
        dsb
0:
        bx     lr

rest_in_hyp:
        ldm    r0!, {r1}
        @ msr  SP_usr, r1       @ rewrite
        cps    #MODE_SYS        @ switch to System mode
        ldr    sp,[r1],#4       @ restore the User SP

        ldm    r0!, {r1-r3}
        @ msr  SP_und, r1       @ rewrite
        cps    #MODE_UND        @ switch to Undefined mode
        ldr    sp,[r1],#4       @ restore the User SP
        @ msr  SPSR_und, r2     @ rewrite
        cps    #MODE_UND        @ switch to Undefined mode
        msr    SPSR_fsxc,r2     @ restore the current SPSR
        @ msr  LR_und, r3       @ rewrite
        cps    #MODE_UND        @ switch to Undefined mode
        ldr    lr,[r3],#4       @ restore the User LR

        ldm    r0!, {r1-r3}
        @ msr  SP_abt, r1       @ rewrite
        cps    #MODE_ABT        @ switch to Abort mode
        ldr    sp,[r1],#4       @ restore the User SP
        @ msr  SPSR_abt, r2     @ rewrite
        cps    #MODE_ABT        @ switch to Abort mode
        msr    SPSR_fsxc,r2     @ restore the current SPSR
        @ msr  LR_abt, r3       @ rewrite
        cps    #MODE_ABT        @ switch to Abort mode
        ldr    lr,[r3],#4       @ restore the User LR

        ldm    r0!, {r1-r3}
        @ msr  SP_svc, r1       @ rewrite
        cps    #MODE_SVC        @ switch to SVC mode
        ldr    sp,[r1],#4       @ restore the User SP
        @ msr  SPSR_svc, r2     @ rewrite
        cps    #MODE_SVC        @ switch to SVC mode
        msr    SPSR_fsxc,r2     @ restore the current SPSR
        @ msr  LR_svc, r3       @ rewrite
        cps    #MODE_SVC        @ switch to SVC mode
        ldr    lr,[r3],#4       @ restore the User LR

        ldm    r0!, {r1-r3}
        @ msr  SP_irq, r1       @ rewrite
        cps    #MODE_IRQ        @ switch to IRQ mode
        ldr    sp,[r1],#4       @ restore the User SP
        @ msr  SPSR_irq, r2     @ rewrite
        cps    #MODE_IRQ        @ switch to IRQ mode
        msr    SPSR_fsxc,r2     @ restore the current SPSR
        @ msr  LR_irq, r3       @ rewrite
        cps    #MODE_IRQ        @ switch to IRQ mode
        ldr    lr,[r3],#4       @ restore the User LR

        ldm    r0!, {r1-r3}
        @ msr  SP_fiq, r1       @ rewrite
        cps    #MODE_FIQ        @ switch to FIQ mode
        ldr    sp,[r1],#4       @ restore the User SP
        @ msr  SPSR_fiq, r2     @ rewrite
        cps    #MODE_FIQ        @ switch to FIQ mode
        msr    SPSR_fsxc,r2     @ restore the current SPSR
        @ msr  LR_fiq, r3       @ rewrite
        cps    #MODE_FIQ        @ switch to FIQ mode
        ldr    lr,[r3],#4       @ restore the User LR

        @ ldm  r0!, {r1-r3}
        @ msr  r8_fiq, r1       @ rewrite
        @ msr  r9_fiq, r2       @ rewrite
        @ msr  r10_fiq, r3      @ rewrite
        @ ldm  r0!, {r1-r2}
        @ msr  r11_fiq, r1      @ rewrite
        @ msr  r12_fiq, r2      @ rewrite
        cps    #MODE_FIQ        @ switch to FIQ mode
        ldm    r0!,{r8-r12}     @ restore the current r8-r12

        bx     lr
        .endfunc


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global restore_cp15
restore_cp15:
        .func
        @ CSSELR Cache Size Selection Register
        ldr    r3,[r0], #4
        mcr    p15,2,r3,c0,c0,0

        bx     lr
        .endfunc


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Function called with two arguments:
@    r0 contains address to read control registers
@    r1 is non-zero if we are Secure

        .global restore_control_registers
restore_control_registers:
        .func
        cmp    r1, #0              @ Are we Secure?
        ldm    r0!, {r2-r3, r12}
        mcr    p15,0,r2,c1,c0,1    @ ACTLR - Auxiliary Control Register
	@mcr    p15,0,r3,c1,c0,0   @ SCTLR - System Control Register
        mcr    p15,0,r12,c1,c0,2   @ CPACR -Coprocessor Access Control Register

        ldmne    r0!, {r1-r3,r12}
        mcrne    p15,0,r1,c12,c0,1   @ MVBAR - Monitor Vector Base
					@Address Register
        mcrne    p15,0,r2,c1,c1,0    @ Secure Configuration Register
        mcrne    p15,0,r3,c1,c1,1    @ Secure Debug Enable Register
        mcrne    p15,0,r12,c1,c1,2   @ Non-Secure Access Control Register

        ldm    r0!, {r1-r3,r12}
        mcr    p15,0,r1,c13,c0,1    @ CONTEXTIDR
        mcr    p15,0,r2,c13,c0,2    @ TPIDRURW
        mcr    p15,0,r3,c13,c0,3    @ TPIDRURO
        mcr    p15,0,r12,c13,c0,4   @ TPIDRPRW

        @ The next two registers are only present if ThumbEE is implemented
        mrc    p15, 0, r1, c0, c1, 0    @ Read ID_PFR0
        tst    r1, #PFR0_THUMB_EE_SUPPORT
        ldmne    r0!, {r1,r2}
        mcrne    p14,6,r1,c0,c0,0    @ TEECR
        mcrne    p14,6,r2,c1,c0,0    @ TEEHBR

        ldm    r0!, {r1, r2}
        mcr    p14,7,r1,c1,c0,0    @ JOSCR
        mcr    p14,7,r2,c2,c0,0    @ JMCR
        isb
        bx    lr
        .endfunc


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global restore_mmu
restore_mmu:
        .func
        push    {r4, r5, r6, r7}
        ldm    r0!, {r4-r7}
        mcr    p15,0,r4,c12,c0,0    @ VBAR
        mcr    p15,0,r5,c2,c0,2     @ TTBCR

        tst    r5, #TTBCR_EAE        @ Are we using LPAE?

        @ restore 32 or 64 bit TTBRs
        mcreq    p15,0,r6,c2,c0,0    @ 32 bit TTBR0
        mcreq    p15,0,r7,c2,c0,1    @ 32 bit TTBR1
        mcrrne    p15,0,r6,r7,c2     @ 64-bit TTBR0
        ldmne    r0!, {r6-r7}
        mcrrne    p15,1,r6,r7,c2     @ 64-bit TTBR1

        ldm    r0!, {r4-r7}
        mcr    p15,0,r4,c3,c0,0     @ DACR
        mcr    p15,0,r5,c7,c4,0     @ PAR
        mcr    p15,0,r6,c10,c2,0    @ PRRR
        mcr    p15,0,r7,c10,c2,1    @ NMRR

        pop    {r4, r5, r6, r7}
        bx    lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global restore_mpu
restore_mpu:
        .func
        mrc    p15, 0, r1, c0, c0, 4     @ Read MPUIR
        and    r1, r1, #0xff00
        lsr    r1, r1, #8                @ Extract number of MPU regions

        @ Loop over the number of regions
10:
        cmp    r1, #0
        beq    20f
        ldm    r0!, {r2, r3, r12}
        sub    r1, r1, #1
        mcr    p15, 0, r1, c6, c2, 0    @ Write RGNR
        mcr    p15, 0, r2, c6, c1, 0    @ Write MPU Region Base Address
					@ Register
        mcr    p15, 0, r3, c6, c1, 2    @ Write Data MPU Region Size and
					@ Enable Register
        mcr    p15, 0, r12, c6, c1, 4   @ Write Region access control Register
        b      10b

20:
        bx     lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global restore_generic_timer
restore_generic_timer:
        .func
        ldm    r0!, {r2, r3, r12}
        mcr    p15,0,r3,c14,c2,0        @ write CNTP_TVAL
        mcr    p15,0,r12,c14,c1,0       @ write CNTKCTL
        mcr    p15,0,r2,c14,c2,1        @ write CNTP_CTL
        cmp    r1, #0
        ldrne  r1, [r0], #4
        mcrne  p15,4,r1,c14,c1,0        @ write CNTHCTL
        bx     lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



        .global restore_fault_status
restore_fault_status:
        .func
        ldm    r0!, {r1,r2,r3,r12}
        mcr    p15,0,r1,c6,c0,0    @ write DFAR
        mcr    p15,0,r2,c6,c0,2    @ write IFAR
        mcr    p15,0,r3,c5,c0,0    @ write DFSR
        mcr    p15,0,r12,c5,c0,1   @ write IFSR
        ldm    r0!, {r1,r2}
        mcr    p15,0,r1,c5,c1,0    @ write ADFSR
        mcr    p15,0,r2,c5,c1,1    @ write AIFSR
        bx     lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .global invalidate_unified_tlb_inner_shareable
invalidate_unified_tlb_inner_shareable:
        .func
        mov    r0, #0
        mcr    p15, 0, r0, c8, c3, 0
        bx     lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
