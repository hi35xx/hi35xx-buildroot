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

#include <net/tcp.h>
#include <net/tnkdrv.h>

#include "tnklock.h"

void tnk_lock(struct tnk_lock *t)
{
	int c = get_cpu();
	int locked = 0;

	while (!locked)	{
		locked = spin_trylock_irqsave(&t->spin, t->flags);

		if (locked)
			break;

		if (t->locking_cpu != c && t->lock_count < 2) {
			/*  Locked by the other CPU, safe for us to spin
			 */

			continue;
		} else {
			/*  Must be locked by us, unsafe re-entrancy
			 */
			WARN(1, "%s ERROR can't lock, nested lock attempt for"
			     " cpu%d lock held by cpu%d count %d\n", __func__,
			     c, t->locking_cpu, t->lock_count);
			break;
		}
	}

	/*  if (locked) */
	{
		t->locking_cpu = c;
		t->lock_count++;
	}

	put_cpu();
}

void tnk_unlock(struct tnk_lock *t)
{
	if (t->lock_count < 1)
		WARN(1, "%s ERROR can't unlock, nested unlock attempt %d\n",
		      __func__, t->lock_count);

	t->lock_count--;
	t->locking_cpu = -1;
	spin_unlock_irqrestore(&t->spin, t->flags);
}

void tnk_lock_init(struct tnk_lock *t)
{
	spin_lock_init(&t->spin);
	t->locking_cpu = -1;
	t->lock_count = 0;
}
