/*
 * himci.c - hisilicon MMC Host driver
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#define pr_fmt(fmt) "himci: " fmt

#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/scatterlist.h>

#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/card.h>
#include <linux/mmc/sd.h>
#include <linux/slab.h>

#include <linux/ioport.h>
#include <linux/device.h>

#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/freezer.h>
#include <asm/dma.h>
#include <asm/irq.h>
#include <asm/sizes.h>
#include <mach/hardware.h>
#include <linux/io.h>

#include "himci_reg.h"
#include "himci.h"
#include "himci_dbg.h"
#include "himci_acl.h"
#include "himci_proc.h"

/*************************************************************************/
#ifdef CONFIG_ARCH_HI3518EV200
#include "himci_hi3518ev200.c"
#endif

/*************************************************************************/
#define DRIVER_NAME "hi_mci"

#define EMMC_DDR50
#define EMMC_HS200

struct himci_host *host0;
struct himci_host *host1;

static unsigned int detect_time = HI_MCI_DETECT_TIMEOUT;
static unsigned int retry_count = MAX_RETRY_COUNT;
static unsigned int request_timeout = HI_MCI_REQUEST_TIMEOUT;
int trace_level = HIMCI_TRACE_LEVEL;

static const u8 tuning_blk_4bit[] = {
	0xff, 0x0f, 0xff, 0x00, 0xff, 0xcc, 0xc3, 0xcc,
	0xc3, 0x3c, 0xcc, 0xff, 0xfe, 0xff, 0xfe, 0xef,
	0xff, 0xdf, 0xff, 0xdd, 0xff, 0xfb, 0xff, 0xfb,
	0xbf, 0xff, 0x7f, 0xff, 0x77, 0xf7, 0xbd, 0xef,
	0xff, 0xf0, 0xff, 0xf0, 0x0f, 0xfc, 0xcc, 0x3c,
	0xcc, 0x33, 0xcc, 0xcf, 0xff, 0xef, 0xff, 0xee,
	0xff, 0xfd, 0xff, 0xfd, 0xdf, 0xff, 0xbf, 0xff,
	0xbb, 0xff, 0xf7, 0xff, 0xf7, 0x7f, 0x7b, 0xde,
};

static const u8 tuning_blk_8bit[] = {
	0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00,
	0xff, 0xff, 0xcc, 0xcc, 0xcc, 0x33, 0xcc, 0xcc,
	0xcc, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xff, 0xff,
	0xff, 0xee, 0xff, 0xff, 0xff, 0xee, 0xee, 0xff,
	0xff, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xdd, 0xdd,
	0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff, 0xbb,
	0xbb, 0xff, 0xff, 0xff, 0x77, 0xff, 0xff, 0xff,
	0x77, 0x77, 0xff, 0x77, 0xbb, 0xdd, 0xee, 0xff,
	0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00,
	0x00, 0xff, 0xff, 0xcc, 0xcc, 0xcc, 0x33, 0xcc,
	0xcc, 0xcc, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xff,
	0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xee, 0xee,
	0xff, 0xff, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xdd,
	0xdd, 0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff,
	0xbb, 0xbb, 0xff, 0xff, 0xff, 0x77, 0xff, 0xff,
	0xff, 0x77, 0x77, 0xff, 0x77, 0xbb, 0xdd, 0xee,
};

struct tuning_blk_info {
	u8 *blk;
	unsigned int blk_size;
};

#ifdef MODULE

module_param(detect_time, uint, 0600);
MODULE_PARM_DESC(detect_timer, "card detect time (default:500ms))");

module_param(retry_count, uint, 0600);
MODULE_PARM_DESC(retry_count, "retry count times (default:100))");

module_param(request_timeout, uint, 0600);
MODULE_PARM_DESC(request_timeout, "Request timeout time (default:3s))");

module_param(trace_level, int, 0600);
MODULE_PARM_DESC(trace_level, "HIMCI_TRACE_LEVEL");

#endif

/* get MMC host controler pointer for sdio wifi interface
 * hostid: sdio number ( sdio0:  0  sdio1:  1 )
*/

/* reset MMC host controler */
static void himciv200_sys_reset(struct himci_host *host)
{
	unsigned int reg_value;
	unsigned int time_out;

	himci_trace(2, "reset");

	reg_value = himci_readl(host->base + MCI_BMOD);
	reg_value |= BMOD_SWR;
	himci_writel(reg_value, host->base + MCI_BMOD);

	time_out = 1000;
	do {
		reg_value = himci_readl(host->base + MCI_BMOD);
		udelay(10);
	} while ((reg_value & BMOD_SWR) && time_out--);

	reg_value = himci_readl(host->base + MCI_BMOD);
	reg_value |= BURST_INCR;
	himci_writel(reg_value, host->base + MCI_BMOD);

	reg_value = himci_readl(host->base + MCI_CTRL);
	reg_value |= CTRL_RESET | FIFO_RESET | DMA_RESET;
	himci_writel(reg_value, host->base + MCI_CTRL);
}

static void hi_mci_ctrl_power(struct himci_slot *slot,
		unsigned int flag, unsigned int force)
{
	struct himci_host *host = slot->host;
	unsigned int reg_value;
	unsigned int port;
	himci_trace(2, "begin");

	port = slot->port;
	if (slot->power_status != flag || force == FORCE_ENABLE) {
		if (flag == POWER_OFF) {
			reg_value = himci_readl(host->base + MCI_RESET_N);
			reg_value &= ~(MMC_RST_N << port);
			himci_writel(reg_value, host->base + MCI_RESET_N);

			reg_value = himci_readl(host->base + MCI_PWREN);
			reg_value &= ~(0x1 << port);
			himci_writel(reg_value, host->base + MCI_PWREN);

			mdelay(5);

		} else {
			reg_value = himci_readl(host->base + MCI_PWREN);
			reg_value |= (0x1 << port);
			himci_writel(reg_value, host->base + MCI_PWREN);

			reg_value = himci_readl(host->base + MCI_RESET_N);
			reg_value |= (MMC_RST_N << port);
			himci_writel(reg_value, host->base + MCI_RESET_N);

			if (in_interrupt())
				mdelay(37);
			else
				msleep(37);
		}

		slot->power_status = flag;
	}
}

/**********************************************
 *1: card off
 *0: card on
 ***********************************************/
static unsigned int hi_mci_sys_card_detect(struct himci_slot *slot)
{
	struct himci_host *host = slot->host;
	unsigned int card_status;

	card_status = readl(host->base + MCI_CDETECT);
	card_status &= (HIMCI_CARD0 << slot->port);
	if (card_status)
		card_status = 1;
	else
		card_status = 0;

	return card_status;
}

/**********************************************
 *1: card readonly
 *0: card read/write
 ***********************************************/
static unsigned int hi_mci_ctrl_card_readonly(struct himci_slot *slot)
{
	struct himci_host *host = slot->host;
	unsigned int card_value = himci_readl(host->base + MCI_WRTPRT);
	unsigned int port = slot->port;
	return card_value & (HIMCI_CARD0 << port);
}

static int hi_mci_wait_cmd(struct himci_host *host)
{
	int wait_retry_count = 0;
	unsigned int reg_data = 0;
	unsigned long flags;

	himci_trace(2, "begin");
	himci_assert(host);

	while (1) {
		/*
		 * Check if CMD::start_cmd bit is clear.
		 * start_cmd = 0 means MMC Host controller has loaded registers
		 * and next command can be loaded in.
		 */
		reg_data = readl(host->base + MCI_CMD);
		if ((reg_data & START_CMD) == 0)
			return 0;

		/* Check if Raw_Intr_Status::HLE bit is set. */
		spin_lock_irqsave(&host->lock, flags);
		reg_data = readl(host->base + MCI_RINTSTS);
		if (reg_data & HLE_INT_STATUS) {
			reg_data |= HLE_INT_STATUS;
			himci_writel(reg_data, host->base + MCI_RINTSTS);
			spin_unlock_irqrestore(&host->lock, flags);

			himci_trace(3, "Other CMD is running,");
			himci_trace(3, "please operate cmd again!");
			return 1;
		}
		spin_unlock_irqrestore(&host->lock, flags);
		udelay(100);

		/* Check if number of retries for this are over. */
		wait_retry_count++;
		if (wait_retry_count >= retry_count) {
			himci_trace(3, "send cmd is timeout!");
			return -1;
		}
	}
}

static void hi_mci_control_cclk(struct himci_slot *slot, unsigned int flag)
{
	struct himci_host *host = slot->host;
	unsigned int port = slot->port;
	unsigned int reg;
	union cmd_arg_u cmd_reg;

	himci_trace(2, "begin");
	himci_assert(host);

	reg = himci_readl(host->base + MCI_CLKENA);
	if (flag == ENABLE)
		reg |= (CCLK_ENABLE << port);
	else
		reg &= ~(CCLK_ENABLE << port);

	himci_writel(reg, host->base + MCI_CLKENA);
	cmd_reg.cmd_arg = himci_readl(host->base + MCI_CMD);
	cmd_reg.bits.start_cmd = 1;
	cmd_reg.bits.card_number = port;
	cmd_reg.bits.cmd_index = 0;
	cmd_reg.bits.data_transfer_expected = 0;
	cmd_reg.bits.update_clk_reg_only = 1;
	himci_writel(cmd_reg.cmd_arg, host->base + MCI_CMD);
	if (hi_mci_wait_cmd(host) != 0)
		himci_trace(3, "disable or enable clk is timeout!");
}

unsigned int himci_clk_div(struct himci_host *host, unsigned int cclk)
{
	unsigned int clk_div = 0;

	himci_trace(2, "begin");
	/*
	 * set card clk divider value,
	 * clk_divider = Fmmcclk/(Fmmc_cclk * 2)
	 */
	if ((cclk > 0) && (cclk < host->hclk)) {
		clk_div = host->hclk / (cclk * 2);
		if (host->hclk % (cclk * 2))
			clk_div++;
		if (clk_div > 0xFF)
			clk_div = 0xFF;
	} else if (cclk <= 0) {
		clk_div = 0xFF;
	}

	return clk_div;
}

static void hi_mci_set_cclk(struct himci_slot *slot, unsigned int cclk)
{
	struct himci_host *host = slot->host;
	unsigned int port = slot->port;
	unsigned int clk_div = 0;
	union cmd_arg_u clk_cmd;

	himci_trace(2, "begin");
	himci_assert(host);

	himci_clk_ctrl(host, false);
	himci_clk_sel(host, cclk);
	himci_clk_ctrl(host, true);

	clk_div = himci_clk_div(host, cclk);
	slot->clk_div = clk_div;

	slot->hclk = host->hclk;
	slot->cclk = clk_div ? (slot->hclk / (clk_div * 2)) : slot->hclk;
	himci_writel(clk_div<<(port*8), host->base + MCI_CLKDIV);

	clk_cmd.cmd_arg = himci_readl(host->base + MCI_CMD);
	clk_cmd.bits.start_cmd = 1;
	clk_cmd.bits.card_number = port;
	clk_cmd.bits.update_clk_reg_only = 1;
	clk_cmd.bits.cmd_index = 0;
	clk_cmd.bits.data_transfer_expected = 0;
	himci_writel(clk_cmd.cmd_arg, host->base + MCI_CMD);
	if (hi_mci_wait_cmd(host) != 0)
		himci_trace(3, "set card clk divider is failed!");
}

static void himci_claim_host(struct himci_host *host, struct himci_slot *slot)
{
	himci_trace(2, "begin");
	while (1) {
		spin_lock(&host->lock);
		if (!host->claimed) {
			host->claimed = true;
			spin_unlock(&host->lock);
			break;
		} else {
			if (host->is_tuning) {
				if (host->slot == slot) {
					spin_unlock(&host->lock);
					break;
				} else {
					spin_unlock(&host->lock);
					schedule();
				}
			} else {
				spin_unlock(&host->lock);
				schedule();

			}
		}
	}
}

static void himci_release_host(struct himci_host *host)
{
	himci_trace(2, "begin");
	spin_lock(&host->lock);

	if (host->claimed) {
		if (host->is_tuning) {
			spin_unlock(&host->lock);
		} else {
			host->claimed = false;
			spin_unlock(&host->lock);

		}
	} else {
		spin_unlock(&host->lock);
		himci_trace(5, "claim host confuse!!!");
	}
}

static void himci_switch_card(struct himci_slot *slot)
{
	struct himci_host *host = slot->host;
	unsigned int phase_shift;
	himci_trace(2, "begin");

	if (!slot->phase_inited) {
		phase_shift = (slot->phase_drv << CLK_DRV_PHS_OFFSET)
			+ (slot->phase_smp << CLK_SMPL_PHS_OFFSET);

		himci_writel(phase_shift, host->base + MCI_UHS_REG_EXT);
		slot->phase_inited = 1;
		himci_trace(3, "phase %d", phase_shift);
	}

	if (host->slot != slot) {
		host->slot = slot;
		host->cur_port = slot->port;
		host->mmc = slot->mmc;

		phase_shift = (slot->phase_drv << CLK_DRV_PHS_OFFSET)
			+ (slot->phase_smp << CLK_SMPL_PHS_OFFSET);

		himci_writel(phase_shift, host->base + MCI_UHS_REG_EXT);
		himci_trace(3, "hclk %d, cclk %d", slot->hclk, slot->cclk);
		hi_mci_set_cclk(slot, slot->cclk);

	}
}

static int hi_mci_do_start_signal_voltage_switch(struct himci_slot *slot,
						 struct mmc_ios *ios)
{
	struct himci_host *host = slot->host;
	unsigned int port = slot->port;
	u32 ctrl;

	/*
	 * We first check whether the request is to set signalling voltage
	 * to 3.3V. If so, we change the voltage to 3.3V and return quickly.
	 */
	ctrl = himci_readl(host->base + MCI_UHS_REG);
	if (ios->signal_voltage == MMC_SIGNAL_VOLTAGE_330) {
		/* Stop SDCLK */
		hi_mci_control_cclk(slot, DISABLE);
		/* Set 1.8V Signal Enable in the MCI_UHS_REG to 1 */
		ctrl &= ~(HI_SDXC_CTRL_VDD_180 << port);
		himci_writel(ctrl, host->base + MCI_UHS_REG);

		/* Wait for 5ms */
		usleep_range(5000, 5500);

		/* Provide SDCLK again and wait for 1ms */
		hi_mci_control_cclk(slot, ENABLE);

		/* 3.3V regulator output should be stable within 5 ms */
		ctrl = himci_readl(host->base + MCI_UHS_REG);
		if (!(ctrl & (HI_SDXC_CTRL_VDD_180 << port))) {
			himci_set_padctrl(slot, MMC_SIGNAL_VOLTAGE_330);
			return 0;
		} else {
			himci_error(": Switching to 3.3V ");
			himci_error("signalling voltage failed\n");
			return -EIO;
		}
	} else if (!(ctrl & (HI_SDXC_CTRL_VDD_180 << port)) &&
		   (ios->signal_voltage == MMC_SIGNAL_VOLTAGE_180)) {
		/* Stop SDCLK */
		hi_mci_control_cclk(slot, DISABLE);

		/*
		 * Enable 1.8V Signal Enable in the MCI_UHS_REG
		 */
		ctrl |= (HI_SDXC_CTRL_VDD_180 << port);
		himci_writel(ctrl, host->base + MCI_UHS_REG);

		/* Wait for 8ms */
		usleep_range(8000, 8500);

		ctrl = himci_readl(host->base + MCI_UHS_REG);
		if (ctrl & (HI_SDXC_CTRL_VDD_180 << port)) {
			/* Provide SDCLK again and wait for 1ms */
			hi_mci_control_cclk(slot, ENABLE);
			usleep_range(1000, 1500);

			if (host->mmc->caps2 & MMC_CAP2_HS200) {
				/* eMMC needn't to check the int status*/
					return 0;
			}
			/*
			 * If CMD11 return CMD down, then the card
			 * was successfully switched to 1.8V signaling.
			 */
			ctrl = himci_readl(host->base + MCI_RINTSTS);
			if ((ctrl & VOLT_SWITCH_INT_STATUS)
			    && (ctrl & CD_INT_STATUS)) {
				himci_writel(VOLT_SWITCH_INT_STATUS
						| CD_INT_STATUS,
						host->base + MCI_RINTSTS);
				himci_set_padctrl(slot, MMC_SIGNAL_VOLTAGE_180);
				return 0;
			}
		}

		/*
		 * If we are here, that means the switch to 1.8V signaling
		 * failed. We power cycle the card, and retry initialization
		 * sequence by setting S18R to 0.
		 */

		ctrl &= ~(HI_SDXC_CTRL_VDD_180 << port);
		himci_writel(ctrl, host->base + MCI_UHS_REG);

		/* Wait for 5ms */
		usleep_range(5000, 5500);

		hi_mci_ctrl_power(slot, POWER_OFF, FORCE_DISABLE);
		/* Wait for 1ms as per the spec */
		usleep_range(1000, 1500);
		hi_mci_ctrl_power(slot, POWER_ON, FORCE_DISABLE);

		hi_mci_control_cclk(slot, DISABLE);
		/* Wait for 1ms as per the spec */
		usleep_range(1000, 1500);
		hi_mci_control_cclk(slot, ENABLE);

		himci_error(": Switching to 1.8V signalling ");
		himci_error("voltage failed, retrying with S18R set to 0\n");
		return -EAGAIN;
	} else {
		/* No signal voltage switch required */
		return 0;
	}
}

static void hi_mci_init_host(struct himci_host *host)
{
	unsigned int tmp_reg;
	unsigned long flags;

	himci_trace(3, "begin init card");
	himci_assert(host);

	himciv200_sys_reset(host);

	/* sd ust clk0 emmc use clk1 */
	himci_writel(0x4, host->base + MCI_CLKSRC);

	/* set drv/smpl phase shift */
	tmp_reg = (DRV_PHASE_SHIFT << CLK_DRV_PHS_OFFSET)
		+ (SMPL_PHASE_SHIFT << CLK_SMPL_PHS_OFFSET);
	himci_writel(tmp_reg, host->base + MCI_UHS_REG_EXT);

	/* set card read threshold */
	himci_writel(READ_THRESHOLD_SIZE, host->base + MCI_CARDTHRCTL);

	/* clear MMC host intr */
	himci_writel(ALL_INT_CLR, host->base + MCI_RINTSTS);

	spin_lock_irqsave(&host->lock, flags);
	host->pending_events = 0;
	spin_unlock_irqrestore(&host->lock, flags);

	/* MASK MMC all host intr */
	tmp_reg = himci_readl(host->base + MCI_INTMASK);
	tmp_reg &= ~ALL_INT_MASK;
	tmp_reg |= DATA_INT_MASK;
	himci_writel(tmp_reg, host->base + MCI_INTMASK);

	/* enable inner DMA mode and close intr of MMC host controler */
	tmp_reg = himci_readl(host->base + MCI_CTRL);
	tmp_reg &= ~INTR_EN;
	tmp_reg |= USE_INTERNAL_DMA | INTR_EN;
	himci_writel(tmp_reg, host->base + MCI_CTRL);

	/* set timeout param */
	himci_writel(DATA_TIMEOUT | RESPONSE_TIMEOUT, host->base + MCI_TIMEOUT);

	/* set FIFO param */
	tmp_reg = 0;
	tmp_reg |= BURST_SIZE | RX_WMARK | TX_WMARK;
	himci_writel(tmp_reg, host->base + MCI_FIFOTH);
}

static void himci_init_card(struct himci_slot *slot)
{
	slot->phase_inited = 0;
	slot->phase_drv = DRV_PHASE_SHIFT;
	slot->phase_smp = SMPL_PHASE_SHIFT;

	/* card power off */
	hi_mci_ctrl_power(slot, POWER_OFF, FORCE_ENABLE);
	/* card power on */
	hi_mci_ctrl_power(slot, POWER_ON, FORCE_ENABLE);
}

static void hi_mci_detect_card(unsigned long arg)
{
	struct himci_slot *slot = (struct himci_slot *)arg;
	struct himci_host *host = slot->host;
	unsigned int i, curr_status, status[3], detect_retry_count = 0;

	himci_assert(host);

	while (1) {
		for (i = 0; i < 3; i++) {
			status[i] = hi_mci_sys_card_detect(slot);
			udelay(10);
		}
		if ((status[0] == status[1]) && (status[0] == status[2]))
			break;
		detect_retry_count++;
		if (detect_retry_count >= retry_count) {
			himci_error("this is a dithering, card detect error!");
			goto err;
		}
	}
	curr_status = status[0];
	if (curr_status != slot->card_status) {
		himci_trace(2, "begin card_status = %d\n",
				slot->card_status);
		slot->card_status = curr_status;
		if (curr_status != CARD_UNPLUGED) {

			/* enable mmc clk */
			hi_mci_sys_ctrl_init(host, host->crg_phy_addr,
					host->devid);

			/* enable card */
			hi_mci_init_host(host);

			slot->power_status = POWER_OFF;
			himci_init_card(slot);

			pr_info("card connected!\n");
		} else {
			pr_info("card disconnected!\n");
		}

		mmc_detect_change(slot->mmc, 0);
	}
err:
	mod_timer(&slot->timer, jiffies + detect_time);
}

static void hi_mci_idma_start(struct himci_host *host)
{
	unsigned int tmp;

	himci_trace(2, "begin");
	himci_writel(host->dma_paddr, host->base + MCI_DBADDR);
	tmp = himci_readl(host->base + MCI_BMOD);
	tmp |= BMOD_DMA_EN;
	himci_writel(tmp, host->base + MCI_BMOD);
}

static void hi_mci_idma_stop(struct himci_host *host)
{
	unsigned int tmp_reg;

	himci_trace(2, "begin");
	tmp_reg = himci_readl(host->base + MCI_BMOD);
	tmp_reg &= ~BMOD_DMA_EN;
	himci_writel(tmp_reg, host->base + MCI_BMOD);
}

static int hi_mci_setup_data(struct himci_host *host, struct mmc_data *data)
{
	unsigned int sg_phyaddr, sg_length;
	unsigned int i, ret = 0;
	unsigned int data_size;
	unsigned int max_des, des_cnt;
	struct himci_des *des;

	himci_trace(2, "begin");
	himci_assert(host);
	himci_assert(data);

	host->data = data;

	if (data->flags & MMC_DATA_READ)
		host->dma_dir = DMA_FROM_DEVICE;
	else
		host->dma_dir = DMA_TO_DEVICE;

	host->dma_sg = data->sg;
	host->dma_sg_num = dma_map_sg(mmc_dev(host->mmc), data->sg,
				      data->sg_len, host->dma_dir);
	himci_assert(host->dma_sg_num);
	himci_trace(2, "host->dma_sg_num is %d\n", host->dma_sg_num);
	data_size = data->blksz * data->blocks;

	if (data_size > (DMA_BUFFER * MAX_DMA_DES)) {
		himci_error("mci request data_size is too big!\n");
		ret = -1;
		goto out;
	}

	himci_trace(2, "host->dma_paddr is 0x%08X,host->dma_vaddr is 0x%08X\n",
			(unsigned int)host->dma_paddr,
			(unsigned int)host->dma_vaddr);

	max_des = (PAGE_SIZE / sizeof(struct himci_des));
	des = (struct himci_des *)host->dma_vaddr;
	des_cnt = 0;

	for (i = 0; i < host->dma_sg_num; i++) {
		sg_length = sg_dma_len(&data->sg[i]);
		sg_phyaddr = sg_dma_address(&data->sg[i]);
		himci_trace(2, "sg[%d] sg_length is 0x%08X, ",
				i, (unsigned int)sg_length);
		himci_trace(2, "sg_phyaddr is 0x%08X\n",
				(unsigned int)sg_phyaddr);
		while (sg_length) {
			des[des_cnt].idmac_des_ctrl =
			    DMA_DES_OWN | DMA_DES_NEXT_DES;
			des[des_cnt].idmac_des_buf_addr = sg_phyaddr;
			/* idmac_des_next_addr is paddr for dma */
			des[des_cnt].idmac_des_next_addr = host->dma_paddr
				+ (des_cnt + 1) * sizeof(struct himci_des);

			/* buffer size <= 4k */
			if (sg_length >= 0x1000) {
				des[des_cnt].idmac_des_buf_size = 0x1000;
				sg_length -= 0x1000;
				sg_phyaddr += 0x1000;
			} else {
				/* data alignment */
				des[des_cnt].idmac_des_buf_size = sg_length;
				sg_length = 0;
			}
			himci_trace(2, "des[%d] vaddr  is 0x%08X",
				    i, (unsigned int)&des[i]);
			himci_trace(2, "des[%d].idmac_des_ctrl is 0x%08X",
				    i, (unsigned int)des[i].idmac_des_ctrl);
			himci_trace(2, "des[%d].idmac_des_buf_size is 0x%08X",
				    i, (unsigned int)des[i].idmac_des_buf_size);
			himci_trace(2, "des[%d].idmac_des_buf_addr 0x%08X",
				    i, (unsigned int)des[i].idmac_des_buf_addr);
			himci_trace(2, "des[%d].idmac_des_next_addr is 0x%08X",
				i, (unsigned int)des[i].idmac_des_next_addr);
			des_cnt++;
		}

		himci_assert(des_cnt < max_des);
	}
	des[0].idmac_des_ctrl |= DMA_DES_FIRST_DES;
	des[des_cnt - 1].idmac_des_ctrl |= DMA_DES_LAST_DES;
	des[des_cnt - 1].idmac_des_next_addr = 0;
out:
	return ret;
}

static int hi_mci_exec_cmd(struct himci_host *host,
			   struct mmc_command *cmd, struct mmc_data *data)
{
	volatile union cmd_arg_u cmd_regs;
	unsigned int port = host->cur_port;

	himci_trace(2, "begin");
	himci_assert(host);
	himci_assert(cmd);

	host->cmd = cmd;

	himci_writel(cmd->arg, host->base + MCI_CMDARG);
	himci_trace(4, "arg_reg 0x%x, val 0x%x\n", MCI_CMDARG, cmd->arg);
	cmd_regs.cmd_arg = himci_readl(host->base + MCI_CMD);
	if (data) {
		cmd_regs.bits.data_transfer_expected = 1;
		if (data->flags & (MMC_DATA_WRITE | MMC_DATA_READ))
			cmd_regs.bits.transfer_mode = 0;
		if (data->flags & MMC_DATA_STREAM)
			cmd_regs.bits.transfer_mode = 1;
		if (data->flags & MMC_DATA_WRITE)
			cmd_regs.bits.read_write = 1;
		else if (data->flags & MMC_DATA_READ)
			cmd_regs.bits.read_write = 0;
	} else {
		cmd_regs.bits.data_transfer_expected = 0;
		cmd_regs.bits.transfer_mode = 0;
		cmd_regs.bits.read_write = 0;
	}

	cmd_regs.bits.send_auto_stop = 0;
#ifdef CONFIG_SEND_AUTO_STOP
	if ((host->mrq->stop) && (!(host->is_tuning)))
		cmd_regs.bits.send_auto_stop = 1;
#endif

	if (cmd == host->mrq->stop) {
		cmd_regs.bits.stop_abort_cmd = 1;
		cmd_regs.bits.wait_prvdata_complete = 0;
	} else {
		cmd_regs.bits.stop_abort_cmd = 0;
		cmd_regs.bits.wait_prvdata_complete = 1;
	}

	switch (mmc_resp_type(cmd)) {
	case MMC_RSP_NONE:
		cmd_regs.bits.response_expect = 0;
		cmd_regs.bits.response_length = 0;
		cmd_regs.bits.check_response_crc = 0;
		break;
	case MMC_RSP_R1:
	case MMC_RSP_R1B:
		cmd_regs.bits.response_expect = 1;
		cmd_regs.bits.response_length = 0;
		cmd_regs.bits.check_response_crc = 1;
		break;
	case MMC_RSP_R2:
		cmd_regs.bits.response_expect = 1;
		cmd_regs.bits.response_length = 1;
		cmd_regs.bits.check_response_crc = 1;
		break;
	case MMC_RSP_R3:
		cmd_regs.bits.response_expect = 1;
		cmd_regs.bits.response_length = 0;
		cmd_regs.bits.check_response_crc = 0;
		break;
	default:
		host->cmd->error = -EINVAL;
		himci_error("hi_mci: unhandled response type %02x\n",
				mmc_resp_type(cmd));
		return -EINVAL;
	}

	himci_trace(3, "cmd->opcode = %d cmd->arg = 0x%X\n",
			cmd->opcode, cmd->arg);
	if (cmd->opcode == MMC_GO_IDLE_STATE)
		cmd_regs.bits.send_initialization = 1;
	else
		cmd_regs.bits.send_initialization = 0;
	if (cmd->opcode == SD_SWITCH_VOLTAGE)
		cmd_regs.bits.volt_switch = 1;
	else
		cmd_regs.bits.volt_switch = 0;

	cmd_regs.bits.card_number = port;
	cmd_regs.bits.cmd_index = cmd->opcode;
	cmd_regs.bits.start_cmd = 1;
	cmd_regs.bits.update_clk_reg_only = 0;

	himci_writel(DATA_INT_MASK, host->base + MCI_RINTSTS);
	himci_writel(cmd_regs.cmd_arg, host->base + MCI_CMD);
	himci_trace(4, "cmd_reg 0x%x, val 0x%x\n", MCI_CMD, cmd_regs.cmd_arg);

	if (hi_mci_wait_cmd(host) != 0) {
		himci_trace(3, "send card cmd is failed!");
		return -EINVAL;
	}
	return 0;
}

static void hi_mci_finish_request(struct himci_slot *slot,
				  struct mmc_request *mrq)
{
	struct himci_host *host = slot->host;
	himci_trace(2, "begin");
	himci_assert(host);
	himci_assert(mrq);

	host->mrq = NULL;
	host->cmd = NULL;
	host->data = NULL;
	mmc_request_done(slot->mmc, mrq);
}

static void hi_mci_cmd_done(struct himci_host *host, unsigned int stat)
{
	unsigned int i;
	struct mmc_command *cmd = host->cmd;

	himci_trace(2, "begin");
	himci_assert(host);
	himci_assert(cmd);

	for (i = 0; i < 4; i++) {
		if (mmc_resp_type(cmd) == MMC_RSP_R2) {
			cmd->resp[i] = himci_readl(host->base +
					MCI_RESP3 - i * 0x4);
			/* R2 must delay some time here ,when use UHI card,
			   need check why */
			udelay(1000);
		} else
			cmd->resp[i] = himci_readl(host->base +
						   MCI_RESP0 + i * 0x4);
	}

	if (stat & RTO_INT_STATUS) {
		cmd->error = -ETIMEDOUT;
		himci_trace(3, "irq cmd status stat = 0x%x is timeout error!",
			    stat);
	} else if (stat & (RCRC_INT_STATUS | RE_INT_STATUS)) {
		cmd->error = -EILSEQ;
		himci_trace(3, "irq cmd status stat = 0x%x is response error!",
				stat);
	}

	host->cmd = NULL;
}

static void hi_mci_data_done(struct himci_host *host, unsigned int stat)
{
	struct mmc_data *data = host->data;

	himci_trace(2, "begin");
	himci_assert(host);
	himci_assert(data);

	dma_unmap_sg(mmc_dev(host->slot->mmc), data->sg,
			data->sg_len, host->dma_dir);

	if (stat & (HTO_INT_STATUS | DRTO_INT_STATUS)) {
		data->error = -ETIMEDOUT;
		himci_trace(3, "irq data status stat = 0x%x is timeout error!",
			    stat);
	} else if (stat & (EBE_INT_STATUS | SBE_INT_STATUS |
			   FRUN_INT_STATUS | DCRC_INT_STATUS)) {
		data->error = -EILSEQ;
		himci_trace(3, "irq data status stat = 0x%x is data error!",
			    stat);
	}

	if (!data->error)
		data->bytes_xfered = data->blocks * data->blksz;
	else
		data->bytes_xfered = 0;

	host->data = NULL;
}

static int hi_mci_wait_cmd_complete(struct himci_host *host)
{
	struct himci_slot *slot = host->slot;
	unsigned int cmd_retry_count = 0;
	unsigned long cmd_jiffies_timeout;
	unsigned int cmd_irq_reg = 0;
	struct mmc_command *cmd = host->cmd;
	unsigned long flags;

	himci_trace(2, "begin");
	himci_assert(host);
	himci_assert(cmd);

	cmd_jiffies_timeout = jiffies + request_timeout;
	while (1) {

		do {
			spin_lock_irqsave(&host->lock, flags);
			cmd_irq_reg = readl(host->base + MCI_RINTSTS);

			if (cmd_irq_reg & CD_INT_STATUS) {
				himci_writel((CD_INT_STATUS | RTO_INT_STATUS
					| RCRC_INT_STATUS | RE_INT_STATUS),
					host->base + MCI_RINTSTS);
				spin_unlock_irqrestore(&host->lock, flags);
				hi_mci_cmd_done(host, cmd_irq_reg);
				return 0;
			} else if (cmd_irq_reg & VOLT_SWITCH_INT_STATUS) {
				himci_writel(VOLT_SWITCH_INT_STATUS,
						host->base + MCI_RINTSTS);
				spin_unlock_irqrestore(&host->lock, flags);
				hi_mci_cmd_done(host, cmd_irq_reg);
				return 0;
			}
			spin_unlock_irqrestore(&host->lock, flags);
			cmd_retry_count++;
		} while (cmd_retry_count < retry_count);

		cmd_retry_count = 0;

		if (slot->card_status == CARD_UNPLUGED) {
			cmd->error = -ETIMEDOUT;
			return -1;
		}

		if (!time_before(jiffies, cmd_jiffies_timeout)) {
			unsigned int i = 0;
			for (i = 0; i < 4; i++) {
				cmd->resp[i] = himci_readl(host->base
						+ MCI_RESP0 + i * 0x4);
				pr_err("voltage switch read MCI_RESP");
				pr_err("%d : 0x%x\n", i, cmd->resp[i]);
			}
			cmd->error = -ETIMEDOUT;
			himci_trace(3, "wait cmd request complete is timeout!");
			return -1;
		}

		schedule();
	}
}
/*
 * designware support send stop command automatically when
 * read or wirte multi blocks
 */
#ifdef CONFIG_SEND_AUTO_STOP
static int hi_mci_wait_auto_stop_complete(struct himci_host *host)
{
	struct himci_slot *slot = host->slot;
	unsigned int cmd_retry_count = 0;
	unsigned long cmd_jiffies_timeout;
	unsigned int cmd_irq_reg = 0;
	unsigned long flags;

	himci_trace(2, "begin");
	himci_assert(host);

	cmd_jiffies_timeout = jiffies + request_timeout;
	while (1) {

		do {
			spin_lock_irqsave(&host->lock, flags);
			cmd_irq_reg = readl(host->base + MCI_RINTSTS);
			if (cmd_irq_reg & ACD_INT_STATUS) {
				himci_writel((ACD_INT_STATUS | RTO_INT_STATUS
					| RCRC_INT_STATUS | RE_INT_STATUS),
					host->base + MCI_RINTSTS);
				spin_unlock_irqrestore(&host->lock, flags);
				return 0;
			}
			spin_unlock_irqrestore(&host->lock, flags);
			cmd_retry_count++;
		} while (cmd_retry_count < retry_count);

		cmd_retry_count = 0;
		if (slot->card_status == CARD_UNPLUGED)
			return -1;
		if (!time_before(jiffies, cmd_jiffies_timeout)) {
			himci_trace(3, "wait auto stop complete is timeout!");
			return -1;
		}

		schedule();
	}
}
#endif

static int hi_mci_wait_data_complete(struct himci_host *host)
{
	struct himci_slot *slot = host->slot;
	unsigned int tmp_reg;
	struct mmc_data *data = host->data;
	long time = request_timeout;
	unsigned long flags;

	himci_trace(2, "begin");
	himci_assert(host);
	himci_assert(data);

	time = wait_event_timeout(host->intr_wait,
				  test_bit(HIMCI_PEND_DTO_B,
					   &host->pending_events), time);

	/* Mask MMC host data intr */
	spin_lock_irqsave(&host->lock, flags);
	tmp_reg = himci_readl(host->base + MCI_INTMASK);
	tmp_reg &= ~DATA_INT_MASK;
	himci_writel(tmp_reg, host->base + MCI_INTMASK);
	host->pending_events &= ~HIMCI_PEND_DTO_M;
	spin_unlock_irqrestore(&host->lock, flags);

	if (((time <= 0)
		&& (!test_bit(HIMCI_PEND_DTO_B, &host->pending_events)))
		|| (slot->card_status == CARD_UNPLUGED)) {

		data->error = -ETIMEDOUT;
		himci_trace(3, "wait data request complete is timeout! 0x%08X",
				host->irq_status);
		hi_mci_idma_stop(host);
		hi_mci_data_done(host, host->irq_status);
		return -1;
	}

	hi_mci_idma_stop(host);
	hi_mci_data_done(host, host->irq_status);
	return 0;
}

static int hi_mci_wait_card_complete(struct himci_host *host,
		struct mmc_data *data)
{
	struct himci_slot *slot = host->slot;
	unsigned int card_retry_count = 0;
	unsigned long card_jiffies_timeout;
	unsigned int card_status_reg = 0;

	himci_trace(2, "begin");
	himci_assert(host);

	card_jiffies_timeout = jiffies + request_timeout;
	while (1) {
		do {
			card_status_reg = readl(host->base + MCI_STATUS);
			if (!(card_status_reg & DATA_BUSY)) {
				himci_trace(2, "end");
				return 0;
			}
			card_retry_count++;
		} while (card_retry_count < retry_count);

		card_retry_count = 0;

		if (slot->card_status == CARD_UNPLUGED) {
			host->mrq->cmd->error = -ETIMEDOUT;
			himci_trace(3, "card is unpluged!");
			return -1;
		}

		if (!time_before(jiffies, card_jiffies_timeout)) {
			host->mrq->cmd->error = -ETIMEDOUT;
			himci_trace(3, "wait card ready complete is timeout!");
			return -1;
		}

		schedule();
	}
}

static void hi_mci_request(struct mmc_host *mmc, struct mmc_request *mrq)
{
	struct himci_slot *slot = mmc_priv(mmc);
	struct himci_host *host = slot->host;
	unsigned int rw = 0;
	unsigned int ofblk = 0, nrblk = 0;
	unsigned int byte_cnt = 0, trans_cnt;
	unsigned int tmp_reg, fifo_count = 0;
	int ret = 0;
	unsigned long flags;

	himci_trace(2, "begin");
	himci_assert(mmc);
	himci_assert(mrq);
	himci_assert(host);

	himci_claim_host(host, slot);
	himci_switch_card(slot);

	host->mrq = mrq;
	host->irq_status = 0;

	if (slot->card_status == CARD_UNPLUGED) {
		mrq->cmd->error = -ENODEV;
		goto request_end;
	}

	ret = hi_mci_wait_card_complete(host, mrq->data);
	if (ret) {
		mrq->cmd->error = ret;
		goto request_end;
	}

	/* prepare data */
	if (mrq->data) {
		nrblk = mrq->data->blocks;

		if (mrq->cmd->opcode == MMC_READ_SINGLE_BLOCK ||
		    mrq->cmd->opcode == MMC_READ_MULTIPLE_BLOCK) {
			rw = MMC_DATA_READ;
			ofblk = mrq->cmd->arg;
		}

		if (mrq->cmd->opcode == MMC_WRITE_MULTIPLE_BLOCK ||
		    mrq->cmd->opcode == MMC_WRITE_BLOCK) {
			rw = MMC_DATA_WRITE;
			ofblk = mrq->cmd->arg;
#ifdef CONFIG_HIMCIV200_READ_ONLY
			if (himci_acl_rw(host->devid, 1, ofblk, nrblk)) {
				mrq->cmd->error = -EACCES;
				goto request_end;
			}
#endif
		}

		ret = hi_mci_setup_data(host, mrq->data);
		if (ret) {
			mrq->data->error = ret;
			himci_trace(3, "data setup is error!");
			goto request_end;
		}

		byte_cnt = mrq->data->blksz * mrq->data->blocks;
		himci_writel(byte_cnt, host->base + MCI_BYTCNT);
		himci_writel(mrq->data->blksz, host->base + MCI_BLKSIZ);

		/* reset fifo */
		tmp_reg = himci_readl(host->base + MCI_CTRL);
		tmp_reg |= FIFO_RESET;
		himci_writel(tmp_reg, host->base + MCI_CTRL);

		do {
			tmp_reg = himci_readl(host->base + MCI_CTRL);
			fifo_count++;
			if (fifo_count >= retry_count) {
				pr_info("fifo reset is timeout!");
				goto request_end;
			}
		} while (tmp_reg & FIFO_RESET);

		/* start DMA */
		hi_mci_idma_start(host);
	} else {
		himci_writel(0, host->base + MCI_BYTCNT);
		himci_writel(0, host->base + MCI_BLKSIZ);
	}

	/* send command */
	ret = hi_mci_exec_cmd(host, mrq->cmd, mrq->data);
	if (ret) {
		mrq->cmd->error = ret;
		hi_mci_idma_stop(host);
		himci_trace(3, "can't send card cmd! ret = %d", ret);
		goto request_end;
	}

	/* wait command send complete */
	ret = hi_mci_wait_cmd_complete(host);

	/* start data transfer */
	if (mrq->data) {
		if (!(mrq->cmd->error)) {
			/* Open MMC host data intr */
			spin_lock_irqsave(&host->lock, flags);
			tmp_reg = himci_readl(host->base + MCI_INTMASK);
			tmp_reg |= DATA_INT_MASK;
			himci_writel(tmp_reg, host->base + MCI_INTMASK);
			spin_unlock_irqrestore(&host->lock, flags);

			/* wait data transfer complete */
			ret = hi_mci_wait_data_complete(host);
		} else {
			/* CMD error in data command */
			hi_mci_idma_stop(host);
		}

		if (mrq->stop) {
#ifdef CONFIG_SEND_AUTO_STOP
			trans_cnt = himci_readl(host->base + MCI_TCBCNT);
			/* send auto stop */
			if ((trans_cnt == byte_cnt) && (!(host->is_tuning))) {
				himci_trace(3, "byte_cnt = %d, trans_cnt = %d",
						byte_cnt, trans_cnt);
				ret = hi_mci_wait_auto_stop_complete(host);
				if (ret) {
					mrq->stop->error = -ETIMEDOUT;
					goto request_end;
				}
			} else {
#endif
				/* send soft stop command */
				himci_trace(3, "this time, send soft stop");
				ret = hi_mci_exec_cmd(host, host->mrq->stop,
						host->data);
				if (ret) {
					mrq->stop->error = ret;
					goto request_end;
				}
				ret = hi_mci_wait_cmd_complete(host);
				if (ret)
					goto request_end;
#ifdef CONFIG_SEND_AUTO_STOP
			}
#endif
		}
		if (rw == MMC_DATA_WRITE)
			himci_dbg_rw(host->devid, 1, ofblk, nrblk);

		if (rw == MMC_DATA_READ)
			himci_dbg_rw(host->devid, 0, ofblk, nrblk);
	}

request_end:
	/* clear MMC host intr */
	spin_lock_irqsave(&host->lock, flags);
	himci_writel(ALL_SD_INT_CLR, host->base + MCI_RINTSTS);
	spin_unlock_irqrestore(&host->lock, flags);

	hi_mci_finish_request(slot, mrq);
	himci_release_host(host);
}

static int hi_mci_start_signal_voltage_switch(struct mmc_host *mmc,
					      struct mmc_ios *ios)
{
	struct himci_slot *slot = mmc_priv(mmc);
	int err;

	himci_claim_host(slot->host, slot);
	himci_switch_card(slot);
	err = hi_mci_do_start_signal_voltage_switch(slot, ios);
	himci_release_host(slot->host);

	return err;
}

#define HIMCI_PHASE_SCALE	8 /* The num of phase shift */
#define NOT_FOUND		-1
/*
 * The procedure of tuning the phase shift of sampling clock
 *
 * 1.Set a phase shift of 0?? on cclk_in_sample
 * 2.Send the Tuning command to the card
 * 3.increase the phase shift value of cclk_in_sample until the
 *   correct sampling point is received such that the host does not
 *   see any of the errors.
 * 4.Mark this phase shift value as the starting point of the sampling
 *   window.
 * 5.increase the phase shift value of cclk_in_sample until the host
 *   sees the errors starting to come again or the phase shift value
 *   reaches 360??.
 * 6.Mark the last successful phase shift value as the ending
 *   point of the sampling window.
 *
 *     A window is established where the tuning block is matched.
 * For example, for a scenario where the tuning block is received
 * correctly for a phase shift window of 90??and 180??, then an appropriate
 * sampling point is established as 135??. Once a sampling point is
 * established, no errors should be visible in the tuning block.
 *
 */
static int hi_mci_execute_tuning(struct mmc_host *mmc, u32 opcode)
{
	struct	himci_slot *slot;
	struct	himci_host *host;
	unsigned long flags;
	unsigned int index, count;
	unsigned int err = 0;
	unsigned int found = 0; /* identify if we have found a valid phase */
	unsigned int reg_value;
	unsigned int start_point;
	unsigned int end_point;
	unsigned int prev_err = NOT_FOUND;
	unsigned int raise_point = NOT_FOUND;
	unsigned int fall_point = NOT_FOUND;
	int	 tmp;

	struct mmc_command cmd = {0};
	struct mmc_command stop = {0};
	struct mmc_request mrq = {NULL};
	struct mmc_data data = {0};

	u32 cmd_opcode = opcode;
	struct tuning_blk_info tuning_info = {NULL, 0};
	struct scatterlist sg;
	void *data_buf;

	slot = mmc_priv(mmc);
	host = slot->host;

	himci_claim_host(host, slot);
	himci_switch_card(slot);

	spin_lock(&host->lock);
	host->is_tuning = 1; /* come into phase tuning process */
	spin_unlock(&host->lock);

	switch (cmd_opcode) {
	case MMC_SEND_TUNING_BLOCK_HS200:
		if (mmc->ios.bus_width == MMC_BUS_WIDTH_8) {
			tuning_info.blk = (u8 *)tuning_blk_8bit;
			tuning_info.blk_size = sizeof(tuning_blk_8bit);
		} else if (mmc->ios.bus_width == MMC_BUS_WIDTH_4) {
			tuning_info.blk = (u8 *)tuning_blk_4bit;
			tuning_info.blk_size = sizeof(tuning_blk_4bit);
		}
		break;

	case MMC_SEND_TUNING_BLOCK:
		tuning_info.blk = (u8 *)tuning_blk_4bit;
		tuning_info.blk_size = sizeof(tuning_blk_4bit);
		break;
	case MMC_SWITCH:
		tuning_info.blk = (u8 *)tuning_blk_4bit;
		tuning_info.blk_size = sizeof(tuning_blk_4bit);
		cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;
		break;
	default:
		himci_trace(3, "command %d not support", cmd_opcode);
		himci_release_host(host);
		return -EINVAL;
	}

	himci_trace(3, "command %d send", cmd_opcode);

	data_buf = kmalloc(tuning_info.blk_size, GFP_KERNEL);
	memset(data_buf, 0, tuning_info.blk_size);
	if (data_buf == NULL)
		return -ENOMEM;

	sg_init_one(&sg, data_buf, tuning_info.blk_size);

	himci_writel(ALL_INT_CLR, host->base + MCI_RINTSTS);
	himci_writel(0x1, host->base + MCI_CARDTHRCTL);
	himci_trace(3, "start sd3.0 phase tuning...");

	for (index = 0; index < HIMCI_PHASE_SCALE; index++) {

		/* Construct mmc_request */
		cmd.opcode = cmd_opcode;
		cmd.arg = 0;
		cmd.retries = 0;
		cmd.data = NULL;
		cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;
		cmd.error = 0;
		data.error = 0;

		mrq.data = &data;
		mrq.data->blksz = tuning_info.blk_size;
		mrq.data->blocks = 1;
		mrq.data->flags = MMC_DATA_READ;
		mrq.data->sg = &sg;
		mrq.data->sg_len = 1;

		mrq.stop = &stop;
		mrq.stop->opcode = MMC_STOP_TRANSMISSION;
		mrq.stop->arg = 0;
		mrq.stop->flags = MMC_RSP_R1B | MMC_CMD_AC;

		mrq.cmd = &cmd;
		host->mrq = &mrq;

		/* set sample clk phase shift */
		spin_lock_irqsave(&host->lock, flags);
		reg_value = himci_readl(host->base + MCI_UHS_REG_EXT);
		reg_value &= ~CLK_SMPL_PHS_MASK;
		reg_value |= (index << CLK_SMPL_PHS_OFFSET);
		himci_writel(reg_value, host->base + MCI_UHS_REG_EXT);
		spin_unlock_irqrestore(&host->lock, flags);

		himci_writel(ALL_INT_CLR, host->base + MCI_RINTSTS);

		count = 0;
		do {
			cmd.error = 0;
			data.error = 0;
			memset(data_buf, 0, tuning_info.blk_size);
			mmc_wait_for_req(mmc, &mrq);

			if ((cmd.error) || (data.error)) {
				himci_trace(3, "cmd.error=%d data.error=%d",
						cmd.error, data.error);
				err = 1;
				break;
			} else {
				if (memcmp(tuning_info.blk, data_buf,
							tuning_info.blk_size)) {
					himci_trace(3, "tuning blk not same!");
					err = 1;
					break;
				}
			}
			count++;
		} while (count < 40);

		if (!err) {
			/* found first valid phase */
			found = 1;
		}
		if (index) {
			if (err && !prev_err)
				fall_point = index - 1;

			if (!err && prev_err)
				raise_point = index;
		}

		if ((raise_point != NOT_FOUND) && (fall_point != NOT_FOUND))
			goto tuning_out;

		prev_err = err;
		err = 0;
	}

tuning_out:
	kfree(data_buf);

	if (!found) {
		himci_trace(5, "%s: no valid phase shift! use default",
				mmc_hostname(mmc));
		/* set drv/smpl phase shift */
		reg_value = (DRV_PHASE_SHIFT << CLK_DRV_PHS_OFFSET)
			+ (SMPL_PHASE_SHIFT << CLK_SMPL_PHS_OFFSET);
		himci_writel(reg_value, host->base + MCI_UHS_REG_EXT);
	} else {
		himci_trace(3, "Tuning finished!!");

		start_point = raise_point;
		end_point = fall_point;

		if (NOT_FOUND == raise_point)
			start_point = 0;
		if (NOT_FOUND == fall_point)
			end_point = HIMCI_PHASE_SCALE - 1;

		tmp = (start_point + end_point) / 2;
		if (end_point < start_point) {
			tmp = tmp - (HIMCI_PHASE_SCALE / 2);
			tmp = (tmp < 0) ? (HIMCI_PHASE_SCALE + tmp) : tmp;
		} else if (end_point - start_point == HIMCI_PHASE_SCALE - 1) {
			/* all of phase valid, it means tuning fail
			 * so we select a theoretical value */
			tmp = 3;
		}

		slot->phase_smp = tmp;
		spin_lock_irqsave(&host->lock, flags);
		reg_value = himci_readl(host->base + MCI_UHS_REG_EXT);
		reg_value &= ~CLK_SMPL_PHS_MASK;
		reg_value |= (tmp << CLK_SMPL_PHS_OFFSET);
		himci_writel(reg_value, host->base + MCI_UHS_REG_EXT);
		spin_unlock_irqrestore(&host->lock, flags);

		pr_info("%s: valid phase shift [%d, %d] Final Phase %d\n",
				mmc_hostname(mmc),
				start_point, end_point, tmp);
	}

	himci_writel(READ_THRESHOLD_SIZE, host->base + MCI_CARDTHRCTL);
	himci_writel(ALL_INT_CLR, host->base + MCI_RINTSTS);

	spin_lock(&host->lock);
	host->is_tuning = 0; /* jump out phase tuning process */
	spin_unlock(&host->lock);

	himci_release_host(host);

	return 0;
}

static void hi_mci_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct himci_slot *slot = mmc_priv(mmc);
	struct himci_host *host = slot->host;
	unsigned int tmp_reg;
	unsigned int port = slot->port;
	u32 ctrl = 0;

	himci_trace(2, "begin");
	himci_assert(mmc);
	himci_assert(ios);
	himci_assert(host);

	himci_claim_host(host, slot);
	himci_switch_card(slot);

	himci_trace(3, "ios->power_mode = %d ", ios->power_mode);
	if (!ios->clock)
		hi_mci_control_cclk(slot, DISABLE);

	switch (ios->power_mode) {
	case MMC_POWER_OFF:
		hi_mci_ctrl_power(slot, POWER_OFF, FORCE_DISABLE);
		break;
	case MMC_POWER_UP:
	case MMC_POWER_ON:
		hi_mci_ctrl_power(slot, POWER_ON, FORCE_DISABLE);
		break;
	}
	himci_trace(3, "ios->clock = %d ", ios->clock);
	if (ios->clock) {
		hi_mci_control_cclk(slot, DISABLE);
		hi_mci_set_cclk(slot, ios->clock);
		hi_mci_control_cclk(slot, ENABLE);

		/* speed mode check ,if it is DDR50 set DDR mode */
		if ((ios->timing == MMC_TIMING_UHS_DDR50)) {
			ctrl = himci_readl(host->base + MCI_UHS_REG);
			if (!((HI_SDXC_CTRL_DDR_REG << port) & ctrl)) {
				ctrl |= (HI_SDXC_CTRL_DDR_REG << port);
				himci_writel(ctrl, host->base + MCI_UHS_REG);
			}
		}
	} else {
		hi_mci_control_cclk(slot, DISABLE);
		if ((ios->timing != MMC_TIMING_UHS_DDR50)) {
			ctrl = himci_readl(host->base + MCI_UHS_REG);
			if ((HI_SDXC_CTRL_DDR_REG << port) & ctrl) {
				ctrl &= ~(HI_SDXC_CTRL_DDR_REG << port);
				himci_writel(ctrl, host->base + MCI_UHS_REG);
			}
		}
	}

	/* set bus_width */
	himci_trace(3, "ios->bus_width = %d ", ios->bus_width);

	/* clear bus width to 1bit first */
	tmp_reg = himci_readl(host->base + MCI_CTYPE);
	tmp_reg &= ~((CARD_WIDTH_0 | CARD_WIDTH_1) << port);

	if (ios->bus_width == MMC_BUS_WIDTH_8) {
		tmp_reg |= (CARD_WIDTH_0 << port);
		himci_writel(tmp_reg, host->base + MCI_CTYPE);
	} else if (ios->bus_width == MMC_BUS_WIDTH_4) {
		tmp_reg |= (CARD_WIDTH_1 << port);
		himci_writel(tmp_reg, host->base + MCI_CTYPE);
	} else {
		himci_writel(tmp_reg, host->base + MCI_CTYPE);
	}

	himci_release_host(host);
}

static int hi_mci_get_ro(struct mmc_host *mmc)
{
	unsigned ret;
	struct himci_slot *slot = mmc_priv(mmc);

	himci_trace(2, "begin");
	himci_assert(mmc);

	ret = hi_mci_ctrl_card_readonly(slot);

	return ret;
}

static void hi_mci_hw_reset(struct mmc_host *mmc)
{
	unsigned int reg_value;
	struct himci_slot *slot = mmc_priv(mmc);
	struct himci_host *host = slot->host;
	unsigned int port = slot->port;

	reg_value = himci_readl(host->base + MCI_RESET_N);
	reg_value &= ~(MMC_RST_N << port);
	himci_writel(reg_value, host->base + MCI_RESET_N);

	/* For eMMC, minimum is 1us but give it 10us for good measure */
	udelay(10);
	reg_value = himci_readl(host->base + MCI_RESET_N);
	reg_value |= (MMC_RST_N << port);
	himci_writel(reg_value, host->base + MCI_RESET_N);

	/* For eMMC, minimum is 200us but give it 300us for good measure */
	usleep_range(300, 1000);
}

static void hi_mci_enable_sdio_irq(struct mmc_host *mmc, int enable)
{
	struct himci_slot *slot = mmc_priv(mmc);
	struct himci_host *host = slot->host;
	unsigned int reg_value;

	himci_switch_card(slot);
	reg_value = readl(host->base + MCI_INTMASK);
	if (enable)
		reg_value |= SDIO_INT_MASK;
	else
		reg_value &= ~SDIO_INT_MASK;
	writel(reg_value, host->base + MCI_INTMASK);
}

static int hi_mci_get_card_detect(struct mmc_host *mmc)
{
	unsigned ret;
	struct himci_slot *slot = mmc_priv(mmc);

	himci_trace(2, "begin");
	ret = hi_mci_sys_card_detect(slot);

	if (ret)
		return 0;
	else
		return 1;
}

static const struct mmc_host_ops hi_mci_ops = {
	.request = hi_mci_request,
	.set_ios = hi_mci_set_ios,
	.get_ro = hi_mci_get_ro,
	.start_signal_voltage_switch = hi_mci_start_signal_voltage_switch,
	.execute_tuning	= hi_mci_execute_tuning,
	.enable_sdio_irq = hi_mci_enable_sdio_irq,
	.hw_reset = hi_mci_hw_reset,
	.get_cd = hi_mci_get_card_detect,
};

static irqreturn_t hisd_irq(int irq, void *dev_id)
{
	struct himci_host *host = dev_id;
	struct himci_slot *slot = host->slot;
	u32 state = 0;
	u32 mstate = 0;
	int handle = 0;

	spin_lock(&host->lock);
	state = himci_readl(host->base + MCI_RINTSTS);
	spin_unlock(&host->lock);

	/* bugfix: when send soft stop to SD Card, Host will report
	   sdio interrupt, This situation needs to be avoided */
	if (host->mmc->caps & MMC_CAP_SDIO_IRQ) {
		if ((slot->mmc->card != NULL)
				&& (slot->mmc->card->type == MMC_TYPE_SDIO)) {
			mstate = himci_readl(host->base + MCI_INTMASK);
			if ((state & SDIO_INT_STATUS) &&
					(mstate & SDIO_INT_MASK)) {
				spin_lock(&host->lock);
				himci_writel(SDIO_INT_STATUS,
						host->base + MCI_RINTSTS);
				spin_unlock(&host->lock);
				handle = 1;
				mmc_signal_sdio_irq(slot->mmc);
			}
		}
	}

	if (state & DATA_INT_MASK) {
		handle = 1;
		host->pending_events |= HIMCI_PEND_DTO_M;

		spin_lock(&host->lock);
		host->irq_status = himci_readl(host->base + MCI_RINTSTS);
		himci_writel(DATA_INT_MASK , host->base + MCI_RINTSTS);
		/* Mask MMC host data intr */
		state = himci_readl(host->base + MCI_INTMASK);
		state &= ~DATA_INT_MASK;
		himci_writel(state, host->base + MCI_INTMASK);
		spin_unlock(&host->lock);

		wake_up(&host->intr_wait);
	}

	if (handle)
		return IRQ_HANDLED;

	return IRQ_NONE;
}

static int __init hi_mci_probe(struct platform_device *pdev)
{
	struct mmc_host *mmc;
	struct himci_host *host = NULL;
	struct himci_slot *slot = NULL;
	struct resource *host_ioaddr_res = NULL;
	struct resource *host_crg_res = NULL;
	int ret = 0, irq;

	himci_trace(2, "begin");
	himci_trace(3, "mmc host probe %d", pdev->id);
	himci_assert(pdev);

	if ((pdev->id == 0) || (pdev->id == 1))
		host = host0;
	else if (pdev->id == 2)
		host = host1;

	mmc = mmc_alloc_host(sizeof(struct himci_slot), &pdev->dev);
	if (!mmc) {
		himci_error("no mem for hi mci host controller!\n");
		ret = -ENOMEM;
		goto out;
	}
	slot = mmc_priv(mmc);
	slot->mmc = mmc;
	slot->host = host;

	himci_claim_host(host, slot);
	if (!host->used) {
		host_ioaddr_res = platform_get_resource(pdev,
				IORESOURCE_MEM, 0);
		if (NULL == host_ioaddr_res) {
			himci_error("no ioaddr rescources config!\n");
			ret = -ENODEV;
			goto out;
		}

		host->base = ioremap_nocache(host_ioaddr_res->start,
				HI_MCI_IO_SIZE);
		if (!host->base) {
			himci_error("no mem for himci base!\n");
			ret = -ENOMEM;
			goto out;
		}

		host->dma_vaddr = dma_alloc_coherent(&pdev->dev, PAGE_SIZE,
				&host->dma_paddr, GFP_KERNEL);
		if (!host->dma_vaddr) {
			himci_error("no mem for himci dma!\n");
			ret = -ENOMEM;
			goto out;
		}

		host_crg_res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
		if (NULL == host_crg_res) {
			himci_error("%s%d:no crg rescources config!\n",
					pdev->name, pdev->id);
			ret = -ENODEV;
			goto out;
		}
		host->crg_phy_addr = host_crg_res->start;

		/* enable mmc clk */
		hi_mci_sys_ctrl_init(host, host->crg_phy_addr, pdev->id);

		/* enable card */
		hi_mci_init_host(host);

		spin_lock_init(&host->lock);
		init_waitqueue_head(&host->intr_wait);
		irq = platform_get_irq(pdev, 0);
		if (irq < 0) {
			pr_err("no IRQ defined!\n");
			goto out;
		}

		host->irq = irq;
		ret = request_irq(irq, hisd_irq, 0, DRIVER_NAME, host);
		if (ret) {
			pr_err("request_irq error!\n");
			goto out;
		}
	} else {
		slot->hclk = host->hclk;
		slot->cclk = 0;

	}
	host->used++;
	himci_release_host(host);

	slot->phase_drv = DRV_PHASE_SHIFT;
	slot->phase_smp = SMPL_PHASE_SHIFT;
	mmc->ops = &hi_mci_ops;

	if ((pdev->id == 0) || (pdev->id == 2))
		slot->port = 0;
	else
		slot->port = 1;

	if ((pdev->id == 0) || (pdev->id == 1)) {
		mmc->f_min = CONFIG_HIMCIV200_SDIO0_CLK_MIN;
		mmc->f_max = CONFIG_HIMCIV200_SDIO0_CLK_MAX;
	} else {
		mmc->f_min = CONFIG_HIMCIV200_SDIO1_CLK_MIN;
		mmc->f_max = CONFIG_HIMCIV200_SDIO1_CLK_MAX;
	}

	mmc->caps |= MMC_CAP_4_BIT_DATA
			| MMC_CAP_SD_HIGHSPEED
			| MMC_CAP_MMC_HIGHSPEED
			| MMC_CAP_UHS_SDR12
		/*	| MMC_CAP_SDIO_IRQ	*/
			| MMC_CAP_UHS_SDR25
			| MMC_CAP_UHS_SDR50
		/*	| MMC_CAP_UHS_DDR50	*/ /* FIXME */
			| MMC_CAP_UHS_SDR104
			;

	if (pdev->id == 1) { /* port1-->eMMC */
		mmc->caps |= MMC_CAP_8_BIT_DATA | MMC_CAP_HW_RESET;
#ifdef EMMC_DDR50
		mmc->caps |= MMC_CAP_UHS_DDR50;
		/*		mmc->caps |= MMC_CAP_1_8V_DDR; */
#endif

#ifdef EMMC_HS200
		/* this caps will enable MMC_CAP2_HS200_1_8V_SDR
		 * | MMC_CAP2_HS200_1_2V_SDR
		 * so it will execte voltage switch
		 */
		mmc->caps2 |= MMC_CAP2_HS200;
#endif
	}

	/* reload by this controller */
	mmc->max_blk_count = 2048;
	mmc->max_segs = 1024;
	mmc->max_seg_size = mmc->max_blk_size * mmc->max_blk_count;
	mmc->max_req_size = mmc->max_blk_size * mmc->max_blk_count;
	mmc->ocr_avail = MMC_VDD_32_33 | MMC_VDD_33_34;
	mmc->ocr = mmc->ocr_avail;

	himci_trace(3, "Max Block Size: %u\n", mmc->max_blk_size);
	himci_trace(3, "Max Block Count: %u\n", mmc->max_blk_count);
	himci_trace(3, "Max Block Queue Segments: %u\n", mmc->max_segs);
	himci_trace(3, "Max Block Queue Segment Size: %u\n", mmc->max_seg_size);
	himci_trace(3, "Max Bytes in One Request: %u\n", mmc->max_req_size);
	himci_trace(3, "\n");

	slot->card_status = hi_mci_sys_card_detect(slot);
	slot->power_status = POWER_OFF;

	himci_init_card(slot);
	init_timer(&slot->timer);
	slot->timer.function = hi_mci_detect_card;
	slot->timer.data = (unsigned long)slot;
	slot->timer.expires = jiffies + detect_time;

	platform_set_drvdata(pdev, mmc);
	add_timer(&slot->timer);
	mmc_add_host(mmc);

	/* only eMMC support cmd line */
	if (pdev->id == 1) {
		himci_dbg_init(host->devid);
		himci_acl_init(host->devid);
	}

	return 0;
out:
	if (mmc) {
		del_timer(&slot->timer);
		mmc_free_host(mmc);
	}

	if (host) {
		if (host->base)
			iounmap(host->base);
		if (host->dma_vaddr)
			dma_free_coherent(&pdev->dev, PAGE_SIZE,
					host->dma_vaddr, host->dma_paddr);
		kfree(host);
	}

	return ret;
}

static int __exit hi_mci_remove(struct platform_device *pdev)
{
	struct mmc_host *mmc = platform_get_drvdata(pdev);

	himci_trace(2, "begin");
	himci_assert(pdev);

	platform_set_drvdata(pdev, NULL);

	if (mmc) {
		struct himci_slot *slot = mmc_priv(mmc);
		struct himci_host *host = slot->host;

		free_irq(host->irq, host);
		del_timer_sync(&slot->timer);
		mmc_remove_host(mmc);
		hi_mci_ctrl_power(slot, POWER_OFF, FORCE_DISABLE);
		hi_mci_control_cclk(slot, DISABLE);

		host->used--;
		if (!host->used) {
			iounmap(host->base);
			dma_free_coherent(&pdev->dev, PAGE_SIZE,
					host->dma_vaddr, host->dma_paddr);
		}
		mmc_free_host(mmc);
	}
	return 0;
}

static void hi_mci_shutdown(struct platform_device *pdev)
{
	struct mmc_host *mmc = platform_get_drvdata(pdev);

	himci_trace(3, "shutdown");
	if (mmc) {
		unsigned int val;
		struct himci_slot *slot = mmc_priv(mmc);
		struct himci_host *host = slot->host;

		/* bugfix: host reset can trigger error intr */
		himci_writel(0, host->base + MCI_IDINTEN);
		himci_writel(0, host->base + MCI_INTMASK);
		val = himci_readl(host->base + MCI_CTRL);
		val |= CTRL_RESET | FIFO_RESET | DMA_RESET;
		himci_writel(val, host->base + MCI_CTRL);

		hi_mci_ctrl_power(slot, POWER_OFF, FORCE_DISABLE);
	}
}

#ifdef CONFIG_PM
static int hi_mci_suspend(struct platform_device *dev, pm_message_t state)
{
	struct mmc_host *mmc = platform_get_drvdata(dev);
	struct himci_slot *slot;
	struct himci_host *host;
	int ret = 0;

	himci_trace(2, "begin");
	himci_assert(dev);

	if (mmc) {
		ret = mmc_suspend_host(mmc);

		slot = mmc_priv(mmc);
		host = slot->host;

		del_timer_sync(&slot->timer);

		himci_claim_host(host, slot);
		host->used--;
		if (!host->used)
			hi_mci_sys_ctrl_suspend(host, host->crg_phy_addr);

		himci_release_host(host);
	}

	himci_trace(2, "end");

	return ret;
}

static int hi_mci_resume(struct platform_device *dev)
{
	struct mmc_host *mmc = platform_get_drvdata(dev);
	struct himci_slot *slot;
	struct himci_host *host;
	int ret = 0;

	himci_trace(2, "begin");
	himci_assert(dev);

	if (mmc) {
		slot = mmc_priv(mmc);
		host = slot->host;

		himci_claim_host(host, slot);
		if (!host->used) {
			/* enable mmc clk */
			hi_mci_sys_ctrl_init(host, host->crg_phy_addr,
					host->devid);
			/* init host */
			hi_mci_init_host(host);
		}
		host->used++;
		himci_release_host(host);

		/* init card */
		himci_init_card(slot);

		ret = mmc_resume_host(mmc);
		add_timer(&slot->timer);
	}

	himci_trace(2, "end");

	return ret;
}
#else
#define hi_mci_suspend	NULL
#define hi_mci_resume	NULL
#endif

#ifdef CONFIG_HIMCIV200_SDIO0
static struct resource hi_mci_slot0_resources[] = {
	[0] = {
		.start          = CONFIG_HIMCIV200_SDIO0_IOBASE,
		.end            = CONFIG_HIMCIV200_SDIO0_IOBASE
					+ HI_MCI_IO_SIZE - 1,
		.flags          = IORESOURCE_MEM,
	},
	[1] = {
		.start          = SDIO_REG_BASE_CRG + PERI_CRG49,
		.end            = SDIO_REG_BASE_CRG + PERI_CRG49,
		.flags          = IORESOURCE_MEM,
	},
	[2] = {
		.start          = CONFIG_HIMCIV200_SDIO0_INTR,
		.end            = CONFIG_HIMCIV200_SDIO0_INTR,
		.flags          = IORESOURCE_IRQ,
	},
};

static struct resource hi_mci_slot1_resources[] = {
	[0] = {
		.start          = CONFIG_HIMCIV200_SDIO0_IOBASE,
		.end            = CONFIG_HIMCIV200_SDIO0_IOBASE
					+ HI_MCI_IO_SIZE - 1,
		.flags          = IORESOURCE_MEM,
	},
	[1] = {
		.start          = SDIO_REG_BASE_CRG + PERI_CRG49,
		.end            = SDIO_REG_BASE_CRG + PERI_CRG49,
		.flags          = IORESOURCE_MEM,
	},
	[2] = {
		.start          = CONFIG_HIMCIV200_SDIO0_INTR,
		.end            = CONFIG_HIMCIV200_SDIO0_INTR,
		.flags          = IORESOURCE_IRQ,
	},
};
#endif

#ifdef CONFIG_HIMCIV200_SDIO1
static struct resource hi_mci_slot2_resources[] = {
	[0] = {
		.start          = CONFIG_HIMCIV200_SDIO1_IOBASE,
		.end            = CONFIG_HIMCIV200_SDIO1_IOBASE
					+ HI_MCI_IO_SIZE - 1,
		.flags          = IORESOURCE_MEM,
	},
	[1] = {
		.start          = SDIO_REG_BASE_CRG + PERI_CRG49,
		.end            = SDIO_REG_BASE_CRG + PERI_CRG49,
		.flags          = IORESOURCE_MEM,
	},
	[2] = {
		.start          = CONFIG_HIMCIV200_SDIO1_INTR,
		.end            = CONFIG_HIMCIV200_SDIO1_INTR,
		.flags          = IORESOURCE_IRQ,
	},
};
#endif

static void hi_mci_platdev_release(struct device *dev)
{
}

static u64 himmc_dmamask = DMA_BIT_MASK(32);

#ifdef CONFIG_HIMCIV200_SDIO0
static struct platform_device hi_mci_sdio0_slot0 = {
	.name = DRIVER_NAME,
	.id = 0,
	.dev = {
		.release = hi_mci_platdev_release,
		.dma_mask = &himmc_dmamask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
		},
	.num_resources = ARRAY_SIZE(hi_mci_slot0_resources),
	.resource = hi_mci_slot0_resources,
};

static struct platform_device hi_mci_sdio0_slot1 = {
	.name = DRIVER_NAME,
	.id = 1,
	.dev = {
		.release = hi_mci_platdev_release,
		.dma_mask = &himmc_dmamask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
		},
	.num_resources = ARRAY_SIZE(hi_mci_slot1_resources),
	.resource = hi_mci_slot1_resources,
};
#endif

#ifdef CONFIG_HIMCIV200_SDIO1
static struct platform_device hi_mci_sdio1_slot0 = {
	.name = DRIVER_NAME,
	.id = 2,
	.dev = {
		.release = hi_mci_platdev_release,
		.dma_mask = &himmc_dmamask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
		},
	.num_resources = ARRAY_SIZE(hi_mci_slot2_resources),
	.resource = hi_mci_slot2_resources,
};
#endif

static struct platform_driver hi_mci_driver = {
	.probe = hi_mci_probe,
	.remove = hi_mci_remove,
	.shutdown = hi_mci_shutdown,
	.suspend = hi_mci_suspend,
	.resume = hi_mci_resume,
	.driver = {
		   .name = DRIVER_NAME,
		   },
};

struct platform_device *get_mci_device(unsigned int index)
{
	if ((index < 0) || (index >= HIMCI_SLOT_NUM))
		goto out;

#ifdef CONFIG_HIMCIV200_SDIO0
	if (0 == index)
		return &hi_mci_sdio0_slot0;
	if (1 == index)
		return &hi_mci_sdio0_slot1;
#else
	goto out;
#endif

	if (2 == index)
#ifdef CONFIG_HIMCIV200_SDIO1
		return &hi_mci_sdio1_slot0;
#else
	goto out;
#endif

out:
	return NULL;
}

static int __init hi_mci_init(void)
{
	int ret;

	himci_trace(2, "begin");

	/*
	 * We should register SDIO1 first to make sure that
	 * the eMMC device,which connected to SDIO1 is mmcblk0.
	 */
#ifdef CONFIG_HIMCIV200_SDIO0 /* support sdxc and eMMC */
	host0 = kzalloc(sizeof(struct himci_host), GFP_KERNEL);
	if (!host0)
		return 0;

	host0->devid = 0;

	ret = platform_device_register(&hi_mci_sdio0_slot1);
	if (ret) {
		himci_error("sdio0:slot1 device register failed!");
		return ret;
	}

	ret = platform_device_register(&hi_mci_sdio0_slot0);
	if (ret) {
		himci_error("sdio0:slot0 device register failed!");
		return ret;
	}
#endif

#ifdef CONFIG_HIMCIV200_SDIO1
	host1 = kzalloc(sizeof(struct himci_host), GFP_KERNEL);
	if (!host1)
		return 0;

	host1->devid = 1;

	ret = platform_device_register(&hi_mci_sdio1_slot0);
	if (ret) {
		himci_error("sdio1 device register failed!");
		return ret;
	}

#endif
	ret = platform_driver_register(&hi_mci_driver);
	if (ret) {
		himci_error("Himci driver register failed!");
		return ret;
	}

	/* device proc entry */
	ret = mci_proc_init(HIMCI_SLOT_NUM);
	if (ret)
		himci_error("device proc init is failed!");
	return ret;
}

static void __exit hi_mci_exit(void)
{
	himci_trace(2, "begin");

	mci_proc_shutdown();

	platform_driver_unregister(&hi_mci_driver);
#ifdef CONFIG_HIMCIV200_SDIO1
	platform_device_unregister(&hi_mci_sdio1_slot0);
	kfree(host1);
	host1 = NULL;
#endif

#ifdef CONFIG_HIMCIV200_SDIO0
	platform_device_unregister(&hi_mci_sdio0_slot0);
	platform_device_unregister(&hi_mci_sdio0_slot1);
	kfree(host0);
	host1 = NULL;
#endif
}

module_init(hi_mci_init);
module_exit(hi_mci_exit);

#ifdef MODULE
MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("MMC/SD driver for the Hisilicon MMC/SD Host Controller");
MODULE_LICENSE("GPL");
#endif
