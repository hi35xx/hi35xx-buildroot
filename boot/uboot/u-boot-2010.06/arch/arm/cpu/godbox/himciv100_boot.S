	.arch armv7-a
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 4
	.eabi_attribute 18, 4
	.file	"himciv100_boot.c"
	.text
	.align	2
	.global	emmc_init
	.type	emmc_init, %function
emmc_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r2, .L65
	ldr	r3, .L65+4
	ldr	r0, [r2, #248]
	ldr	r1, .L65+8
	and	r3, r0, r3
	orr	r3, r3, #65536
	orr	r3, r3, #768
	str	r3, [r2, #248]
	orr	r3, r3, #1
	str	r3, [r2, #248]
	b	.L2
.L3:
@ 73 "himciv100_boot.c" 1
	nop
@ 0 "" 2
.L2:
	subs	r1, r1, #1
	bcs	.L3
	ldr	r2, .L65+12
	bic	r3, r3, #1
	ldr	r0, .L65
	ldr	r1, .L65+16
	str	r3, [r0, #248]
	ldr	r3, [r2, #0]
	orr	r3, r3, #7
	str	r3, [r2, #0]
	mov	r3, #0
	str	r3, [r2, #4]
	b	.L4
.L5:
@ 73 "himciv100_boot.c" 1
	nop
@ 0 "" 2
.L4:
	subs	r1, r1, #1
	bcs	.L5
	ldr	r3, .L65+12
	add	r1, r1, #2
	ldr	r2, .L65+20
	str	r1, [r3, #4]
	b	.L6
.L7:
@ 73 "himciv100_boot.c" 1
	nop
@ 0 "" 2
.L6:
	subs	r2, r2, #1
	bcs	.L7
	ldr	r3, .L65+12
	mov	r0, #0
	ldr	ip, .L65+24
	ldr	r1, .L65+28
	mov	r4, r3
	str	ip, [r3, #68]
	ldr	ip, [r3, #36]
	and	r1, ip, r1
	str	r1, [r3, #36]
	ldr	r1, [r3, #0]
	bic	r1, r1, #33554432
	bic	r1, r1, #16
	str	r1, [r3, #0]
	str	r2, [r3, #20]
	ldr	r2, .L65+32
	str	r2, [r3, #76]
	ldr	r2, .L65+36
	str	r0, [r3, #24]
	str	r0, [r3, #16]
	str	r2, [r3, #44]
.L8:
	ldr	r2, [r4, #44]
	ldr	r3, .L65+12
	cmp	r2, #0
	blt	.L8
	mov	r2, #63
	str	r2, [r3, #8]
	ldr	r2, .L65+36
	mov	r1, r3
	str	r2, [r3, #44]
.L9:
	ldr	r2, [r1, #44]
	ldr	r3, .L65+12
	cmp	r2, #0
	blt	.L9
	mov	r2, #1
	str	r2, [r3, #16]
	ldr	r2, .L65+36
	mov	r1, r3
	str	r2, [r3, #44]
.L10:
	ldr	r3, [r1, #44]
	cmp	r3, #0
	blt	.L10
	ldr	r3, .L65+8
	b	.L11
.L12:
@ 73 "himciv100_boot.c" 1
	nop
@ 0 "" 2
.L11:
	subs	r3, r3, #1
	bcs	.L12
	ldr	r3, .L65+12
	ldr	r2, .L65+24
	mov	r1, r3
	str	r2, [r3, #68]
	mov	r2, #0
	str	r2, [r3, #40]
	ldr	r2, .L65+40
	str	r2, [r3, #44]
.L13:
	ldr	r3, [r1, #44]
	cmp	r3, #0
	blt	.L13
	ldr	r3, .L65+12
.L43:
	ldr	r2, [r3, #68]
	tst	r2, #4
	beq	.L43
	ldr	r3, .L65+44
	b	.L15
.L16:
@ 73 "himciv100_boot.c" 1
	nop
@ 0 "" 2
.L15:
	subs	r3, r3, #1
	bcs	.L16
	ldr	r3, .L65+12
	ldr	r4, .L65+24
	ldr	ip, .L65+48
	mov	r0, r3
	ldr	r1, .L65+52
.L41:
	str	r4, [r3, #68]
	str	ip, [r3, #40]
	str	r1, [r3, #44]
.L17:
	ldr	r2, [r3, #44]
	cmp	r2, #0
	blt	.L17
.L42:
	ldr	r2, [r3, #68]
	tst	r2, #4
	beq	.L42
	ldr	r5, [r0, #48]
	ldr	r2, .L65+8
	b	.L19
.L20:
@ 73 "himciv100_boot.c" 1
	nop
@ 0 "" 2
.L19:
	subs	r2, r2, #1
	bcs	.L20
	cmp	r5, #0
	bge	.L41
	ldr	r3, .L65+12
	ldr	r2, .L65+24
	mov	r1, r3
	str	r2, [r3, #68]
	mov	r2, #0
	str	r2, [r3, #40]
	ldr	r2, .L65+56
	str	r2, [r3, #44]
.L22:
	ldr	r3, [r1, #44]
	cmp	r3, #0
	blt	.L22
	ldr	r2, .L65+12
.L40:
	ldr	r1, [r2, #68]
	ldr	r3, .L65+12
	tst	r1, #4
	beq	.L40
	ldr	r2, .L65+24
	mov	r1, r3
	str	r2, [r3, #68]
	mov	r2, #0
	str	r2, [r3, #40]
	ldr	r2, .L65+60
	str	r2, [r3, #44]
.L24:
	ldr	r3, [r1, #44]
	cmp	r3, #0
	blt	.L24
	ldr	r2, .L65+12
.L39:
	ldr	r1, [r2, #68]
	ldr	r3, .L65+12
	tst	r1, #4
	beq	.L39
	ldr	r2, .L65+24
	mov	r1, r3
	str	r2, [r3, #68]
	mov	r2, #0
	str	r2, [r3, #40]
	ldr	r2, .L65+64
	str	r2, [r3, #44]
.L26:
	ldr	r3, [r1, #44]
	cmp	r3, #0
	blt	.L26
	ldr	r3, .L65+12
.L38:
	ldr	r2, [r3, #68]
	tst	r2, #4
	beq	.L38
	ldr	r2, .L65+12
.L37:
	ldr	r1, [r2, #72]
	ldr	r3, .L65+12
	tst	r1, #512
	bne	.L37
	ldr	r2, .L65+24
	mov	r1, r3
	str	r2, [r3, #68]
	ldr	r2, .L65+68
	str	r2, [r3, #40]
	ldr	r2, .L65+72
	str	r2, [r3, #44]
.L29:
	ldr	r3, [r1, #44]
	cmp	r3, #0
	blt	.L29
	ldr	r3, .L65+12
.L36:
	ldr	r2, [r3, #68]
	tst	r2, #4
	beq	.L36
	ldr	r2, .L65+12
.L35:
	ldr	r1, [r2, #72]
	ldr	r3, .L65+12
	tst	r1, #512
	bne	.L35
	mov	r2, #65536
	str	r2, [r3, #24]
	mov	r2, #2
	str	r2, [r3, #8]
	ldr	r2, .L65+36
	mov	r1, r3
	str	r2, [r3, #44]
.L32:
	ldr	r2, [r1, #44]
	ldr	r3, .L65+12
	cmp	r2, #0
	blt	.L32
	mov	r2, #1
	str	r2, [r3, #16]
	ldr	r2, .L65+36
	mov	r1, r3
	str	r2, [r3, #44]
.L33:
	ldr	r3, [r1, #44]
	cmp	r3, #0
	blt	.L33
	bx	lr
.L66:
	.align	2
.L65:
	.word	270487552
	.word	-197378
	.word	1000000
	.word	1612447744
	.word	500000
	.word	200000
	.word	110590
	.word	-131071
	.word	537329672
	.word	-2145386496
	.word	-2147442688
	.word	2000000
	.word	1076887552
	.word	-2147475391
	.word	-2147475006
	.word	-2147475133
	.word	-2147475129
	.word	62325248
	.word	-2147475130
	.size	emmc_init, .-emmc_init
	.align	2
	.global	emmc_deinit
	.type	emmc_deinit, %function
emmc_deinit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L73
	ldr	r2, .L73+4
	mov	r1, r3
	str	r2, [r3, #68]
	mov	r2, #0
	str	r2, [r3, #16]
	ldr	r2, .L73+8
	str	r2, [r3, #44]
.L68:
	ldr	r3, [r1, #44]
	cmp	r3, #0
	blt	.L68
	ldr	r3, .L73
	mov	r2, #0
	str	r2, [r3, #4]
	ldr	r3, .L73+12
	b	.L69
.L70:
@ 73 "himciv100_boot.c" 1
	nop
@ 0 "" 2
.L69:
	subs	r3, r3, #1
	bcs	.L70
	bx	lr
.L74:
	.align	2
.L73:
	.word	1612447744
	.word	110590
	.word	-2145386496
	.word	500000
	.size	emmc_deinit, .-emmc_deinit
	.align	2
	.global	emmc_read
	.type	emmc_read, %function
emmc_read:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L102
	add	r2, r1, #508
	add	r2, r2, #3
	mov	ip, #512
	bic	r2, r2, #508
	bic	r2, r2, #3
	str	ip, [r3, #28]
	mov	r4, r3
	str	r2, [r3, #32]
	ldr	r2, .L102+4
	str	r2, [r3, #68]
	ldr	r2, .L102+8
	str	ip, [r3, #40]
	str	r2, [r3, #44]
.L76:
	ldr	r3, [r4, #44]
	cmp	r3, #0
	blt	.L76
	ldr	r2, .L102
.L94:
	ldr	ip, [r2, #68]
	ldr	r3, .L102
	tst	ip, #4
	beq	.L94
	ldr	r2, .L102+4
	mov	ip, r3
	str	r2, [r3, #68]
	mov	r2, #(CONFIG_MMC_BOOT_ADDR/512)
	str	r2, [r3, #40]
	ldr	r2, .L102+12
	str	r2, [r3, #44]
.L78:
	ldr	r3, [ip, #44]
	cmp	r3, #0
	blt	.L78
	ldr	r3, .L102
.L93:
	ldr	r2, [r3, #68]
	tst	r2, #4
	beq	.L93
	ldr	r3, .L102
	mov	r1, r1, lsr #2
	ldr	r6, .L102+4
	mov	r5, r3
	b	.L80
.L86:
	ldr	r2, [r3, #72]
.L83:
	ldr	ip, [r3, #68]
	tst	ip, #32
	bne	.L81
	tst	ip, #38400
	beq	.L83
	bx	lr
.L81:
	ubfx	r2, r2, #17, #13
	str	r6, [r5, #68]
	cmp	r1, r2
	movcc	r2, r1
	mov	r4, #0
	mov	ip, r2
	b	.L84
.L85:
	ldr	r7, [r3, #256]
	sub	ip, ip, #1
	str	r7, [r0, r4]
	add	r4, r4, #4
.L84:
	cmp	ip, #0
	bne	.L85
	add	r0, r0, r2, asl #2
	rsb	r1, r2, r1
.L80:
	cmp	r1, #0
	bne	.L86
	ldr	r3, .L102
	ldr	r2, .L102+4
	mov	r0, r3
	str	r2, [r3, #68]
	ldr	r2, .L102+16
	str	r1, [r3, #40]
	str	r2, [r3, #44]
.L87:
	ldr	r3, [r0, #44]
	cmp	r3, #0
	blt	.L87
	ldr	r3, .L102
.L92:
	ldr	r2, [r3, #68]
	tst	r2, #4
	beq	.L92
	ldr	r3, .L102
.L91:
	ldr	r2, [r3, #72]
	tst	r2, #512
	bne	.L91
	bx	lr
.L103:
	.align	2
.L102:
	.word	1612447744
	.word	110590
	.word	-2147475120
	.word	-2147474606
	.word	-2147475124
	.size	emmc_read, .-emmc_read
	.ident	"GCC: (GNU) 4.4.0"
