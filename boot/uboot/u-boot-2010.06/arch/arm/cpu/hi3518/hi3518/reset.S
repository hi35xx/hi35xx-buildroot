/*
 * Copyright (c) 2009 hisilicon.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>

.global reset_cpu
reset_cpu:
	adrl	r0, tcm_start
	mov     r1, #0x100
	mov     r2, #0x80
	add     r2, r0, r2
copy_tcm:
	ldmia   r0!, {r3 - r10}
	stmia   r1!, {r3 - r10}
	cmp     r0, r2
	ble     copy_tcm
	mov	pc, #0x100
tcm_start:
	ldr	r0, =REG_BASE_SCTL
	ldr	r1, [r0]
	bic     r1, r1, #0x00000007
	orr     r1, r1, #0x00000002
	str	r1, [r0]
slow_mode:
	ldr     r1, [r0]
	bic     r1, r1, #0xffffff87
	mov	r1, r1, lsr#3
	cmp	r1, #2
	bne	slow_mode

	ldr	r1, rstctl			@ get addr for global reset
						@ reg
	mov	r3, #0x2			@ full reset pll + mpu
	str	r3, [r1]			@ force reset
	mov	r0, r0

_loop_forever:
	b	_loop_forever
rstctl:
	.word  REG_BASE_SCTL + REG_SC_SYSRES
