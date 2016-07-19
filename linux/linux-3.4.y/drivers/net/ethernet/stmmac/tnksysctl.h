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

/*
 * tnksysctl.c: sysctl interface for TNK subsystem.
 */

#ifndef __TNK_SYSCTL_H
#define __TNK_SYSCTL_H

extern int tnk_tcp_persist_time;
extern int tnk_tcp_rtt_sample_interval;
extern int tnk_threshold;
extern int tnk_poll_limit;
#ifdef TNK_RX_CHANNEL_FLOW_CONTROL
extern int tnk_rx_q_limit;
#endif
extern int tnk_send_max_msgsize;
extern int tnk_tcp_gmac0_enable;
extern int tnk_tcp_gmac1_enable;
extern int hitoe;
extern int control_dbg;
extern int max_mss;
extern int tnk_macfifo_nospace_workaround;

void tnk_sysctl_init(void);
void tnk_sysctl_shutdown(void);

#endif
