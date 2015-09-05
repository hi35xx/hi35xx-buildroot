
/*
 make ARCH=arm CROSS_COMPILE=arm-hisiv200-linux- godbox_config

 cd arch/arm/cpu/godbox/

 arm-eabi-gcc -D__ASSEMBLY__ -Os -fno-strict-aliasing -fno-common -ffixed-r8
  -D__KERNEL__ -DTEXT_BASE=0x80500000 -I../../../../include -fno-builtin
  -ffreestanding -nostdinc -isystem /home/jzl/android/toolchain/arm-eabi-4.4.0/
 lib/gcc/arm-eabi/4.4.0/include -pipe -DCONFIG_ARM -D__ARM__ -marm
  -mabi=aapcs-linux -mno-thumb-interwork -march=armv7-a
  -S himciv100_boot.c -o himciv100_boot.S
*/

#include <config.h>
#include <asm/arch/platform.h>
#include <himciv100_reg.h>

#define MCI_SRC_CLK		(50*1000*1000)	/* 50MHz */
#define MCI_ID_CLK		(400*1000)	/* 400KHz */
#define MCI_BOOT_CLK		(20*1000*1000)	/* 20MHz */

#define DELAY_US		(1000)

/* mmc.h */
#define MMC_CMD_GO_IDLE_STATE		0
#define MMC_CMD_SEND_OP_COND		1
#define MMC_CMD_ALL_SEND_CID		2
#define MMC_CMD_SET_RELATIVE_ADDR	3
#define MMC_CMD_SET_DSR			4
#define MMC_CMD_SWITCH			6
#define MMC_CMD_SELECT_CARD		7
#define MMC_CMD_SEND_EXT_CSD		8
#define MMC_CMD_SEND_CSD		9
#define MMC_CMD_SEND_CID		10
#define MMC_CMD_STOP_TRANSMISSION	12
#define MMC_CMD_SEND_STATUS		13
#define MMC_CMD_SET_BLOCKLEN		16
#define MMC_CMD_READ_SINGLE_BLOCK	17
#define MMC_CMD_READ_MULTIPLE_BLOCK	18
#define MMC_CMD_WRITE_SINGLE_BLOCK	24
#define MMC_CMD_WRITE_MULTIPLE_BLOCK	25
#define MMC_CMD_APP_CMD			55

#define OCR_BUSY		0x80000000
#define OCR_HCS			0x40000000

#define MMC_VDD_32_33		0x00100000	/* VDD voltage 3.2 ~ 3.3 */
#define MMC_VDD_33_34		0x00200000	/* VDD voltage 3.3 ~ 3.4 */

#define MMC_SWITCH_MODE_WRITE_BYTE	0x03 /* Set target byte to value */

#define EXT_CSD_BUS_WIDTH	183	/* R/W */
#define EXT_CSD_HS_TIMING	185	/* R/W */

#define EXT_CSD_BUS_WIDTH_1	0	/* Card is in 1 bit mode */
#define EXT_CSD_BUS_WIDTH_4	1	/* Card is in 4 bit mode */
#define EXT_CSD_BUS_WIDTH_8	2	/* Card is in 8 bit mode */

static inline unsigned int himci_readl(unsigned addr)
{
	return *((volatile unsigned *) (addr));
}

static inline void himci_writel(unsigned val, unsigned addr)
{
	(*(volatile unsigned *) (addr)) = (val);
}

static inline void delay(unsigned int cnt)
{
	while (cnt--)
		__asm__ __volatile__("nop");
}

static inline void emmc_io_init(void)
{
}

#ifdef CONFIG_HIMCI_GODBOX
#include "himciv100_boot_godbox.c"
#endif

static void emmc_update_clk(void)
{
	unsigned int tmp_reg;
	unsigned int mmc_base = REG_BASE_MCI;

	tmp_reg = (START_CMD | UP_CLK_ONLY);
	himci_writel(tmp_reg, mmc_base + MCI_CMD);

	do {
		tmp_reg = himci_readl(mmc_base + MCI_CMD);
	} while (tmp_reg & START_CMD);
}

static void emmc_send_cmd(unsigned cmd, unsigned arg, unsigned wait_busy)
{
	unsigned int tmp_reg;
	unsigned int mmc_base = REG_BASE_MCI;

	himci_writel(ALL_INT_CLR, mmc_base + MCI_RINTSTS);
	himci_writel(arg, mmc_base + MCI_CMDARG);
	himci_writel(cmd | WT_PD_CPT | START_CMD, mmc_base + MCI_CMD);

	do {
		tmp_reg = himci_readl(mmc_base + MCI_CMD);
	} while (tmp_reg & START_CMD);

	do {
		tmp_reg = himci_readl(mmc_base + MCI_RINTSTS);
	} while (!(tmp_reg & CD_INT_STATUS));

	if (wait_busy) {
		do {
			tmp_reg = himci_readl(mmc_base + MCI_STATUS);
		} while ((tmp_reg & DATA_BUSY));
	}
}

void emmc_init(void)
{
	unsigned int tmp_reg;
	unsigned int mmc_base = REG_BASE_MCI;

	emmc_io_init();

	emmc_sys_init();

	/* reset all */
	tmp_reg = himci_readl(mmc_base + MCI_CTRL);
	tmp_reg |= CTRL_RESET | FIFO_RESET | DMA_RESET;
	himci_writel(tmp_reg, mmc_base + MCI_CTRL);

	/* card power off and power on */
	himci_writel(0, mmc_base + MCI_PWREN);
	delay(500 * DELAY_US);
	himci_writel(1, mmc_base + MCI_PWREN);
	delay(200 * DELAY_US);

	/* clear MMC host intr */
	himci_writel(ALL_INT_CLR, mmc_base + MCI_RINTSTS);

	/* MASK MMC host intr */
	tmp_reg = himci_readl(mmc_base + MCI_INTMASK);
	tmp_reg &= ~ALL_INT_MASK;
	himci_writel(tmp_reg, mmc_base + MCI_INTMASK);

	/* disable inner DMA mode and close intr of MMC host controler */
	tmp_reg = himci_readl(mmc_base + MCI_CTRL);
	tmp_reg &= ~(INTR_EN | USE_INTERNAL_DMA);
	himci_writel(tmp_reg, mmc_base + MCI_CTRL);

	/* set timeout param */
	himci_writel(DATA_TIMEOUT | RESPONSE_TIMEOUT, mmc_base + MCI_TIMEOUT);

	/* set FIFO param */
	himci_writel(BURST_SIZE | RX_WMARK | TX_WMARK, mmc_base + MCI_FIFOTH);

	/* set data width */
	himci_writel(CARD_WIDTH_1BIT, mmc_base + MCI_CTYPE);

	/* clk update */
	himci_writel(0, mmc_base + MCI_CLKENA);
	emmc_update_clk();

	tmp_reg = MCI_SRC_CLK / (MCI_ID_CLK * 2);
	if (MCI_SRC_CLK % (MCI_ID_CLK * 2))
		tmp_reg++;
	if (tmp_reg > 0xFF)
		tmp_reg = 0xFF;
	himci_writel(tmp_reg, mmc_base + MCI_CLKDIV);
	emmc_update_clk();

	himci_writel(1, mmc_base + MCI_CLKENA);
	emmc_update_clk();

	/* Send CMD0 */
	delay(1000 * DELAY_US);
	emmc_send_cmd(MMC_CMD_GO_IDLE_STATE | SEND_INIT, 0, 0);
	delay(2000 * DELAY_US);

	/* Send CMD1 */
	do {
		emmc_send_cmd(MMC_CMD_SEND_OP_COND | RESP_EXPECT,
			 OCR_HCS | MMC_VDD_32_33 | MMC_VDD_33_34, 0);
		tmp_reg = himci_readl(mmc_base + MCI_RESP0);
		delay(1000 * DELAY_US);
	} while (!(tmp_reg & OCR_BUSY));

	/* Send CMD2 */
	emmc_send_cmd(MMC_CMD_ALL_SEND_CID | RESP_EXPECT | RESP_LENGTH
		 | CHECK_RESP_CRC, 0, 0);

	/* Send CMD3 */
	emmc_send_cmd(MMC_CMD_SET_RELATIVE_ADDR | RESP_EXPECT
		 | CHECK_RESP_CRC, 0, 0);

	/* Send CMD7 */
	emmc_send_cmd(MMC_CMD_SELECT_CARD | RESP_EXPECT | CHECK_RESP_CRC,
		 0, 1);

	/* Send CMD6 */
	emmc_send_cmd(MMC_CMD_SWITCH | RESP_EXPECT | CHECK_RESP_CRC,
		 (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
		 (EXT_CSD_BUS_WIDTH << 16) | (EXT_CSD_BUS_WIDTH_8 << 8),
		 1);

	/* set data width */
	himci_writel(CARD_WIDTH_8BIT, mmc_base + MCI_CTYPE);

	tmp_reg = MCI_SRC_CLK / (MCI_BOOT_CLK * 2);
	if (MCI_SRC_CLK % (MCI_BOOT_CLK * 2))
		tmp_reg++;
	if (tmp_reg > 0xFF)
		tmp_reg = 0xFF;
	himci_writel(tmp_reg, mmc_base + MCI_CLKDIV);
	emmc_update_clk();

	himci_writel(1, mmc_base + MCI_CLKENA);
	emmc_update_clk();
}

void emmc_deinit(void)
{
	unsigned int mmc_base = REG_BASE_MCI;

	/* clear MMC host intr */
	himci_writel(ALL_INT_CLR, mmc_base + MCI_RINTSTS);

	/* disable clk */
	himci_writel(0, mmc_base + MCI_CLKENA);
	emmc_update_clk();

	/* power off */
	himci_writel(0, mmc_base + MCI_PWREN);
	delay(500*DELAY_US);
}

void emmc_read(void *ptr, unsigned int size)
{
	unsigned int count, val, tmp_reg = 0;
	unsigned int mmc_base = REG_BASE_MCI;
	unsigned int *buf, data;

	himci_writel(512, mmc_base + MCI_BLKSIZ);
	himci_writel((size + 511) & (~511), mmc_base + MCI_BYTCNT);

	/* Send CMD7 */
	emmc_send_cmd(MMC_CMD_SET_BLOCKLEN | RESP_EXPECT | CHECK_RESP_CRC,
		 512, 0);

	/* Send CMD18 */
	emmc_send_cmd(MMC_CMD_READ_MULTIPLE_BLOCK | RESP_EXPECT
		 | CHECK_RESP_CRC | DATA_EXPECT, CONFIG_MMC_BOOT_ADDR/512, 0);

	buf = ptr;
	size >>= 2;
	while (size > 0) {
		tmp_reg = himci_readl(mmc_base + MCI_STATUS);
		count = (tmp_reg >> 17) & 0x1fff;

		if (count > size)
			count = size;

		/*start to read data*/
		while (1) {
			tmp_reg = himci_readl(mmc_base + MCI_RINTSTS);
			if (tmp_reg & RXDR_INT_STATUS)
				break;

			val = (DRTO_INT_STATUS | HTO_INT_STATUS
				 | HLE_INT_STATUS | EBE_INT_STATUS);
			if (tmp_reg & val)
				return;
		}
		himci_writel(ALL_INT_CLR, mmc_base + MCI_RINTSTS);

		for (; count != 0; --count) {
			data = himci_readl(mmc_base + MCI_FIFO_START);

			*buf = data;
			buf++;
			--size;
		}
	}

	/* Send CMD12 */
	emmc_send_cmd(MMC_CMD_STOP_TRANSMISSION | RESP_EXPECT | CHECK_RESP_CRC,
		 0, 1);
}
