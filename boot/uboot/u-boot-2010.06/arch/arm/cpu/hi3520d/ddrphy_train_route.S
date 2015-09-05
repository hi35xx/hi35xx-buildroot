/*
 * 1. R0 should be reserved
 * 2. SP used space: 0x200 bytes
 * 3. this function use physical address.
 * 4. this function will destory ddr data 0x90101000 - 0x90402800
 * 5. return: 0 -> success, 1 -> fail
 */

_gate_pad:
	/* move this train between 2k */
	.fill 200,1,0
ddrphy_train_route:
	stmfd sp!, {r1-r12,r14}

ddrphy_train_start:
	/* used space */
	sub   r8, sp, #0x200

	/* phy training bypass */
	ldr   r0, =DDRC_PHY_REG1
	mov   r1, #0x2
	str   r1, [r0,#0x8]         // calibrarion bypass

train_low_start:
	mov   r1, #0              // dll_sel0
	mov   r2, #0              // oph_sel0
	mov   r3, #1              // cyc_sel0
	mov   r4, #2              // low_byte_count
	str   r1, [r8,#0x0]
	str   r2, [r8,#0x4]
	str   r3, [r8,#0x8]
	str   r1, [r8,#0x10]
	str   r1, [r8,#0x14]
	str   r1, [r8,#0x18]

train_low_loop:
	bl    ddrphy_train_data
	ldr   r5,  =0xd22d
	ldr   r6,  =0xffff
	and   r9, r9, r6
	cmp   r9, r5
	bne   train_low_adj
	ldr   r5,  =0x5aa5
	ldr   r6,  =0xffff
	and   r10, r10, r6
	cmp   r10, r5
	bne   train_low_adj
	ldr   r5,  =0x7887
	ldr   r6,  =0xffff
	and   r11, r11, r6
	cmp   r11, r5
	bne   train_low_adj
	ldr   r5,  =0xc33c
	ldr   r6,  =0xffff
	and   r12, r12, r6
	cmp   r12, r5
	bne   train_low_adj
	lsl   r5, r4, #4
	str   r1, [r8,r5]
	add   r5, r5, #0x4
	str   r2, [r8,r5]
	add   r5, r5, #0x4
	str   r3, [r8,r5]
	add   r4, r4, #1
	bl    train_low_step
	b     train_low_loop

train_low_adj:
	cmp   r4, #5
	bhi   train_low_finish
	bl    train_low_step
	b     train_low_loop

train_low_step:
	ldr   r5, [r0, #0x204]
	and   r5, r5, #0x7
	cmp   r1, r5
	beq   oph_sel0_add
	add   r1, r1, #1
	str   r1, [r8,#0x0]  // dll_sel0
	b     sel0_add_back

oph_sel0_add:
	mov   r1, #0
	str   r1, [r8,#0x0]  // dll_sel0
	cmp   r2, #1
	beq   cyc_sel0_add
	add   r2, r2, #1
	str   r2, [r8,#0x4]  // oph_sel0
	b     sel0_add_back

cyc_sel0_add:
	mov   r2, #0
	str   r2, [r8,#0x4]  // oph_sel0
	cmp   r3, #7
	beq   cyc_sel0_full
	add   r3, r3, #1
	str   r3, [r8,#0x8]  // cyc_sel0
	b     sel0_add_back

cyc_sel0_full:
	cmp   r4, #2
	moveq r5, #0
	beq   train_low_result
	cmp   r4, #6
	movcc r5, #2
	bcc   train_low_result
	mov   r3, #0
	str   r3, [r8,#0x8]  // cyc_sel0

sel0_add_back:
	bx    lr

train_low_finish:
	add   r5, r4, #1
	lsr   r5, r5, #1

train_low_result:
	lsl   r5, r5, #4
	ldr   r6, [r8, r5]
	str   r6, [r8, #0x0]
	add   r5, r5, #4
	ldr   r6, [r8, r5]
	str   r6, [r8, #0x4]
	add   r5, r5, #4
	ldr   r6, [r8, r5]
	str   r6, [r8, #0x8]

train_high_start:
	mov   r1, #0              // dll_sel1
	mov   r2, #0              // oph_sel1
	mov   r3, #1              // cyc_sel1
	mov   r4, #2              // high_byte_count
	str   r1, [r8,#0x10]
	str   r2, [r8,#0x14]
	str   r3, [r8,#0x18]

train_high_loop:
	bl    ddrphy_train_data
	ldr   r7,  =0xe11e
	lsr   r9, r9, #16
	cmp   r9, r7
	bne   train_high_adj
	ldr   r7,  =0x6996
	lsr   r10, r10, #16
	cmp   r10, r7
	bne   train_high_adj
	ldr   r7,  =0xa55a
	lsr   r11, r11, #16
	cmp   r11, r7
	bne   train_high_adj
	ldr   r7,  =0xb44b
	lsr   r12, r12, #16
	cmp   r12, r7
	bne   train_high_adj
	lsl   r5, r4, #4
	str   r1, [r8,r5]
	add   r5, r5, #0x4
	str   r2, [r8,r5]
	add   r5, r5, #0x4
	str   r3, [r8,r5]
	add   r4, r4, #1
	bl    train_high_step
	b     train_high_loop

train_high_adj:
	cmp   r4, #5
	bhi   train_high_finish
	bl    train_high_step
	b     train_high_loop

train_high_step:
	ldr   r5, [r0, #0x204]
	and   r5, r5, #0x7
	cmp   r1, r5
	beq   oph_sel1_add
	add   r1, r1, #1
	str   r1, [r8,#0x10]  // dll_sel1
	b     sel1_add_back

oph_sel1_add:
	mov   r1, #0
	str   r1, [r8,#0x10]  // dll_sel1
	cmp   r2, #1
	beq   cyc_sel1_add
	add   r2, r2, #1
	str   r2, [r8,#0x14]  // oph_sel1
	b     sel1_add_back

cyc_sel1_add:
	mov   r2, #0
	str   r2, [r8,#0x14]  // oph_sel1
	cmp   r3, #7
	beq   cyc_sel1_full
	add   r3, r3, #1
	str   r3, [r8,#0x18]  // cyc_sel1
	b     sel1_add_back

cyc_sel1_full:
	cmp   r4, #2
	moveq r5, #0
	beq   train_high_result
	cmp   r4, #6
	movcc r5, #2
	bcc   train_high_result
	mov   r3, #0
	str   r3, [r8,#0x18]  // cyc_sel1

sel1_add_back:
	bx    lr

train_high_finish:
	add   r5, r4, #1
	lsr   r5, r5, #1

train_high_result:
	lsl   r5, r5, #4
	ldr   r6, [r8, r5]
	str   r6, [r8, #0x10]
	add   r5, r5, #4
	ldr   r6, [r8, r5]
	str   r6, [r8, #0x14]
	add   r5, r5, #4
	ldr   r6, [r8, r5]
	str   r6, [r8, #0x18]
	bl    ddrphy_train_data
	ldr   r7, =0xe11ed22d
	cmp   r7, r9
	bne   ddrphy_train_error
	ldr   r7, =0x69965aa5
	cmp   r7, r10
	bne   ddrphy_train_error
	ldr   r7, =0xa55a7887
	cmp   r7, r11
	bne   ddrphy_train_error
	ldr   r7, =0xb44bc33c
	cmp   r7, r12
	bne   ddrphy_train_error

ddrphy_train_return:
	mov   r0, #0
	ldmfd sp!, {r1-r12,pc}       // return

ddrphy_train_data:

	ldr   r7, [r8,#0x8]
	ldr   r9, [r8,#0x18]
	add   r10, r7, r9, lsl #3    // {cyc_sel1[2:0],cyc_sel0[2:0]}
	str   r10, [r0,#0x3c]

	ldr   r7, [r8,#0x4]
	ldr   r9, [r8,#0x14]
	orr   r7, r7, #4
	orr   r9, r9, #4
	add   r10, r7, r9, lsl #3    // {1, oph_sel1[4:3], 1, oph_sel0[4:3]}
	str   r10, [r0,#0x1c0]

	ldr   r7, [r8,#0x0]
	lsl   r7, r7, #3
	ldr   r9, [r0,#0xe0]
	and   r9, r9, #7
	add   r9, r9, r7
	str   r9, [r0,#0xe0]         // {dll_sel0[7:5],3'Bxxx}

	ldr   r7, [r8,#0x10]
	lsl   r7, r7, #3
	ldr   r9, [r0,#0x120]
	and   r9, r9, #7
	add   r9, r9, r7
	str   r9, [r0,#0x120]        // {dll_sel1[7:5],3'Bxxx}

	ldr     r7,  =0X90101000
	ldr     r9,  =0xe11ed22d
	ldr     r10, =0x69965aa5
	ldr     r11, =0xa55a7887
	ldr     r12, =0xb44bc33c
	stmia   r7, {r9-r12}
	ldmia   r7, {r9-r12}
	ldr     r7,  =0X90201800
	stmia   r7, {r9-r12}
	ldmia   r7, {r9-r12}
	ldr     r7,  =0X90302000
	stmia   r7, {r9-r12}
	ldmia   r7, {r9-r12}
	ldr     r7,  =0X90402800
	stmia   r7, {r9-r12}
	ldmia   r7, {r9-r12}

	bx      lr

ddrphy_train_error:
	mov   r0, #1
	ldmfd sp!, {r1-r12,pc}       // return

