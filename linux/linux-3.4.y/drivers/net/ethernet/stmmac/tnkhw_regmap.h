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
 *  TNK hardware register map
 */

#ifndef __TNKHW_REGMAP_H__
#define __TNKHW_REGMAP_H__

/* Offsets in the overall register map for each logical block
 * Note that these are relative to a base I/O address which needs
 * to be mapped into kernel virtual memory using ioremap() or similar
 */

/* GMAC0 - Includes CSR's for the 3 channel DMA engine
 * and the MAC port 0 instance */
#define TNK_GMAC0_CSR_OFFSET         (0x00000)
/* GMAC1 - CSR's for MAC port 1 instance only */
#define TNK_GMAC1_CSR_OFFSET         (0x04000)
/* TOENK Engine CSRs */
#define TNK_ENGINE_CSR_OFFSET        (0x08000)
/* TOENK System CSRs */
#define TNK_SYSTEM_CSR_OFFSET        (0x09000)
/* TOE transmit descriptor database */
#define TNK_DMA_TTX_DB_OFFSET        (0x10000)
/* TOE transmit statistics per channel */
#define TNK_DMA_TTX_DB_TXSTAT_OFFSET (0x18000)
/* TOE receive statistics per channel */
#define TNK_DMA_TTX_DB_RXSTAT_OFFSET (0x1C000)
/* TOE transmit DMA engine CSRs */
#define TNK_DMA_TTX_DB_CSR_OFFSET    (0x1E000)

/* Connection Table access registers */
#define TNK_REG_CT_CPU_WRADDR          \
	(TNK_ENGINE_CSR_OFFSET + 0x000) /* RW */
#define TNK_REG_CT_CPU_RDADDR          \
	(TNK_ENGINE_CSR_OFFSET + 0x004) /* RW */
#define TNK_REG_CT_CPU_WRDATA          \
	(TNK_ENGINE_CSR_OFFSET + 0x008) /* RW */
#define TNK_REG_CT_CPU_RDDATA          \
	(TNK_ENGINE_CSR_OFFSET + 0x00C) /* RO */
#define TNK_REG_CT_CPU_ACSTAT          \
	(TNK_ENGINE_CSR_OFFSET + 0x010) /* RO */
#define TNK_REG_CT_CPU_WRMASK          \
	(TNK_ENGINE_CSR_OFFSET + 0x014) /* RW */

/* Tx/Rx Engine configuration registers */
#define TNK_REG_CT_LOCAL_MACADDR0_U16   \
	(TNK_ENGINE_CSR_OFFSET + 0x100) /* RW */
#define TNK_REG_CT_LOCAL_MACADDR0_L32   \
	(TNK_ENGINE_CSR_OFFSET + 0x104) /* RW */
#define TNK_REG_CT_LOCAL_IPADDR0        \
	(TNK_ENGINE_CSR_OFFSET + 0x108) /* RW */
#define TNK_REG_CT_REMOTE_IPADDR        \
	(TNK_ENGINE_CSR_OFFSET + 0x120) /* RW */
#define TNK_REG_CT_TCP_PORTS            \
	(TNK_ENGINE_CSR_OFFSET + 0x124) /* RW */
#define TNK_REG_CT_CONN_CTRL            \
	(TNK_ENGINE_CSR_OFFSET + 0x128) /* RW */
#define TNK_REG_TOE_INTR_STAT           \
	(TNK_ENGINE_CSR_OFFSET + 0x13C) /* RW */
#define TNK_REG_TOE_INTR_EN             \
	(TNK_ENGINE_CSR_OFFSET + 0x140) /* RW */
#define TNK_REG_TOE_CTRL0               \
	(TNK_ENGINE_CSR_OFFSET + 0x144) /* RW */
#define TNK_REG_TOE_CTRL1               \
	(TNK_ENGINE_CSR_OFFSET + 0x148) /* RW */
#define TNK_REG_TOE_CTRL2               \
	(TNK_ENGINE_CSR_OFFSET + 0x14C) /* RW */
#define TNK_REG_TOE_CONFIG0             \
	(TNK_ENGINE_CSR_OFFSET + 0x150) /* RW */
#define TNK_REG_CT_LOCAL_MACADDR1_U16   \
	(TNK_ENGINE_CSR_OFFSET + 0x154) /* RW */
#define TNK_REG_CT_LOCAL_MACADDR1_L32   \
	(TNK_ENGINE_CSR_OFFSET + 0x158) /* RW */
#define TNK_REG_CT_LOCAL_IPADDR1        \
	(TNK_ENGINE_CSR_OFFSET + 0x15C) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR0_W0  \
	(TNK_ENGINE_CSR_OFFSET + 0x160) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR0_W1  \
	(TNK_ENGINE_CSR_OFFSET + 0x164) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR0_W2  \
	(TNK_ENGINE_CSR_OFFSET + 0x168) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR0_W3  \
	(TNK_ENGINE_CSR_OFFSET + 0x16C) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR1_W0  \
	(TNK_ENGINE_CSR_OFFSET + 0x170) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR1_W1  \
	(TNK_ENGINE_CSR_OFFSET + 0x174) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR1_W2  \
	(TNK_ENGINE_CSR_OFFSET + 0x178) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR1_W3  \
	(TNK_ENGINE_CSR_OFFSET + 0x17C) /* RW */
#define TNK_REG_CT_REMOTE_IPV6_ADDR_W0  \
	(TNK_ENGINE_CSR_OFFSET + 0x180) /* RW */
#define TNK_REG_CT_REMOTE_IPV6_ADDR_W1  \
	(TNK_ENGINE_CSR_OFFSET + 0x184) /* RW */
#define TNK_REG_CT_REMOTE_IPV6_ADDR_W2  \
	(TNK_ENGINE_CSR_OFFSET + 0x188) /* RW */
#define TNK_REG_CT_REMOTE_IPV6_ADDR_W3  \
	(TNK_ENGINE_CSR_OFFSET + 0x18C) /* RW */
#define TNK_REG_TOE_SWITCH_CTRL	\
	(TNK_ENGINE_CSR_OFFSET + 0x1C0) /* RW */
#define TNK_REG_TOE_BONDING_CTRL	\
	(TNK_ENGINE_CSR_OFFSET + 0x1d0) /* RW */
#define TNK_REG_CT_LOCAL_IPADDR_SEL     \
	(TNK_ENGINE_CSR_OFFSET + 0x300) /* RW */
#define TNK_REG_CT_LOCAL_IPADDR0_1      \
	(TNK_ENGINE_CSR_OFFSET + 0x304) /* RW */
#define TNK_REG_CT_LOCAL_IPADDR0_2      \
	(TNK_ENGINE_CSR_OFFSET + 0x308) /* RW */
#define TNK_REG_CT_LOCAL_IPADDR0_3      \
	(TNK_ENGINE_CSR_OFFSET + 0x30c) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR0_W0_1	\
	(TNK_ENGINE_CSR_OFFSET + 0x310) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR0_W1_1	\
	(TNK_ENGINE_CSR_OFFSET + 0x314) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR0_W2_1	\
	(TNK_ENGINE_CSR_OFFSET + 0x318) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR0_W3_1	\
	(TNK_ENGINE_CSR_OFFSET + 0x31c) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR0_W0_2	\
	(TNK_ENGINE_CSR_OFFSET + 0x320) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR0_W1_2	\
	(TNK_ENGINE_CSR_OFFSET + 0x324) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR0_W2_2	\
	(TNK_ENGINE_CSR_OFFSET + 0x328) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR0_W3_2	\
	(TNK_ENGINE_CSR_OFFSET + 0x32c) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR0_W0_3	\
	(TNK_ENGINE_CSR_OFFSET + 0x330) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR0_W1_3	\
	(TNK_ENGINE_CSR_OFFSET + 0x334) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR0_W2_3	\
	(TNK_ENGINE_CSR_OFFSET + 0x338) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR0_W3_3	\
	(TNK_ENGINE_CSR_OFFSET + 0x33c) /* RW */
#define TNK_REG_CT_LOCAL_IPADDR1_1		\
	(TNK_ENGINE_CSR_OFFSET + 0x344) /* RW */
#define TNK_REG_CT_LOCAL_IPADDR1_2		\
	(TNK_ENGINE_CSR_OFFSET + 0x348) /* RW */
#define TNK_REG_CT_LOCAL_IPADDR1_3		\
	(TNK_ENGINE_CSR_OFFSET + 0x34c) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR1_W0_1	\
	(TNK_ENGINE_CSR_OFFSET + 0x350) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR1_W1_1	\
	(TNK_ENGINE_CSR_OFFSET + 0x354) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR1_W2_1	\
	(TNK_ENGINE_CSR_OFFSET + 0x358) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR1_W3_1	\
	(TNK_ENGINE_CSR_OFFSET + 0x35c) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR1_W0_2	\
	(TNK_ENGINE_CSR_OFFSET + 0x360) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR1_W1_2	\
	(TNK_ENGINE_CSR_OFFSET + 0x364) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR1_W2_2	\
	(TNK_ENGINE_CSR_OFFSET + 0x368) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR1_W3_2	\
	(TNK_ENGINE_CSR_OFFSET + 0x36c) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR1_W0_3	\
	(TNK_ENGINE_CSR_OFFSET + 0x370) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR1_W1_3	\
	(TNK_ENGINE_CSR_OFFSET + 0x374) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR1_W2_3	\
	(TNK_ENGINE_CSR_OFFSET + 0x378) /* RW */
#define TNK_REG_CT_LOCAL_IPV6_ADDR1_W3_3	\
	(TNK_ENGINE_CSR_OFFSET + 0x37c) /* RW */
#define TNK_REG_CT_LOCAL_IPADDR_INDEX_SEL	\
	(TNK_ENGINE_CSR_OFFSET + 0x380) /* RW */

/* TCP/UDP Connection configuration registers */
#define TNK_REG_CT_TCP_MSS             (TNK_ENGINE_CSR_OFFSET + 0x118) /* RW */
#define TNK_REG_CT_RTT_STATUS          (TNK_ENGINE_CSR_OFFSET + 0x11C) /* RO */

/* TCP/UDP Connection status registers */

/* TNK System interrupt registers */
#define TNK_REG_INTR_STAT              (TNK_SYSTEM_CSR_OFFSET + 0x000) /* RO */
#define TNK_REG_INTR_EN                (TNK_SYSTEM_CSR_OFFSET + 0x004) /* RW */
#define TNK_REG_RESET_STAT             (TNK_SYSTEM_CSR_OFFSET + 0x008) /* RW */
#define TNK_REG_ID                     (TNK_SYSTEM_CSR_OFFSET + 0x00C) /* RW */
#define TNK_REG_AHB_DEBUG_OUT          (TNK_SYSTEM_CSR_OFFSET + 0x100) /* RW */
#define TNK_REG_AHB_DEBUG_IN           (TNK_SYSTEM_CSR_OFFSET + 0x120) /* RW */

/* TNK DMA TTX registers */
#define TNK_REG_DMA_TTX_CTRL0 \
	(TNK_DMA_TTX_DB_CSR_OFFSET + 0x000) /* RW */
#define TNK_REG_DMA_TTX_ADD_SIZE \
	(TNK_DMA_TTX_DB_CSR_OFFSET + 0x004) /* RW */
#define TNK_REG_DMA_TTX_ADD \
	(TNK_DMA_TTX_DB_CSR_OFFSET + 0x008) /* RW */
#define TNK_REG_DMA_TTX_ACKD_CID \
	(TNK_DMA_TTX_DB_CSR_OFFSET + 0x00C) /* RO */
#define TNK_REG_DMA_TTX_ERR_STATUS \
	(TNK_DMA_TTX_DB_CSR_OFFSET + 0x010) /* RO */
#define TNK_REG_DMA_TTX_DFETCH_ERR_CID \
	(TNK_DMA_TTX_DB_CSR_OFFSET + 0x014) /* RO */
#define TNK_REG_DMA_TTX_RETRY_ERR_CID \
	(TNK_DMA_TTX_DB_CSR_OFFSET + 0x018) /* RO */
#define TNK_REG_DMA_TTX_ACKD_ERR_CID \
	(TNK_DMA_TTX_DB_CSR_OFFSET + 0x01C) /* RO */
#define TNK_REG_DMA_TRX_CTRL \
	(TNK_DMA_TTX_DB_CSR_OFFSET + 0x020) /* RW */
#define TNK_REG_DMA_TRX_ADD_SIZE \
	(TNK_DMA_TTX_DB_CSR_OFFSET + 0x024) /* RW */
#define TNK_REG_DMA_TTX_ALT_REQ \
	(TNK_DMA_TTX_DB_CSR_OFFSET + 0x028) /* RW */


/* WRADDR register fields */
#define TNK_OFFSET_CT_CPU_WRADDR_LOC       0
#define TNK_MASK_CT_CPU_WRADDR_LOC         0x0000001F
#define TNK_OFFSET_CT_CPU_WRADDR_CSEL      5
#define TNK_MASK_CT_CPU_WRADDR_CSEL        0x000007FF

/* RDADDR register fields */
#define TNK_OFFSET_CT_CPU_RDADDR_LOC       0
#define TNK_MASK_CT_CPU_RDADDR_LOC         0x0000001F
#define TNK_OFFSET_CT_CPU_RDADDR_CSEL      5
#define TNK_MASK_CT_CPU_RDADDR_CSEL        0x000007FF

/* ACSTAT register fields */
#define TNK_OFFSET_CT_CPU_ACSTAT_RDDONE    0
#define TNK_MASK_CT_CPU_ACSTAT_RDDONE      0x00000001
#define TNK_OFFSET_CT_CPU_ACSTAT_WRDONE    1
#define TNK_MASK_CT_CPU_ACSTAT_WRDONE      0x00000001

/* LOCAL_MACADDR_U16 register fields */
#define TNK_MASK_CT_LOCAL_MACADDR_U16      0x0000FFFF

/* TCP_MSS register fields */
#define TNK_MASK_CT_TCP_MSS                0x000007FF

/* TCP_PORTS register fields */
#define TNK_OFFSET_CT_TCP_PORTS_LOCAL      16
#define TNK_MASK_CT_TCP_PORTS_LOCAL        0x0000FFFF
#define TNK_OFFSET_CT_TCP_PORTS_REMOTE     0
#define TNK_MASK_CT_TCP_PORTS_REMOTE       0x0000FFFF

/* CONN_CTRL register fields */
#define TNK_OFFSET_CT_CONN_CTRL_ERR        16
#define TNK_MASK_CT_CONN_CTRL_ERR          0x00000001
#define TNK_OFFSET_CT_CONN_CTRL_IPV6       15
#define TNK_MASK_CT_CONN_CTRL_IPV6         0x00000001
#define TNK_OFFSET_CT_CONN_CTRL_CSEL       2
#define TNK_MASK_CT_CONN_CTRL_CSEL         0x000007FF
#define TNK_OFFSET_CT_CONN_CTRL_ADD        1
#define TNK_MASK_CT_CONN_CTRL_ADD          0x00000001
#define TNK_OFFSET_CT_CONN_CTRL_UPDATE     0
#define TNK_MASK_CT_CONN_CTRL_UPDATE       0x00000001

/* CONN_STAT register fields */
#define TNK_OFFSET_CT_CONN_STAT_RXDROPPED  31
#define TNK_MASK_CT_CONN_STAT_RXDROPPED    0x00000001
#define TNK_OFFSET_CT_CONN_STAT_INTR_VALID 11
#define TNK_MASK_CT_CONN_INTR_VALID        0x00000001
#define TNK_OFFSET_CT_CONN_STAT_CSEL       0
#define TNK_MASK_CT_CONN_STAT_CSEL         0x000007FF


/* TNK_INTR_STAT register fields */
#define TNK_OFFSET_INTR_STAT_DMA_TTX_ACK   0
#define TNK_MASK_INTR_STAT_DMA_TTX_ACK     0x00000001
#define TNK_OFFSET_INTR_STAT_DMA_TTX_ERR   1
#define TNK_MASK_INTR_STAT_DMA_TTX_ERR     0x00000001
#define TNK_OFFSET_INTR_STAT_DMA_CH0       2
#define TNK_MASK_INTR_STAT_DMA_CH0         0x00000001
#define TNK_OFFSET_INTR_STAT_DMA_CH1       3
#define TNK_MASK_INTR_STAT_DMA_CH1         0x00000001
#define TNK_OFFSET_INTR_STAT_DMA_CH2       4
#define TNK_MASK_INTR_STAT_DMA_CH2         0x00000001
#define TNK_OFFSET_INTR_STAT_GMAC0         5
#define TNK_MASK_INTR_STAT_GMAC0           0x00000001
#define TNK_OFFSET_INTR_STAT_GMAC1         6
#define TNK_MASK_INTR_STAT_GMAC1           0x00000001
#define TNK_OFFSET_INTR_STAT_TOE           7
#define TNK_MASK_INTR_STAT_TOE             0x00000001

/* TNK_INTR_EN register fields */
#define TNK_OFFSET_INTR_EN_DMA_TTX_ACK   0
#define TNK_MASK_INTR_EN_DMA_TTX_ACK     0x00000001
#define TNK_OFFSET_INTR_EN_DMA_TTX_ERR   1
#define TNK_MASK_INTR_EN_DMA_TTX_ERR     0x00000001
#define TNK_OFFSET_INTR_EN_DMA_CH0       2
#define TNK_MASK_INTR_EN_DMA_CH0         0x00000001
#define TNK_OFFSET_INTR_EN_DMA_CH1       3
#define TNK_MASK_INTR_EN_DMA_CH1         0x00000001
#define TNK_OFFSET_INTR_EN_DMA_CH2       4
#define TNK_MASK_INTR_EN_DMA_CH2         0x00000001
#define TNK_OFFSET_INTR_EN_GMAC0         5
#define TNK_MASK_INTR_EN_GMAC0           0x00000001
#define TNK_OFFSET_INTR_EN_GMAC1         6
#define TNK_MASK_INTR_EN_GMAC1           0x00000001
#define TNK_OFFSET_INTR_EN_TOE           7
#define TNK_MASK_INTR_EN_TOE             0x00000001

/* TOE_CTRL1 register fields */
#define TNK_OFFSET_CTRL1_PERSISTCOUNT    0
#define TNK_MASK_CTRL1_PERSISTCOUNT      0x000000FF

/* TOE_CONFIG0 register fields */
#define TNK_OFFSET_CONFIG0_PROMISCMODE    0
#define TNK_MASK_CONFIG0_PROMISCMODE      0x00000001
#define TNK_OFFSET_CONFIG0_RXBYPASSMODE   1
#define TNK_MASK_CONFIG0_RXBYPASSMODE     0x00000001
#define TNK_OFFSET_CONFIG0_IPV6_EN        2
#define TNK_MASK_CONFIG0_IPV6_EN          0x00000001
#define TNK_OFFSET_CONFIG0_VLAN_EN        3
#define TNK_MASK_CONFIG0_VLAN_EN          0x00000001
#define TNK_OFFSET_CONFIG0_CONG_WIN_EN    4
#define TNK_MASK_CONFIG0_CONG_WIN_EN      0x00000001
#define TNK_OFFSET_CONFIG0_FORCE_RD_CT    5
#define TNK_MASK_CONFIG0_FORCE_RD_CT      0x00000001
#define TNK_OFFSET_CONFIG0_MAC_SPACE_CHK  6
#define TNK_MASK_CONFIG0_MAC_SPACE_CHK    0x00000001

/* TNK_ID register fields */
#define TNK_OFFSET_ID_MAX_CONNECTIONS    0
#define TNK_MASK_ID_MAX_CONNECTIONS      0x0000FFFF
#define TNK_OFFSET_ID_HWID               16
#define TNK_MASK_ID_HWID                 0x0000FFFF

/* DMA_TTX_ADD_SIZE register fields */
#define TNK_OFFSET_DMA_TTX_ADD_SIZE      0
#define TNK_MASK_DMA_TTX_ADD_SIZE        0x00FFFFFF

/* DMA_TTX_ADD register fields */
#define TNK_OFFSET_DMA_TTX_ADD_CID       0
#define TNK_MASK_DMA_TTX_ADD_CID         0x000007FF
#define TNK_OFFSET_DMA_TTX_ADD_FLUSH     16
#define TNK_MASK_DMA_TTX_ADD_FLUSH       0x00000001

/* DMA_TTX_ALT_REQ register fields */
#define TNK_OFFSET_DMA_TTX_ALT_REQ_CID   0
#define TNK_MASK_DMA_TTX_ALT_REQ_CID     0x000007FF
#define TNK_OFFSET_DMA_TTX_ALT_REQ_SIZE  11
#define TNK_MASK_DMA_TTX_ALT_REQ_SIZE    0x001FFFFF

/* DMA_TTX_ACKD_CID register fields */
#define TNK_OFFSET_DMA_TTX_ACKD_OVFLOW   0
#define TNK_MASK_DMA_TTX_ACKD_OVFLOW     0x00000001
#define TNK_OFFSET_DMA_TTX_ACKD_VALID    1
#define TNK_MASK_DMA_TTX_ACKD_VALID      0x00000001
#define TNK_OFFSET_DMA_TTX_ACKD_CID      4
#define TNK_MASK_DMA_TTX_ACKD_CID        0x000007FF

/* DMA_TTX_ERR_STATUS register fields */
#define TNK_OFFSET_TTX_ERR_DFETCH0       0
#define TNK_MASK_TTX_ERR_DFETCH0         0x00000001
#define TNK_OFFSET_TTX_ERR_DFETCH1       1
#define TNK_MASK_TTX_ERR_DFETCH1         0x00000001
#define TNK_OFFSET_TTX_ERR_DFETCH2       2
#define TNK_MASK_TTX_ERR_DFETCH2         0x00000001
#define TNK_OFFSET_TTX_ERR_RETRY0        3
#define TNK_MASK_TTX_ERR_RETRY0          0x00000001
#define TNK_OFFSET_TTX_ERR_RETRY1        4
#define TNK_MASK_TTX_ERR_RETRY1          0x00000001
#define TNK_OFFSET_TTX_ERR_ACKD0         5
#define TNK_MASK_TTX_ERR_ACKD0           0x00000001
#define TNK_OFFSET_TTX_ERR_ACKD1         6
#define TNK_MASK_TTX_ERR_ACKD1           0x00000001
#define TNK_OFFSET_TTX_ERR_ACKD2         7
#define TNK_MASK_TTX_ERR_ACKD2           0x00000001

/* DMA_TTX_DFETCH_ERR_CID register fields */
#define TNK_OFFSET_TTX_DFETCH_ERR_CID    0
#define TNK_MASK_TTX_DFETCH_ERR_CID      0x000007FF

/* DMA_TTX_RETRY_ERR_CID register fields */
#define TNK_OFFSET_TTX_RETRY_ERR_CID     0
#define TNK_MASK_TTX_RETRY_ERR_CID       0x000007FF

/* DMA_TTX_ACKD_ERR_CID register fields */
#define TNK_OFFSET_TTX_ACKD_ERR_CID      0
#define TNK_MASK_TTX_ACKD_ERR_CID        0x000007FF

/* DMA_TTX_CTRL0 register fields */
#define TNK_OFFSET_TTX_CTRL0_MAXBURSTLEN 0
#define TNK_MASK_TTX_CTRL0_MAXBURSTLEN   0x000000FF

/* DMA_TRX_CTRL register fields */
#define TNK_OFFSET_TRX_CTRL_RXBUFFERSIZE 0
#define TNK_MASK_TRX_CTRL_RXBUFFERSIZE   0x00000007
#define TNK_OFFSET_TRX_CTRL_TOE_STAT_EN  3
#define TNK_MASK_TRX_CTRL_TOE_STAT_EN    0x00000001
#define TNK_OFFSET_TRX_CTRL_IPC_STAT_EN  4
#define TNK_MASK_TRX_CTRL_IPC_STAT_EN    0x00000001

/* DMA_TRX_ADD_SIZE register fields */
#define TNK_OFFSET_TRX_ADD_COUNT         0
#define TNK_MASK_TRX_ADD_COUNT           0x00FFFFFF

/* TOE_INTR_STAT register fields */
#define TNK_OFFSET_TOE_INTR_STAT_FLUSH      0
#define TNK_MASK_TOE_INTR_STAT_FLUSH        0x00000001
#define TNK_OFFSET_TOE_INTR_STAT_TX_ERR     1
#define TNK_MASK_TOE_INTR_STAT_TX_ERR       0x00000001
#define TNK_OFFSET_TOE_INTR_STAT_TX_ERR_CID 2
#define TNK_MASK_TOE_INTR_STAT_TX_ERR_CID   0x000007FF
#define TNK_OFFSET_TOE_INTR_STAT_RETRY_MAX  13
#define TNK_MASK_TOE_INTR_STAT_RETRY_MAX    0x00000001
#define TNK_OFFSET_TOE_INTR_STAT_RETRY_CID  14
#define TNK_MASK_TOE_INTR_STAT_RETRY_CID    0x000007FF

/* TOE_INTR_EN register fields */
#define TNK_OFFSET_TOE_INTR_EN_FLUSH      0
#define TNK_MASK_TOE_INTR_EN_FLUSH        0x00000001
#define TNK_OFFSET_TOE_INTR_EN_TX_ERR     1
#define TNK_MASK_TOE_INTR_EN_TX_ERR       0x00000001
#define TNK_OFFSET_TOE_INTR_EN_RETRY_MAX  2
#define TNK_MASK_TOE_INTR_EN_RETRY_MAX    0x00000001

/*  TOE out of order register fields */
#define TNK_HW_OUT_OF_ORDER_ENABLE		0x81c0
#define	TNK_HW_OUT_OF_ORDER_CRTL		0x81e0
#define	TNK_HW_OUT_OF_ORDER_WVAL		0x81e4
#define	TNK_HW_OUT_OF_ORDER_RVAL		0x81e8
#define	TNK_HW_OUT_OF_ORDER_IS01_ENDNUM		0
#define	TNK_HW_OUT_OF_ORDER_IS01_SEQNUM		1
#define	TNK_HW_OUT_OF_ORDER_IS02_ENDNUM		2
#define	TNK_HW_OUT_OF_ORDER_IS02_SEQNUM		3
#define	TNK_HW_OUT_OF_ORDER_IS01_IS02_FLAG	4

#define TNK_TTX_DMA_DB_ENTRY_SIZE (4 * sizeof(uint32_t))
#define TNK_TTX_DMA_DB_TXSTAT_SIZE (2 * sizeof(uint32_t))
#define TNK_TTX_DMA_DB_RXSTAT_SIZE (1 * sizeof(uint32_t))

#endif /*  __TNKHW_REGMAP_H__ */
