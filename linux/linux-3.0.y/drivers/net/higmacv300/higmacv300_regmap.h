/*****************************************************************************
  This is the driver for the CreVinn GMAC-NK-2G TCP Offload Engine.
  GMAC-NK-2G incorporates a Synopsys Ethernet MAC core.

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
 *  GMAC hardware register map
 */

#ifndef __GMACHW_REGMAP_H__
#define __GMACHW_REGMAP_H__

/* Offsets in the overall register map for each logical block
 * Note that these are relative to a base I/O address which needs
 * to be mapped into kernel virtual memory using ioremap() or similar
 */

/* GMAC0 - Includes CSR's for the 3 channel DMA engine
 * and the MAC port 0 instance */
/*#define GMAC_CSR_OFFSET         (0x00000)*/
/* GMAC1 - CSR's for MAC port 1 instance only */
#define GMAC_CSR_OFFSET         (0x04000)
/* GMAC Engine CSRs */
#define GMAC_ENGINE_CSR_OFFSET        (0x08000)
/* GMAC System CSRs */
#define GMAC_SYSTEM_CSR_OFFSET        (0x09000)
/* GMAC transmit descriptor database */
#define GMAC_DMA_TTX_DB_OFFSET        (0x10000)
/* GMAC transmit statistics per channel */
#define GMAC_DMA_TTX_DB_TXSTAT_OFFSET (0x18000)
/* GMAC receive statistics per channel */
#define GMAC_DMA_TTX_DB_RXSTAT_OFFSET (0x1C000)
/* GMAC transmit DMA engine CSRs */
#define GMAC_DMA_TTX_DB_CSR_OFFSET    (0x1E000)

/* Connection Table access registers */
#define GMAC_REG_CT_CPU_WRADDR          (GMAC_ENGINE_CSR_OFFSET + 0x000)
#define GMAC_REG_CT_CPU_RDADDR          (GMAC_ENGINE_CSR_OFFSET + 0x004)
#define GMAC_REG_CT_CPU_WRDATA          (GMAC_ENGINE_CSR_OFFSET + 0x008)
#define GMAC_REG_CT_CPU_RDDATA          (GMAC_ENGINE_CSR_OFFSET + 0x00C)
#define GMAC_REG_CT_CPU_ACSTAT          (GMAC_ENGINE_CSR_OFFSET + 0x010)
#define GMAC_REG_CT_CPU_WRMASK          (GMAC_ENGINE_CSR_OFFSET + 0x014)

/* Tx/Rx Engine configuration registers */
#define GMAC_REG_CT_LOCAL_MACADDR0_U16   (GMAC_ENGINE_CSR_OFFSET + 0x100)
#define GMAC_REG_CT_LOCAL_MACADDR0_L32   (GMAC_ENGINE_CSR_OFFSET + 0x104)
#define GMAC_REG_CT_LOCAL_IPADDR0        (GMAC_ENGINE_CSR_OFFSET + 0x108)
#define GMAC_REG_CT_REMOTE_IPADDR        (GMAC_ENGINE_CSR_OFFSET + 0x120)
#define GMAC_REG_CT_TCP_PORTS            (GMAC_ENGINE_CSR_OFFSET + 0x124)
#define GMAC_REG_CT_CONN_CTRL            (GMAC_ENGINE_CSR_OFFSET + 0x128)
/*#define GMAC_REG_INTR_STAT           (GMAC_ENGINE_CSR_OFFSET + 0x13C)*/
/*#define GMAC_REG_INTR_EN             (GMAC_ENGINE_CSR_OFFSET + 0x140)*/
#define GMAC_REG_CTRL0               (GMAC_ENGINE_CSR_OFFSET + 0x144)
#define GMAC_REG_CTRL1               (GMAC_ENGINE_CSR_OFFSET + 0x148)
#define GMAC_REG_CTRL2               (GMAC_ENGINE_CSR_OFFSET + 0x14C)
#define GMAC_REG_CONFIG0             (GMAC_ENGINE_CSR_OFFSET + 0x150)
#define GMAC_REG_CT_LOCAL_MACADDR1_U16   (GMAC_ENGINE_CSR_OFFSET + 0x154)
#define GMAC_REG_CT_LOCAL_MACADDR1_L32   (GMAC_ENGINE_CSR_OFFSET + 0x158)
#define GMAC_REG_CT_LOCAL_IPADDR1        (GMAC_ENGINE_CSR_OFFSET + 0x15C)
#define GMAC_REG_CT_LOCAL_IPV6_ADDR0_W0  (GMAC_ENGINE_CSR_OFFSET + 0x160)
#define GMAC_REG_CT_LOCAL_IPV6_ADDR0_W1  (GMAC_ENGINE_CSR_OFFSET + 0x164)
#define GMAC_REG_CT_LOCAL_IPV6_ADDR0_W2  (GMAC_ENGINE_CSR_OFFSET + 0x168)
#define GMAC_REG_CT_LOCAL_IPV6_ADDR0_W3  (GMAC_ENGINE_CSR_OFFSET + 0x16C)
#define GMAC_REG_CT_LOCAL_IPV6_ADDR1_W0  (GMAC_ENGINE_CSR_OFFSET + 0x170)
#define GMAC_REG_CT_LOCAL_IPV6_ADDR1_W1  (GMAC_ENGINE_CSR_OFFSET + 0x174)
#define GMAC_REG_CT_LOCAL_IPV6_ADDR1_W2  (GMAC_ENGINE_CSR_OFFSET + 0x178)
#define GMAC_REG_CT_LOCAL_IPV6_ADDR1_W3  (GMAC_ENGINE_CSR_OFFSET + 0x17C)
#define GMAC_REG_CT_REMOTE_IPV6_ADDR_W0  (GMAC_ENGINE_CSR_OFFSET + 0x180)
#define GMAC_REG_CT_REMOTE_IPV6_ADDR_W1  (GMAC_ENGINE_CSR_OFFSET + 0x184)
#define GMAC_REG_CT_REMOTE_IPV6_ADDR_W2  (GMAC_ENGINE_CSR_OFFSET + 0x188)
#define GMAC_REG_CT_REMOTE_IPV6_ADDR_W3  (GMAC_ENGINE_CSR_OFFSET + 0x18C)

/* TCP/UDP Connection configuration registers */
#define GMAC_REG_CT_TCP_MSS             (GMAC_ENGINE_CSR_OFFSET + 0x118)
#define GMAC_REG_CT_RTT_STATUS          (GMAC_ENGINE_CSR_OFFSET + 0x11C)

/* TCP/UDP Connection status registers */

/* GMAC System interrupt registers */
#define GMAC_REG_INTR_STAT              (GMAC_SYSTEM_CSR_OFFSET + 0x000)
#define GMAC_REG_INTR_EN                (GMAC_SYSTEM_CSR_OFFSET + 0x004)
#define GMAC_REG_RESET_STAT             (GMAC_SYSTEM_CSR_OFFSET + 0x008)
#define GMAC_REG_ID                     (GMAC_SYSTEM_CSR_OFFSET + 0x00C)
#define GMAC_REG_AHB_DEBUG_OUT          (GMAC_SYSTEM_CSR_OFFSET + 0x100)
#define GMAC_REG_AHB_DEBUG_IN           (GMAC_SYSTEM_CSR_OFFSET + 0x120)

/* GMAC DMA TTX registers */
#define GMAC_REG_DMA_TTX_CTRL0 \
	(GMAC_DMA_TTX_DB_CSR_OFFSET + 0x000) /* RW */
#define GMAC_REG_DMA_TTX_ADD_SIZE \
	(GMAC_DMA_TTX_DB_CSR_OFFSET + 0x004) /* RW */
#define GMAC_REG_DMA_TTX_ADD \
	(GMAC_DMA_TTX_DB_CSR_OFFSET + 0x008) /* RW */
#define GMAC_REG_DMA_TTX_ACKD_CID \
	(GMAC_DMA_TTX_DB_CSR_OFFSET + 0x00C) /* RO */
#define GMAC_REG_DMA_TTX_ERR_STATUS \
	(GMAC_DMA_TTX_DB_CSR_OFFSET + 0x010) /* RO */
#define GMAC_REG_DMA_TTX_DFETCH_ERR_CID \
	(GMAC_DMA_TTX_DB_CSR_OFFSET + 0x014) /* RO */
#define GMAC_REG_DMA_TTX_RETRY_ERR_CID \
	(GMAC_DMA_TTX_DB_CSR_OFFSET + 0x018) /* RO */
#define GMAC_REG_DMA_TTX_ACKD_ERR_CID \
	(GMAC_DMA_TTX_DB_CSR_OFFSET + 0x01C) /* RO */
#define GMAC_REG_DMA_TRX_CTRL \
	(GMAC_DMA_TTX_DB_CSR_OFFSET + 0x020) /* RW */
#define GMAC_REG_DMA_TRX_ADD_SIZE \
	(GMAC_DMA_TTX_DB_CSR_OFFSET + 0x024) /* RW */
#define GMAC_REG_DMA_TTX_ALT_REQ \
	(GMAC_DMA_TTX_DB_CSR_OFFSET + 0x028) /* RW */


/* WRADDR register fields */
#define GMAC_OFFSET_CT_CPU_WRADDR_LOC       0
#define GMAC_MASK_CT_CPU_WRADDR_LOC         0x0000001F
#define GMAC_OFFSET_CT_CPU_WRADDR_CSEL      5
#define GMAC_MASK_CT_CPU_WRADDR_CSEL        0x000007FF

/* RDADDR register fields */
#define GMAC_OFFSET_CT_CPU_RDADDR_LOC       0
#define GMAC_MASK_CT_CPU_RDADDR_LOC         0x0000001F
#define GMAC_OFFSET_CT_CPU_RDADDR_CSEL      5
#define GMAC_MASK_CT_CPU_RDADDR_CSEL        0x000007FF

/* ACSTAT register fields */
#define GMAC_OFFSET_CT_CPU_ACSTAT_RDDONE    0
#define GMAC_MASK_CT_CPU_ACSTAT_RDDONE      0x00000001
#define GMAC_OFFSET_CT_CPU_ACSTAT_WRDONE    1
#define GMAC_MASK_CT_CPU_ACSTAT_WRDONE      0x00000001

/* LOCAL_MACADDR_U16 register fields */
#define GMAC_MASK_CT_LOCAL_MACADDR_U16      0x0000FFFF

/* TCP_MSS register fields */
#define GMAC_MASK_CT_TCP_MSS                0x000007FF

/* TCP_PORTS register fields */
#define GMAC_OFFSET_CT_TCP_PORTS_LOCAL      16
#define GMAC_MASK_CT_TCP_PORTS_LOCAL        0x0000FFFF
#define GMAC_OFFSET_CT_TCP_PORTS_REMOTE     0
#define GMAC_MASK_CT_TCP_PORTS_REMOTE       0x0000FFFF

/* CONN_CTRL register fields */
#define GMAC_OFFSET_CT_CONN_CTRL_ERR        16
#define GMAC_MASK_CT_CONN_CTRL_ERR          0x00000001
#define GMAC_OFFSET_CT_CONN_CTRL_IPV6       15
#define GMAC_MASK_CT_CONN_CTRL_IPV6         0x00000001
#define GMAC_OFFSET_CT_CONN_CTRL_CSEL       2
#define GMAC_MASK_CT_CONN_CTRL_CSEL         0x000007FF
#define GMAC_OFFSET_CT_CONN_CTRL_ADD        1
#define GMAC_MASK_CT_CONN_CTRL_ADD          0x00000001
#define GMAC_OFFSET_CT_CONN_CTRL_UPDATE     0
#define GMAC_MASK_CT_CONN_CTRL_UPDATE       0x00000001

/* CONN_STAT register fields */
#define GMAC_OFFSET_CT_CONN_STAT_RXDROPPED  31
#define GMAC_MASK_CT_CONN_STAT_RXDROPPED    0x00000001
#define GMAC_OFFSET_CT_CONN_STAT_INTR_VALID 11
#define GMAC_MASK_CT_CONN_INTR_VALID        0x00000001
#define GMAC_OFFSET_CT_CONN_STAT_CSEL       0
#define GMAC_MASK_CT_CONN_STAT_CSEL         0x000007FF


/* GMAC_INTR_STAT register fields */
#define GMAC_OFFSET_INTR_STAT_DMA_TTX_ACK   0
#define GMAC_MASK_INTR_STAT_DMA_TTX_ACK     0x00000001
#define GMAC_OFFSET_INTR_STAT_DMA_TTX_ERR   1
#define GMAC_MASK_INTR_STAT_DMA_TTX_ERR     0x00000001
#define GMAC_OFFSET_INTR_STAT_DMA_CH0       2
#define GMAC_MASK_INTR_STAT_DMA_CH0         0x00000001
#define GMAC_OFFSET_INTR_STAT_DMA_CH1       3
#define GMAC_MASK_INTR_STAT_DMA_CH1         0x00000001
#define GMAC_OFFSET_INTR_STAT_DMA_CH2       4
#define GMAC_MASK_INTR_STAT_DMA_CH2         0x00000001
#define GMAC_OFFSET_INTR_STAT_GMAC0         5
#define GMAC_MASK_INTR_STAT_GMAC0           0x00000001
#define GMAC_OFFSET_INTR_STAT_GMAC1         6
#define GMAC_MASK_INTR_STAT_GMAC1           0x00000001
#define GMAC_OFFSET_INTR_STAT           7
#define GMAC_MASK_INTR_STAT             0x00000001

/* GMAC_INTR_EN register fields */
#define GMAC_OFFSET_INTR_EN_DMA_TTX_ACK   0
#define GMAC_MASK_INTR_EN_DMA_TTX_ACK     0x00000001
#define GMAC_OFFSET_INTR_EN_DMA_TTX_ERR   1
#define GMAC_MASK_INTR_EN_DMA_TTX_ERR     0x00000001
#define GMAC_OFFSET_INTR_EN_DMA_CH0       2
#define GMAC_MASK_INTR_EN_DMA_CH0         0x00000001
#define GMAC_OFFSET_INTR_EN_DMA_CH1       3
#define GMAC_MASK_INTR_EN_DMA_CH1         0x00000001
#define GMAC_OFFSET_INTR_EN_DMA_CH2       4
#define GMAC_MASK_INTR_EN_DMA_CH2         0x00000001
#define GMAC_OFFSET_INTR_EN_GMAC0         5
#define GMAC_MASK_INTR_EN_GMAC0           0x00000001
#define GMAC_OFFSET_INTR_EN_GMAC1         6
#define GMAC_MASK_INTR_EN_GMAC1           0x00000001
#define GMAC_OFFSET_INTR_EN           7
#define GMAC_MASK_INTR_EN             0x00000001

/* GMAC_CTRL1 register fields */
#define GMAC_OFFSET_CTRL1_PERSISTCOUNT    0
#define GMAC_MASK_CTRL1_PERSISTCOUNT      0x000000FF

/* GMAC_CONFIG0 register fields */
#define GMAC_OFFSET_CONFIG0_PROMISCMODE    0
#define GMAC_MASK_CONFIG0_PROMISCMODE      0x00000001
#define GMAC_OFFSET_CONFIG0_RXBYPASSMODE   1
#define GMAC_MASK_CONFIG0_RXBYPASSMODE     0x00000001
#define GMAC_OFFSET_CONFIG0_IPV6_EN        2
#define GMAC_MASK_CONFIG0_IPV6_EN          0x00000001
#define GMAC_OFFSET_CONFIG0_VLAN_EN        3
#define GMAC_MASK_CONFIG0_VLAN_EN          0x00000001
#define GMAC_OFFSET_CONFIG0_CONG_WIN_EN    4
#define GMAC_MASK_CONFIG0_CONG_WIN_EN      0x00000001
#define GMAC_OFFSET_CONFIG0_FORCE_RD_CT    5
#define GMAC_MASK_CONFIG0_FORCE_RD_CT      0x00000001
#define GMAC_OFFSET_CONFIG0_MAC_SPACE_CHK  6
#define GMAC_MASK_CONFIG0_MAC_SPACE_CHK    0x00000001

/* GMAC_ID register fields */
#define GMAC_OFFSET_ID_MAX_CONNECTIONS    0
#define GMAC_MASK_ID_MAX_CONNECTIONS      0x0000FFFF
#define GMAC_OFFSET_ID_HWID               16
#define GMAC_MASK_ID_HWID                 0x0000FFFF

/* DMA_TTX_ADD_SIZE register fields */
#define GMAC_OFFSET_DMA_TTX_ADD_SIZE      0
#define GMAC_MASK_DMA_TTX_ADD_SIZE        0x00FFFFFF

/* DMA_TTX_ADD register fields */
#define GMAC_OFFSET_DMA_TTX_ADD_CID       0
#define GMAC_MASK_DMA_TTX_ADD_CID         0x000007FF
#define GMAC_OFFSET_DMA_TTX_ADD_FLUSH     16
#define GMAC_MASK_DMA_TTX_ADD_FLUSH       0x00000001

/* DMA_TTX_ALT_REQ register fields */
#define GMAC_OFFSET_DMA_TTX_ALT_REQ_CID   0
#define GMAC_MASK_DMA_TTX_ALT_REQ_CID     0x000007FF
#define GMAC_OFFSET_DMA_TTX_ALT_REQ_SIZE  11
#define GMAC_MASK_DMA_TTX_ALT_REQ_SIZE    0x001FFFFF

/* DMA_TTX_ACKD_CID register fields */
#define GMAC_OFFSET_DMA_TTX_ACKD_OVFLOW   0
#define GMAC_MASK_DMA_TTX_ACKD_OVFLOW     0x00000001
#define GMAC_OFFSET_DMA_TTX_ACKD_VALID    1
#define GMAC_MASK_DMA_TTX_ACKD_VALID      0x00000001
#define GMAC_OFFSET_DMA_TTX_ACKD_CID      4
#define GMAC_MASK_DMA_TTX_ACKD_CID        0x000007FF

/* DMA_TTX_ERR_STATUS register fields */
#define GMAC_OFFSET_TTX_ERR_DFETCH0       0
#define GMAC_MASK_TTX_ERR_DFETCH0         0x00000001
#define GMAC_OFFSET_TTX_ERR_DFETCH1       1
#define GMAC_MASK_TTX_ERR_DFETCH1         0x00000001
#define GMAC_OFFSET_TTX_ERR_DFETCH2       2
#define GMAC_MASK_TTX_ERR_DFETCH2         0x00000001
#define GMAC_OFFSET_TTX_ERR_RETRY0        3
#define GMAC_MASK_TTX_ERR_RETRY0          0x00000001
#define GMAC_OFFSET_TTX_ERR_RETRY1        4
#define GMAC_MASK_TTX_ERR_RETRY1          0x00000001
#define GMAC_OFFSET_TTX_ERR_ACKD0         5
#define GMAC_MASK_TTX_ERR_ACKD0           0x00000001
#define GMAC_OFFSET_TTX_ERR_ACKD1         6
#define GMAC_MASK_TTX_ERR_ACKD1           0x00000001
#define GMAC_OFFSET_TTX_ERR_ACKD2         7
#define GMAC_MASK_TTX_ERR_ACKD2           0x00000001

/* DMA_TTX_DFETCH_ERR_CID register fields */
#define GMAC_OFFSET_TTX_DFETCH_ERR_CID    0
#define GMAC_MASK_TTX_DFETCH_ERR_CID      0x000007FF

/* DMA_TTX_RETRY_ERR_CID register fields */
#define GMAC_OFFSET_TTX_RETRY_ERR_CID     0
#define GMAC_MASK_TTX_RETRY_ERR_CID       0x000007FF

/* DMA_TTX_ACKD_ERR_CID register fields */
#define GMAC_OFFSET_TTX_ACKD_ERR_CID      0
#define GMAC_MASK_TTX_ACKD_ERR_CID        0x000007FF

/* DMA_TTX_CTRL0 register fields */
#define GMAC_OFFSET_TTX_CTRL0_MAXBURSTLEN 0
#define GMAC_MASK_TTX_CTRL0_MAXBURSTLEN   0x000000FF

/* DMA_TRX_CTRL register fields */
#define GMAC_OFFSET_TRX_CTRL_RXBUFFERSIZE 0
#define GMAC_MASK_TRX_CTRL_RXBUFFERSIZE   0x00000007
#define GMAC_OFFSET_TRX_CTRL_STAT_EN  3
#define GMAC_MASK_TRX_CTRL_STAT_EN    0x00000001
#define GMAC_OFFSET_TRX_CTRL_IPC_STAT_EN  4
#define GMAC_MASK_TRX_CTRL_IPC_STAT_EN    0x00000001

/* DMA_TRX_ADD_SIZE register fields */
#define GMAC_OFFSET_TRX_ADD_COUNT         0
#define GMAC_MASK_TRX_ADD_COUNT           0x00FFFFFF

/* GMAC_INTR_STAT register fields */
#define GMAC_OFFSET_INTR_STAT_FLUSH      0
#define GMAC_MASK_INTR_STAT_FLUSH        0x00000001
#define GMAC_OFFSET_INTR_STAT_TX_ERR     1
#define GMAC_MASK_INTR_STAT_TX_ERR       0x00000001
#define GMAC_OFFSET_INTR_STAT_TX_ERR_CID 2
#define GMAC_MASK_INTR_STAT_TX_ERR_CID   0x000007FF
#define GMAC_OFFSET_INTR_STAT_RETRY_MAX  13
#define GMAC_MASK_INTR_STAT_RETRY_MAX    0x00000001
#define GMAC_OFFSET_INTR_STAT_RETRY_CID  14
#define GMAC_MASK_INTR_STAT_RETRY_CID    0x000007FF

/* GMAC_INTR_EN register fields */
#define GMAC_OFFSET_INTR_EN_FLUSH      0
#define GMAC_MASK_INTR_EN_FLUSH        0x00000001
#define GMAC_OFFSET_INTR_EN_TX_ERR     1
#define GMAC_MASK_INTR_EN_TX_ERR       0x00000001
#define GMAC_OFFSET_INTR_EN_RETRY_MAX  2
#define GMAC_MASK_INTR_EN_RETRY_MAX    0x00000001


#define GMAC_TTX_DMA_DB_ENTRY_SIZE (4 * sizeof(uint32_t))
#define GMAC_TTX_DMA_DB_TXSTAT_SIZE (2 * sizeof(uint32_t))
#define GMAC_TTX_DMA_DB_RXSTAT_SIZE (1 * sizeof(uint32_t))

#endif /*  __GMACHW_REGMAP_H__ */
