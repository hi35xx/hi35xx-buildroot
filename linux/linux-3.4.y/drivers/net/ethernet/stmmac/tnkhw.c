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

  Authors: Dan O'Donovan <dan@emutex.com>

*******************************************************************************/

/*
 *  TNK hardware interface
 */

#include <linux/io.h>
#include <linux/device.h>
#include <linux/atomic.h>
#include <linux/semaphore.h>
#include <linux/wait.h>
#include <linux/sched.h>

#include "tnkhw.h"
#include "tnkmem.h"
#include "tnkhw_regmap.h"
#include "tnkinfo.h"
#include "tnksysctl.h"
#include "common.h"
#include "descs.h"
#include "dwmac1000.h"
#ifdef CONFIG_ARCH_HI3535
#include "tnk_hi3535.h"
#endif
#include "tnktcp.h"
/*  TODO - for debug only, remove later */
#undef TNK_TTX_FLUSH_DELAYED_RECLAIM
#define TNK_TTX_BUF_RECLAIMED 0x00000BAD

#define TNK_DMA_RX_SIZE  STMMAC_ALIGN(4096)
#define TNK_DMA_RX_RECYCLE_SIZE		(16)

#define TNK_TCP_HEADER_RESERVED (32)

/* Maximum amount of outstanding data in TTX pipeline for any single connection
 * Note that the actual limit imposed by the TOE is 16MB.  For now, we limit it
 * further to just 1MB here to impose a stricter ceiling on memory usage */
#define TNK_TTX_DATA_LIMIT ((1 << 24) - 1)
#if defined(TNK_BONDING) || defined(TNK_BONJOUR)
#define TNK_IP_MAX_NUM 4
static int eth0_ipv4_cnt;
static int eth1_ipv4_cnt;
static int eth0_ipv6_cnt;
static int eth1_ipv6_cnt;
#endif

/* Abbreviated masks for interrupt source register */
#define TNK_INTR_TRX \
	(TNK_MASK_INTR_STAT_DMA_CH2 << TNK_OFFSET_INTR_STAT_DMA_CH2)
#define TNK_INTR_TOE \
	(TNK_MASK_INTR_STAT_TOE << TNK_OFFSET_INTR_STAT_TOE)
#define TNK_INTR_TTX_ACK \
	(TNK_MASK_INTR_STAT_DMA_TTX_ACK << TNK_OFFSET_INTR_STAT_DMA_TTX_ACK)
#define TNK_INTR_TTX_ERR \
	(TNK_MASK_INTR_STAT_DMA_TTX_ERR << TNK_OFFSET_INTR_STAT_DMA_TTX_ERR)

/* Macro for validating cindex parameter */
#define TNK_CINDEX_VALIDATE(cindex) \
	BUG_ON(((cindex) < TNK_TTX_CINDEX_START) \
	       || ((cindex) >= tnk_max_connections));

/*  io-remapped base address for GMAC and TOE register map */
static void __iomem *tnkhw_ioaddr;
static struct device *tnk_dev;
static tnkhw_poll_wakeup_cb_t tnk_pollwakeup;
static tnkhw_rx_cb_t tnk_rxcallback;
static tnkhw_txfree_cb_t tnk_txcallback;
static tnkhw_tx_max_retries_cb_t tnk_txmaxretries;

/* Maximum number of TOE connections supported */
static unsigned int tnk_max_connections;

int tnk_tx_fifo = TNK_DMA_TX_SIZE;
module_param(tnk_tx_fifo, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(tnk_tx_fifo, "min:16, max:1024, default:32");

int tnk_rx_fifo = TNK_DMA_RX_SIZE;
module_param(tnk_rx_fifo, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(tnk_rx_fifo, "min:256, max:8192, default:4096");

int tnk_rx_recycle_fifo = TNK_DMA_RX_RECYCLE_SIZE;
module_param(tnk_rx_recycle_fifo, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(tnk_rx_recycle_fifo, "min:16, max:4096, default:16");

#ifdef TNK_DEBUG
struct tnkhw_rx_dma_info tnk_rx_dma;
struct tnkhw_tx_dma_info *tnk_tx_dma_list;
#else
static struct tnkhw_rx_dma_info tnk_rx_dma;
static struct tnkhw_tx_dma_info *tnk_tx_dma_list;
#endif

static struct tnkhw_stats tnk_stats;
struct tnkhw_conn_stats *tnk_conn_stats;
static struct stmmac_extra_stats tnk_stmmac_stats;

static int tnk_flush_pending;

static int count_skb;

static spinlock_t tnkhw_reg_lock;

static void tnkhw_toe_interrupt(void);
static void tnkhw_toe_flush_completed(void);
static void tnkhw_dump_tx_descriptors(unsigned cindex);
static void tnk_switch_enable(void)
{
	/* init all switch on TOE */
	writel(0x3fffd2, tnkhw_ioaddr + TNK_REG_TOE_SWITCH_CTRL);
	TNK_DBG("TOE switch reg enable\n");
}

static inline void tnkhw_ct_read_addr(uint32_t rd_addr, uint32_t *data)
{
	uint32_t rd_done;
	unsigned limit = 10000;

	/*  1. Write the CT read address */
	writel(rd_addr, tnkhw_ioaddr + TNK_REG_CT_CPU_RDADDR);

	/*  2. Write for the read to complete */
	do {
		rd_done = readl(tnkhw_ioaddr + TNK_REG_CT_CPU_ACSTAT);
		rd_done =
		    (rd_done >> TNK_OFFSET_CT_CPU_ACSTAT_RDDONE) &
		    TNK_MASK_CT_CPU_ACSTAT_RDDONE;
	} while (!rd_done && limit--);

	/*  3. Return the read result */
	*data = readl(tnkhw_ioaddr + TNK_REG_CT_CPU_RDDATA);
}

void tnkhw_ct_read(unsigned cindex, uint8_t loc, uint32_t *data)
{
	uint32_t rd_addr;

	rd_addr =
	    (cindex & TNK_MASK_CT_CPU_RDADDR_CSEL) <<
	    TNK_OFFSET_CT_CPU_RDADDR_CSEL;
	rd_addr |=
	    (loc & TNK_MASK_CT_CPU_RDADDR_LOC) << TNK_OFFSET_CT_CPU_RDADDR_LOC;

	tnkhw_ct_read_addr(rd_addr, data);
}

static inline void tnkhw_ct_write_addr(uint32_t wr_addr, uint32_t data)
{
	uint32_t wr_done;
	unsigned limit = 10000;

	/*  1. Set the data to be written */
	writel(data, tnkhw_ioaddr + TNK_REG_CT_CPU_WRDATA);

	/*  2. Set the CT write address */
	writel(wr_addr, tnkhw_ioaddr + TNK_REG_CT_CPU_WRADDR);

	/*  3. Wait for the write to complete */
	do {
		wr_done = readl(tnkhw_ioaddr + TNK_REG_CT_CPU_ACSTAT);
		wr_done =
		    (wr_done >> TNK_OFFSET_CT_CPU_ACSTAT_WRDONE) &
		    TNK_MASK_CT_CPU_ACSTAT_WRDONE;
	} while (!wr_done && limit--);
}

void tnkhw_ct_write(unsigned cindex, uint8_t loc, uint32_t data)
{
	uint32_t wr_addr;

	wr_addr =
	    (cindex & TNK_MASK_CT_CPU_WRADDR_CSEL) <<
	    TNK_OFFSET_CT_CPU_WRADDR_CSEL;
	wr_addr |=
	    (loc & TNK_MASK_CT_CPU_WRADDR_LOC) << TNK_OFFSET_CT_CPU_WRADDR_LOC;

	tnkhw_ct_write_addr(wr_addr, data);
}

#if defined(TNK_BONDING) || defined(TNK_BONJOUR)
static inline void ct_lookup_data_load_ipv4(uint32_t remote_ipv4_addr,
					    uint16_t remote_port,
					    uint16_t local_port,
						int local_ip_pos)
#else
static inline void ct_lookup_data_load_ipv4(uint32_t remote_ipv4_addr,
					    uint16_t remote_port,
					    uint16_t local_port)
#endif
{
	uint32_t ports;

	/*  1. Load the remote IP address */
	writel(remote_ipv4_addr, tnkhw_ioaddr + TNK_REG_CT_REMOTE_IPADDR);

	/*  2. Load the remote and local ports */
	ports =
	    (remote_port & TNK_MASK_CT_TCP_PORTS_REMOTE) <<
	    TNK_OFFSET_CT_TCP_PORTS_REMOTE;
	ports |=
	    (local_port & TNK_MASK_CT_TCP_PORTS_LOCAL) <<
	    TNK_OFFSET_CT_TCP_PORTS_LOCAL;
	writel(ports, tnkhw_ioaddr + TNK_REG_CT_TCP_PORTS);
#if defined(TNK_BONDING) || defined(TNK_BONJOUR)
	/* 3. Load the Local IP address position */
	if (local_ip_pos >= 0) {
		writel(local_ip_pos & 0x00000003,
				tnkhw_ioaddr + TNK_REG_CT_LOCAL_IPADDR_SEL);
		TNKBD_DBG("%s local_ip_pos = %d\n", __func__,
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR_SEL));
	}
#endif
}

#if defined(TNK_BONDING) || defined(TNK_BONJOUR)
static inline void ct_lookup_data_load_ipv6(uint32_t remote_ipv6_addr_31_0,
					    uint32_t remote_ipv6_addr_63_32,
					    uint32_t remote_ipv6_addr_95_64,
					    uint32_t remote_ipv6_addr_127_96,
					    uint16_t remote_port,
					    uint16_t local_port,
						int local_ip_pos)
#else
static inline void ct_lookup_data_load_ipv6(uint32_t remote_ipv6_addr_31_0,
					    uint32_t remote_ipv6_addr_63_32,
					    uint32_t remote_ipv6_addr_95_64,
					    uint32_t remote_ipv6_addr_127_96,
					    uint16_t remote_port,
					    uint16_t local_port)

#endif
{
	uint32_t ports;

	/*  1. Load the remote IP address */
	writel(remote_ipv6_addr_127_96,
	       tnkhw_ioaddr + TNK_REG_CT_REMOTE_IPV6_ADDR_W0);
	writel(remote_ipv6_addr_95_64,
	       tnkhw_ioaddr + TNK_REG_CT_REMOTE_IPV6_ADDR_W1);
	writel(remote_ipv6_addr_63_32,
	       tnkhw_ioaddr + TNK_REG_CT_REMOTE_IPV6_ADDR_W2);
	writel(remote_ipv6_addr_31_0,
	       tnkhw_ioaddr + TNK_REG_CT_REMOTE_IPV6_ADDR_W3);

	/*  2. Load the remote and local ports */
	ports =
	    (remote_port & TNK_MASK_CT_TCP_PORTS_REMOTE) <<
	    TNK_OFFSET_CT_TCP_PORTS_REMOTE;
	ports |=
	    (local_port & TNK_MASK_CT_TCP_PORTS_LOCAL) <<
	    TNK_OFFSET_CT_TCP_PORTS_LOCAL;
	writel(ports, tnkhw_ioaddr + TNK_REG_CT_TCP_PORTS);

#if defined(TNK_BONDING) || defined(TNK_BONJOUR)
       /* 3. Load the Local IP address position */
	if (local_ip_pos >= 0) {
		writel(local_ip_pos & 0x00000003,
				tnkhw_ioaddr + TNK_REG_CT_LOCAL_IPADDR_SEL);
		TNKBD_DBG("%s local_ip_pos = %d\n", __func__,
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR_SEL));
	}
#endif
}

static inline void tnkhw_ct_update(unsigned cindex, int add, int ipv6)
{
	uint32_t ctrl_val, update_pending, err;
	unsigned limit, retry = 0;

begin:
	ctrl_val =
	    (cindex & TNK_MASK_CT_CONN_CTRL_CSEL) <<
	    TNK_OFFSET_CT_CONN_CTRL_CSEL;
	ctrl_val |=
	    (add & TNK_MASK_CT_CONN_CTRL_ADD) << TNK_OFFSET_CT_CONN_CTRL_ADD;
	ctrl_val |=
	    (ipv6 & TNK_MASK_CT_CONN_CTRL_IPV6) << TNK_OFFSET_CT_CONN_CTRL_IPV6;
	ctrl_val |=
	    (0x1 & TNK_MASK_CT_CONN_CTRL_UPDATE) <<
	    TNK_OFFSET_CT_CONN_CTRL_UPDATE;

	/*  1. Write the connection control command */
	writel(ctrl_val, tnkhw_ioaddr + TNK_REG_CT_CONN_CTRL);

	/*  2. Wait for the connection table update to complete */
	limit = 100000;
	do {
		ctrl_val = readl(tnkhw_ioaddr + TNK_REG_CT_CONN_CTRL);
		update_pending =
		    (ctrl_val >> TNK_OFFSET_CT_CONN_CTRL_UPDATE) &
		    TNK_MASK_CT_CONN_CTRL_UPDATE;
	} while (update_pending && limit--);

	err =
	    (ctrl_val >> TNK_OFFSET_CT_CONN_CTRL_ERR) &
	    TNK_MASK_CT_CONN_CTRL_ERR;
	if (err) {
		/*  TODO - tidy up the following error handling */
		pr_err("%s: Got error, cindex = %u, add = %d, retry = %u\n",
		       __func__, cindex, add, retry);
		if (!retry) {
			retry = 1;	/*  Retry once */
			goto begin;
		}
	}
}

static inline void tnkhw_ttx_db_write(unsigned cindex, unsigned entry_size,
				      unsigned entry, uint32_t val)
{
	unsigned db_offset = (cindex * entry_size) + (entry * sizeof(uint32_t));

	writel(val, tnkhw_ioaddr + TNK_DMA_TTX_DB_OFFSET + db_offset);
}

static inline void tnkhw_ttx_db_init(unsigned cindex, uint32_t desc_hd_ptr)
{
	/* Set tx_ch_desc_hd_ptr */
	tnkhw_ttx_db_write(cindex, TNK_TTX_DMA_DB_ENTRY_SIZE, 0, desc_hd_ptr);
	/* Clear tx_offset */
	tnkhw_ttx_db_write(cindex, TNK_TTX_DMA_DB_ENTRY_SIZE, 1, 0);
	/* Set tx_ch_desc_ack_ptr */
	tnkhw_ttx_db_write(cindex, TNK_TTX_DMA_DB_ENTRY_SIZE, 2, desc_hd_ptr);
	/* Clear tx_byte_count */
	tnkhw_ttx_db_write(cindex, TNK_TTX_DMA_DB_ENTRY_SIZE, 3, 0);
}

static inline void tnkhw_ttx_txstat_clear(unsigned cindex)
{
	unsigned offset = cindex * TNK_TTX_DMA_DB_TXSTAT_SIZE;
	memset(tnkhw_ioaddr + TNK_DMA_TTX_DB_TXSTAT_OFFSET + offset,
	       0, TNK_TTX_DMA_DB_TXSTAT_SIZE);
}

static inline void tnkhw_ttx_rxstat_clear(unsigned cindex)
{
	unsigned offset = cindex * TNK_TTX_DMA_DB_RXSTAT_SIZE;
	memset(tnkhw_ioaddr + TNK_DMA_TTX_DB_RXSTAT_OFFSET + offset,
	       0, TNK_TTX_DMA_DB_RXSTAT_SIZE);
}

static inline void tnkhw_ttx_txstat_get(unsigned cindex,
					uint32_t *tx_acked_count,
					uint32_t *tx_retry_count)
{
	unsigned offset = cindex * TNK_TTX_DMA_DB_TXSTAT_SIZE;
	*tx_acked_count =
	    readl(tnkhw_ioaddr + TNK_DMA_TTX_DB_TXSTAT_OFFSET + offset);
	*tx_retry_count =
	    readl(tnkhw_ioaddr + TNK_DMA_TTX_DB_TXSTAT_OFFSET + offset +
		  sizeof(uint32_t));
}

static inline void tnkhw_ttx_rxstat_get(unsigned cindex,
					uint32_t *rxed_byte_count)
{
	unsigned offset = cindex * TNK_TTX_DMA_DB_RXSTAT_SIZE;
	*rxed_byte_count =
	    readl(tnkhw_ioaddr + TNK_DMA_TTX_DB_RXSTAT_OFFSET + offset);
}

static inline void tnkhw_ttx_data_add(unsigned cindex, uint32_t size)
{
	/*  If size < 2^21, use TTX_ALT_REQ */
	if (likely((size & TNK_MASK_DMA_TTX_ALT_REQ_SIZE) == size)) {
		uint32_t regval = (size << TNK_OFFSET_DMA_TTX_ALT_REQ_SIZE) |
		    (cindex << TNK_OFFSET_DMA_TTX_ADD_CID);
		writel(regval, tnkhw_ioaddr + TNK_REG_DMA_TTX_ALT_REQ);
	}
	/*  Otherwise, use TTX_ADD_SIZE and TTX_ADD registers */
	else {
		size =
		    (size & TNK_MASK_DMA_TTX_ADD_SIZE) <<
		    TNK_OFFSET_DMA_TTX_ADD_SIZE;
		TNK_DBG("%s size=%d to %p\n", __func__, size,
			tnkhw_ioaddr + TNK_REG_DMA_TTX_ADD_SIZE);
		writel(size, tnkhw_ioaddr + TNK_REG_DMA_TTX_ADD_SIZE);

		cindex =
		    (cindex & TNK_MASK_DMA_TTX_ADD_CID) <<
		    TNK_OFFSET_DMA_TTX_ADD_CID;
		TNK_DBG("%s cindex=%d to %p\n", __func__, cindex,
			tnkhw_ioaddr + TNK_REG_DMA_TTX_ADD);
		writel(cindex, tnkhw_ioaddr + TNK_REG_DMA_TTX_ADD);
	}
}

static inline void tnkhw_ttx_wait_flush(void)
{
	unsigned int debug_reg;
	unsigned char rd_ptr0, wr_ptr0;
	unsigned char rd_ptr1, wr_ptr1;
	unsigned int initial_state = 0;
	unsigned int  i = 0;

	debug_reg = readl(tnkhw_ioaddr + 0x84f0);
	rd_ptr0 = (debug_reg >> 16) & 0xff;
	wr_ptr0 = (debug_reg >> 7) & 0xff;
	wr_ptr0 = wr_ptr0 + 7;

	if (rd_ptr0 < wr_ptr0)
		initial_state = 0;
	else
		initial_state = 1;

#if 0
	while (i < 256) {
		mdelay(5);
		data_pool[i++] = readl(tnkhw_ioaddr + 0x84f0);
	}
#else
	i = 0;
	do {
		debug_reg = 0;
		debug_reg = readl(tnkhw_ioaddr + 0x84f0);
		rd_ptr1 = (debug_reg >> 16) & 0x1ff;
		wr_ptr1 = (debug_reg >> 7) & 0x1ff;

		if (!initial_state && rd_ptr1 > wr_ptr0)
			break;
		if (!initial_state && rd_ptr1 < rd_ptr0)
			break;
		if (initial_state && rd_ptr1 > wr_ptr0 && rd_ptr1 < rd_ptr0)
			break;
/*
		if (initial_state && rd_ptr1 > wr_ptr0 && rd_ptr1 < wr_ptr1)
			break;
*/

		if (rd_ptr1 == wr_ptr1) {
			mdelay(1);
			break;
		}
	} while (i++ < 100000);

	if (i >= 100000)
		pr_err("%s timeout rd0:%d wr0:%d rd1:%d wr1:%d\n",
				__func__, rd_ptr0, wr_ptr0, rd_ptr1, wr_ptr1);
#endif

#if 0
	i = 0;
	while (i < 256) {
		printk(KERN_INFO "i:%d %x \t", i, data_pool[i]);
		i++;
	}
#endif
}

/* Flushes any data currently in the TX pipeline for a specified channel
 * Useful for resetting the TX pipeline on startup, or in case of error
 */
static inline void tnkhw_ttx_data_flush(unsigned cindex)
{
	uint32_t regval;
	int limit;
	uint32_t toe_int_en, toe_int_stat;

	TNK_DBG("%s: cindex %u\n", cindex);

#if 0
	tnkhw_ttx_data_add(cindex, 64);
#endif

	tnk_flush_pending = 1;

	toe_int_en = readl(tnkhw_ioaddr + TNK_REG_TOE_INTR_EN);
	/*  enable the flush complete interrupt */
	toe_int_en |=
	    (TNK_MASK_TOE_INTR_EN_FLUSH << TNK_OFFSET_TOE_INTR_EN_FLUSH);
	writel(toe_int_en, tnkhw_ioaddr + TNK_REG_TOE_INTR_EN);

	/*  Request the flush */
	regval =
	    (cindex & TNK_MASK_DMA_TTX_ADD_CID) << TNK_OFFSET_DMA_TTX_ADD_CID;
	regval |= (1 << TNK_OFFSET_DMA_TTX_ADD_FLUSH);
	writel(regval, tnkhw_ioaddr + TNK_REG_DMA_TTX_ADD);

	/* NOTE - this code assumes that interrupts are disabled
	 * Because the TOE_INTR_STAT register is cleared-on-read,
	 * an interrupt handler could clear this status bit before
	 * we see it
	 */

	limit = 100000;
	do {
		toe_int_stat = readl(tnkhw_ioaddr + TNK_REG_INTR_STAT);
		toe_int_stat =
		    (toe_int_stat >> TNK_OFFSET_INTR_STAT_TOE) &
		    TNK_MASK_INTR_STAT_TOE;
	} while (!toe_int_stat && limit--);

	if (toe_int_stat) {
		limit = 10000;
		do {
			/* tnkhw_toe_interrupt(); */
			/* Here we only care about flush_pending_complete
			* interrupt, so we will omit ct_max_retry interrupt.
			* We trust logic will generate ct_max_retry interrupt
			* again and again.
			* Why we omit ct_max_retry interrupt, because
			* in tnk_ct_tx_max_retries_callback(),
			* tnkhw_connection_disable() need get tnkhw_reg_lock,
			* but here we has got tnkhw_reg_lock,
			* it results dead spin_lock. */
			tnkhw_toe_flush_completed();
			mb();
		} while (tnk_flush_pending && limit--);
	}

	/* This flush mechanism isn't always reliable. A suitable fallback
	 * is to wait instead for 30us to allow for any outstanding TTX DMA
	 * engine transfers to complete (see TNK data sheet)
	 * Note that flushing the TTX channel should be a rare event
	 */
	if (tnk_flush_pending) {
		pr_warn("%s: Flush completion pending on cid %d,",
				__func__, cindex);
		pr_warn(" adding 30us delay\n");
		udelay(30);
		tnkhw_toe_flush_completed();
		pr_warn("cindex:%d after delay 30us tnk_flush_pending is %d\n",
				cindex, tnk_flush_pending);
	}

	/*  disable the flush complete interrupt */
	toe_int_en &=
	    ~(TNK_MASK_TOE_INTR_EN_FLUSH << TNK_OFFSET_TOE_INTR_EN_FLUSH);
	writel(toe_int_en, tnkhw_ioaddr + TNK_REG_TOE_INTR_EN);
}

/* Returns 1 if overflow occurred, otherwise returns 0 and ack_cid contains
 * next CID with a free TX descriptor
 */
static inline int tnkhw_ttx_ackd_cid_read(unsigned *ack_cid)
{
	uint32_t ackd_val = readl(tnkhw_ioaddr + TNK_REG_DMA_TTX_ACKD_CID);
	int overflow =
	    (ackd_val >> TNK_OFFSET_DMA_TTX_ACKD_OVFLOW) &
	    TNK_MASK_DMA_TTX_ACKD_OVFLOW;
	int valid =
	    (ackd_val >> TNK_OFFSET_DMA_TTX_ACKD_VALID) &
	    TNK_MASK_DMA_TTX_ACKD_VALID;

	if (valid)
		*ack_cid =
		    (ackd_val >> TNK_OFFSET_DMA_TTX_ACKD_CID) &
		    TNK_MASK_DMA_TTX_ACKD_CID;
	else
		*ack_cid = 0;

	if (overflow)
		return 1;

	return 0;
}

static inline void tnkhw_ttx_ctrl_set(unsigned axi_burst_len)
{
	uint32_t ctrl_val =
	    (axi_burst_len & TNK_MASK_TTX_CTRL0_MAXBURSTLEN) <<
	    TNK_OFFSET_TTX_CTRL0_MAXBURSTLEN;
	writel(ctrl_val, tnkhw_ioaddr + TNK_REG_DMA_TTX_CTRL0);
}

static inline void tnkhw_trx_ctrl_set(unsigned rx_buffer_len)
{
	/*  TODO - consider tying this to value of DMA_BUFFER_LEN.
	 *  For now, we're assuming 2kBytes (0x3) */
	uint32_t ctrl_val =
	    (3 & TNK_MASK_TRX_CTRL_RXBUFFERSIZE) <<
	    TNK_OFFSET_TRX_CTRL_RXBUFFERSIZE;
	/* Extra TRX status options are intentionally left as disabled
	 * However, if forwarding of error'd frames was ever enabled in the
	 * DMA engine, then we would have to enable some bits here also
	 */
	ctrl_val |= (1 & TNK_MASK_TRX_CTRL_IPC_STAT_EN) <<
		TNK_OFFSET_TRX_CTRL_IPC_STAT_EN;
	writel(ctrl_val, tnkhw_ioaddr + TNK_REG_DMA_TRX_CTRL);
}

static inline void tnkhw_trx_desc_add(uint32_t count)
{
	TNK_DBG("%s write count of %d\n", __func__, count);
	count = (count & TNK_MASK_TRX_ADD_COUNT) << TNK_OFFSET_TRX_ADD_COUNT;
	writel(count, tnkhw_ioaddr + TNK_REG_DMA_TRX_ADD_SIZE);
}

#define TNKHW_CT_LOC_FLAGS 12
static void tnkhw_ct_enable(unsigned cindex, int enable)
{
	uint32_t flags;
	/*  We only want to read/write the lower 16 bits of the CT entry */
	writel(0x3, tnkhw_ioaddr + TNK_REG_CT_CPU_WRMASK);
	tnkhw_ct_read(cindex, TNKHW_CT_LOC_FLAGS, &flags);
	if (enable)
		flags |= 0x10;	/*  Set the enable bit */
	else
		flags &= ~0x10;	/*  Clear the enable bit */
	tnkhw_ct_write(cindex, TNKHW_CT_LOC_FLAGS, flags & 0xFFFF);
	writel(0xF, tnkhw_ioaddr + TNK_REG_CT_CPU_WRMASK);
}

#define TNKHW_CT_LOC_TX_MSS 11
void tnkhw_set_tx_mss(unsigned int cindex, unsigned int mss_now)
{
	unsigned int tx_mss;
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);

	writel(0xC, tnkhw_ioaddr + TNK_REG_CT_CPU_WRMASK);
	tnkhw_ct_read(cindex, TNKHW_CT_LOC_TX_MSS, &tx_mss);
	tx_mss &= 0xFFFF;
	tx_mss |= mss_now << 16;
	tnkhw_ct_write(cindex, TNKHW_CT_LOC_TX_MSS, tx_mss);
	writel(0xF, tnkhw_ioaddr + TNK_REG_CT_CPU_WRMASK);

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
}

void tnkhw_dma_interrupt_disable(void)
{
	uint32_t enabled;
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);

	enabled = readl(tnkhw_ioaddr + TNK_REG_INTR_EN);
	enabled &= ~(TNK_INTR_TRX | TNK_INTR_TTX_ACK);
	writel(enabled, tnkhw_ioaddr + TNK_REG_INTR_EN);

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
}

void tnkhw_dma_interrupt_enable(void)
{
	uint32_t enabled = readl(tnkhw_ioaddr + TNK_REG_INTR_EN);
	enabled |= (TNK_INTR_TRX | TNK_INTR_TTX_ACK);
	writel(enabled, tnkhw_ioaddr + TNK_REG_INTR_EN);
}

static int tnkhw_map_skb(struct sk_buff *skb, dma_addr_t *dma)
{
	struct tcp_skb_cb *tcp_cb = TCP_SKB_CB(skb);
	struct tnkcb *cb = &(tcp_cb->header.tcb);

	BUG_ON(cb->magic != TNK_MAGIC);

	if (cb->dma) {
		pr_err("%s ERROR skb is already DMA mapped!\n", __func__);
		return 0;
	}

	*dma = dma_map_single(tnk_dev,
			      skb->data, DMA_BUFFER_SIZE, DMA_FROM_DEVICE);

	if (dma_mapping_error(tnk_dev, *dma)) {
		pr_err("%s: DMA Mapping Error\n", __func__);
		return -ENOMEM;
	}

	return 0;
}

static int tnkhw_alloc_and_map_skb(struct sk_buff **skb, dma_addr_t *dma)
{
	struct tnkcb *cb;
	int addr;

	*skb = tnk_alloc_skb(DMA_BUFFER_SIZE + TNK_TCP_HEADER_RESERVED,
			GFP_ATOMIC | __GFP_NOWARN | __GFP_HIGH);
	if (unlikely(*skb == NULL)) {
		pr_warn("%s: Rx init fails; skb is NULL\n", __func__);
		return -ENOMEM;
	}
	skb_reserve(*skb, TNK_TCP_HEADER_RESERVED);

	addr = (uint32_t) (*skb)->data;

	if ((addr % 32) != 0) {
		/*  Assuming 2k buffer size, we have room for realigning */
		TNK_DBG("%s: Unaligned buffer\n", __func__);
		skb_reserve(*skb, 8 - (addr % 32));
	}

	/* Initialise the tnk private area */
	cb = &(TCP_SKB_CB(*skb)->header.tcb);
	cb->magic = TNK_MAGIC;
	cb->type = TNK_TYPE_FREE;
	cb->dma = 0;

	if (tnkhw_map_skb(*skb, dma) != 0) {
		tnk_free_skb(*skb, SKB_FROM_RECV);
		return -ENOMEM;
	}

	cb->dma = *dma;

	return 0;
}

static int tnkhw_unmap_skb(struct sk_buff *skb)
{
	struct tcp_skb_cb *tcp_cb = TCP_SKB_CB(skb);
	struct tnkcb *cb = &(tcp_cb->header.tcb);

	BUG_ON(cb->magic != TNK_MAGIC);

	if (!cb->dma) {
		pr_err("%s ERROR skb is not DMA mapped!\n", __func__);
		return 0;
	}

	dma_unmap_single(tnk_dev, cb->dma, DMA_BUFFER_SIZE, DMA_FROM_DEVICE);

	cb->dma = 0;
	return 0;
}

static int tnkhw_unmap_and_free_skb(struct sk_buff *skb)
{
	tnkhw_unmap_skb(skb);
	tnk_free_skb(skb, SKB_FROM_RECV);

	return 0;
}

#define TNK_RXBYPASSMODE_BIT						\
	(TNK_MASK_CONFIG0_RXBYPASSMODE << TNK_OFFSET_CONFIG0_RXBYPASSMODE)
#define TNK_IPV6_EN_BIT							\
	(TNK_MASK_CONFIG0_IPV6_EN << TNK_OFFSET_CONFIG0_IPV6_EN)

static inline void tnkhw_toe_config(void)
{
	uint32_t reg_val = readl(tnkhw_ioaddr + TNK_REG_TOE_CONFIG0);

	/*  Disable TOE By-Pass mode (enabled by default) */
	reg_val &= ~TNK_RXBYPASSMODE_BIT;

#ifdef CONFIG_IPV6
	/*  Enable IPv6 (disabled by default), if kernel support enabled */
	reg_val |= TNK_IPV6_EN_BIT;
#endif

	writel(reg_val, tnkhw_ioaddr + TNK_REG_TOE_CONFIG0);
}

static inline void tnkhw_ipv6_enable(int enabled)
{
	uint32_t reg_val = readl(tnkhw_ioaddr + TNK_REG_TOE_CONFIG0);

	if (enabled)
		reg_val |= TNK_IPV6_EN_BIT;
	else
		reg_val &= ~TNK_IPV6_EN_BIT;

	writel(reg_val, tnkhw_ioaddr + TNK_REG_TOE_CONFIG0);
}

static int tnkhw_rx_dma_init(void)
{
	int i, err;
	struct tnkhw_rx_dma_info *r = &tnk_rx_dma;
	struct tnk_rx_dma_desc *p = NULL;

	TNK_DBG("%s\n", __func__);

	r->skbuff =
	    kzalloc(sizeof(struct sk_buff *) * tnk_rx_fifo, GFP_KERNEL);

	if (!r->skbuff) {
		pr_err("%s:ERROR allocating DMA Rx skbuff pointers\n",
		       __func__);
		return -ENOMEM;
	}

	r->skbuff_dma =
	    kzalloc(sizeof(dma_addr_t) * tnk_rx_fifo, GFP_KERNEL);
	if (!r->skbuff_dma) {
		pr_err("%s:ERROR allocating DMA Rx skbuff DMA pointers\n",
		       __func__);
		return -ENOMEM;
	}

	/* TODO - Ensure that descriptors are 128-bit (16-byte) aligned
	 * dma_alloc_coherent should normally return page-aligned memory,
	 * but it may not be guaranteed.  Low priority for now.
	 */
	r->desc_list =
	    (struct tnk_rx_dma_desc *)
		dma_alloc_coherent(tnk_dev,
				   tnk_rx_fifo *
				   sizeof(struct tnk_rx_dma_desc),
				   &r->desc_list_phys,
				   GFP_KERNEL);
	if (r->desc_list == NULL) {
		pr_err("%s:ERROR allocating DMA Rx descriptors\n", __func__);
		return -ENOMEM;
	}

	TNK_DBG("%s alloc desc_list %p\n", __func__, r->desc_list);
	/* Clear the descriptors initially */
	memset(r->desc_list, 0,
	       tnk_rx_fifo * sizeof(struct tnk_rx_dma_desc));

	/*  Pre-fill the RX Descriptor ring with empty buffers */
	for (i = 0; i < tnk_rx_fifo; i++) {
		p = r->desc_list + i;

		err = tnkhw_alloc_and_map_skb(&r->skbuff[i], &r->skbuff_dma[i]);
		if (err != 0)
			return err;

		/* Assign the skb data buffer phys address to the descriptor */
		p->base.des2 = r->skbuff_dma[i];
		p->base.des01.erx.buffer1_size = DMA_BUFFER_DESC_SIZE;

		/* Assign the descriptor ownership to hardware */
		p->base.des01.erx.own = 1;
		/* Set mutipacket int,the default value is 16 packet per int*/
		if (i % TNK_RX_NAPI_LIMIT)
			p->base.des01.erx.disable_ic = 1;
	}
	/* Mark the last descriptor as the end of the ring */
	p->base.des01.erx.end_ring = 1;

	/* Reset our head/tail indices */
	r->head = 0;
	r->tail = 0;
	spin_lock_init(&r->rx_desc_lock);

	/*  Pre-fill the RX recycle queue ring with additional empty buffers */
	skb_queue_head_init(&r->free_skbs);
#ifndef TNK_RX_CHANNEL_FLOW_CONTROL
	for (i = 0; i < TNK_RX_SW_Q_SIZE; i++) {
		struct sk_buff *skb;
		dma_addr_t dma;

		err = tnkhw_alloc_and_map_skb(&skb, &dma);
		if (err != 0)
			return err;

		skb_queue_head(&r->free_skbs, skb);
	}
#endif

	TNK_DBG("%s init dma ops\n", __func__);
	/* Configure the Rx DMA queue (alternate descriptor size enabled) */
	dwmac1000_dma_ops.init(tnkhw_ioaddr, DMA_CHANNEL_TOERX, DMA_BURST_LEN,
			       0, r->desc_list_phys, 1);
	STMMAC_SYNC_BARRIER();
	tnkhw_trx_desc_add(tnk_rx_fifo);

	TNK_DBG("%s done\n", __func__);
	return 0;
}

static void tnkhw_rx_dma_free(void)
{
	int i;
	struct tnkhw_rx_dma_info *r = &tnk_rx_dma;
	struct sk_buff *skb;

	while ((skb = skb_dequeue(&r->free_skbs))) {
		tnk_mem_free_pool_dequeued(skb);
		tnkhw_unmap_and_free_skb(skb);
	}

	if (r->skbuff) {
		for (i = 0; i < tnk_rx_fifo; i++) {
			if (r->skbuff[i]) {
				tnkhw_unmap_and_free_skb(r->skbuff[i]);
				r->skbuff[i] = NULL;
			}
		}
	}

	/* Free the region of consistent memory previously allocated for
	 * the DMA */
	if (r->desc_list) {
		dma_free_coherent(tnk_dev,
				tnk_rx_fifo * sizeof(struct tnk_rx_dma_desc),
				r->desc_list, r->desc_list_phys);
	}
	kfree(r->skbuff_dma);
	r->skbuff_dma = NULL;
	kfree(r->skbuff);
	r->skbuff = NULL;

}

static int tnkhw_tx_dma_init(void)
{
	int cindex;

	tnk_tx_dma_list =
	    kzalloc(tnk_max_connections * sizeof(struct tnkhw_tx_dma_info),
		    GFP_KERNEL);
	TNK_DBG("%s alloc %p\n", __func__, tnk_tx_dma_list);
	if (!tnk_tx_dma_list) {
		pr_err("%s: Failed to allocate memory for TX DMA info table\n",
		       __func__);
		return -ENOMEM;
	}

	for (cindex = TNK_TTX_CINDEX_START; cindex < tnk_max_connections;
	     cindex++) {
		int i;
		struct tnkhw_tx_dma_info *t = &tnk_tx_dma_list[cindex];
		struct tnk_ttx_dma_desc *last = NULL;

		t->skbuff = kzalloc(tnk_tx_fifo * sizeof(struct sk_buff *),
				    GFP_KERNEL);
		if (!t->skbuff) {
			pr_err("%s: Failed to allocate memory for TX skbuff",
					__func__);
			pr_err(" table\n");
			return -ENOMEM;
		}

		/* TODO - Ensure that descriptors are 128-bit (16-byte) aligned
		 * (in physical memory) dma_alloc_coherent should normally
		 * return page-aligned memory, but it may not be guaranteed.
		 */
		t->desc_list =
		    (struct tnk_ttx_dma_desc *)
			dma_alloc_coherent(tnk_dev,
					   tnk_tx_fifo *
					   sizeof(struct tnk_ttx_dma_desc),
					   &t->desc_list_phys,
					   GFP_KERNEL);
		if (!t->desc_list) {
			pr_err("%s: ERROR allocating DMA Tx descriptor list\n",
			       __func__);
			return -ENOMEM;
		}

		memset(t->desc_list, 0,
		       tnk_tx_fifo * sizeof(struct tnk_ttx_dma_desc));
		/* Clear the Tx descriptors, construct a circular linked list */
		last = &t->desc_list[tnk_tx_fifo - 1];
		for (i = 0; i < tnk_tx_fifo; i++) {
			struct tnk_ttx_dma_desc *desc = &t->desc_list[i];
			int desc_offset = (i * sizeof(struct tnk_ttx_dma_desc));
			last->next_desc_ptr =
			    (uint32_t) t->desc_list_phys + desc_offset;
			last = desc;
		}

		spin_lock_init(&t->tx_desc_lock);
		t->head = 0;
		t->tail = 0;
#ifdef DBG_TX_DMA
		t->head_overflow = 0;
#endif

		/* Add this descriptor ring to the TTX DB */
		tnkhw_ttx_db_init(cindex, t->desc_list_phys);

		init_waitqueue_head(&t->waitqueue);
	}

	return 0;
}

static void tnkhw_tx_dma_free(void)
{
	unsigned cindex;
	struct tnk_ttx_dma_desc *desc;

	if (tnk_tx_dma_list == NULL)
		return;

	for (cindex = TNK_TTX_CINDEX_START; cindex < tnk_max_connections;
	     cindex++) {
		int i;
		struct tnkhw_tx_dma_info *t = &tnk_tx_dma_list[cindex];

		for (i = 0; i < tnk_tx_fifo ; i++) {
			if (t->desc_list) {
				desc = &t->desc_list[i];
				if (desc->buffer_ptr &&
					(desc->buffer_ptr !=
						TNK_TTX_BUF_RECLAIMED)) {
					dma_unmap_single(tnk_dev,
							desc->buffer_ptr,
							desc->buffer_size,
							DMA_TO_DEVICE);
					desc->buffer_ptr = 0;
				}
			}

			if (t->skbuff && t->skbuff[i]) {
				tnk_free_skb(t->skbuff[i], SKB_FROM_SEND);
				t->skbuff[i] = NULL;

			}
		}

		/* Free the region of consistent memory previously allocated for
		 * the DMA */
		if (t->desc_list) {
			dma_free_coherent(tnk_dev,
					tnk_tx_fifo *
					sizeof(struct tnk_ttx_dma_desc),
					t->desc_list,
					t->desc_list_phys);
		}
		kfree(t->skbuff);
		t->skbuff = NULL;
	}

	kfree(tnk_tx_dma_list);
	tnk_tx_dma_list = NULL;
}

void tnkhw_rx_refill(void)
{
	unsigned int rxsize = tnk_rx_fifo;
	struct tnkhw_rx_dma_info *r = &tnk_rx_dma;
	unsigned count = 0;
	unsigned long long bitwize_tnk;

	spin_lock(&r->rx_desc_lock);

	bitwize_tnk = r->head + 0x100000000ull;

	TNK_DBG("%s begin\n", __func__);

	for (; ((r->head > r->tail) || ((r->head < r->tail) &&
		(bitwize_tnk > r->tail)));  r->tail++) {
		unsigned int entry = r->tail % rxsize;
		struct tnk_rx_dma_desc *p = r->desc_list + entry;

		TNK_DBG("%s skb for entry %d is %p\n", __func__,
			entry, r->skbuff[entry]);

		if (likely(r->skbuff[entry] == NULL)) {
			struct sk_buff *skb;
			int err;

			skb = skb_dequeue(&r->free_skbs);
			tnk_mem_free_pool_dequeued(skb);

			if (skb == NULL) {
				err =
				    tnkhw_alloc_and_map_skb(&skb,
							    &r->
							    skbuff_dma[entry]);

				if (unlikely(err != 0)) {
					pr_warn
					    ("%s: Failed to allocate new skb\n",
					     __func__);
					/* Check for buffer starvation due to
					 * lack of memory as this may stall the
					 * rx queue until memory frees up and
					 * we're (hopefully) woken up by a
					 * transmit event */
					break;
				}
				tnk_stats.rx_refills_alloc++;
			} else {
				struct tnkcb *cb
					= &(TCP_SKB_CB(skb)->header.tcb);
				r->skbuff_dma[entry] = cb->dma;
			}

			count_skb++;
			TNK_DBG("%s refill skb\n", __func__);
			r->skbuff[entry] = skb;
			p->base.des2 = r->skbuff_dma[entry];
			p->base.des01.erx.buffer1_size = DMA_BUFFER_DESC_SIZE;
		}
		p->extra.flags = 0;

		/* barrier here because hardware may read this desc
		 * before desc is right configured.
		 */
		STMMAC_SYNC_BARRIER();

		p->base.des01.erx.own = 1;
		BUG_ON(p->base.des01.erx.own == 0);
		count++;
	}

	if (count > 0) {
		unsigned long flags;

		TNK_DBG("%s tell hw about %d new buffs\n", __func__, count);

		/* This barrier is important here.  It is required to ensure
		 * the ARM CPU flushes it's DMA write buffers before proceeding
		 * to the next instruction, to ensure that the TOE will see
		 * our descriptor changes in memory */
		STMMAC_SYNC_BARRIER();

		/*  Let the hardware know that new buffers are available */
		spin_lock_irqsave(&tnkhw_reg_lock, flags);
		tnkhw_trx_desc_add(count);
		spin_unlock_irqrestore(&tnkhw_reg_lock, flags);

		TNK_DBG("%s enable dma rcv on chan %d io=%p\n", __func__,
			DMA_CHANNEL_TOERX, tnkhw_ioaddr);
		dwmac1000_dma_ops.enable_dma_receive(tnkhw_ioaddr,
						     DMA_CHANNEL_TOERX);

		tnk_stats.rx_refills += count;
		TNK_DBG("%s: alloc_skb: %d\n", __func__, count_skb);
	}

	spin_unlock(&r->rx_desc_lock);

	TNK_DBG("%s end\n", __func__);
}

static void tnkhw_fix_tcp_seq(struct sk_buff *skb)
{
	struct ethhdr *eth;
	unsigned char *ip_start, *tcp_start;
	const struct iphdr *iph;
	const struct tcphdr *th;

	eth = (struct ethhdr *)skb->data;

#define ETH_P_802_3_MIN 0x0600
	if (!(ntohs(eth->h_proto) >= ETH_P_802_3_MIN))
		return;

	ip_start = skb->data + ETH_HLEN;
#ifdef STMMAC_VLAN_TAG_USED
	if (eth->h_proto == cpu_to_be16(ETH_P_8021Q))
		ip_start += VLAN_HLEN;
#endif

	iph = (struct iphdr *)ip_start;

	tcp_start = ip_start + iph->ihl * 4;
	th = (struct tcphdr *)tcp_start;
	skb->len = ETH_HLEN + ntohs(iph->tot_len);
#ifdef STMMAC_VLAN_TAG_USED
	if (eth->h_proto == cpu_to_be16(ETH_P_8021Q))
		skb->len += VLAN_HLEN;
#endif

	TCP_SKB_CB(skb)->seq = ntohl(th->seq);
	TCP_SKB_CB(skb)->end_seq = TCP_SKB_CB(skb)->seq + th->syn + th->fin +
		(ntohs(iph->tot_len) - iph->ihl * 4 - th->doff * 4);
}

int tnkhw_rx(int limit)
{
	unsigned int rxsize = tnk_rx_fifo;
	struct tnkhw_rx_dma_info *r = &tnk_rx_dma;
	unsigned int entry;
	unsigned int next_entry;
	unsigned int count = 0;
	unsigned int max_valid;
	struct tnk_rx_dma_desc *p;
	struct tnk_rx_dma_desc *p_next;

	TNK_DBG("%s begin\n", __func__);

	if (unlikely((limit < 0) || (limit > rxsize)))
		limit = rxsize;

	spin_lock(&r->rx_desc_lock);

	entry = r->head % rxsize;
	p = r->desc_list + entry;

	/*  Prevent overtaking the tail pointer on wrap-around */
	max_valid = r->tail + rxsize - r->head;
	if (limit > max_valid)
		limit = max_valid;

	count = 0;
	while (!p->base.des01.erx.own) {
		int status;
		int cindex = p->extra.cid;

		if (count >= limit)
			break;

		TNK_DBG("%s update counters\n", __func__);
		/*  Update counters */
		count++;

		/*  Prefetch the next entry in the queue */
		next_entry = (++r->head) % rxsize;
		p_next = r->desc_list + next_entry;
		prefetch(p_next);

#ifdef TNK_HW_DEBUG
		{
			int i;
			uint32_t *desc_ptr = (uint32_t *) p;
			TNK_DBG("Rx Descriptor contents (in words)\n");
			for (i = 0;
			     i <
			     sizeof(struct tnk_rx_dma_desc) / sizeof(uint32_t);
			     i++) {
				TNK_DBG("Offset %d, value 0x%08X\n", i,
					desc_ptr[i]);
			}
		}
#endif

		if (((cindex) < TNK_TTX_CINDEX_START)
		    || ((cindex) >= tnk_max_connections)) {
			pr_err("%s descriptor %d has invalid index %d\n",
			       __func__, entry, cindex);
			status = discard_frame;
		} else {
			tnk_conn_stats[cindex].rx_pkts++;

			/* TODO: half duplex problem, fix next version */
#if 0
			TNK_DBG("%s read status\n", __func__);
			/* read the status of the incoming frame */
			status = enh_desc_ops.rx_status(NULL,
							&tnk_stmmac_stats,
							&p->base);
			/*we do not know the exact order of the frame,
			 so all frames are theat as out of order
			 */
			if (status == csum_none)
#endif
				status = out_of_order_frame;

		}

		if (unlikely(p->extra.flags & TNKHW_CB_FLAG_TOE_ERR)) {

			tnk_conn_stats[cindex].rx_toe_err++;

			/* NOTE
			 * We don't discard the frame automatically if this
			 * flag is set because of a known issue in the TOE
			 * which can cause this flag to be set erroneously
			 * if the IPC/CSUM bit is asserted in RDES0
			 * So we only discard the frame if one of the following
			 * error flags are set, effectively ignoring the
			 * RDS0 IPC/CSUM bit, which is safe because the TOE
			 * does it's own IP and TCP checks anyway.
			 */

			if (p->extra.flags & TNKHW_CB_FLAG_PREMATURE_EOP) {
				tnk_conn_stats[cindex].rx_premature_eop++;
				status = discard_frame;
			}

			if (p->extra.flags & TNKHW_CB_FLAG_BAD_ETHERTYPE) {
				tnk_conn_stats[cindex].rx_bad_ethertype++;
				status = discard_frame;
			}

			if (p->extra.flags & TNKHW_CB_FLAG_BAD_DST_MACADDR) {
				tnk_conn_stats[cindex].rx_bad_dst_macaddr++;
				status = discard_frame;
			}

			if (p->extra.flags & TNKHW_CB_FLAG_TCP_CSUM_ERR) {
				tnk_conn_stats[cindex].rx_tcp_csum_err++;
				status = discard_frame;
			}

			if (p->extra.flags & TNKHW_CB_FLAG_IPHDR_CSUM_ERR) {
				tnk_conn_stats[cindex].rx_iphdr_csum_err++;
				status = discard_frame;
			}

			if (p->extra.flags & TNKHW_CB_FLAG_NON_TCP_PROT) {
				tnk_conn_stats[cindex].rx_non_tcp_prot++;
				status = discard_frame;
			}

			if (p->extra.flags & TNKHW_CB_FLAG_DST_IP_MISMATCH) {
				tnk_conn_stats[cindex].rx_bad_dst_ipaddr++;
				status = discard_frame;
			}

			if (p->extra.flags & TNKHW_CB_FLAG_CONNECTION_ERR) {
				tnk_conn_stats[cindex].rx_connection_err++;
				status = discard_frame;
			}

			if (p->extra.flags & TNKHW_CB_FLAG_RX_SEQNUM_ERR) {
				tnk_conn_stats[cindex].rx_bad_seq_num++;
				/* seq num erro frame is out of order frame **/
				status = out_of_order_frame;
			}

			if (p->extra.flags & TNKHW_CB_FLAG_PKT_DROPPED) {
				tnk_conn_stats[cindex].rx_busy_pkt_drop++;
				status = discard_frame;
			}

			if (status == discard_frame) {
				TNK_DBG("%s dropping rx pkt, desc %d,",
						__func__, entry);
				TNK_DBG(" cindex %d, flags=0x%x, status=%d\n",
						cindex, p->extra.flags, status);
				/* Errored frame, increment stats
				 * NOTE that the skbuff is left in the Rx DMA
				 * ring, to be reused later
				 */
				tnk_conn_stats[cindex].rx_dropped++;
			}
		}

		if (likely(status == out_of_order_frame)) {
			struct sk_buff *skb;
			int frame_len;

			skb = r->skbuff[entry];
			if (unlikely(!skb)) {
				pr_err("%s: Inconsistent Rx descriptor chain,",
						__func__);
				pr_err(" no skb at index %d\n", entry);
				tnk_conn_stats[cindex].rx_dropped++;
				break;
			}
			frame_len = p->base.des01.erx.frame_length;

			tnkhw_unmap_skb(skb);
			r->skbuff[entry] = NULL;
			r->skbuff_dma[entry] = 0;
			skb_put(skb, frame_len);

			TCP_SKB_CB(skb)->seq = p->extra.seq_num;
			TCP_SKB_CB(skb)->end_seq =
				TCP_SKB_CB(skb)->seq + frame_len;
			TCP_SKB_CB(skb)->header.tcb.flag = p->extra.flags;
			if (TCP_SKB_CB(skb)->header.tcb.flag &
					TNKHW_CB_FLAG_FULL_PKT)
				tnkhw_fix_tcp_seq(skb);
			tnk_rxcallback(skb, cindex, p->extra.flags,
					       p->extra.urg_ptr);
		}
		entry = next_entry;
		p = p_next;	/* use prefetched values */
	}

	spin_unlock(&r->rx_desc_lock);

	TNK_DBG("%s do refill\n", __func__);
	tnkhw_rx_refill();

	TNK_DBG("%s done\n", __func__);
	return count;
}

void tnkhw_init_out_of_order(unsigned int cindex)
{
	unsigned long flags;
	unsigned val;
	int valid = 0;
	int count = 0;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);
	/*  set iso flag1&&iso flag2 to 0 */
	val = TNK_HW_OUT_OF_ORDER_IS01_IS02_FLAG | (cindex << 4) | (1 << 3);

	while (count < 100000) {
		count++;
		valid = readl(tnkhw_ioaddr + TNK_HW_OUT_OF_ORDER_CRTL);
		valid = (valid >> 12) & 0x1;
		if (valid)
			break;
	}

	if (!valid)
		pr_err("wait out of order cpu operation timeout\n");
	writel(0, tnkhw_ioaddr + TNK_HW_OUT_OF_ORDER_WVAL);
	writel(val, tnkhw_ioaddr + TNK_HW_OUT_OF_ORDER_CRTL);
	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);

}
#if 0
void tnkhw_update_fin_seqnum(unsigned int cindex)
{
	unsigned int data;
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);

	/* update seqnum when fin recieved  */
	tnkhw_ct_read(cindex, 8, &data);
	data += 1;
	tnkhw_ct_write(cindex, 8, data);

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
}
#endif
void tnkhw_update_rx_seqnum(unsigned int cindex, unsigned int seq_num)
{
	unsigned long flags;
	struct tnkhw_connection conn;
	int i, offset;
	uint32_t *data = (uint32_t *)&conn;
	int max_conn;

	conn.word30_updated_tx_ack_num = seq_num;
	conn.word31_updated_flag = 1;

	offset = offsetof(struct tnkhw_connection, word30_updated_tx_ack_num);
	offset = offset/sizeof(uint32_t);

	if (offset != 30)
		pr_warn("error calculate of offset :%d\n", offset);
	offset = 30;
	spin_lock_irqsave(&tnkhw_reg_lock, flags);

	/*  Write the partial connection table entry, one word at a time */
	max_conn = sizeof(struct tnkhw_connection) / sizeof(uint32_t);
	for (i = offset; i < max_conn; i++)
		tnkhw_ct_write(cindex, i, data[i]);

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
}

/*  Return number of free Tx descriptors */
static inline int tnkhw_tx_avail_desc(unsigned cindex)
{
	struct tnkhw_tx_dma_info *t = &tnk_tx_dma_list[cindex];

#ifdef DBG_TX_DMA
	if ((t->tail + tnk_tx_fifo - t->head) > tnk_tx_fifo) {
		pr_warn("DEBUG_L:(%s,%d): wrong index, cindex=%d, ",
			__func__, __LINE__, cindex);
		dump_tx_dma_info(t);
		dump_stack();
	}
#endif
	return t->tail + tnk_tx_fifo - t->head - 1;
}

/* We have a 16MB limit on the amount of data that we can have in the pipeline
 * for any given channel
 */
int tnkhw_tx_avail_bytes(unsigned cindex)
{
	struct tnkhw_tx_dma_info *t = &tnk_tx_dma_list[cindex];
	int pipelined_bytes = atomic_read(&t->pipelined_bytes);

	/* This counter should never go out of range  */
	BUG_ON(pipelined_bytes < 0);
	BUG_ON(pipelined_bytes > TNK_TTX_DATA_LIMIT);

	return TNK_TTX_DATA_LIMIT - pipelined_bytes;
}

void tnkhw_skb_recycle(struct sk_buff *skb)
{
#ifdef TNK_RX_CHANNEL_FLOW_CONTROL
	int limit = tnk_rx_recycle_fifo;
#else
	int limit = (tnk_rx_fifo + TNK_RX_SW_Q_SIZE);
#endif

	if ((skb_queue_len(&tnk_rx_dma.free_skbs) < limit)) {
		struct tnkcb *cb = &(TCP_SKB_CB(skb)->header.tcb);
		if (likely(cb && cb->magic == TNK_MAGIC)) {
			skb->tail = skb->data = skb->head;
			skb->len = 0;

			skb_reserve(skb, TNK_TCP_HEADER_RESERVED);
			tnkhw_map_skb(skb, &cb->dma);

			skb_queue_head(&tnk_rx_dma.free_skbs, skb);
			tnk_mem_free_pool_queued(skb);
		} else {
			tnk_free_skb(skb, SKB_FROM_RECV);
		}
	} else {
		tnk_free_skb(skb, SKB_FROM_RECV);
	}
}

void tnkhw_tx_channel_reclaim(unsigned cindex, int cleanup)
{
	struct tnkhw_tx_dma_info *t;
	unsigned int txsize = tnk_tx_fifo;
	unsigned count = 0, acked_bytes = 0;
	unsigned long flags;
#ifdef DBG_TX_DMA
	unsigned int max_free_count;
#endif

	if (unlikely(tnk_tx_dma_list == NULL))
		return;

	t = &tnk_tx_dma_list[cindex];

	spin_lock_irqsave(&t->tx_desc_lock, flags);

#ifdef DBG_TX_DMA
	max_free_count = t->head - t->tail;
	if ((t->head >= t->tail) && ((t->head - t->tail) < tnk_tx_fifo) &&
		!(t->head_overflow)) {
		/* normal */
	} else if ((t->head < t->tail) &&
		(t->tail + tnk_tx_fifo - 1 >= t->head) &&
		(t->head_overflow)) {
		/* t->head has overflow */
	} else {
		pr_warn("DEBUG_L:(%s,%d): wrong index. index=%d, cleanup=%d, ",
			__func__, __LINE__, cindex, cleanup);
		dump_tx_dma_info(t);
		dump_stack();
	}
#endif

	while (t->tail != t->head) {
		unsigned int entry = t->tail % txsize;
		struct sk_buff *skb = t->skbuff[entry];
		struct tnk_ttx_dma_desc *p = t->desc_list + entry;

		/*  On shutdown/cleanup, we need to reclaim ALL buffers in the
		 *  pipeline */
		if (unlikely(cleanup))
			p->hw_own = 0;

		/* Check if the descriptor is owned by the DMA. */
		if (p->hw_own)
			break;

		if (likely(p->buffer_ptr)
		    && p->buffer_ptr != TNK_TTX_BUF_RECLAIMED) {
			acked_bytes += p->buffer_size;

			dma_unmap_single(tnk_dev, p->buffer_ptr,
					 p->buffer_size, DMA_TO_DEVICE);

			/*  Mark the buffer pointer as invalid */
			/*  p->buffer_ptr = 0; */
			p->buffer_ptr = TNK_TTX_BUF_RECLAIMED;

			if (likely(skb != NULL)) {
				tnk_free_skb(skb, SKB_FROM_SEND);
				t->skbuff[entry] = NULL;
			}
		}

		entry = (++t->tail) % txsize;
	#ifdef DBG_TX_DMA
		if (!(t->tail)) {
			pr_warn("(%s,%d): t->tail will overflow.",
				__func__, __LINE__);
			pr_warn("index=%d, cleanup=%d,",
				cindex, cleanup);
			pr_warn("max_free_count=%d, count=%d\n",
				max_free_count, count);
			dump_tx_dma_info(t);
			t->head_overflow = 0;
		}
	#endif
		count++;
	#ifdef DBG_TX_DMA
		if ((t->head >= t->tail) &&
			((t->head - t->tail) < tnk_tx_fifo) &&
			!(t->head_overflow)) {
			/* normal */
		} else if ((t->head < t->tail) &&
				(t->tail + tnk_tx_fifo - 1 >= t->head) &&
				(t->head_overflow)) {
			/* t->head has overflow */
		} else {
			pr_warn("(%s,%d): wrong index. index=%d, cleanup=%d, ",
					__func__, __LINE__, cindex, cleanup);
			dump_tx_dma_info(t);
			dump_stack();
		}
	#endif
	}
	spin_unlock_irqrestore(&t->tx_desc_lock, flags);

	if (count) {
		TNK_DBG("%s: Reclaimed %d descriptors for channel %d\n",
			__func__, count, cindex);

		atomic_sub(acked_bytes, &t->pipelined_bytes);

		/*  TODO - remove this flag if no longer needed */
		if (t->overflow)
			t->overflow = 0;

		tnk_conn_stats[cindex].tx_desc_ackd += count;
		tnk_conn_stats[cindex].tx_bytes_ackd += acked_bytes;
		tnk_stats.tx_returns += acked_bytes;

		if (likely(!cleanup)) {
			/* Trigger a callback to let the socket know that
			 * there's room on the queue again to transmit more
			 * data */
			if (tnk_txcallback)
				tnk_txcallback(cindex, acked_bytes);
			else
				pr_err("%s: ERROR: TX Callback not set\n",
				       __func__);
		}

		/* once reclaim skb, wakeup transmit */
		if (waitqueue_active(&t->waitqueue))
			wake_up_interruptible(&t->waitqueue);
	}
}

int tnkhw_tx_reclaim(void)
{
	unsigned cindex = 0;
	int overflow = 0;
	unsigned long flags;

	/*  Read from ACK FIFO until empty */
	do {
		spin_lock_irqsave(&tnkhw_reg_lock, flags);
		overflow |= tnkhw_ttx_ackd_cid_read(&cindex);
		spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
		TNK_DBG("%s reclaim cindex %d\n", __func__, cindex);

		/*  If overflow set, we'll check all channels below */
		if (cindex > 0)
			tnkhw_tx_channel_reclaim(cindex, 0);

	} while (cindex > 0);

	if (overflow) {
		tnk_stats.ttx_ack_ovflow++;
		/*  ACK FIFO overflowed, so we need to scan all channels for
		 *  free TX buffers */
		return 1;
	}

	return 0;
}

int tnkhw_dma_start(tnkhw_poll_wakeup_cb_t poll_wakeup_cb,
		    tnkhw_rx_cb_t rx_cb,
		    tnkhw_txfree_cb_t tx_cb,
		    tnkhw_tx_max_retries_cb_t tx_max_retries_cb)
{
	tnk_pollwakeup = poll_wakeup_cb;
	tnk_rxcallback = rx_cb;
	tnk_txcallback = tx_cb;
	tnk_txmaxretries = tx_max_retries_cb;

	dwmac1000_dma_ops.start_rx(tnkhw_ioaddr, DMA_CHANNEL_TOERX);

	return 0;
}

void tnkhw_dma_stop(void)
{
	int cindex;

	dwmac1000_dma_ops.stop_rx(tnkhw_ioaddr, DMA_CHANNEL_TOERX);

	/* Clear out any buffers left in the DMA rings
	 * ASSUMPTION - all active connections have been removed
	 * at this point
	 * TODO - add a check for the above assumption
	 */
	for (cindex = TNK_TTX_CINDEX_START; cindex < tnk_max_connections;
	     cindex++)
		tnkhw_tx_channel_reclaim(cindex, 1);

	tnk_pollwakeup = NULL;
	tnk_rxcallback = NULL;
	tnk_txcallback = NULL;
	tnk_txmaxretries = NULL;
}

#define TNK_DMA_TTX_DFETCH0_ERR \
	(TNK_MASK_TTX_ERR_DFETCH0 << TNK_OFFSET_TTX_ERR_DFETCH0)
#define TNK_DMA_TTX_DFETCH1_ERR \
	(TNK_MASK_TTX_ERR_DFETCH1 << TNK_OFFSET_TTX_ERR_DFETCH1)
#define TNK_DMA_TTX_DFETCH2_ERR \
	(TNK_MASK_TTX_ERR_DFETCH2 << TNK_OFFSET_TTX_ERR_DFETCH2)
#define TNK_DMA_TTX_DFETCH_ERRORS \
	(TNK_DMA_TTX_DFETCH0_ERR | TNK_DMA_TTX_DFETCH1_ERR \
	 | TNK_DMA_TTX_DFETCH2_ERR)

#define TNK_DMA_TTX_RETRY0_ERR \
	(TNK_MASK_TTX_ERR_RETRY0 << TNK_OFFSET_TTX_ERR_RETRY0)
#define TNK_DMA_TTX_RETRY1_ERR \
	(TNK_MASK_TTX_ERR_RETRY1 << TNK_OFFSET_TTX_ERR_RETRY1)
#define TNK_DMA_TTX_RETRY_ERRORS \
	(TNK_DMA_TTX_RETRY0_ERR | TNK_DMA_TTX_RETRY1_ERR)

#define TNK_DMA_TTX_ACKD0_ERR \
	(TNK_MASK_TTX_ERR_ACKD0 << TNK_OFFSET_TTX_ERR_ACKD0)
#define TNK_DMA_TTX_ACKD1_ERR \
	(TNK_MASK_TTX_ERR_ACKD1 << TNK_OFFSET_TTX_ERR_ACKD1)
#define TNK_DMA_TTX_ACKD2_ERR \
	(TNK_MASK_TTX_ERR_ACKD2 << TNK_OFFSET_TTX_ERR_ACKD2)
#define TNK_DMA_TTX_ACKD_ERRORS \
	(TNK_DMA_TTX_ACKD0_ERR | TNK_DMA_TTX_ACKD1_ERR | TNK_DMA_TTX_ACKD2_ERR)


static void tnkhw_dump_tx_db(unsigned int cindex)
{
	int i;
	unsigned int offset = cindex * TNK_TTX_DMA_DB_ENTRY_SIZE
				+ TNK_DMA_TTX_DB_OFFSET;

	pr_info("%s:\n", __func__);

	for (i = 0; i < 4; i++)
		pr_info("word[%d]:%08x\n",
			i, readl(tnkhw_ioaddr + offset + i*4));
}

static void tnkhw_dump_debug_reg(void)
{
	int i = 0;

	pr_info("debug registers:\n");
	for (i = 0; i < 64; i++) {
		if ((i%16) == 0)
			printk(KERN_ERR "\n%x :",
				CONFIG_STMMAC_IOADDR + 0x8200 + i * 4);
		pr_info("%08x\t", readl(tnkhw_ioaddr + 0x8200 + i * 4));
	}

	pr_info("\n");

	for (i = 0; i < 64; i++) {
		if ((i%16) == 0)
			printk(KERN_ERR "\n%x :",
				CONFIG_STMMAC_IOADDR + 0x8400 + i * 4);
		pr_info("%08x\t", readl(tnkhw_ioaddr + 0x8400 + i * 4));
	}

	pr_info("\n");

	for (i = 0; i < 64; i++) {
		if ((i%16) == 0)
			printk(KERN_ERR "\n%x :",
				CONFIG_STMMAC_IOADDR + 0x1e100 + i * 4);
		pr_info("%08x\t", readl(tnkhw_ioaddr + 0x1e100 + i * 4));
	}

}

static void tnkhw_tx_err(void)
{
	/*  Read the DMA_TTX_ERR_STATUS register, and related CID registers */
	uint32_t err_status = readl(tnkhw_ioaddr + TNK_REG_DMA_TTX_ERR_STATUS);
	uint32_t cid;

	/*  Check for dfetch errors */
	if (err_status & TNK_DMA_TTX_DFETCH_ERRORS) {
		cid = readl(tnkhw_ioaddr + TNK_REG_DMA_TTX_DFETCH_ERR_CID);
		cid =
		    (cid >> TNK_OFFSET_TTX_DFETCH_ERR_CID) &
		    TNK_MASK_TTX_DFETCH_ERR_CID;

		if (err_status & TNK_DMA_TTX_DFETCH0_ERR)
			tnk_conn_stats[cid].tx_dfetch_desc++;
		if (err_status & TNK_DMA_TTX_DFETCH1_ERR)
			tnk_conn_stats[cid].tx_dfetch_data++;
		if (err_status & TNK_DMA_TTX_DFETCH2_ERR) {
			pr_err("%s: TTX error 0x%08x (cid %d)\n",
					__func__, err_status, cid);
			tnkhw_dump_tx_descriptors(cid);
			tnkhw_connection_dump(cid);
			tnkhw_dump_tx_db(cid);
			tnkhw_dump_debug_reg();
			tnk_conn_stats[cid].tx_dfetch_own++;
		}
	}

	/*  Check for retry errors */
	if (err_status & TNK_DMA_TTX_RETRY_ERRORS) {
		cid = readl(tnkhw_ioaddr + TNK_REG_DMA_TTX_RETRY_ERR_CID);
		cid =
		    (cid >> TNK_OFFSET_TTX_RETRY_ERR_CID) &
		    TNK_MASK_TTX_RETRY_ERR_CID;

		if (err_status & TNK_DMA_TTX_RETRY0_ERR)
			tnk_conn_stats[cid].tx_retry_desc++;
		if (err_status & TNK_DMA_TTX_RETRY1_ERR)
			tnk_conn_stats[cid].tx_retry_data++;
	}

	/*  Check for ackd errors */
	if (err_status & TNK_DMA_TTX_ACKD_ERRORS) {
		cid = readl(tnkhw_ioaddr + TNK_REG_DMA_TTX_ACKD_ERR_CID);
		cid =
		    (cid >> TNK_OFFSET_TTX_ACKD_ERR_CID) &
		    TNK_MASK_TTX_ACKD_ERR_CID;

		if (err_status & TNK_DMA_TTX_ACKD0_ERR)
			tnk_conn_stats[cid].tx_ackd_read++;
		if (err_status & TNK_DMA_TTX_ACKD1_ERR)
			tnk_conn_stats[cid].tx_ackd_size++;
		if (err_status & TNK_DMA_TTX_ACKD2_ERR)
			tnk_conn_stats[cid].tx_ackd_write++;
	}
}

int tnkhw_tx_desc_check(int cindex)
{
	struct tnkhw_tx_dma_info *t;
	unsigned int txsize = tnk_tx_fifo;
	unsigned long flags;
	int i;
	int ret = 0;

	if (unlikely(tnk_tx_dma_list == NULL))
		return ret;

	t = &tnk_tx_dma_list[cindex];

	spin_lock_irqsave(&t->tx_desc_lock, flags);

	for (i = 0; i < txsize; i++) {
		struct tnk_ttx_dma_desc *desc = t->desc_list + i;

		/* Check if the descriptor is owned by the DMA. */
		if (desc->hw_own) {
			desc->hw_own = 0;
			ret = 1;
		}
	}
	spin_unlock_irqrestore(&t->tx_desc_lock, flags);
	return ret;
}

int tnkhw_tx_send(struct sk_buff *skb, unsigned cindex)
{
	struct tnkhw_tx_dma_info *t = &tnk_tx_dma_list[cindex];
	unsigned int txsize = tnk_tx_fifo;
	unsigned int entry;
	unsigned int last;
	unsigned int count = 0;
	int entries_needed;
	uint32_t total_len = 0;
	int i;
	int nfrags = skb_shinfo(skb)->nr_frags;
	struct tnk_ttx_dma_desc *desc = NULL;
	unsigned int nopaged_len;
	unsigned long flags;
	unsigned long flags_tx;

	TNK_DBG("%s cindex is %d range is [%d..%d]\n",
		__func__, cindex, TNK_TTX_CINDEX_START, tnk_max_connections);

	TNK_CINDEX_VALIDATE(cindex);

	TNK_DBG("%s skb is %p len is %d\n", __func__, skb, skb->len);
	if (unlikely(!skb || !skb->len)) {
		pr_err("%s: Invalid/empty skb\n", __func__);
		return -EINVAL;
	}

	nopaged_len = skb_headlen(skb);
	TNK_DBG("%s nopaged len is %u\n", __func__, nopaged_len);

	entries_needed = nfrags + ((nopaged_len > 0) ? 1 : 0);

	spin_lock_irqsave(&t->tx_desc_lock, flags_tx);

	/* get t->head in spin_lock(tx_desc_lock) to make sure safe */
	entry = t->head % txsize;
	last = entry;

	/*  Ensure that we have enough descriptors to send this packet */
	if (unlikely(tnkhw_tx_avail_desc(cindex) < entries_needed)) {
		int free;
		/* Set the overflow flag to indicate that queue full condition
		 * occurred */
		t->overflow = 1;
		/* NOTE - We need to prevent against possible deadlock
		 * race-condition, where ISR may come in and reclaim ALL TX
		 * buffers BEFORE we get to set this flag, meaning the flag
		 * would never get cleared?
		 */
		free = tnkhw_tx_avail_desc(cindex);
		if (likely(free < entries_needed)) {
			TNK_DBG("%s: Need %d, got %d desc free on channel %d,",
				__func__, entries_needed, free, cindex);
			TNK_DBG(" rejecting packet\n");
			/* Overflow condition still exists, so tell the caller
			 * to try again later */
			tnk_conn_stats[cindex].tx_overflow++;
			spin_unlock_irqrestore(&t->tx_desc_lock, flags_tx);
			return -EAGAIN;
		} else {
			/* Overflow condition has been cleared */
			t->overflow = 0;
		}
	}

	/* Ensure that we have enough free space in the Tx pipeline to send
	 * this packet.  We're currently limited to 16MB due to 24-bit counters
	 * used by the TOE hardware
	 */
	if (unlikely(tnkhw_tx_avail_bytes(cindex) < skb->len)) {
		/* Set the overflow flag to indicate that queue full condition
		 * occurred */
		t->overflow = 1;
		/* NOTE - We need to prevent against possible deadlock
		 * race-condition, where ISR may come in and reclaim ALL
		 * TX buffers BEFORE we get to set this flag, meaning the
		 * flag would never get cleared?
		 */
		if (likely(tnkhw_tx_avail_bytes(cindex) < skb->len)) {
			TNK_DBG("%s: Not enough free space on channel %d,",
					__func__, cindex);

			TNK_DBG(" rejecting packet\n");

			/* Overflow condition still exists, so tell the caller
			 * to try again later */
			tnk_conn_stats[cindex].tx_overflow++;
			spin_unlock_irqrestore(&t->tx_desc_lock, flags_tx);
			return -EAGAIN;
		} else {
			/* Overflow condition has been cleared */
			t->overflow = 0;
		}
	}

	if (nopaged_len) {
		desc = t->desc_list + entry;
		/*  TODO lock needed?  Bug still occurs as of 1/9/11 */
		BUG_ON(desc->hw_own);

		t->skbuff[entry] = NULL;
		desc->buffer_ptr = dma_map_single(tnk_dev, skb->data,
						  nopaged_len, DMA_TO_DEVICE);
		if (unlikely(dma_mapping_error(tnk_dev, desc->buffer_ptr))) {
			pr_err("%s: DMA Mapping Error\n", __func__);
			spin_unlock_irqrestore(&t->tx_desc_lock, flags_tx);
			return -ENOMEM;
		}
		TNK_DBG("%s send skb %p, phys %x, cindex %u, len %u,",
			__func__, skb->data, desc->buffer_ptr, cindex,
			nopaged_len);
		TNK_DBG("data '%-5.5s...'\n", skb->data);

		desc->buffer_size = nopaged_len;

		desc->ack_offset = 0;
		desc->hw_own = 1;
		total_len += desc->buffer_size;
		entry = (entry + 1) % txsize;
		count++;
	}

	for (i = 0; i < nfrags; i++) {
		skb_frag_t *frag = &skb_shinfo(skb)->frags[i];
		int len = frag->size;

		TNK_DBG("%s queueing frag %d of %d (%d bytes)\n", __func__, i,
			nfrags, len);
		/*  Don't transmit 0-byte frags, for efficiency */
		if (len == 0)
			continue;

		last = entry;
		desc = t->desc_list + entry;
		BUG_ON(desc->hw_own);
		t->skbuff[entry] = NULL;
		desc->buffer_ptr = \
		    skb_frag_dma_map(tnk_dev, frag, 0, len, DMA_TO_DEVICE);
		if (unlikely(dma_mapping_error(tnk_dev, desc->buffer_ptr))) {
			pr_err("%s: DMA Mapping Error\n", __func__);
			spin_unlock_irqrestore(&t->tx_desc_lock, flags_tx);
			return -ENOMEM;
		}
		TNK_DBG("%s send frag %d, ptr %p, len %u, data '%-5.5s...'\n",
			__func__, i, addr, len,
			(char *)page_address(frag->page));

		desc->buffer_size = len;

		desc->ack_offset = 0;
		desc->hw_own = 1;
		total_len += desc->buffer_size;
		entry = (entry + 1) % txsize;
		count++;
	}

#ifdef DBG_TX_DMA
	if (count >= tnk_tx_fifo) {
		pr_warn("DEBUG_L:(%s,%d): wrong (count >= tnk_tx_fifo).",
			__func__, __LINE__);
		pr_warn("cindex=%d, ", cindex);
		dump_tx_dma_info(t);
		dump_stack();
	} else if (count == 0) {
		pr_warn("(%s,%d): wired, tnkhw_tx_send do nothing, cindex=%d,",
			__func__, __LINE__, cindex);
		dump_tx_dma_info(t);
	}
#endif

	if (total_len) {
	#ifdef DBG_TX_DMA
		unsigned int prev_t_head = t->head;
	#endif
		TNK_DBG("%s tell hw about %d bytes\n", __func__, total_len);

		/* Save the skb pointer with the last descriptor in the chain so
		 * that the skb will not be freed until the whole chain has been
		 * sent */
		t->skbuff[last] = skb;

		/* Interrupt on completition only for the latest segment */
		desc->intr_on_completion = 1;

		/* Advance head, now that the ownership bit has been set on all
		 * buffers */
		t->head += count;
	#ifdef DBG_TX_DMA
		if (t->head < prev_t_head) {
			pr_warn("(%s,%d): tx_dma_list, head will overflow.",
				__func__, __LINE__);
			pr_warn("cindex=%d, prev t->head=0x%x, ",
				cindex, prev_t_head);
			pr_warn("prev_t_head_index=%d, count=%d\n",
				prev_t_head % txsize, count);
			dump_tx_dma_info(t);
			t->head_overflow = 1;
		}
	#endif

		atomic_add(total_len, &t->pipelined_bytes);

		/* This barrier is important here.  It is required to ensure
		 * the ARM CPU flushes it's DMA write buffers before proceeding
		 * to the next instruction, to ensure that the TOE will see
		 * our descriptor changes in memory */
		STMMAC_SYNC_BARRIER();

		/* Let the TOE know that we've added some new TX data */
		spin_lock_irqsave(&tnkhw_reg_lock, flags);
try_again:
		if (readl(t->desc_list + (t->head - 1) % txsize) & TNK_OWN_BIT)
			tnkhw_ttx_data_add(cindex, total_len);
		else
			goto try_again;
		spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
		TNK_DBG("%s bump tx pkt\n", __func__);

		tnk_conn_stats[cindex].tx_desc_sent += count;
		tnk_conn_stats[cindex].tx_bytes_sent += total_len;
	} else {
		tnk_free_skb(skb, SKB_FROM_SEND);
	}

	spin_unlock_irqrestore(&t->tx_desc_lock, flags_tx);

	TNK_DBG("%s all done\n", __func__);

	return 0;
}

int tnkhw_dma_has_work(void)
{
	uint32_t int_status = readl(tnkhw_ioaddr + TNK_REG_INTR_STAT);
	int rx_status = 0;

	if (int_status & TNK_INTR_TRX)
		rx_status = dwmac1000_dma_ops.dma_interrupt(tnkhw_ioaddr,
							    DMA_CHANNEL_TOERX,
							    &tnk_stmmac_stats);
	if (likely(rx_status == handle_tx_rx) ||	/*  Rx work to do */
	    likely(int_status & TNK_INTR_TTX_ACK))	/*  Tx work to do */
		return 1;
	else
		return 0;
}

void tnk_hw_proc(struct seq_file *s)
{
	unsigned int rxsize = tnk_rx_fifo;
	struct tnkhw_rx_dma_info *r = &tnk_rx_dma;
	unsigned int entry = r->head % rxsize;
	struct tnk_rx_dma_desc *p = r->desc_list + entry;

	seq_printf(s, "\n\ntnkhw:\n");
	seq_printf(s, "rx entry=%d\n", entry);
	seq_printf(s, "rx sw-owns=%d\n", !p->base.des01.erx.own);
	seq_printf(s, "rx free=%d:\n", r->tail + tnk_rx_fifo - r->head);
	seq_printf(s, "haswork=%d:\n", tnkhw_dma_has_work());
	seq_printf(s, "polls=%d:\n", tnk_stats.polls);
	seq_printf(s, "rx refill new alloc=%d:\n", tnk_stats.rx_refills_alloc);
}

void tnkhw_dma_interrupt(uint32_t source_mask)
{
	int rx_status = 0;

	if (source_mask & TNK_INTR_TRX)
		rx_status = dwmac1000_dma_ops.dma_interrupt(tnkhw_ioaddr,
							    DMA_CHANNEL_TOERX,
							    &tnk_stmmac_stats);

	if (likely(rx_status == handle_tx_rx) ||    /*  Rx work to do */
	    likely(source_mask & TNK_INTR_TTX_ACK)) /*  Tx work to do */
		tnk_pollwakeup();
}

static void tnkhw_toe_interrupt(void)
{
	int intr_stat = readl(tnkhw_ioaddr + TNK_REG_TOE_INTR_STAT);
	int intr_tx_err, intr_tx_err_cid;

	tnk_stats.toe_irq++;

	if ((intr_stat >> TNK_OFFSET_TOE_INTR_STAT_FLUSH) &
	    TNK_MASK_TOE_INTR_STAT_FLUSH) {
		tnk_flush_pending = 0;
		TNK_DBG("%s: Got flush completion interrupt\n", __func__);
	}

	intr_tx_err = (intr_stat >> TNK_OFFSET_TOE_INTR_STAT_TX_ERR) &
			TNK_MASK_TOE_INTR_STAT_TX_ERR;
	intr_tx_err_cid = (intr_stat >> TNK_OFFSET_TOE_INTR_STAT_TX_ERR_CID) &
				TNK_MASK_TOE_INTR_STAT_TX_ERR_CID;
	WARN(intr_tx_err, "tx err cindex=%d\n", intr_tx_err_cid);

	if ((intr_stat >> TNK_OFFSET_TOE_INTR_STAT_RETRY_MAX) &
	    TNK_MASK_TOE_INTR_STAT_RETRY_MAX) {
		unsigned cindex =
		    (intr_stat >> TNK_OFFSET_TOE_INTR_STAT_RETRY_CID) &
		    TNK_MASK_TOE_INTR_STAT_RETRY_CID;

		tnk_txmaxretries(cindex);
	}
}

static void tnkhw_toe_flush_completed(void)
{
	int intr_stat = readl(tnkhw_ioaddr + TNK_REG_TOE_INTR_STAT);
	int intr_tx_err, intr_tx_err_cid;

	tnk_stats.toe_irq++;

	if ((intr_stat >> TNK_OFFSET_TOE_INTR_STAT_FLUSH) &
	    TNK_MASK_TOE_INTR_STAT_FLUSH) {
		tnk_flush_pending = 0;
		TNK_DBG("%s: Got flush completion interrupt\n", __func__);
	}

	intr_tx_err = (intr_stat >> TNK_OFFSET_TOE_INTR_STAT_TX_ERR) &
			TNK_MASK_TOE_INTR_STAT_TX_ERR;
	intr_tx_err_cid = (intr_stat >> TNK_OFFSET_TOE_INTR_STAT_TX_ERR_CID) &
				TNK_MASK_TOE_INTR_STAT_TX_ERR_CID;
	WARN(intr_tx_err, "tx err cindex=%d\n", intr_tx_err_cid);

	/* TODO: TNK_REG_TOE_INTR_STAT register is RC(read-clear),
	* we omit TOE_INTR_STAT_RETRY_MAX interrupt because we
	* trust logic will generate this interrupt again and again
	* unless we disable the connection.
	*/
}

void tnkhw_interrupt(int source_mask)
{
	if (source_mask & (TNK_INTR_TRX | TNK_INTR_TTX_ACK))
		tnkhw_dma_interrupt(source_mask);

	if (source_mask & TNK_INTR_TTX_ERR)
		tnkhw_tx_err();

	if (source_mask & TNK_INTR_TOE)
		tnkhw_toe_interrupt();
}

static void tnkhw_reset(void)
{
	int cindex, i, lookup_entry_size;
	unsigned toe_int_en;
#ifndef TNK_HW_PLATFORM_FPGA
	unsigned long tnkclk;
#endif

	/*  Clear the lookup table */
	lookup_entry_size = (tnk_max_connections <= 256) ? 1 : 2;
	for (i = 0;
	     i <
	     (tnk_max_connections * lookup_entry_size * 2) / sizeof(uint32_t);
	     i++) {
		uint32_t wr_addr = 0x100000 + i;
		tnkhw_ct_write_addr(wr_addr, 0);
	}

	/*  Clear out the connection table */
	for (cindex = 0; cindex < tnk_max_connections; cindex++) {
		for (i = 0; i < sizeof(struct tnkhw_connection)
			/ sizeof(uint32_t); i++)
			tnkhw_ct_write(cindex, i, 0);

		tnkhw_ttx_db_init(cindex, 0);
		tnkhw_ttx_txstat_clear(cindex);
		tnkhw_ttx_rxstat_clear(cindex);
	}

	tnkhw_toe_config();

	/*  Configure AXI burst len (using power-on default for now) */
	/*  tnkhw_ttx_ctrl_set(0x80); */

	/*  Configure the default Rx buffer size */
	tnkhw_trx_ctrl_set(DMA_BUFFER_SIZE);

	/*  Configure the TOE with a scaling factor for advertised rx window */
	/*  TODO - what is a suitable value? */
	writel(16, tnkhw_ioaddr + TNK_REG_TOE_CTRL0);

#if 1
	/*  Configure the TOE for immediate ACKs */
	writel(0x8000, tnkhw_ioaddr + TNK_REG_TOE_CTRL1);
#endif

	/*  For FPGA platform, the TOE system clock is 24MHz */
#ifdef TNK_HW_PLATFORM_FPGA
	writel(0x05DBF0FF, tnkhw_ioaddr + TNK_REG_TOE_CTRL2);
#else

	tnkclk = tnk_clk_init();
	writel(tnkclk, tnkhw_ioaddr + TNK_REG_TOE_CTRL2);

	/*  For ASIC platform, the TOE system clock is 155MHz
	writel(0x25D780FF, tnkhw_ioaddr + TNK_REG_TOE_CTRL2);
	*/
#endif
	tnkhw_config_tcp_retries2(sysctl_tcp_retries2);

	/*  Enable retry_exceeded_max and flush completion TOE interrupts */
	toe_int_en =
	    (TNK_MASK_TOE_INTR_EN_RETRY_MAX <<
	     TNK_OFFSET_TOE_INTR_EN_RETRY_MAX);

	writel(toe_int_en, tnkhw_ioaddr + TNK_REG_TOE_INTR_EN);
}

inline void tnkhw_config_tcp_retries2(int tcp_retries2)
{
	int toe_ctrl2;

	if (tcp_retries2 <= 0)
		tcp_retries2 = TCP_RETR2;
	else if (tcp_retries2 > TNK_MAX_RETRANS)
		tcp_retries2 = TNK_MAX_RETRANS;

	toe_ctrl2 = readl(tnkhw_ioaddr + TNK_REG_TOE_CTRL2);
	toe_ctrl2 = (toe_ctrl2 & ~0xfff) | tcp_retries2;
	writel(toe_ctrl2, tnkhw_ioaddr + TNK_REG_TOE_CTRL2);
}

int tnkhw_init(void __iomem *ioaddr, unsigned int max_connections,
	       struct device *dev)
{
	int ret = 0;

	TNK_DBG("Initialising TNK driver...\n");

	/* Control Path initialisation */
	tnkhw_ioaddr = ioaddr;
	tnk_max_connections = max_connections;
	tnk_dev = dev;

	spin_lock_init(&tnkhw_reg_lock);

	tnkhw_reset();

	tnk_conn_stats = kzalloc(max_connections *
				 sizeof(struct tnkhw_conn_stats),
				 GFP_KERNEL);
	if (!tnk_conn_stats) {
		pr_err("%s: Failed to allocate memory for connection stats\n",
		       __func__);
		return -ENOMEM;
	}

	/* Configure DMA channels, etc */
	ret = tnkhw_rx_dma_init();
	if (ret) {
		pr_err("%s: Failed to set initialise Rx DMA channel\n",
		       __func__);
		goto out_freemem;
	}
	TNK_DBG("%s: tnkhw_rx_dma_init returned ok\n", __func__);
	ret = tnkhw_tx_dma_init();
	if (ret) {
		pr_err("%s: Failed to set initialise Tx DMA channel\n",
		       __func__);
		goto out_freerxdma;
	}
	TNK_DBG("%s: tnkhw_tx_dma_init returned ok\n", __func__);

	/* init all switch on TOE*/
	tnk_switch_enable();

	/*  If we get here, we had no errors */
	return 0;

out_freerxdma:
	/* tnkhw_rx_dma_free(); */
out_freemem:
	/* kfree(tnk_conn_stats); */

	return ret;
}

void tnkhw_shutdown(void)
{
	tnkhw_dma_interrupt_disable();

	tnkhw_rx_dma_free();
	tnkhw_tx_dma_free();

	kfree(tnk_conn_stats);
	tnk_conn_stats = NULL;
}
#ifdef TNK_BONDING
void sync_ipv4_pos_to_logic(uint32_t addr)
{
	uint32_t reg_offset[2][4] = {
		{TNK_REG_CT_LOCAL_IPADDR0,
			TNK_REG_CT_LOCAL_IPADDR0_1,
			TNK_REG_CT_LOCAL_IPADDR0_2,
			TNK_REG_CT_LOCAL_IPADDR0_3},
		{TNK_REG_CT_LOCAL_IPADDR1,
			TNK_REG_CT_LOCAL_IPADDR1_1,
			TNK_REG_CT_LOCAL_IPADDR1_2,
			TNK_REG_CT_LOCAL_IPADDR1_3} };
	int i, j;
	uint32_t addrtmp = 0;
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);
	/* eth0 local ip position */
	for (i = 0; i < 4; i++) {
		addrtmp = readl(tnkhw_ioaddr + reg_offset[0][i]);
		if (addrtmp == addr)
			break;
	}
	/* eth1 local ip position */
	for (j = 0; j < 4; j++) {
		addrtmp = readl(tnkhw_ioaddr + reg_offset[1][j]);
		if (addrtmp == addr)
			break;
	}
	/* make eth1 and eth0 same ip same position */
	if ((i < 4) && (j < 4) && (i != j)) {
		TNKBD_DBG("eth0 local ip pos = %d, eth1 pos = %d\n",
				i, j);
		addrtmp = readl(tnkhw_ioaddr + reg_offset[1][i]);
		writel(readl(tnkhw_ioaddr + reg_offset[1][j]),
				tnkhw_ioaddr + reg_offset[1][i]);
		writel(addrtmp, tnkhw_ioaddr + reg_offset[1][j]);
	}
	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
}
void sync_ipv6_pos_to_logic(uint32_t *ipv6_addr)
{
	uint32_t reg_offset[2][4] = {
		{TNK_REG_CT_LOCAL_IPV6_ADDR0_W0,
			TNK_REG_CT_LOCAL_IPV6_ADDR0_W0_1,
			TNK_REG_CT_LOCAL_IPV6_ADDR0_W0_2,
			TNK_REG_CT_LOCAL_IPV6_ADDR0_W0_3},
		{TNK_REG_CT_LOCAL_IPV6_ADDR1_W0,
			TNK_REG_CT_LOCAL_IPV6_ADDR1_W0_1,
			TNK_REG_CT_LOCAL_IPV6_ADDR1_W0_2,
			TNK_REG_CT_LOCAL_IPV6_ADDR1_W0_3} };
	int i, j, eth0_pos, eth1_pos;
	uint32_t addrtmp = 0;
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);
	/* eth0 local ip position */
	/* input ipv6 addr compare with the logic ipv6 addr */
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			addrtmp = readl(tnkhw_ioaddr
					+ reg_offset[0][i] + (j * 4));
			if (addrtmp != ipv6_addr[j])
				break;
		}
		if (j == 4)
			break;
	}
	eth0_pos = i;
	/* eth1 local ip position */
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			addrtmp = readl(tnkhw_ioaddr
					+ reg_offset[1][i] + (j * 4));
			if (addrtmp != ipv6_addr[j])
				break;
		}
		if (j == 4)
			break;
	}
	eth1_pos = j;
	/* make eth1 and eth0 same ip same position */
	if ((eth0_pos < 4) && (eth1_pos < 4)
			&& (eth0_pos != eth1_pos)) {
		TNKBD_DBG("eth0 local ip pos = %d, eth1 pos = %d\n",
				eth0_pos, eth1_pos);
		for (i = 0; i < 4; i++) {
			addrtmp = readl(tnkhw_ioaddr
					+ reg_offset[1][eth0_pos] + (i * 4));
			writel(readl(tnkhw_ioaddr
					+ reg_offset[1][eth1_pos] + (i * 4)),
					tnkhw_ioaddr
					+ reg_offset[1][eth0_pos] + (i * 4));
			writel(addrtmp, tnkhw_ioaddr
					+ reg_offset[1][eth1_pos] + (i * 4));
		}
	}
	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
}
#endif
#if defined(TNK_BONDING) || defined(TNK_BONJOUR)
uint32_t select_ipv4_pos_to_logic(int gmac, uint32_t addr)
{
	uint32_t reg_offset[2][4] = {
		{TNK_REG_CT_LOCAL_IPADDR0,
			TNK_REG_CT_LOCAL_IPADDR0_1,
			TNK_REG_CT_LOCAL_IPADDR0_2,
			TNK_REG_CT_LOCAL_IPADDR0_3},
		{TNK_REG_CT_LOCAL_IPADDR1,
			TNK_REG_CT_LOCAL_IPADDR1_1,
			TNK_REG_CT_LOCAL_IPADDR1_2,
			TNK_REG_CT_LOCAL_IPADDR1_3} };
	int i;
	uint32_t addrtmp = 0;

	for (i = 0; i < 4; i++) {
		addrtmp = readl(tnkhw_ioaddr + reg_offset[gmac][i]);
		if (addrtmp == addr)
			break;
	}
	if (i == 4) {
		TNKBD_DBG(KERN_ERR"TOE only support 4 IPaddrs on one dev\n");
		TNKBD_DBG(KERN_ERR"This ip is not compare with those\n");
		return -1;
	}
	return i;
}
uint32_t select_ipv6_pos_to_logic(int gmac, uint32_t *ipv6_addr)
{
	uint32_t reg_offset[2][4] = {
		{TNK_REG_CT_LOCAL_IPV6_ADDR0_W0,
			TNK_REG_CT_LOCAL_IPV6_ADDR0_W0_1,
			TNK_REG_CT_LOCAL_IPV6_ADDR0_W0_2,
			TNK_REG_CT_LOCAL_IPV6_ADDR0_W0_3},
		{TNK_REG_CT_LOCAL_IPV6_ADDR1_W0,
			TNK_REG_CT_LOCAL_IPV6_ADDR1_W0_1,
			TNK_REG_CT_LOCAL_IPV6_ADDR1_W0_2,
			TNK_REG_CT_LOCAL_IPV6_ADDR1_W0_3} };
	int i, j;
	uint32_t addrtmp = 0;

	/* input ipv6 addr compare with the logic ipv6 addr */
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			addrtmp = readl(tnkhw_ioaddr
					+ reg_offset[gmac][i] + (j * 4));
			if (addrtmp != ipv6_addr[j])
				break;
		}
		if (j == 4)
			break;
	}
	if (i == 4) {
		printk(KERN_ERR"TOE only support 4 IPaddrs on one dev\n");
		printk(KERN_ERR"This ip is not compare with those\n");
		return -1;
	}
	return i;
}
void tnk_ct_ip_position_sel_extend(int index, int local_ip_pos)
{
	uint32_t reg;
	uint32_t bits;
	uint32_t tmp;
	uint32_t postion;
	/* 1 Select reg for TOE connection */
	reg = (index / 8) * 4 + TNK_REG_CT_LOCAL_IPADDR_INDEX_SEL;

	/* 2 Select reg bits for TOE connection */
	bits = (0x00000003 & local_ip_pos) << ((index % 8) * 4);

	/* 3 Read reg and clear the two bits */
	postion = (0x00000003) << ((index % 8) * 4);
	tmp = readl(tnkhw_ioaddr + reg);
	tmp = (~postion) & tmp;

	/* 4 Load the Local IP address position for TOE connection */
	writel(bits | tmp, tnkhw_ioaddr + reg);
	TNKBD_DBG("%s  bits = %.8x\n",
			__func__, readl(tnkhw_ioaddr + reg));
}
#endif

#if defined(TNK_BONDING) || defined(TNK_BONJOUR)
int tnkhw_connection_add(unsigned cindex, struct tnkhw_connection *entry,
		int local_ip_pos)
#else
int tnkhw_connection_add(unsigned cindex, struct tnkhw_connection *entry)
#endif
{
	int i;
	struct tnkhw_tx_dma_info *t = &tnk_tx_dma_list[cindex];
	uint32_t *data = (uint32_t *) entry;
	unsigned long flags;

	TNK_CINDEX_VALIDATE(cindex);

	TNK_DBG("%s db init and stat clear for t=%p\n", __func__, t);

#ifdef TNK_TTX_FLUSH_DELAYED_RECLAIM
	if (t->head != t->tail) {
		/* Clear out the old buffers as well */
		tnkhw_tx_channel_reclaim(cindex, 1);
	}
#endif
	tnkhw_tx_desc_check(cindex);

	/* Re-initialise the channel variables */
	tnkhw_ttx_db_init(cindex, t->desc_list_phys);
	tnkhw_ttx_txstat_clear(cindex);
	tnkhw_ttx_rxstat_clear(cindex);
	memset(&tnk_conn_stats[cindex], 0, sizeof(tnk_conn_stats[0]));
	t->head = 0;
	t->tail = 0;
	atomic_set(&t->pipelined_bytes, 0);
	t->rst_received = 0;
	t->overflow = 0;
#ifdef DBG_TX_DMA
	t->head_overflow = 0;
#endif

	TNK_DBG("%s writing conn table for cindex %d\n", __func__, cindex);

	tnkhw_init_out_of_order(cindex);
	spin_lock_irqsave(&tnkhw_reg_lock, flags);

	/*  Write the complete connection table entry, one word at a time */
	for (i = 0; i < sizeof(struct tnkhw_connection) / sizeof(uint32_t); i++)
		tnkhw_ct_write(cindex, i, data[i]);

#if defined(TNK_BONDING) || defined(TNK_BONJOUR)
	/* One TOE connection to a 2 bits ip position */
	if (local_ip_pos >= 0) {
		/* for tx like ct table */
		tnk_ct_ip_position_sel_extend(cindex, local_ip_pos);
	}

	/*  Add this connection to the lookup table */
	if (entry->flag_ipv6)
		ct_lookup_data_load_ipv6(entry->r_ipv6_addr_31_0,
					 entry->r_ipv6_addr_63_32,
					 entry->r_ipv6_addr_95_64,
					 entry->r_ipv6_addr_127_96,
					 entry->r_port,
					 entry->l_port,
					 local_ip_pos);
	else
		ct_lookup_data_load_ipv4(entry->r_ipaddr, entry->r_port,
					 entry->l_port, local_ip_pos);
#else
	/*  Add this connection to the lookup table */
	if (entry->flag_ipv6)
		ct_lookup_data_load_ipv6(entry->r_ipv6_addr_31_0,
					 entry->r_ipv6_addr_63_32,
					 entry->r_ipv6_addr_95_64,
					 entry->r_ipv6_addr_127_96,
					 entry->r_port, entry->l_port);
	else
		ct_lookup_data_load_ipv4(entry->r_ipaddr, entry->r_port,
					 entry->l_port);
#endif
	/*  Commit the CT update by writing CT_CONN_CTRL, and wait for
	 *  operation to complete */
	tnkhw_ct_update(cindex, 1, entry->flag_ipv6);

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);

	return 0;
}
#ifdef TNK_BONDING
int is_bonding_enable(void)
{
	uint32_t data;
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);
	data = readl(tnkhw_ioaddr + TNK_REG_TOE_BONDING_CTRL);
	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
	if (data & 0x00000080) {
		TNKBD_DBG("%s Bonding disable\n", __func__);
		return 0;
	} else {
		TNKBD_DBG("%s Bonding enable\n", __func__);
		return 1;
	}
}
void tnkhw_bonding_enable(int enable)
{
	uint32_t data;
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);
	data = readl(tnkhw_ioaddr + TNK_REG_TOE_BONDING_CTRL);
	if (enable) {
		writel((0xffffff7f & data),
				tnkhw_ioaddr + TNK_REG_TOE_BONDING_CTRL);
		TNKBD_DBG("%s Bonding enable\n", __func__);
	} else {
		writel((0x00000080 | data),
				tnkhw_ioaddr + TNK_REG_TOE_BONDING_CTRL);
		TNKBD_DBG("%s Bonding disable\n", __func__);
	}
	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
}
EXPORT_SYMBOL(tnkhw_bonding_enable);
void tnkhw_bonding_setmode(int mode)
{
	uint32_t data;
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);
	data = readl(tnkhw_ioaddr + TNK_REG_TOE_BONDING_CTRL);
	if (mode) {
		TNKBD_DBG("%s BOND_MODE_ACTIVEBACKUP\n", __func__);
		writel((0x00000040 | data),
				tnkhw_ioaddr + TNK_REG_TOE_BONDING_CTRL);
	} else {
		TNKBD_DBG("%s BOND_MODE_ROUNDROBIN\n", __func__);
		writel((0xffffffbf & data),
				tnkhw_ioaddr + TNK_REG_TOE_BONDING_CTRL);
	}
	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
}
EXPORT_SYMBOL(tnkhw_bonding_setmode);
int tnkhw_bonding_getmode(void)
{
	uint32_t data;
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);
	data = readl(tnkhw_ioaddr + TNK_REG_TOE_BONDING_CTRL);
	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
	if (data & 0x00000040) {
		TNKBD_DBG("%s BOND_MODE_ACTIVEBACKUP\n", __func__);
		return 1;
	} else {
		TNKBD_DBG("%s BOND_MODE_ROUNDROBIN\n", __func__);
		return 0;
	}
}
int curr_bonding_dev(void)
{
	uint32_t data;
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);
	data = readl(tnkhw_ioaddr + TNK_REG_TOE_BONDING_CTRL);
	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
	if (data & 0x00000004) {
		TNKBD_DBG("%s curr_slave = eth1\n", __func__);
		return 1;
	} else {
		TNKBD_DBG("%s curr_slave = eth0\n", __func__);
		return 0;
	}
}
int curr_active_dev(void)
{
	uint32_t data;
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);
	data = readl(tnkhw_ioaddr + TNK_REG_TOE_BONDING_CTRL);
	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
	if (0x2 == (data & 0x00000003)) {
		TNKBD_DBG("%s Only eth1 is active now\n", __func__);
		return 1;
	} else if (0x1 == (data & 0x00000003)) {
		TNKBD_DBG("%s Only eth0 is active now\n", __func__);
		return 0;
	} else {
		TNKBD_DBG("%s eth0 and eth1 are active\n", __func__);
		return 2;
	}
}
void tnkhw_bonding_setcurr_active_slave(int curr_slave)
{
	uint32_t data;
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);
	data = readl(tnkhw_ioaddr + TNK_REG_TOE_BONDING_CTRL);
	if (curr_slave) {
		TNKBD_DBG("%s curr_slave = eth1\n", __func__);
		writel((0x00000004 | data),
				tnkhw_ioaddr + TNK_REG_TOE_BONDING_CTRL);
	} else {
		TNKBD_DBG("%s curr_slave = eth0\n", __func__);
		writel((0xfffffffb & data),
				tnkhw_ioaddr + TNK_REG_TOE_BONDING_CTRL);
	}
	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
}
EXPORT_SYMBOL(tnkhw_bonding_setcurr_active_slave);
void tnkhw_bonding_setstatus_slave(int gmac, int enable)
{
	uint32_t data;
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);
	data = readl(tnkhw_ioaddr + TNK_REG_TOE_BONDING_CTRL);
	if (gmac == 0) {
		if (enable)
			writel((0x00000001 | data), tnkhw_ioaddr
					+ TNK_REG_TOE_BONDING_CTRL);
		else
			writel((0xfffffffe & data), tnkhw_ioaddr
					+ TNK_REG_TOE_BONDING_CTRL);
	} else {
		if (enable)
			writel((0x00000002 | data), tnkhw_ioaddr
					+ TNK_REG_TOE_BONDING_CTRL);
		else
			writel((0xfffffffd & data), tnkhw_ioaddr
					+ TNK_REG_TOE_BONDING_CTRL);
	}
	TNKBD_DBG("%s gmac = %d, enable = %d\n",
			__func__, gmac, enable);
	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
}
EXPORT_SYMBOL(tnkhw_bonding_setstatus_slave);
#endif
static void tnkhw_dump_tx_descriptors(unsigned cindex)
{
	struct tnkhw_tx_dma_info *t = &tnk_tx_dma_list[cindex];
	int i;
	pr_info("%s cindex %d, head %d, tail %d\n",
		__func__, cindex, t->head, t->tail);

	for (i = 0; i < tnk_tx_fifo; i++) {
		struct tnk_ttx_dma_desc *desc = &t->desc_list[i];
		pr_info("TTX Descriptor (chan %u, ring index %u)\n"
			"\tack_offset: %u\n"
			"\treserved1: %u\n"
			"\thw_own: %u\n"
			"\tbuffer_size: %u\n"
			"\treserved2: %u\n"
			"\tintr_on_completion: %u\n"
			"\tbuffer_ptr: 0x%08X\n"
			"\tnext_desc_ptr: 0x%08X\n",
			cindex, i,
			desc->ack_offset,
			desc->reserved1,
			desc->hw_own,
			desc->buffer_size,
			desc->reserved2,
			desc->intr_on_completion,
			desc->buffer_ptr, desc->next_desc_ptr);
	}
}

void tnkhw_connection_disable(unsigned cindex)
{
	unsigned long flags;

	TNK_CINDEX_VALIDATE(cindex);

	spin_lock_irqsave(&tnkhw_reg_lock, flags);

	/* Clear the enable bit in the connection table */
	tnkhw_ct_enable(cindex, 0);

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
}

void tnkhw_connection_enable(unsigned cindex)
{
	unsigned long flags;

	TNK_CINDEX_VALIDATE(cindex);

	spin_lock_irqsave(&tnkhw_reg_lock, flags);

	/* Set the enable bit in the connection table */
	tnkhw_ct_enable(cindex, 1);

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
}

#if defined(TNK_BONDING) || defined(TNK_BONJOUR)
int tnkhw_connection_remove(unsigned cindex, struct tnkhw_connection *entry,
		int local_ip_pos, int ct_hw_removed)
#else
int tnkhw_connection_remove(unsigned cindex, struct tnkhw_connection *entry,
		int ct_hw_removed)
#endif
{
	struct tnkhw_tx_dma_info *t = &tnk_tx_dma_list[cindex];
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);

	TNK_CINDEX_VALIDATE(cindex);

	TNK_DBG("%s ip %x\n", __func__, entry->r_ipaddr);

	if (ct_hw_removed)
		goto tnkhw_ct_removed;

	/* 1. Remove the lookup-table entry.
	 * This stops any ACK processing or receiving packets for this
	 * connection
	 */
#if defined(TNK_BONDING) || defined(TNK_BONJOUR)
	if (entry->flag_ipv6)
		ct_lookup_data_load_ipv6(entry->r_ipv6_addr_31_0,
					 entry->r_ipv6_addr_63_32,
					 entry->r_ipv6_addr_95_64,
					 entry->r_ipv6_addr_127_96,
					 entry->r_port,
					 entry->l_port,
					 local_ip_pos);
	else
		ct_lookup_data_load_ipv4(entry->r_ipaddr, entry->r_port,
					 entry->l_port, local_ip_pos);
#else
	if (entry->flag_ipv6)
		ct_lookup_data_load_ipv6(entry->r_ipv6_addr_31_0,
				entry->r_ipv6_addr_63_32,
				entry->r_ipv6_addr_95_64,
				entry->r_ipv6_addr_127_96,
				entry->r_port, entry->l_port);
	else
		ct_lookup_data_load_ipv4(entry->r_ipaddr, entry->r_port,
				entry->l_port);
#endif

	/*  Commit the CT update by writing CT_CONN_CTRL, and wait for
	 *  operation to complete */
	tnkhw_ct_update(cindex, 0, entry->flag_ipv6);

	/* 2. Clear the enable bit in the connection table
	 * This stops any further new Tx processing, including reading
	 * descriptors, although the TOE may be in the process of
	 * completing one.
	 *
	 * It is not guaranteed that the flush will complete immediately,
	 * so we will wait until we go to re-use the channel or shut-down
	 * before reclaiming the buffers that may be sitting in the ring
	 */
	tnkhw_ct_enable(cindex, 0);

tnkhw_ct_removed:
	/* 3. Send a flush command for that connection
	 * This will clear out any accumulated count for that connection
	 */
	if (t->head != t->tail) {
		mdelay(1);
		tnkhw_ttx_data_flush(cindex);
		tnkhw_ttx_wait_flush();
#ifndef TNK_TTX_FLUSH_DELAYED_RECLAIM
		/* Clear out the old buffers as well */
		tnkhw_tx_channel_reclaim(cindex, 1);
#endif
	}
#ifdef DBG_TX_DMA
	if (t->head_overflow) {
		pr_warn("DEBUG_L:(%s,%d): t->head_overflow non-zero.",
			__func__, __LINE__);
		pr_warn("cindex=%d, ", cindex);
		dump_tx_dma_info(t);
		dump_stack();
	}
#endif

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);

	return 0;
}

#if defined(TNK_BONDING) || defined(TNK_BONJOUR)
int tnkhw_connection_remove_hw(unsigned cindex, struct tnkhw_connection *entry,
		int local_ip_pos)
#else
int tnkhw_connection_remove_hw(unsigned cindex, struct tnkhw_connection *entry)
#endif
{
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);

	TNK_CINDEX_VALIDATE(cindex);

	TNK_DBG("%s ip %x\n", __func__, entry->r_ipaddr);

	/* 1. Remove the lookup-table entry.
	 * This stops any ACK processing or receiving packets for this
	 * connection
	 */
#if defined(TNK_BONDING) || defined(TNK_BONJOUR)
	if (entry->flag_ipv6)
		ct_lookup_data_load_ipv6(entry->r_ipv6_addr_31_0,
					 entry->r_ipv6_addr_63_32,
					 entry->r_ipv6_addr_95_64,
					 entry->r_ipv6_addr_127_96,
					 entry->r_port,
					 entry->l_port,
					 local_ip_pos);
	else
		ct_lookup_data_load_ipv4(entry->r_ipaddr, entry->r_port,
					 entry->l_port, local_ip_pos);
#else
	if (entry->flag_ipv6)
		ct_lookup_data_load_ipv6(entry->r_ipv6_addr_31_0,
				entry->r_ipv6_addr_63_32,
				entry->r_ipv6_addr_95_64,
				entry->r_ipv6_addr_127_96,
				entry->r_port, entry->l_port);
	else
		ct_lookup_data_load_ipv4(entry->r_ipaddr, entry->r_port,
				entry->l_port);
#endif

	/*  Commit the CT update by writing CT_CONN_CTRL, and wait for
	 *  operation to complete */
	tnkhw_ct_update(cindex, 0, entry->flag_ipv6);

	/* 2. Clear the enable bit in the connection table
	 * This stops any further new Tx processing, including reading
	 * descriptors, although the TOE may be in the process of
	 * completing one.
	 *
	 * It is not guaranteed that the flush will complete immediately,
	 * so we will wait until we go to re-use the channel or shut-down
	 * before reclaiming the buffers that may be sitting in the ring
	 */
	tnkhw_ct_enable(cindex, 0);

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);

	return 0;
}

int tnkhw_connection_get(unsigned cindex, struct tnkhw_connection *entry)
{
	int i;
	uint32_t *data = (uint32_t *) entry;
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);

	TNK_CINDEX_VALIDATE(cindex);

	/*  Write the complete connection table entry, one word at a time */
	for (i = 0; i < sizeof(struct tnkhw_connection) / sizeof(uint32_t); i++)
		tnkhw_ct_read(cindex, i, &data[i]);

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);

	return 0;
}

void tnkhw_connection_dump(unsigned cindex)
{
	struct tnkhw_connection conn;

	tnkhw_connection_get(cindex, &conn);

	pr_info("r_macaddr_u16       %8x\n", conn.r_macaddr_u16);
	pr_info("l_port              %8x\n", conn.l_port);
	pr_info("r_macaddr_l32       %8x\n", conn.r_macaddr_l32);
	pr_info("r_ipaddr            %8x\n", conn.r_ipaddr);
	pr_info("r_port              %8x\n", conn.r_port);
	pr_info("ttl                 %8x\n", conn.ttl);
	pr_info("tos                 %8x\n", conn.tos);
	pr_info("rx_ack_num          %8x\n", conn.rx_ack_num);
	pr_info("next_rx_seq_num     %8x\n", conn.next_rx_seq_num);
	pr_info("next_tx_seq_num     %8x\n", conn.next_tx_seq_num);
	pr_info("dup_ack_cnt         %8x\n", conn.dup_ack_cnt);
	pr_info("rx_adv_wnd          %8x\n", conn.rx_adv_wnd);
	pr_info("timer_tx_retry      %8x\n", conn.timer_tx_retry);
	pr_info("retry_count         %8x\n", conn.retry_count);
	pr_info("tx_mss              %8x\n", conn.tx_mss);
	pr_info("adv_wnd_scale       %8x\n", conn.adv_wnd_scale);
	pr_info("flag_conn_enable    %8x\n", conn.flag_conn_enable);
	pr_info("flag_mac_sel        %8x\n", conn.flag_mac_sel);
	pr_info("flag_ipv6           %8x\n", conn.flag_ipv6);
	pr_info("flag_disable_nagle  %8x\n", conn.flag_disable_nagle);
	pr_info("flag_use_local_wnd  %8x\n", conn.flag_use_local_wnd);
	pr_info("flag_use_local_scl  %8x\n", conn.flag_use_local_scl);
	pr_info("flag_full_pkt       %8x\n", conn.flag_full_pkt);
	pr_info("tx_adv_wnd          %8x\n", conn.tx_adv_wnd);
	pr_info("cong_wnd            %8x\n", conn.cong_wnd);
	pr_info("timer_timestamp     %8x\n", conn.timer_timestamp);
	pr_info("timer_active        %8x\n", conn.timer_active);
	pr_info("timer_seq_num       %8x\n", conn.timer_seq_num);
	pr_info("timeout_value       %8x\n", conn.timeout_value);
	pr_info("sampled_ack_num     %8x\n", conn.sampled_ack_num);
	pr_info("timer_rx_fast_retry %8x\n", conn.timer_rx_fast_retry);
	pr_info("retry_seq_num       %8x\n", conn.retry_seq_num);
	pr_info("en_rtt_calc         %8x\n", conn.en_rtt_calc);
	pr_info("ss_threshold        %8x\n", conn.ss_threshold);
	pr_info("tx_timestamp        %8x\n", conn.tx_timestamp);
	pr_info("timer_rtt           %8x\n", conn.timer_rtt);
	pr_info("timer_rtt_valid     %8x\n", conn.timer_rtt_valid);
	pr_info("last_retry_seq_num  %8x\n", conn.last_retry_seq_num);
	pr_info("ipv6_addr_31_0      %8x\n", conn.r_ipv6_addr_31_0);
	pr_info("ipv6_addr_63_32     %8x\n", conn.r_ipv6_addr_63_32);
	pr_info("ipv6_addr_95_64     %8x\n", conn.r_ipv6_addr_95_64);
	pr_info("ipv6_addr_127_96    %8x\n", conn.r_ipv6_addr_127_96);
	pr_info("ipv6_flow_label     %8x\n", conn.r_ipv6_flow_label);
}

int tnkhw_macaddress_set(uint8_t *mac_addr, int gmac)
{
	uint32_t u16, l32;
	unsigned long flags;
	int ret = 0;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);

	u16 = (mac_addr[0] << 8) | mac_addr[1];
	l32 =
	    (mac_addr[2] << 24) | (mac_addr[3] << 16) | (mac_addr[4] << 8) |
	    mac_addr[5];

	switch (gmac) {
	case 0:
		writel(u16, tnkhw_ioaddr + TNK_REG_CT_LOCAL_MACADDR0_U16);
		writel(l32, tnkhw_ioaddr + TNK_REG_CT_LOCAL_MACADDR0_L32);
		break;
	case 1:
		writel(u16, tnkhw_ioaddr + TNK_REG_CT_LOCAL_MACADDR1_U16);
		writel(l32, tnkhw_ioaddr + TNK_REG_CT_LOCAL_MACADDR1_L32);
		break;
	default:
		pr_err("%s: Invalid gmac %d specified\n", __func__, gmac);
		ret = -EINVAL;
	}

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);

	return ret;
}

void *tnkhw_ioaddr_get(void)
{
	return tnkhw_ioaddr;
}

#if defined(TNK_BONDING) || defined(TNK_BONJOUR)
uint32_t tnkhw_ipv4select_reg_write(int gmac)
{
	uint32_t reg_offset[2][4] = {
		{TNK_REG_CT_LOCAL_IPADDR0,
			TNK_REG_CT_LOCAL_IPADDR0_1,
			TNK_REG_CT_LOCAL_IPADDR0_2,
			TNK_REG_CT_LOCAL_IPADDR0_3},
		{TNK_REG_CT_LOCAL_IPADDR1,
			TNK_REG_CT_LOCAL_IPADDR1_1,
			TNK_REG_CT_LOCAL_IPADDR1_2,
			TNK_REG_CT_LOCAL_IPADDR1_3} };
	int i;
	uint32_t addr = 0;

	for (i = 0; i < 4; i++) {
		addr = readl(tnkhw_ioaddr + reg_offset[gmac][i]);
		TNKBD_DBG("%s addr[%d] = %x\n", __func__, i, addr);
		if (addr == 0)
			break;
	}
	if (i == 4) {
		printk(KERN_ERR"TOE only support 4 IPaddrs on one dev\n");
		printk(KERN_ERR"This ip will not be actived for logic\n");
		return 0;
	}
	return reg_offset[gmac][i];
}
uint32_t tnkhw_ipv6select_reg_write(int gmac)
{
	uint32_t reg_offset[2][4] = {
		{TNK_REG_CT_LOCAL_IPV6_ADDR0_W0,
			TNK_REG_CT_LOCAL_IPV6_ADDR0_W0_1,
			TNK_REG_CT_LOCAL_IPV6_ADDR0_W0_2,
			TNK_REG_CT_LOCAL_IPV6_ADDR0_W0_3},
		{TNK_REG_CT_LOCAL_IPV6_ADDR1_W0,
			TNK_REG_CT_LOCAL_IPV6_ADDR1_W0_1,
			TNK_REG_CT_LOCAL_IPV6_ADDR1_W0_2,
			TNK_REG_CT_LOCAL_IPV6_ADDR1_W0_3} };
	int i;
	uint32_t addr1, addr2, addr3, addr4;

	addr1 = addr2 = addr3 = addr4 = 0;

	for (i = 0; i < 4; i++) {
		addr1 = readl(tnkhw_ioaddr + reg_offset[gmac][i]);
		addr2 = readl(tnkhw_ioaddr + reg_offset[gmac][i] + 4);
		addr3 = readl(tnkhw_ioaddr + reg_offset[gmac][i] + 8);
		addr4 = readl(tnkhw_ioaddr + reg_offset[gmac][i] + 12);
		if (!addr1 && !addr2 && !addr3 && !addr4)
			break;
	}
	if (i == 4) {
		printk(KERN_ERR"TOE only support 4 IPaddrs on one dev\n");
		printk(KERN_ERR"This ip will not be actived for logic\n");
		return 0;
	}
	return reg_offset[gmac][i];
}

uint32_t tnkhw_ipv4select_reg_rst(int gmac, uint32_t addr)
{
	uint32_t reg_offset[2][4] = {
		{ TNK_REG_CT_LOCAL_IPADDR0,
			TNK_REG_CT_LOCAL_IPADDR0_1,
			TNK_REG_CT_LOCAL_IPADDR0_2,
			TNK_REG_CT_LOCAL_IPADDR0_3},
		{TNK_REG_CT_LOCAL_IPADDR1,
			TNK_REG_CT_LOCAL_IPADDR1_1,
			TNK_REG_CT_LOCAL_IPADDR1_2,
			TNK_REG_CT_LOCAL_IPADDR1_3} };
	int i;
	uint32_t addrtmp = 0;

	for (i = 0; i < 4; i++) {
		addrtmp = readl(tnkhw_ioaddr + reg_offset[gmac][i]);
		if (addrtmp == addr)
			break;
	}
	if (i == 4) {
		printk(KERN_ERR"TOE only support 4 IPaddrs on one dev\n");
		printk(KERN_ERR"This ip is not compare with those\n");
		return 0;
	}
	return reg_offset[gmac][i];
}
int tnkhw_ipv4address_rst(uint8_t *ipv4_addr, int gmac)
{
	uint32_t addr = 0, reg_offset;
	unsigned long flags;
	static int ipv4_cnt;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);
	if (ipv4_addr) {
		TNKBD_DBG("rst:Setting GMAC %d ipv4 address to %d.%d.%d.%d\n",
			gmac, ipv4_addr[0], ipv4_addr[1], ipv4_addr[2],
			ipv4_addr[3]);
		addr =
		    (ipv4_addr[0] << 24) | (ipv4_addr[1] << 16) | (ipv4_addr[2]
								   << 8) |
		    ipv4_addr[3];
	} else
		TNKBD_DBG("Setting GMAC %d ipv4 address to 0\n", gmac);
	/* bonjour function for TOE */
	ipv4_cnt = (gmac == 0) ? --eth0_ipv4_cnt : --eth1_ipv4_cnt;
	TNKBD_DBG("ipv4_cnt = %d\n", ipv4_cnt);

	if (ipv4_cnt < 0) {
		ipv4_cnt = (gmac == 0) ? ++eth0_ipv4_cnt : ++eth1_ipv4_cnt;
		printk(KERN_ERR"TOE ipv4 address are all deleted\n");
		spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
		return -1;
	}

	reg_offset = tnkhw_ipv4select_reg_rst(gmac, addr);

	if (reg_offset == 0) {
		ipv4_cnt = (gmac == 0) ? ++eth0_ipv4_cnt : ++eth1_ipv4_cnt;
		TNKBD_DBG("ipv4_cnt = %d\n", ipv4_cnt);
		spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
		return -1;
	}
	writel(0, tnkhw_ioaddr + reg_offset);

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);

	return 0;
}
uint32_t tnkhw_ipv6select_reg_rst(int gmac, uint8_t *ipv6_addr)
{
	uint32_t reg_offset[2][4] = {
		{TNK_REG_CT_LOCAL_IPV6_ADDR0_W0,
			TNK_REG_CT_LOCAL_IPV6_ADDR0_W0_1,
			TNK_REG_CT_LOCAL_IPV6_ADDR0_W0_2,
			TNK_REG_CT_LOCAL_IPV6_ADDR0_W0_3},
		{TNK_REG_CT_LOCAL_IPV6_ADDR1_W0,
			TNK_REG_CT_LOCAL_IPV6_ADDR1_W0_1,
			TNK_REG_CT_LOCAL_IPV6_ADDR1_W0_2,
			TNK_REG_CT_LOCAL_IPV6_ADDR1_W0_3} };
	int i, j;
	uint32_t addrtmp = 0;
	uint32_t addr[4] = {0};

	for (i = 0; i < 4; i++) {
		if (ipv6_addr)
			addr[i] = (ipv6_addr[0 + (i * 4)] << 24) |
			    (ipv6_addr[1 + (i * 4)] << 16) |
			    (ipv6_addr[2 + (i * 4)] << 8) |
			    ipv6_addr[3 + (i * 4)];
	}
	/* input ipv6 addr compare with the logic ipv6 addr */
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			addrtmp = readl(tnkhw_ioaddr
					+ reg_offset[gmac][i] + (j * 4));
			if (addrtmp != addr[j])
				break;
		}
		if (j == 4)
			break;
	}
	if (i == 4) {
		printk(KERN_ERR "TOE only support 4 IPaddrs on one dev\n");
		printk(KERN_ERR"This ip is not compare with those\n");
		return 0;
	}
	return reg_offset[gmac][i];
}
int tnkhw_ipv6address_rst(uint8_t *ipv6_addr, int gmac)
{
	uint32_t reg_offset, i;
	unsigned long flags;
	int ipv6_cnt;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);
	/* bonjour function for TOE */
	ipv6_cnt = (gmac == 0) ? --eth0_ipv6_cnt : --eth1_ipv6_cnt;

	if (ipv6_cnt < 0) {
		ipv6_cnt = (gmac == 0) ? ++eth0_ipv6_cnt : ++eth1_ipv6_cnt;
		TNKBD_DBG("ipv6_cnt = %d\n", ipv6_cnt);
		printk(KERN_ERR "TOE ipv6 address are all deleted\n");
		spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
		return -1;
	}
	reg_offset = tnkhw_ipv6select_reg_rst(gmac, ipv6_addr);
	TNKBD_DBG("ipv6_cnt = %d\n", ipv6_cnt);
	if (reg_offset == 0) {
		ipv6_cnt = (gmac == 0) ? ++eth0_ipv6_cnt : ++eth1_ipv6_cnt;
		TNKBD_DBG("ipv6_cnt = %d\n", ipv6_cnt);
		spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
		return -1;
	}
	if (ipv6_addr) {
		TNKBD_DBG("Setting GMAC %d ipv6 address to", gmac);
		TNKBD_DBG
			(" %x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x\n",
			 ipv6_addr[0], ipv6_addr[1], ipv6_addr[2],
			 ipv6_addr[3], ipv6_addr[4], ipv6_addr[5],
			 ipv6_addr[6], ipv6_addr[7], ipv6_addr[8],
			 ipv6_addr[9], ipv6_addr[10], ipv6_addr[11],
			 ipv6_addr[12], ipv6_addr[13], ipv6_addr[14],
			 ipv6_addr[15]);
	} else
		TNKBD_DBG("Setting GMAC %d ipv6 address to 0\n", gmac);

	for (i = 0; i < 4; i++)
		writel(0, tnkhw_ioaddr + reg_offset + (i * 4));

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);

	return 0;
}
#endif

int tnkhw_ipv4address_set(uint8_t *ipv4_addr, int gmac)
{
	uint32_t addr = 0, reg_offset;
	unsigned long flags;
#if defined(TNK_BONJOUR) || defined(TNK_BONDING)
	int ipv4_cnt;
#endif
	spin_lock_irqsave(&tnkhw_reg_lock, flags);

	if (ipv4_addr) {
#if defined(TNK_BONJOUR) || defined(TNK_BONDING)
		TNKBD_DBG("Setting GMAC %d ipv4 address to %d.%d.%d.%d\n",
			gmac, ipv4_addr[0], ipv4_addr[1], ipv4_addr[2],
			ipv4_addr[3]);
#endif
		addr =
		    (ipv4_addr[0] << 24) | (ipv4_addr[1] << 16) | (ipv4_addr[2]
								   << 8) |
		    ipv4_addr[3];
	} else
		TNK_DBG("Setting GMAC %d ipv4 address to 0\n", gmac);
#if defined(TNK_BONJOUR) || defined(TNK_BONDING)
	/* bonjour function for TOE */
	ipv4_cnt = (gmac == 0) ? ++eth0_ipv4_cnt : ++eth1_ipv4_cnt;
	if (ipv4_cnt > TNK_IP_MAX_NUM) {
		ipv4_cnt = (gmac == 0) ? --eth0_ipv4_cnt : --eth1_ipv4_cnt;
		printk(KERN_ERR"TOE only support 4 IPaddrs on one dev\n");
		printk(KERN_ERR"This IP will not be actived for logic\n");
		spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
		return -1;
	}
	TNKBD_DBG("ipv4_cnt = %d\n", ipv4_cnt);

	reg_offset = tnkhw_ipv4select_reg_write(gmac);
	if (reg_offset == 0) {
		ipv4_cnt = (gmac == 0) ? --eth0_ipv4_cnt : --eth1_ipv4_cnt;
		TNKBD_DBG(" The ipv4_cnt = %d,", ipv4_cnt);
		TNKBD_DBG(" however there are not usefull Ipaddrs registors\n");
		spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
		return -1;
	}
#else
	reg_offset = (gmac == 1) ? TNK_REG_CT_LOCAL_IPADDR1
		: TNK_REG_CT_LOCAL_IPADDR0;
	writel(addr, tnkhw_ioaddr + reg_offset);
#endif
#if defined(TNK_BONJOUR) || defined(TNK_BONDING)
	if (gmac == 0) {
		TNKBD_DBG("TNK_REG_CT_LOCAL_IPADDR0 = %x\n",
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR0));
		TNKBD_DBG("TNK_REG_CT_LOCAL_IPADDR0_1 = %x\n",
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR0_1));
		TNKBD_DBG("TNK_REG_CT_LOCAL_IPADDR0_2 = %x\n",
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR0_2));
		TNKBD_DBG("TNK_REG_CT_LOCAL_IPADDR0_3 = %x\n",
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR0_3));
		writel(addr, tnkhw_ioaddr + reg_offset);
		TNKBD_DBG("TNK_REG_CT_LOCAL_IPADDR0 = %x\n",
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR0));
		TNKBD_DBG("TNK_REG_CT_LOCAL_IPADDR0_1 = %x\n",
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR0_1));
		TNKBD_DBG("TNK_REG_CT_LOCAL_IPADDR0_2 = %x\n",
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR0_2));
		TNKBD_DBG("TNK_REG_CT_LOCAL_IPADDR0_3 = %x\n",
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR0_3));
	}
	if (gmac == 1) {
		TNKBD_DBG("TNK_REG_CT_LOCAL_IPADDR1 = %x\n",
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR1));
		TNKBD_DBG("TNK_REG_CT_LOCAL_IPADDR1_1 = %x\n",
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR1_1));
		TNKBD_DBG("TNK_REG_CT_LOCAL_IPADDR1_2 = %x\n",
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR1_2));
		TNKBD_DBG("TNK_REG_CT_LOCAL_IPADDR1_3 = %x\n",
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR1_3));
		writel(addr, tnkhw_ioaddr + reg_offset);
		TNKBD_DBG("TNK_REG_CT_LOCAL_IPADDR1 = %x\n",
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR1));
		TNKBD_DBG("TNK_REG_CT_LOCAL_IPADDR1_1 = %x\n",
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR1_1));
		TNKBD_DBG("TNK_REG_CT_LOCAL_IPADDR1_2 = %x\n",
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR1_2));
		TNKBD_DBG("TNK_REG_CT_LOCAL_IPADDR1_3 = %x\n",
				readl(tnkhw_ioaddr
					+ TNK_REG_CT_LOCAL_IPADDR1_3));
	}
#endif

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);

	return 0;
}

int tnkhw_ipv6address_set(uint8_t *ipv6_addr, int gmac)
{
	uint32_t reg_offset, i;
	unsigned long flags;
#if defined(TNK_BONDING) || defined(TNK_BONJOUR)
	int ipv6_cnt;
#endif
	spin_lock_irqsave(&tnkhw_reg_lock, flags);

#if defined(TNK_BONDING) || defined(TNK_BONJOUR)
	/* bonjour function for TOE */
	ipv6_cnt = (gmac == 0) ? ++eth0_ipv6_cnt : ++eth1_ipv6_cnt;
	if (ipv6_cnt > TNK_IP_MAX_NUM) {
		ipv6_cnt = (gmac == 0) ? --eth0_ipv6_cnt : --eth1_ipv6_cnt;
		printk(KERN_ERR"TOE only support 4 IPaddrs on one dev\n");
		printk(KERN_ERR"This ip will not be actived for logic\n");
		spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
		return -1;
	}
	reg_offset = tnkhw_ipv6select_reg_write(gmac);
	TNKBD_DBG("ipv6_cnt = %d\n", ipv6_cnt);
	if (reg_offset == 0) {
		ipv6_cnt = (gmac == 0) ? --eth0_ipv6_cnt : --eth1_ipv6_cnt;
		TNKBD_DBG(" The ipv6_cnt = %d,", ipv6_cnt);
		TNKBD_DBG(" however there are not usefull Ipaddrs registors\n");
		spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
		return -1;
	}
#else
	reg_offset =
	    (gmac ==
	     1) ? TNK_REG_CT_LOCAL_IPV6_ADDR1_W0 :
	    TNK_REG_CT_LOCAL_IPV6_ADDR0_W0;
#endif
	if (ipv6_addr) {
#if defined(TNK_BONDING) || defined(TNK_BONJOUR)
		TNKBD_DBG
			("Setting GMAC %d ipv6 address to",
			 gmac);
		TNKBD_DBG(" %x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x\n",
				ipv6_addr[0], ipv6_addr[1], ipv6_addr[2],
				ipv6_addr[3], ipv6_addr[4], ipv6_addr[5],
				ipv6_addr[6], ipv6_addr[7], ipv6_addr[8],
				ipv6_addr[9], ipv6_addr[10], ipv6_addr[11],
				ipv6_addr[12], ipv6_addr[13], ipv6_addr[14],
				ipv6_addr[15]);
#endif
	} else
		TNK_DBG("Setting GMAC %d ipv6 address to 0\n", gmac);

	for (i = 0; i < 4; i++) {
		uint32_t addr = 0;
		if (ipv6_addr)
			addr = (ipv6_addr[0 + (i * 4)] << 24) |
			    (ipv6_addr[1 + (i * 4)] << 16) |
			    (ipv6_addr[2 + (i * 4)] << 8) |
			    ipv6_addr[3 + (i * 4)];

		writel(addr, tnkhw_ioaddr + reg_offset + (i * 4));
	}

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);

	return 0;
}

void tnkhw_stats_get(struct tnkhw_stats *stats)
{
	memcpy(stats, &tnk_stats, sizeof(struct tnkhw_stats));
}

void tnkhw_conn_stats_get(unsigned cindex, struct tnkhw_conn_stats *stats)
{
	struct tnkhw_tx_dma_info *t = &tnk_tx_dma_list[cindex];

	TNK_CINDEX_VALIDATE(cindex);

	memcpy(stats, &tnk_conn_stats[cindex], sizeof(struct tnkhw_conn_stats));
	tnkhw_ttx_txstat_get(cindex, &stats->tx_acked_count,
			     &stats->tx_retry_count);
	tnkhw_ttx_rxstat_get(cindex, &stats->rxed_byte_count);
	stats->ttx_pipelined_bytes = atomic_read(&t->pipelined_bytes);
}

void tnkhw_stats_clearall(void)
{
	int cindex;

	memset(&tnk_stats, 0, sizeof(struct tnkhw_stats));
	memset(tnk_conn_stats, 0,
	       tnk_max_connections * sizeof(struct tnkhw_conn_stats));

	for (cindex = TNK_TTX_CINDEX_START; cindex < tnk_max_connections;
	     cindex++) {
		tnkhw_ttx_txstat_clear(cindex);
		tnkhw_ttx_rxstat_clear(cindex);
	}
}

void tnkhw_inc_persist(void)
{
	int val;
	int count;
	int mask = TNK_MASK_CTRL1_PERSISTCOUNT << TNK_OFFSET_CTRL1_PERSISTCOUNT;
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);

	val = readl(tnkhw_ioaddr + TNK_REG_TOE_CTRL1);
	count = (val & mask) + 1;
	val = (val & ~mask) | (count & mask);
	writel(val, tnkhw_ioaddr + TNK_REG_TOE_CTRL1);

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
}

int tnkhw_tx_done_test(unsigned cindex)
{
	struct tnkhw_tx_dma_info *t = &tnk_tx_dma_list[cindex];

	return (t->tail == t->head);
}

int tnkhw_tx_done_wait(unsigned cindex, unsigned timeout_jiffies)
{
	struct tnkhw_tx_dma_info *t = &tnk_tx_dma_list[cindex];

	while (t->tail != t->head && timeout_jiffies > 0 && !t->rst_received) {
		DEFINE_WAIT(wait);

		prepare_to_wait(&t->waitqueue, &wait, TASK_INTERRUPTIBLE);
		if (t->tail != t->head)
			timeout_jiffies = schedule_timeout(timeout_jiffies);
		finish_wait(&t->waitqueue, &wait);
	}
	return atomic_read(&t->pipelined_bytes);
}

inline void tnkhw_rcv_rst(unsigned cindex)
{
	struct tnkhw_tx_dma_info *t = &tnk_tx_dma_list[cindex];

	t->rst_received = 1;

	if (waitqueue_active(&t->waitqueue))
		wake_up_interruptible(&t->waitqueue);
}

#define TNKHW_CT_LOC_RTT_SAMPLE 22
#define TNKHW_CT_MASK_RTT_SAMPLE_VALID 0x10000
#define TNKHW_CT_MASK_RTT_SAMPLE_VALUE 0xFFFF
int tnkhw_rtt_sample(unsigned cindex)
{
	uint32_t data;
	int ret;
	unsigned long flags;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);

	tnkhw_ct_read(cindex, TNKHW_CT_LOC_RTT_SAMPLE, &data);

	if (data & TNKHW_CT_MASK_RTT_SAMPLE_VALID) {
		/*  Clear the RTT sample valid bit, so that a new sample
		 *  will be collected */
		tnkhw_ct_write(cindex, TNKHW_CT_LOC_RTT_SAMPLE, 0);

		ret = data & TNKHW_CT_MASK_RTT_SAMPLE_VALUE;
		/* Assuming RTT value > TNK_TX_RTT_FILITER indicates
		 * invalid sample */
		if (ret > TNK_TX_RTT_FILITER)
			ret = -1;
	} else
		ret = -1; /*  negative number indicates invalid sample */

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);

	return ret;
}

#define TNKHW_CT_LOC_TXTIMEOUT 16
void tnkhw_txtimeout_update(unsigned cindex, unsigned timeout_ms)
{
	unsigned long flags;

	if (timeout_ms > TNK_TX_TIMEOUT_MAX)
		timeout_ms = TNK_TX_TIMEOUT_MAX;

	if (timeout_ms < TNK_TX_TIMEOUT_MIN)
		timeout_ms = TNK_TX_TIMEOUT_MIN;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);

	/*  We only want to write the lower 16 bits of the CT entry */
	writel(0x3, tnkhw_ioaddr + TNK_REG_CT_CPU_WRMASK);
	tnkhw_ct_write(cindex, TNKHW_CT_LOC_TXTIMEOUT, timeout_ms & 0xFFFFF);
	writel(0xF, tnkhw_ioaddr + TNK_REG_CT_CPU_WRMASK);

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);
}

#ifdef TNK_RX_CHANNEL_FLOW_CONTROL

#define TNKHW_CT_LOC_TXADVWND 12
void tnkhw_txadvwnd_update(unsigned cindex, unsigned tx_adv_wnd)
{
	unsigned long flags;

	if (tx_adv_wnd > 0xFFFF)
		tx_adv_wnd = 0xFFFF;

	spin_lock_irqsave(&tnkhw_reg_lock, flags);

	/*  We only want to write the upper 16 bits of the CT entry */
	writel(0xC, tnkhw_ioaddr + TNK_REG_CT_CPU_WRMASK);
	tnkhw_ct_write(cindex, TNKHW_CT_LOC_TXADVWND,
		       (tx_adv_wnd << 16) & 0xFFFF0000);
	writel(0xF, tnkhw_ioaddr + TNK_REG_CT_CPU_WRMASK);

	spin_unlock_irqrestore(&tnkhw_reg_lock, flags);

	TNK_DBG("%s: Updated tx_adv_wnd to %u for cid %u\n",
		__func__, cindex, tx_adv_wnd);
}

#endif
