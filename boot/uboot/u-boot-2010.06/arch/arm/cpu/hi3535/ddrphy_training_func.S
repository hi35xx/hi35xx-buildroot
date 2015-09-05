/******************************************************************************
 *    COPYRIGHT (C) 2013 Hisilicon
 *    All rights reserved.
 *
******************************************************************************/

.globl DDRPHY_WRLV_TRAIN_ROUTE
.globl DDRPHY_GATE_TRAIN_ROUTE
.globl DDRPHY_DATA_TRAIN_ROUTE

DDRPHY_WRLV_TRAIN_ROUTE:
	PUSH    {R0-R8,LR}
	LDR     R8,  =0X20118000
	MOV     R1,  #0
	MOV     R2,  #0
	MOV     R3,  #0
	MOV     R4,  #0
SEARCH_WDQSBDL:
	BL      UPDATE_WDQSBDL
	BL      TRAIN_WDQS
	AND     R0,  #0XF
	CMP     R0,  #0XF
	BEQ     WRL_TRAIN_RETURN
	TST     R0,  #1
	ADDEQ   R1,  R1, #1
	TST     R0,  #2
	ADDEQ   R2,  R2, #1
	TST     R0,  #4
	ADDEQ   R3,  R3, #1
	TST     R0,  #8
	ADDEQ   R4,  R4, #1
	CMP     R1,  #0X20
	MOVEQ   R1,  #0X1F
	ORREQ   R0,  R0, #1
	CMP     R2,  #0X20
	MOVEQ   R2,  #0X1F
	ORREQ   R0,  R0, #2
	CMP     R3,  #0X20
	MOVEQ   R3,  #0X1F
	ORREQ   R0,  R0, #4
	CMP     R4,  #0X20
	MOVEQ   R4,  #0X1F
	ORREQ   R0,  R0, #8
	CMP     R0,  #0XF
	BNE     SEARCH_WDQSBDL
WRL_TRAIN_RETURN:
	POP     {R0-R8,PC}

TRAIN_WDQS:
	PUSH    {R1, LR}
	MOV     R1,  #1
	STR     R1,  [R8, #0XA0]
	STR     R1,  [R8, #0XA4]
	LDR     R0,  [R8, #0XA8]
	MOV     R1,  #0
	STR     R1,  [R8, #0XA4]
	STR     R1,  [R8, #0XA0]
	LDR     R1,  [R8, #0X208]
	AND     R1,  R1, #1
	ORR     R0,  R0, R1
	LDR     R1,  [R8, #0X288]
	AND     R1,  R1, #1
	LSL     R1,  R1, #1
	ORR     R0,  R0, R1
	LDR     R1,  [R8, #0X308]
	AND     R1,  R1, #1
	LSL     R1,  R1, #2
	ORR     R0,  R0, R1
	LDR     R1,  [R8, #0X388]
	AND     R1,  R1, #1
	LSL     R1,  R1, #3
	ORR     R0,  R0, R1
	POP     {R1, PC}

UPDATE_WDQSBDL:
	LDR     R5,  [R8, #0X230]
	BIC     R5,  R5, #0XFF
	ORR     R5,  R5, R1
	STR     R5,  [R8, #0X230]
	LDR     R5,  [R8, #0X2B0]
	BIC     R5,  R5, #0XFF
	ORR     R5,  R5, R2
	STR     R5,  [R8, #0X2B0]
	LDR     R5,  [R8, #0X330]
	BIC     R5,  R5, #0XFF
	ORR     R5,  R5, R3
	STR     R5,  [R8, #0X330]
	LDR     R5,  [R8, #0X3B0]
	BIC     R5,  R5, #0XFF
	ORR     R5,  R5, R4
	STR     R5,  [R8, #0X3B0]
	LDR     R5,  [R8, #0X70]
	ORR     R5,  R5, #0X100000
	STR     R5,  [R8, #0X70]
	BIC     R5,  R5, #0X100000
	STR     R5,  [R8, #0X70]
	BX      LR

DDRPHY_GATE_TRAIN_ROUTE:
	PUSH    {R1-R9, LR}
	LDR     R8,  =0X20118000
	MOV     R1,  #0
	MOV     R2,  #0
	MOV     R3,  #0
	MOV     R4,  #0
SEARCH_DQSGBDL:
	BL      UPDATE_DQSGBDL
	BL      TRAIN_GATE
	LSR     R0,  #0X8
	AND     R0,  #0XF
	CMP     R0,  #0XF
	MOVEQ   R0,  #0
	BEQ     GATE_TRAIN_RETURN
	TST     R0,  #1
	ADDEQ   R1,  R1, #1
	TST     R0,  #2
	ADDEQ   R2,  R2, #1
	TST     R0,  #4
	ADDEQ   R3,  R3, #1
	TST     R0,  #8
	ADDEQ   R4,  R4, #1
	CMP     R1,  #0X40
	MVNEQ   R0,  #0
	BEQ     GATE_TRAIN_RETURN
	CMP     R2,  #0X40
	MVNEQ   R0,  #0
	BEQ     GATE_TRAIN_RETURN
	CMP     R3,  #0X40
	MVNEQ   R0,  #0
	BEQ     GATE_TRAIN_RETURN
	CMP     R4,  #0X40
	MVNEQ   R0,  #0
	BEQ     GATE_TRAIN_RETURN
	B       SEARCH_DQSGBDL
GATE_TRAIN_RETURN:
	POP	{R1-R9,PC}

TRAIN_GATE:
	PUSH    {R1-R11, LR}
	MOV     R1,  #0x80000000
	LDMIA   R1,  {R2-R9}
	LDR     R8,  =0X20118000
	LDR     R0,  [R8, #0XA8]
	MOV     R1,  #0X8000
	STR     R1,  [R8, #0X04]
	MOV     R1,  #0
	STR     R1,  [R8, #0X04]
	LDR     R1,  [R8, #0X208]
	AND     R1,  R1, #1
	LSL     R1,  R1, #8
	ORR     R0,  R0, R1
	LDR     R1,  [R8, #0X288]
	AND     R1,  R1, #1
	LSL     R1,  R1, #9
	ORR     R0,  R0, R1
	LDR     R1,  [R8, #0X308]
	AND     R1,  R1, #1
	LSL     R1,  R1, #10
	ORR     R0,  R0, R1
	LDR     R1,  [R8, #0X388]
	AND     R1,  R1, #1
	LSL     R1,  R1, #11
	ORR     R0,  R0, R1
	POP     {R1-R11, PC}

UPDATE_DQSGBDL:
	PUSH    {R1-R11, LR}
	MOV     R6,  #0X0
	CMP     R1,  #0X20
	SUBCS   R6,  R1, #0X1F
	MOVCS   R1,  #0X1F
	LDR     R5,  [R8, #0X23C]
	BIC     R5,  R5, #0XFF0000
	ORR     R5,  R5, R1, LSL #16
	ORR     R5,  R5, R6
	STR     R5,  [R8, #0X23C]
	MOV     R6,  #0X0
	CMP     R2,  #0X20
	SUBCS   R6,  R2, #0X1F
	MOVCS   R2,  #0X1F
	LDR     R5,  [R8, #0X2BC]
	BIC     R5,  R5, #0XFF0000
	ORR     R5,  R5, R2, LSL #16
	ORR     R5,  R5, R6
	STR     R5,  [R8, #0X2BC]
	MOV     R6,  #0X0
	CMP     R3,  #0X20
	SUBCS   R6,  R3, #0X1F
	MOVCS   R3,  #0X1F
	LDR     R5,  [R8, #0X33C]
	BIC     R5,  R5, #0XFF0000
	ORR     R5,  R5, R3, LSL #16
	ORR     R5,  R5, R6
	STR     R5,  [R8, #0X33C]
	MOV     R6,  #0X0
	CMP     R4,  #0X20
	SUBCS   R6,  R4, #0X1F
	MOVCS   R4,  #0X1F
	LDR     R5,  [R8, #0X3BC]
	BIC     R5,  R5, #0XFF0000
	ORR     R5,  R5, R4, LSL #16
	ORR     R5,  R5, R6
	STR     R5,  [R8, #0X3BC]
	LDR     R5,  [R8, #0X70]
	ORR     R5,  R5, #0X100000
	STR     R5,  [R8, #0X70]
	BIC     R5,  R5, #0X100000
	STR     R5,  [R8, #0X70]
	POP     {R1-R11, PC}

DDRPHY_DATA_TRAIN_ROUTE:
	PUSH    {R1-R11, LR}
	MOV     R1,  #0x80000000
	LDR     R2,  =0X00FF00FF
	LDR     R3,  =0XFF00FF00
	LDR     R4,  =0X11EE11EE
	LDR     R5,  =0XEE11EE11
	LDR     R6,  =0X22DD22DD
	LDR     R7,  =0XDD22DD22
	LDR     R8,  =0X55AA55AA
	LDR     R9,  =0XAA55AA55
	STRD    R2,  R3, [R1, #0X00]
	STRD    R4,  R5, [R1, #0X08]
	STRD    R6,  R7, [R1, #0X10]
	STRD    R8,  R9, [R1, #0X18]
	LDMIA   R1,  {R2-R9}
	MOV     R1,  #0
	AND     R2,  R2, R0
	AND     R3,  R3, R0
	AND     R4,  R4, R0
	AND     R5,  R5, R0
	AND     R6,  R6, R0
	AND     R7,  R7, R0
	AND     R8,  R8, R0
	AND     R9,  R9, R0
	LDR     R10, =0X00FF00FF
	AND     R10, R10, R0
	CMP     R2,  R10
	MVNNE   R1,  #0
	LDR     R10, =0XFF00FF00
	AND     R10, R10, R0
	CMP     R3,  R10
	MVNNE   R1,  #0
	LDR     R10, =0X11EE11EE
	AND     R10, R10, R0
	CMP     R4,  R10
	MVNNE   R1,  #0
	LDR     R10, =0XEE11EE11
	AND     R10, R10, R0
	CMP     R5,  R10
	MVNNE   R1,  #0
	LDR     R10, =0X22DD22DD
	AND     R10, R10, R0
	CMP     R6,  R10
	MVNNE   R1,  #0
	LDR     R10, =0XDD22DD22
	AND     R10, R10, R0
	CMP     R7,  R10
	MVNNE   R1,  #0
	LDR     R10, =0X55AA55AA
	AND     R10, R10, R0
	CMP     R8,  R10
	MVNNE   R1,  #0
	LDR     R10, =0XAA55AA55
	AND     R10, R10, R0
	CMP     R9,  R10
	MVNNE   R1,  #0
	MOV     R0,  R1
	LDR     R8,  =0X20118000
	MOV     R1,  #0X8000
	STR     R1,  [R8, #0X4]
	MOV     R1,  #0
	STR     R1,  [R8, #0X4]
	POP     {R1-R11, PC}

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@  void ddr_training_info(int value);
@
@
.align	2
.global	ddr_training_info
.type	ddr_training_info, %function
ddr_training_info:
	mov     r5, lr
	mov     r6, r0
	add     r0, pc, #24
	bl      uart_early_puts
	mov     r0, r6
	bl      uart_early_put_hex
	mov     r0, #'\r'
	bl      uart_early_putc
	mov     r0, #'\n'
	bl      uart_early_putc
	mov     pc, r5
	.ascii "\r\n\r\nDDR training failed, result: \0"
