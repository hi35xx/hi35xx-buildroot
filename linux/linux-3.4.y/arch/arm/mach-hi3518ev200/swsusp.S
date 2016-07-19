/*
 * Hibernation support specific for ARM
 *
 * Copyright (C) 2010 Nokia Corporation
 * Copyright (C) 2010 Texas Instruments, Inc.
 * Copyright (C) 2006 Rafael J. Wysocki <rjw at sisk.pl>
 *
 * Contact: Hiroshi DOYU <Hiroshi.DOYU at nokia.com>
 *
 * License terms: GNU General Public License (GPL) version 2
 */

#include <linux/linkage.h>
.text

#define LOCAL_WORD(x)         \
    .data                   ; \
    .p2align 2              ; \
    .type   x, #object      ; \
    .size   x, 4            ; \
x:                          ; \
    .long 1

#define WORD_ADDR(x)          \
    .align 2                ; \
.L##x:                      ; \
    .word x

/*user*/
LOCAL_WORD(saved_context_r0)
LOCAL_WORD(saved_context_r1)
LOCAL_WORD(saved_context_r2)
LOCAL_WORD(saved_context_r3)
LOCAL_WORD(saved_context_r4)
LOCAL_WORD(saved_context_r5)
LOCAL_WORD(saved_context_r6)
LOCAL_WORD(saved_context_r7)
LOCAL_WORD(saved_context_r8)
LOCAL_WORD(saved_context_r9)
LOCAL_WORD(saved_context_r10)
LOCAL_WORD(saved_context_r11)
LOCAL_WORD(saved_context_r12)
LOCAL_WORD(saved_context_r13)
LOCAL_WORD(saved_context_r14)
LOCAL_WORD(saved_cpsr)

LOCAL_WORD(saved_context_r8_fiq)
LOCAL_WORD(saved_context_r9_fiq)
LOCAL_WORD(saved_context_r10_fiq)
LOCAL_WORD(saved_context_r11_fiq)
LOCAL_WORD(saved_context_r12_fiq)
LOCAL_WORD(saved_context_r13_fiq)
LOCAL_WORD(saved_context_r14_fiq)
LOCAL_WORD(saved_spsr_fiq)

LOCAL_WORD(saved_context_r13_irq)
LOCAL_WORD(saved_context_r14_irq)
LOCAL_WORD(saved_spsr_irq)

LOCAL_WORD(saved_context_r13_svc)
LOCAL_WORD(saved_context_r14_svc)
LOCAL_WORD(saved_spsr_svc)

LOCAL_WORD(saved_context_r13_abt)
LOCAL_WORD(saved_context_r14_abt)
LOCAL_WORD(saved_spsr_abt)

LOCAL_WORD(saved_context_r13_und)
LOCAL_WORD(saved_context_r14_und)
LOCAL_WORD(saved_spsr_und)

#define CHANGE_MODE(x)        \
    mov     r1, r0          ; \
    bic     r1, r1, #0x1f   ; \
    orr     r1, r1, #0x##x  ; \
    msr     cpsr_c, r1

ENTRY(swsusp_arch_suspend)
   /*
     * Save current program status register
     */
    ldr     r3, .Lsaved_cpsr
    mrs     r0, cpsr
    str     r0, [r3]

    CHANGE_MODE(1f)  /* Change to system(user) mode*/
    ldr     r3, .Lsaved_context_r0
    stmia   r3, {r0-r14}

    CHANGE_MODE(11) /* change to fiq mode */
    /* save nonvolatile int register */
    ldr r3, .Lsaved_context_r8_fiq
    stmia   r3, {r8-r14}
    /* save spsr_fiq register */
    ldr r3, .Lsaved_spsr_fiq
    mrs r1, spsr
    str r1, [r3]

    CHANGE_MODE(12) /* change to irq mode */
    /* save nonvolatile int register */
    ldr r3, .Lsaved_context_r13_irq
    stmia   r3, {r13-r14}
    /* save spsr_irq register */
    ldr r3, .Lsaved_spsr_irq
    mrs r1, spsr
    str r1, [r3]

    CHANGE_MODE(13) /* change to svc mode */
    /* save nonvolatile int register */
    ldr r3, .Lsaved_context_r13_svc
    stmia   r3, {r13-r14}
    /* save spsr_svc register */
    ldr r3, .Lsaved_spsr_svc
    mrs r1, spsr
    str r1, [r3]

    CHANGE_MODE(17) /* change to abt mode */
    /* save nonvolatile int register */
    ldr r3, .Lsaved_context_r13_abt
    stmia   r3, {r13-r14}
    /* save spsr_abt register */
    ldr r3, .Lsaved_spsr_abt
    mrs r1, spsr
    str r1, [r3]

    CHANGE_MODE(1b) /* change to und mode */
    /* save nonvolatile int register */
    ldr r3, .Lsaved_context_r13_und
    stmia   r3, {r13-r14}
    /* save spsr_und register */
    ldr r3, .Lsaved_spsr_und
    mrs r1, spsr
    str r1, [r3]

    /*
     * Go back to original SVC mode
     */
    msr     cpsr_c, r0

    bl      swsusp_save

#ifdef CONFIG_DEBUG_LL
    adr r0, str_marker_1
    bl  printascii
    mov r0, #0
#endif

    /*
     * Restore return address
     */
    ldr     r3, .Lsaved_context_r14_svc
    ldr     lr, [r3]
    mov     pc, lr

    WORD_ADDR(saved_context_r0)
    WORD_ADDR(saved_cpsr)

    WORD_ADDR(saved_context_r8_fiq)
    WORD_ADDR(saved_spsr_fiq)

    WORD_ADDR(saved_context_r13_irq)
    WORD_ADDR(saved_spsr_irq)

    WORD_ADDR(saved_context_r13_svc)
    WORD_ADDR(saved_context_r14_svc)
    WORD_ADDR(saved_spsr_svc)

    WORD_ADDR(saved_context_r13_abt)
    WORD_ADDR(saved_spsr_abt)

    WORD_ADDR(saved_context_r13_und)
    WORD_ADDR(saved_spsr_und)

ENDPROC(swsusp_arch_suspend)

ENTRY(swsusp_arch_resume)

#ifdef CONFIG_DEBUG_LL
    adr r0, str_marker_1
    bl  printascii
    mov r0, #0
#endif

#ifdef CONFIG_HISI_SNAPSHOT_BOOT
    /* FIXME: need to check the cp15, c0 #4,
     *     "Instruction Set Attributes Register 4"
     */
    #dsb
    #isb

#else /* CONFIG_HISI_SNAPSHOT_BOOT */

    /*
     * Restore_pblist is the starting point for loaded pages
     */
    ldr     r0, .Lrestore_pblist
    ldr     r6, [r0]

    .Lcopy_loop:
    ldr     r4, [r6]     /* src IOW present address */
    ldr     r5, [r6, #4] /* dst IOW original address*/
    mov     r9, #1024    /* No. of entries in one page(4 bytes each entry)*/

    .Lcopy_one_page:
    /*
     * This loop could be optimized by using stm and ldm.
     */
    ldr     r8, [r4], #4
    str     r8, [r5], #4
    subs    r9, r9, #1
    bne     .Lcopy_one_page

    /* The last field of struct pbe is a pointer to the next pbe struct */
    ldr     r6, [r6, #8]
    cmp     r6, #0
    bne     .Lcopy_loop
#endif /* CONFIG_HISI_SNAPSHOT_BOOT */

    /*
     * Restore SVC context
     */
    ldr     r3, .Lsaved_context_r13_svc
    ldmia   r3, {r13-r14}
    ldr     r3, .Lsaved_spsr_svc
    ldr     r1, [r3]
    msr     spsr_cxsf, r1

    mrs     r0, cpsr    /* Save current mode into r0 */

    CHANGE_MODE(11) /* change to fiq mode */
    /* restore nonvolatile int register */
    ldr r3, .Lsaved_context_r8_fiq
    ldmia   r3, {r8-r14}
    /* restore spsr_fiq register */
    ldr r3, .Lsaved_spsr_fiq
    ldr r1, [r3]
    msr spsr_cxsf, r1

    CHANGE_MODE(12) /* change to irq mode */
    /* restore nonvolatile int register */
    ldr r3, .Lsaved_context_r13_irq
    ldmia   r3, {r13-r14}
    /* restore spsr_irq register */
    ldr r3, .Lsaved_spsr_irq
    ldr r1, [r3]
    msr spsr_cxsf, r1

    CHANGE_MODE(17) /* change to abt mode */
    /* restore nonvolatile int register */
    ldr r3, .Lsaved_context_r13_abt
    ldmia   r3, {r13-r14}
    /* restore spsr_abt register */
    ldr r3, .Lsaved_spsr_abt
    ldr r1, [r3]
    msr spsr_cxsf, r1

    CHANGE_MODE(1b) /* change to und mode */
    /* restore nonvolatile int register */
    ldr r3, .Lsaved_context_r13_und
    ldmia   r3, {r13-r14}
    /* restore spsr_und register */
    ldr r3, .Lsaved_spsr_und
    ldr r1, [r3]
    msr spsr_cxsf, r1

    CHANGE_MODE(1f) /* Change to system(user) mode */

    /* in_suspend = 0 */
    ldr     r3,.Lsaved_in_suspend
    mov     r1,#0
    str     r1,[r3]

    /*
     * Restore User context
     */
    ldr     r3, .Lsaved_context_r0
    ldmia   r3, {r0-r14}
    ldr     r3, .Lsaved_cpsr
    ldr     r1, [r3]
    msr     cpsr_cxsf, r1

    /*
     * Flush TLB (Invalidate unified TLB unlocked entries)
     */
    mov     r1, #0
    mcr     p15, 0, r1, c8, c7, 0

#ifdef CONFIG_DEBUG_LL
    adr r0, str_marker_1
    bl  printascii
    mov r0, #0
#endif

    /* Set the return value */
    mov r0, #0

    /* Restore return address */
    ldr     r3, .Lsaved_context_r14_svc
    ldr     lr, [r3]
    mov     pc, lr
ENDPROC(swsusp_arch_resume)

str_marker_1: .asciz "swsusp_arch_mark!!!\n\0"

    .align  4
.Lsaved_in_suspend:         .long   in_suspend


