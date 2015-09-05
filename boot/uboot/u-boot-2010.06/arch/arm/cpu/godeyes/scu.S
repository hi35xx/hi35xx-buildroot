
.globl scu_invalidate

scu_invalidate:
    mvn    r0, #0
    ldr    r1, = REG_BASE_A9_PERI
    str    r0, [r1, #0xc]              // Invalidate all SCU TAG RAMs
    BX     lr
.globl scu_enable
scu_enable:
    //allow non secure accesses to scu & timers.
    LDR    r1, =REG_BASE_A9_PERI
    LDR    r0, [r1, #0x54]             // Read SCU Secure Access Control Register
    ORR    r0, r0, #0xff               // Enable access in NS by all cpus
    STR    r0, [r1, #0x54]             // Update SCU Secure Access Control Register
    
	LDR    r0, [r1, #0x0]              // Read SCU Control Register
    ORR    r0, r0, #0x1                // Set Enable SCU bit
    STR    r0, [r1, #0x0]              // Update SCU Control Register
    BX     lr
.globl scu_disable
scu_disable:
    LDR    r1, =REG_BASE_A9_PERI
    LDR    r0, [r1, #0x0]              // Read SCU Control Register
    BIC    r0, r0, #0x1                // Clear Enable SCU bit
    STR    r0, [r1, #0x0]              // Update SCU Control Register
    BX     lr
.globl scu_enabled
scu_enabled:
    LDR    r1, =REG_BASE_A9_PERI
    LDR    r0, [r1, #0x0]              // Get SCU state
    AND    r0, r0, #0x01
    BX     lr

