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
#define MCI_BMOD		0x80
#define MCI_DBADDR		0x88
#define MCI_IDSTS		0x8C
#define MCI_IDINTEN		0x90
#define MCI_DSCADDR		0x94
#define MCI_BUFADDR		0x98
#define MCI_READ_THRESHOLD_SIZE		0x100
#define MCI_UHS_EXT		0x108
/* MCI_UHS_REG(0x74) details */
#define HI_SDXC_CTRL_VDD_180	(1<<0)
#define HI_SDXC_CTRL_DDR_REG	(1<<16)

/* MCI_BMOD(0x80) details */
#define BMOD_SWR		(1<<0)
#define BURST_INCR		(1<<1)
#define BURST_8			(0x2<<8)

/* MCI_CTRL(0x00) details */
#define CTRL_RESET             (1<<0)
#define FIFO_RESET             (1<<1)
#define DMA_RESET              (1<<2)
#define INTR_EN                (1<<4)
#define USE_INTERNAL_DMA       (1<<25)

/* IDMAC DEST1 details */
#define DMA_BUFFER		0x2000
#define MAX_DMA_DES		(20480)

/* MCI_CDETECT(0x50) details */
#define HIMCI_CARD0		(1<<0)

/* MCI_TIMEOUT(0x14) details: */
/*bit 31-8: data read timeout param*/
#define DATA_TIMEOUT		(0xffffff<<8)

/* bit 7-0: response timeout param */
#define RESPONSE_TIMEOUT	0xff

/* bit 0: enable of card clk*/
#define CCLK_ENABLE		(1<<0)

/* IDMAC DEST0 details */
#define DMA_DES_OWN		(1<<31)
#define DMA_DES_NEXT_DES	(1<<4)
#define DMA_DES_FIRST_DES	(1<<3)
#define DMA_DES_LAST_DES	(1<<2)

/* MCI_BMOD(0x80) details */
#define BMOD_DMA_EN		(1<<7)

/* MCI_CTYPE(0x18) details */
#define CARD_WIDTH		(0x1<<0)

/* MCI_INTMASK(0x24) details:
   bit 16-1: mask MMC host controller each interrupt
*/
#define ALL_INT_MASK				0x1ffff
#define DTO_INT_MASK				(1<<3)
#define SDIO_INT_MASK				(1<<16)

/* bit[18:16] sampling phase */
#define CLK_SMPL_PHS_MASK			(7<<16)

/* MCI_CMD(0x2c) details:
   bit 31: cmd execute or load start param of interface clk bit
*/
#define START_CMD		(1<<31)


/* MCI_INTSTS(0x44) details */
/***************************************************************/
/* bit 16: sdio interrupt status */
#define SDIO_INT_STATUS		(0x1<<16)

/* bit 15: end-bit error (read)/write no CRC interrupt status */
#define EBE_INT_STATUS		(0x1<<15)

/* bit 14: auto command done interrupt status */
#define ACD_INT_STATUS		(0x1<<14)

/* bit 13: start bit error interrupt status */
#define SBE_INT_STATUS		(0x1<<13)

/* bit 12: hardware locked write error interrupt status */
#define HLE_INT_STATUS		(0x1<<12)

/* bit 11: FIFO underrun/overrun error interrupt status */
#define FRUN_INT_STATUS		(0x1<<11)

/* bit 10: data starvation-by-host timeout interrupt status */
#define HTO_INT_STATUS		(0x1<<10)

/* bit 10: volt_switch to 1.8v for sdxc */
#define VOLT_SWITCH_INT_STATUS		(0x1<<10)

/* bit 9: data read timeout interrupt status */
#define DRTO_INT_STATUS		(0x1<<9)

/* bit 8: response timeout interrupt status */
#define RTO_INT_STATUS		(0x1<<8)

/* bit 7: data CRC error interrupt status */
#define DCRC_INT_STATUS		(0x1<<7)

/* bit 6: response CRC error interrupt status */
#define RCRC_INT_STATUS		(0x1<<6)

/* bit 5: receive FIFO data request interrupt status */
#define RXDR_INT_STATUS		(0x1<<5)

/* bit 4: transmit FIFO data request interrupt status */
#define TXDR_INT_STATUS		(0x1<<4)

/* bit 3: data transfer Over interrupt status */
#define DTO_INT_STATUS		(0x1<<3)

/* bit 2: command done interrupt status */
#define CD_INT_STATUS		(0x1<<2)

/* bit 1: response error interrupt status */
#define RE_INT_STATUS		(0x1<<1)

#define DATA_INT_MASK	(DTO_INT_STATUS | DCRC_INT_STATUS \
		| SBE_INT_STATUS | EBE_INT_STATUS)
/***************************************************************/

/* MCI_RINTSTS(0x44) details:bit 16-1: clear
   MMC host controller each interrupt but
   hardware locked write error interrupt
*/
#define ALL_INT_CLR       0x1efff
#define ALL_SD_INT_CLR    0xefff

#define PHASE_SHIFT		0x1030000
#define READ_THRESHOLD_SIZE	0x2000001

/* MCI_STATUS(0x48) details */
#define DATA_BUSY		(0x1<<9)

/* MCI_FIFOTH(0x4c) details */
/*
#define BURST_SIZE		(0x2<<28)
#define RX_WMARK		(0x7<<16)
#define TX_WMARK		0x8
*/
#define BURST_SIZE		(0x6<<28)
#define RX_WMARK		(0x7f<<16)
#define TX_WMARK		0x80

#endif
