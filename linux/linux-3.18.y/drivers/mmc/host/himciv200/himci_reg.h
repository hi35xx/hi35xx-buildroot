#ifndef _HI_MCI_REG_H_
#define _HI_MCI_REG_H_

#define HI_MCI_IO_SIZE		0x1000

#define MCI_CTRL		0x00
#define MCI_PWREN		0x04
#define MCI_CLKDIV		0x08
#define MCI_CLKSRC		0x0C
#define MCI_CLKENA		0x10
#define MCI_TIMEOUT		0x14
#define MCI_CTYPE		0x18
#define MCI_BLKSIZ		0x1c
#define MCI_BYTCNT		0x20
#define MCI_INTMASK		0x24
#define MCI_CMDARG		0x28
#define MCI_CMD			0x2C
#define MCI_RESP0		0x30
#define MCI_RESP1		0x34
#define MCI_RESP2		0x38
#define MCI_RESP3		0x3C
#define MCI_MINTSTS		0x40
#define MCI_RINTSTS		0x44
#define MCI_STATUS		0x48
#define MCI_FIFOTH		0x4C
#define MCI_CDETECT		0x50
#define MCI_WRTPRT		0x54
#define MCI_GPIO		0x58
#define MCI_TCBCNT		0x5C
#define MCI_TBBCNT		0x60
#define MCI_DEBNCE		0x64
#define MCI_USRID		0x68
#define MCI_VERID		0x6C
#define MCI_HCON		0x70
#define MCI_UHS_REG		0x74
#define MCI_RESET_N		0x78
#define MCI_BMOD		0x80
#define MCI_DBADDR		0x88
#define MCI_IDSTS		0x8C
#define MCI_IDINTEN		0x90
#define MCI_DSCADDR		0x94
#define MCI_BUFADDR		0x98
#define ADMA_CTRL		0xb0
#define ADMA_Q_ADDR		0xb4
#define ADMA_Q_DEEPTH	0xb8
#define ADMA_Q_RDPTR	0xbc
#define ADMA_Q_WRPTR	0xc0
#define ADMA_Q_TO		0xc4
#define MCI_CARDTHRCTL		0x100
#define MCI_UHS_REG_EXT     0x108
#define MCI_EMMC_DDR_REG    0x10c
#define MCI_TUNING_CTRL		0x118

/* MCI_IDSTS(0x8c) detals */
#define CMD_LOCK_ERR	(0x1<<29)
#define OWNBIT_ERR		(0x1<<28)
#define QUEUE_OVERFLOW	(0x1<<27)
#define RESP_CHECK_ERR	(0x1<<26)
#define PACKET_INT		(0x1<<25)
#define PACKET_TO_INT	(0x1<<24)
#define AUTO_STOP_ERR	(0x1<<23)
#define QUEUE_FULL		(0x1<<22)
#define QUEUE_EMPTY		(0x1<<21)
#define ADMA3_FSM_SHIFT	(17)
#define FSM_SHIFT		(13)
#define CES			(0x1<<5)
#define DU			(0x1<<4)
#define FBE			(0x1<<2)

#define ADMA_INT_ERR		(CMD_LOCK_ERR | OWNBIT_ERR | QUEUE_OVERFLOW \
				| AUTO_STOP_ERR | PACKET_TO_INT | DU | FBE)

#define ADMA_INT_ALL		(CMD_LOCK_ERR | OWNBIT_ERR | QUEUE_OVERFLOW \
					| RESP_CHECK_ERR | PACKET_INT \
					| PACKET_TO_INT | AUTO_STOP_ERR \
					| QUEUE_FULL | CES | DU | FBE)
#define RESP_CHK_EN		(0x1<<4)
#define RDPTR_MOD_EN	(0x1<<3)
#define PACKET_INT_EN	(0x1<<2)
#define ADMA3_RESTART	(0x1<<1)
#define ADMA3_EN		(0x1<<0)

/* GPIO config */
#define DTO_FIX_BYPASS		(0x1<<23)
#define CMD_OUT_EN_FIX_BYPASS	(0x1<<8)

/* MCI_UHS_REG(0x74) details */
#define HI_EMMC_CTRL_VDD_180	(0x2<<0)
#define HI_EMMC_CTRL_DDR_REG	(0x2<<16)
#define HI_SDXC_CTRL_VDD_180	(0x1<<0)
#define HI_SDXC_CTRL_DDR_REG	(0x1<<16)

/* MCI_BMOD(0x80) details */
#define BMOD_SWR		(0x1<<0)
#define BURST_INCR		(0x1<<1)
#define BMOD_DMA_EN		(0x1<<7)
#define BURST_8			(0x2<<8)
#define BURST_16		(0x3<<8)

/* MCI_CTRL(0x00) details */
#define CTRL_RESET		(1<<0)
#define FIFO_RESET		(1<<1)
#define DMA_RESET		(1<<2)
#define INTR_EN			(1<<4)
#define USE_INTERNAL_DMA	(1<<25)

/* IDMAC DEST1 details */
#define DMA_BUFFER		(0x2000)
#define MAX_DMA_DES		(20480)

/* IDMAC DEST0 details */
#define DMA_DES_OWN		(1<<31)
#define DMA_DES_NEXT_DES	(1<<4)
#define DMA_DES_FIRST_DES	(1<<3)
#define DMA_DES_LAST_DES	(1<<2)

/* MCI_CDETECT(0x50) details */
#define HIMCI_CARD0		(0x1<<0)
#define HIMCI_CARD_MASK		(0x3<<0)
#define HIMCI_CARD_EMMC		(0x1<<1)
#define HIMCI_CARD_SD		(0x1<<0)

/* MCI_TIMEOUT(0x14) details: */
/*bit 31-8: data read timeout param */
#define DATA_TIMEOUT		(0xffffff<<8)
/* bit 7-0: response timeout param */
#define RESPONSE_TIMEOUT	0xff

/* MCI_CLKENA(0x10) details */
#define CCLK_ENABLE		(0x1<<0)	/* bit 0: enable of card clk*/

/* MCI_CTYPE(0x18) details */
#define EMMC_CARD_WIDTH_1	(0x2<<0)
#define EMMC_CARD_WIDTH_0	(0x2<<16)
#define CARD_WIDTH_1		(0x1<<0)
#define CARD_WIDTH_0		(0x1<<16)

/* MCI_CARDTHRCTL(0x100) details */
#define RW_THRESHOLD_SIZE	(0x2000005)

/* MCI_SRC(0x0C) details */
#define EMMC_CLK_SOURCE		(0x1<<2)

/* MCI_EMMC_DDR_REG(0x10c) details */
#define HI_EMMC_HS400_MODE	(0x1<<31)

/* MCI_RESET_N(0x78) details */
#define MMC_RST_N		(0x1<<0)	/* control reset*/

/* MCI_INTMASK(0x24) details:
   bit 16-1: mask MMC host controller each interrupt
*/
#define ALL_INT_MASK		0x1ffff
#define DTO_INT_MASK		(0x1<<3)
#define SDIO_INT_MASK		(0x1<<16)

/* MCI_UHS_REG_EXT(0x108) details */
/* bit[19:16] sampling phase */
#define CLK_SMPL_PHS_SHIFT	(16)
#define CLK_SMPL_PHS_MASK	(0xF<<16)
#define CLK_SMPLA_PHS_SHIFT	(9)
#define CLK_SMPLA_PHS_MASK	(0x7<<9)
/* bit[26:23] drv phase */
#define CLK_DRV_PHS_SHIFT	(23)
#define CLK_DRV_PHS_MASK	(0xF<<23)
#define DEFAULT_SMPL_PHASE	(0x5)

/* MCI_CMD(0x2c) details:
   bit 31: cmd execute or load start param of interface clk bit
*/
#define START_CMD         (0x1<<31)

/* MCI_INTSTS(0x44) details */
/***************************************************************/
/* bit 16: sdio interrupt status */
#define SDIO_INT_STATUS    (0x1<<16)

/* bit 15: end-bit error (read)/write no CRC interrupt status */
#define EBE_INT_STATUS    (0x1<<15)

/* bit 14: auto command done interrupt status */
#define ACD_INT_STATUS    (0x1<<14)

/* bit 13: start bit error interrupt status */
#define SBE_INT_STATUS    (0x1<<13)

/* bit 12: hardware locked write error interrupt status */
#define HLE_INT_STATUS    (0x1<<12)

/* bit 11: FIFO underrun/overrun error interrupt status */
#define FRUN_INT_STATUS    (0x1<<11)

/* bit 10: data starvation-by-host timeout interrupt status */
#define HTO_INT_STATUS    (0x1<<10)

/* bit 10: volt_switch to 1.8v for sdxc */
#define VOLT_SWITCH_INT_STATUS    (0x1<<10)

/* bit 9: data read timeout interrupt status */
#define DRTO_INT_STATUS    (0x1<<9)

/* bit 8: response timeout interrupt status */
#define RTO_INT_STATUS    (0x1<<8)

/* bit 7: data CRC error interrupt status */
#define DCRC_INT_STATUS    (0x1<<7)

/* bit 6: response CRC error interrupt status */
#define RCRC_INT_STATUS    (0x1<<6)

/* bit 5: receive FIFO data request interrupt status */
#define RXDR_INT_STATUS    (0x1<<5)

/* bit 4: transmit FIFO data request interrupt status */
#define TXDR_INT_STATUS    (0x1<<4)

/* bit 3: data transfer Over interrupt status */
#define DTO_INT_STATUS    (0x1<<3)

/* bit 2: command done interrupt status */
#define CD_INT_STATUS    (0x1<<2)

/* bit 1: response error interrupt status */
#define RE_INT_STATUS    (0x1<<1)

#define CMD_INT_MASK	(RTO_INT_STATUS | RCRC_INT_STATUS | RE_INT_STATUS)
#define DATA_INT_MASK	(DCRC_INT_STATUS | SBE_INT_STATUS | EBE_INT_STATUS)
/***************************************************************/

/* MCI_RINTSTS(0x44) details:bit 16-1: clear
   MMC host controller each interrupt but
   hardware locked write error interrupt
*/
#define ALL_INT_CLR       0x1efff
#define ALL_ADMA_INT_CLR  (0xffe<<21)

/* MCI_STATUS(0x48) details */
#define DATA_BUSY         (0x1<<9)

/* MCI_FIFOTH(0x4c) details */
#define BURST_SIZE        (0x6<<28)
#define RX_WMARK          (0x7f<<16)
#define TX_WMARK          (0x80)

/* MCI_TUNING_CTRL(0x118) details */
#define HW_TUNING_EN	(0x1 << 0)
#define EDGE_CTRL		(0x1 << 1)
#define FOUND_EDGE		(0x1 << 5)

#define DEFAULT_CMD_VALUE	0x20000000

#endif
