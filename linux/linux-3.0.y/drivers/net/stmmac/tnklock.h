/*****************************************************************************
  This is the driver for the CreVinn TOE-NK-2G TCP Offload Engine.
  TOE-NK-2G incorporates a Synopsys Ethernet MAC core.

  Copyright (C) 2011 Emutex Ltd. All rights reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  Authors: Mark Burkley <mark@emutex.com>

*******************************************************************************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>

#ifndef __TNKLOCK_H
#define __TNKLOCK_H

struct tnk_lock {
	spinlock_t spin;
	unsigned long flags;
	int locking_cpu;
	int lock_count;
};

void tnk_lock(struct tnk_lock *t);
void tnk_unlock(struct tnk_lock *t);
void tnk_lock_init(struct tnk_lock *t);

#endif
