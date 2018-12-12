/*
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <config.h>

.global reset_cpu
reset_cpu:
	ldr	r1, rstctl			@ get addr for global reset
						@ reg
	mov	r3, #0x2			@ full reset pll + mpu
	str	r3, [r1]			@ force reset
	mov	r0, r0

_loop_forever:
	b	_loop_forever
rstctl:
	.word  SYS_CTRL_REG_BASE + REG_SC_SYSRES
