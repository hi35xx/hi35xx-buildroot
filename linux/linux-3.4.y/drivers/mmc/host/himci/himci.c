/*
 * himci.c - hisilicon MMC Host driver
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/scatterlist.h>
#include <linux/regulator/consumer.h>
#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <linux/mmc/card.h>
#include <linux/slab.h>

#include <linux/ioport.h>
#include <linux/device.h>

#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/freezer.h>
#include <asm/dma.h>
#include <linux/io.h>
#include <asm/irq.h>
#include <asm/sizes.h>
#include <linux/uaccess.h>
#include <mach/hardware.h>

#include "hi_mci_reg.h"
#include "hi_mci.h"
#include "himci_proc.h"

#ifdef CONFIG_ARCH_HI3516A
#include "himci_hi3516a.c"
#endif

#define DRIVER_NAME	"hi_mci"

static unsigned int detect_time = HI_MCI_DETECT_TIMEOUT;
static unsigned int retry_count = MAX_RETRY_COUNT;
static unsigned int request_timeout = HI_MCI_REQUEST_TIMEOUT;
int trace_level = HIMCI_TRACE_LEVEL;

static const u8 tuning_blk_pattern[] = {
	0xff, 0x0f, 0xff, 0x00, 0xff, 0xcc, 0xc3, 0xcc,
	0xc3, 0x3c, 0xcc, 0xff, 0xfe, 0xff, 0xfe, 0xef,
	0xff, 0xdf, 0xff, 0xdd, 0xff, 0xfb, 0xff, 0xfb,
	0xbf, 0xff, 0x7f, 0xff, 0x77, 0xf7, 0xbd, 0xef,
	0xff, 0xf0, 0xff, 0xf0, 0x0f, 0xfc, 0xcc, 0x3c,
	0xcc, 0x33, 0xcc, 0xcf, 0xff, 0xef, 0xff, 0xee,
	0xff, 0xfd, 0xff, 0xfd, 0xdf, 0xff, 0xbf, 0xff,
	0xbb, 0xff, 0xf7, 0xff, 0xf7, 0x7f, 0x7b, 0xde,
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

/* reset MMC host controler */
static void hi_mci_sys_reset(struct himci_host *host)
{
	unsigned int reg_value;
	unsigned long flags;

	himci_trace(2, "reset");

	local_irq_save(flags);

	hi_mci_ctr_reset(host);
	udelay(50);

	hi_mci_ctr_undo_reset(host);

	reg_value = himci_readl(host->base + MCI_BMOD);
	reg_value |= BMOD_SWR;
	himci_writel(reg_value, host->base + MCI_BMOD);
	udelay(50);

	reg_value = himci_readl(host->base + MCI_BMOD);
	reg_value |= BURST_INCR;
	himci_writel(reg_value, host->base + MCI_BMOD);

	reg_value = himci_readl(host->base + MCI_CTRL);
	reg_value |=  CTRL_RESET | FIFO_RESET | DMA_RESET;
	himci_writel(reg_value, host->base + MCI_CTRL);

	local_irq_restore(flags);
}

static void hi_mci_ctrl_power(struct himci_host *host, unsigned int flag)
{
	himci_trace(2, "begin");

	himci_writel(flag, host->base + MCI_PWREN);
}

/**********************************************
 *1: card off
 *0: card on
 ***********************************************/
static unsigned int hi_mci_sys_card_detect(struct himci_host *host)
{
	unsigned int card_status;

	card_status = readl(host->base + MCI_CDETECT);

	return card_status & HIMCI_CARD0;
}

/**********************************************
 *1: card readonly
 *0: card read/write
 ***********************************************/
static unsigned int hi_mci_ctrl_card_readonly(struct himci_host *host)
{
	unsigned int card_value = himci_readl(host->base + MCI_WRTPRT);
	return card_value & HIMCI_CARD0;
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

			himci_trace(3, "Other CMD is running," \
					"please operate cmd again!");
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

static void hi_mci_control_cclk(struct himci_host *host, unsigned int flag)
{
	unsigned int reg;
	union cmd_arg_s cmd_reg;

	himci_trace(2, "begin");
	himci_assert(host);

	reg = himci_readl(host->base + MCI_CLKENA);
	if (flag == ENABLE)
		reg |= CCLK_ENABLE;
	else
		reg &= 0xffff0000;
	himci_writel(reg, host->base + MCI_CLKENA);

	cmd_reg.cmd_arg = himci_readl(host->base + MCI_CMD);
	cmd_reg.bits.start_cmd = 1;
	cmd_reg.bits.update_clk_reg_only = 1;
	himci_writel(cmd_reg.cmd_arg, host->base + MCI_CMD);
	if (hi_mci_wait_cmd(host) != 0)
		himci_trace(3, "disable or enable clk is timeout!");
}

static void hi_mci_set_cclk(struct himci_host *host, unsigned int cclk)
{
	unsigned int reg_value;
	union cmd_arg_s clk_cmd;

	himci_trace(2, "begin");
	himci_assert(host);
	himci_assert(cclk);

	/*
	 * set card clk divider value,
	 * clk_divider = Fmmcclk/(Fmmc_cclk * 2)
	 */

	if (0 == host->id) {
#ifdef CONFIG_HIMCI0
		if (CONFIG_MMC0_CLK <= cclk)
			reg_value = 0;
		else {
			reg_value = CONFIG_MMC0_CLK / (cclk * 2);
			if (CONFIG_MMC0_CLK % (cclk * 2))
				reg_value++;
		}
#endif
	} else if (1 == host->id) {
#ifdef CONFIG_HIMCI1
		if (CONFIG_MMC1_CLK <= cclk)
			reg_value = 0;
		else {
			reg_value = CONFIG_MMC1_CLK / (cclk * 2);
			if (CONFIG_MMC1_CLK % (cclk * 2))
				reg_value++;
		}
#endif
	} else {
		himci_error("himci host id error!");
		return;
	}

	himci_writel(reg_value, host->base + MCI_CLKDIV);

	clk_cmd.cmd_arg = himci_readl(host->base + MCI_CMD);
	clk_cmd.bits.start_cmd = 1;
	clk_cmd.bits.update_clk_reg_only = 1;
	himci_writel(clk_cmd.cmd_arg, host->base + MCI_CMD);

	if (hi_mci_wait_cmd(host) != 0)
		himci_trace(3, "set card clk divider is failed!");
}

static void hi_mci_init_card(struct himci_host *host)
{
	unsigned int tmp_reg;
	unsigned long flags;

	himci_trace(2, "begin");
	himci_assert(host);

	hi_mci_sys_reset(host);

	hi_mci_ctrl_power(host, POWER_OFF);
	udelay(500);
	/* card power on */
	hi_mci_ctrl_power(host, POWER_ON);
	udelay(200);

	/* set phase shift */
	himci_writel(PHASE_SHIFT, host->base + MCI_UHS_EXT);
	/* set card read threshold */
	himci_writel(READ_THRESHOLD_SIZE, host->base + MCI_READ_THRESHOLD_SIZE);

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

static void hi_mci_detect_card(unsigned long arg)
{
	struct himci_host *host = (struct himci_host *)arg;
	unsigned int i, curr_status, status[5], detect_retry_count = 0;

	himci_assert(host);

	while (1) {
		for (i = 0; i < 5; i++) {
			status[i] = hi_mci_sys_card_detect(host);
			udelay(10);
		}
		if ((status[0] == status[1])
				&& (status[0] == status[2])
				&& (status[0] == status[3])
				&& (status[0] == status[4]))
			break;

		detect_retry_count++;
		if (detect_retry_count >= retry_count) {
			himci_error("this is a dithering, card detect error!");
			goto err;
		}
	}
	curr_status = status[0];
	if (curr_status != host->card_status) {
		himci_trace(2, "begin card_status = %d\n", host->card_status);
		host->card_status = curr_status;
		if (curr_status != CARD_UNPLUGED) {
			hi_mci_init_card(host);
			printk(KERN_INFO "card connected!\n");
		} else
			printk(KERN_INFO "card disconnected!\n");

		mmc_detect_change(host->mmc, 0);
	}
err:
	mod_timer(&host->timer, jiffies + detect_time);
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
	host->dma_sg_num = dma_map_sg(mmc_dev(host->mmc),
			data->sg, data->sg_len, host->dma_dir);
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

	max_des = (PAGE_SIZE/sizeof(struct himci_des));
	des = (struct himci_des *)host->dma_vaddr;
	des_cnt = 0;

	for (i = 0; i < host->dma_sg_num; i++) {
		sg_length = sg_dma_len(&data->sg[i]);
		sg_phyaddr = sg_dma_address(&data->sg[i]);
		himci_trace(2, "sg[%d] sg_length is 0x%08X, " \
				"sg_phyaddr is 0x%08X\n", \
				i, (unsigned int)sg_length, \
				(unsigned int)sg_phyaddr);
		while (sg_length) {
			des[des_cnt].idmac_des_ctrl = DMA_DES_OWN
				| DMA_DES_NEXT_DES;
			des[des_cnt].idmac_des_buf_addr = sg_phyaddr;
			/* idmac_des_next_addr is paddr for dma */
			des[des_cnt].idmac_des_next_addr = host->dma_paddr
				+ (des_cnt + 1) * sizeof(struct himci_des);

			if (sg_length >= 0x1F00) {
				des[des_cnt].idmac_des_buf_size = 0x1F00;
				sg_length -= 0x1F00;
				sg_phyaddr += 0x1F00;
			} else {
				/* FIXME:data alignment */
				des[des_cnt].idmac_des_buf_size = sg_length;
				sg_length = 0;
			}

			himci_trace(2, "des[%d] vaddr  is 0x%08X", i,
					(unsigned int)&des[i]);
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

static int hi_mci_exec_cmd(struct himci_host *host, struct mmc_command *cmd,
		struct mmc_data *data)
{
	volatile union cmd_arg_s  cmd_regs;

	himci_trace(2, "begin");
	himci_assert(host);
	himci_assert(cmd);

	host->cmd = cmd;

	himci_writel(cmd->arg, host->base + MCI_CMDARG);
	himci_trace(4, "arg_reg 0x%x, val 0x%x", MCI_CMDARG, cmd->arg);
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
		himci_error("hi_mci: unhandled response type %02x\n",
				mmc_resp_type(cmd));
		return -EINVAL;
	}

	himci_trace(3, "send cmd of card is cmd->opcode = %d ", cmd->opcode);
	if (cmd->opcode == MMC_GO_IDLE_STATE)
		cmd_regs.bits.send_initialization = 1;
	else
		cmd_regs.bits.send_initialization = 0;
	/* CMD 11 check switch voltage */
	if (cmd->opcode == SD_SWITCH_VOLTAGE)
		cmd_regs.bits.volt_switch = 1;
	else
		cmd_regs.bits.volt_switch = 0;

	cmd_regs.bits.card_number = 0;
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

static void hi_mci_finish_request(struct himci_host *host,
		struct mmc_request *mrq)
{
	himci_trace(2, "begin");
	himci_assert(host);
	himci_assert(mrq);

	host->mrq = NULL;
	host->cmd = NULL;
	host->data = NULL;
	mmc_request_done(host->mmc, mrq);
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


	dma_unmap_sg(mmc_dev(host->mmc), data->sg, data->sg_len, host->dma_dir);

	if (stat & (HTO_INT_STATUS | DRTO_INT_STATUS)) {
		data->error = -ETIMEDOUT;
		himci_trace(3, "irq data status stat = 0x%x is timeout error!",
				stat);
	} else if (stat & (EBE_INT_STATUS | SBE_INT_STATUS | FRUN_INT_STATUS
				| DCRC_INT_STATUS)) {
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
				himci_writel(VOLT_SWITCH_INT_STATUS, \
						host->base + MCI_RINTSTS);
				spin_unlock_irqrestore(&host->lock, flags);
				hi_mci_cmd_done(host, cmd_irq_reg);
				return 0;
			}
			spin_unlock_irqrestore(&host->lock, flags);
			cmd_retry_count++;
		} while (cmd_retry_count < retry_count);

		cmd_retry_count = 0;

		if (host->card_status == CARD_UNPLUGED) {
			cmd->error = -ETIMEDOUT;
			return -1;
		}

		if (!time_before(jiffies, cmd_jiffies_timeout)) {
			unsigned int i = 0;
			for (i = 0; i < 4; i++) {
				cmd->resp[i] = himci_readl(host->base \
						+ MCI_RESP0 + i * 0x4);
				printk(KERN_ERR "voltage switch read MCI_RESP");
				printk(KERN_ERR "%d : 0x%x\n", i, cmd->resp[i]);
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
		if (host->card_status == CARD_UNPLUGED) {
			return -1;
		}
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
	unsigned int tmp_reg;
	struct mmc_data *data = host->data;
	long time = request_timeout;
	unsigned long flags;

	himci_trace(2, "begin");
	himci_assert(host);
	himci_assert(data);

	time = wait_event_timeout(host->intr_wait,
		test_bit(HIMCI_PEND_DTO_b, &host->pending_events),
		time);

	/* Mask MMC host data intr */
	spin_lock_irqsave(&host->lock, flags);
	tmp_reg = himci_readl(host->base + MCI_INTMASK);
	tmp_reg &= ~DATA_INT_MASK;
	himci_writel(tmp_reg, host->base + MCI_INTMASK);
	host->pending_events &= ~HIMCI_PEND_DTO_m;
	spin_unlock_irqrestore(&host->lock, flags);

	if (((time <= 0)
		&& (!test_bit(HIMCI_PEND_DTO_b, &host->pending_events)))
		|| (host->card_status == CARD_UNPLUGED)) {

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

		if (host->card_status == CARD_UNPLUGED) {
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
	struct himci_host *host = mmc_priv(mmc);
	int byte_cnt = 0, trans_cnt;
	int fifo_count = 0, tmp_reg;
	int ret = 0;
	unsigned long flags;

	himci_trace(2, "begin");
	himci_assert(mmc);
	himci_assert(mrq);
	himci_assert(host);

	host->mrq = mrq;
	host->irq_status = 0;


	if (host->card_status == CARD_UNPLUGED) {
		mrq->cmd->error = -ENODEV;
		goto  request_end;
	}
#if 1
	ret = hi_mci_wait_card_complete(host, mrq->data);

	if (ret) {
		mrq->cmd->error = ret;
		goto request_end;
	}
#endif
	/* prepare data */
	if (mrq->data) {
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
				printk(KERN_INFO "fifo reset is timeout!");
				return;
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
	}

request_end:
	/* clear MMC host intr */
	spin_lock_irqsave(&host->lock, flags);
	himci_writel(ALL_SD_INT_CLR, host->base + MCI_RINTSTS);
	spin_unlock_irqrestore(&host->lock, flags);

	hi_mci_finish_request(host, mrq);
}

static int hi_mci_do_start_signal_voltage_switch(struct himci_host *host,
						struct mmc_ios *ios)
{
	u32 ctrl;

	/*
	 * We first check whether the request is to set signalling voltage
	 * to 3.3V. If so, we change the voltage to 3.3V and return quickly.
	 */
	ctrl = himci_readl(host->base + MCI_UHS_REG);
	if (ios->signal_voltage == MMC_SIGNAL_VOLTAGE_330) {
		/* Set 1.8V Signal Enable in the MCI_UHS_REG to 1 */
		ctrl &= ~HI_SDXC_CTRL_VDD_180;
		himci_writel(ctrl, host->base + MCI_UHS_REG);

		/* Wait for 5ms */
		usleep_range(5000, 5500);

		/* 3.3V regulator output should be stable within 5 ms */
		ctrl = himci_readl(host->base + MCI_UHS_REG);
		if (!(ctrl & HI_SDXC_CTRL_VDD_180)) {
			/* config Pin drive capability */
			if (0 == host->id) {
				himci_writel(SDIO_CCLK_DS_3V3,
						REG_CTRL_SDIO0_CCLK);
				himci_writel(SDIO_CCMD_DS_3V3,
						REG_CTRL_SDIO0_CCMD);
				himci_writel(SDIO_CDATA0_DS_3V3,
						REG_CTRL_SDIO0_CDATA0);
				himci_writel(SDIO_CDATA1_DS_3V3,
						REG_CTRL_SDIO0_CDATA1);
				himci_writel(SDIO_CDATA2_DS_3V3,
						REG_CTRL_SDIO0_CDATA2);
				himci_writel(SDIO_CDATA3_DS_3V3,
						REG_CTRL_SDIO0_CDATA3);
			} else if (1 == host->id) {
				himci_writel(SDIO_CCLK_DS_3V3,
						REG_CTRL_SDIO1_CCLK);
				himci_writel(SDIO_CCMD_DS_3V3,
						REG_CTRL_SDIO1_CCMD);
				himci_writel(SDIO_CDATA0_DS_3V3,
						REG_CTRL_SDIO1_CDATA0);
				himci_writel(SDIO_CDATA1_DS_3V3,
						REG_CTRL_SDIO1_CDATA1);
				himci_writel(SDIO_CDATA2_DS_3V3,
						REG_CTRL_SDIO1_CDATA2);
				himci_writel(SDIO_CDATA3_DS_3V3,
						REG_CTRL_SDIO1_CDATA3);
			}
			return 0;
		} else {
			pr_info(DRIVER_NAME ": Switching to 3.3V " \
				"signalling voltage failed\n");
			return -EIO;
		}
	} else if (!(ctrl & HI_SDXC_CTRL_VDD_180) &&
		  (ios->signal_voltage == MMC_SIGNAL_VOLTAGE_180)) {
		/* Stop SDCLK */
		hi_mci_control_cclk(host, DISABLE);


		/*
		 * Enable 1.8V Signal Enable in the MCI_UHS_REG
		 */
		ctrl |= HI_SDXC_CTRL_VDD_180;
		himci_writel(ctrl, host->base + MCI_UHS_REG);

		/* Wait for 5ms */
		usleep_range(8000, 8500);

		ctrl = himci_readl(host->base + MCI_UHS_REG);
		if (ctrl & HI_SDXC_CTRL_VDD_180) {
			/* Provide SDCLK again and wait for 1ms */
			hi_mci_control_cclk(host, ENABLE);
			usleep_range(1000, 1500);

			/*
			 * If CMD11 return CMD down, then the card
			 * was successfully switched to 1.8V signaling.
			 */
			ctrl = himci_readl(host->base + MCI_RINTSTS);
			if ((ctrl & VOLT_SWITCH_INT_STATUS)
					&& (ctrl & CD_INT_STATUS)) {
				/* config Pin drive capability */
				if (0 == host->id) {
					himci_writel(SDIO_CCLK_DS_1V8,
							REG_CTRL_SDIO0_CCLK);
					himci_writel(SDIO_CCMD_DS_1V8,
							REG_CTRL_SDIO0_CCMD);
					himci_writel(SDIO_CDATA0_DS_1V8,
							REG_CTRL_SDIO0_CDATA0);
					himci_writel(SDIO_CDATA1_DS_1V8,
							REG_CTRL_SDIO0_CDATA1);
					himci_writel(SDIO_CDATA2_DS_1V8,
							REG_CTRL_SDIO0_CDATA2);
					himci_writel(SDIO_CDATA3_DS_1V8,
							REG_CTRL_SDIO0_CDATA3);
				} else if (1 == host->id) {
					himci_writel(SDIO_CCLK_DS_1V8,
							REG_CTRL_SDIO1_CCLK);
					himci_writel(SDIO_CCMD_DS_1V8,
							REG_CTRL_SDIO1_CCMD);
					himci_writel(SDIO_CDATA0_DS_1V8,
							REG_CTRL_SDIO1_CDATA0);
					himci_writel(SDIO_CDATA1_DS_1V8,
							REG_CTRL_SDIO1_CDATA1);
					himci_writel(SDIO_CDATA2_DS_1V8,
							REG_CTRL_SDIO1_CDATA2);
					himci_writel(SDIO_CDATA3_DS_1V8,
							REG_CTRL_SDIO1_CDATA3);
				}

				return 0;
			}
		}

		/*
		 * If we are here, that means the switch to 1.8V signaling
		 * failed. We power cycle the card, and retry initialization
		 * sequence by setting S18R to 0.
		 */
		hi_mci_control_cclk(host, DISABLE);

		/* Wait for 1ms as per the spec */
		usleep_range(1000, 1500);
		hi_mci_control_cclk(host, ENABLE);

		pr_info(DRIVER_NAME ": Switching to 1.8V signalling " \
			"voltage failed, retrying with S18R set to 0\n");
		return -EAGAIN;
	} else
		/* No signal voltage switch required */
		return 0;
}

static int hi_mci_start_signal_voltage_switch(struct mmc_host *mmc,
	struct mmc_ios *ios)
{
	struct himci_host *host = mmc_priv(mmc);
	int err;

	err = hi_mci_do_start_signal_voltage_switch(host, ios);
	return err;
}

/*
 * The procedure of tuning the phase shift of sampling clock
 *
 * 1.Set a phase shift of 0° on cclk_in_sample
 * 2.Send the Tuning command to the card
 * 3.increase the phase shift value of cclk_in_sample until the
 *   correct sampling point is received such that the host does not
 *   see any of the errors.
 * 4.Mark this phase shift value as the starting point of the sampling
 *   window.
 * 5.increase the phase shift value of cclk_in_sample until the host
 *   sees the errors starting to come again or the phase shift value
 *   reaches 360°.
 * 6.Mark the last successful phase shift value as the ending
 *   point of the sampling window.
 *
 *     A window is established where the tuning block is matched.
 * For example, for a scenario where the tuning block is received
 * correctly for a phase shift window of 90°and 180°, then an appropriate
 * sampling point is established as 135°. Once a sampling point is
 * established, no errors should be visible in the tuning block.
 *
 */
static int hi_mci_execute_tuning(struct mmc_host *mmc, u32 opcode)
{
	struct	himci_host *host;
	unsigned long flags;
	unsigned int index, count;
	unsigned int err = 0;
	unsigned int prev_err = -1;
	unsigned int phase_scale = 8; /* The num of phase shift is 8 */
	unsigned int found = 0; /* identify if we have found a valid phase */
	unsigned int reg_value;
	int	 tmp;
	unsigned int start_point;
	unsigned int end_point;
	unsigned int raise_point = -1;
	unsigned int fall_point = -1;

	struct mmc_command cmd = {0};
	struct mmc_command stop = {0};
	struct mmc_request mrq = {NULL};
	struct mmc_data data = {0};
	u32 cmd_opcode = opcode;
	struct scatterlist sg;
	void *data_buf;

	data_buf = kmalloc(64, GFP_KERNEL);
	memset(data_buf, 0, 64);
	if (data_buf == NULL)
		return -ENOMEM;

	sg_init_one(&sg, data_buf, 64);

	host = mmc_priv(mmc);
	host->is_tuning = 1; /* come into phase tuning process */
	himci_writel(ALL_INT_CLR, host->base + MCI_RINTSTS);

	switch (cmd_opcode) {

	case MMC_SEND_TUNING_BLOCK:
		himci_trace(3, "command %d send", \
				MMC_SEND_TUNING_BLOCK);
		cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;
		break;
	case MMC_SWITCH:
		himci_trace(3, "command %d send", MMC_SWITCH);
		cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;
		break;
	default:
		himci_trace(3, "command %d not support", cmd_opcode);
		goto tuning_out;
	}
	himci_writel(0x1, host->base + MCI_READ_THRESHOLD_SIZE);
	himci_trace(3, "start sd3.0 phase tuning...");

	for (index = 0; index < phase_scale; index++) {

		/* Construct mmc_request */
		cmd.opcode = cmd_opcode;
		cmd.arg = 0;
		cmd.retries = 0;
		cmd.data = NULL;
		cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;
		cmd.error = 0;
		data.error = 0;

		mrq.data = &data;
		mrq.data->blksz = 64;
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
		reg_value = himci_readl(host->base + MCI_UHS_EXT);
		reg_value &= ~CLK_SMPL_PHS_MASK;
		reg_value |= (index << 16);
		himci_writel(reg_value, host->base + MCI_UHS_EXT);
		spin_unlock_irqrestore(&host->lock, flags);

		himci_writel(ALL_INT_CLR, host->base + MCI_RINTSTS);

		count = 0;
		do {
			cmd.error = 0;
			data.error = 0;
			memset(data_buf, 0, 64);
			mmc_wait_for_req(mmc, &mrq);

			if ((cmd.error) || (data.error)) {
				himci_trace(3, "cmd.error=%d data.error=%d",
						cmd.error, data.error);
				err = 1;
				break;
			} else {
				if (memcmp(tuning_blk_pattern, data_buf, 64)) {
					himci_trace(3, "pattern blk not same!");
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

		if ((raise_point != -1) && (fall_point != -1))
			goto tuning_out;

		prev_err = err;
		err = 0;
	}

tuning_out:
	kfree(data_buf);
	host->is_tuning = 0; /* jump out phase tuning process */

	if (!found) {
		himci_trace(5, "%s: no valid phase shift! use default",
				mmc_hostname(mmc));
		himci_writel(PHASE_SHIFT, host->base + MCI_UHS_EXT);
	} else {
		himci_trace(3, "Tuning finished!!");

		start_point = raise_point;
		end_point = fall_point;

		if (-1 == raise_point)
			start_point = 0;
		if (-1 == fall_point)
			end_point = phase_scale - 1;

		tmp = (start_point + end_point) / 2;
		if (end_point < start_point) {
			tmp = tmp - (phase_scale / 2);
			tmp = (tmp < 0) ? (phase_scale + tmp) : tmp;
		} else {
			/* all of phase valid, it means tuning fail
			 * so we select a theoretical value */
			if (end_point - start_point == phase_scale - 1)
				tmp = 5;
		}

		spin_lock_irqsave(&host->lock, flags);
		reg_value = himci_readl(host->base + MCI_UHS_EXT);
		reg_value &= ~CLK_SMPL_PHS_MASK;
		reg_value |= (tmp << 16);
		himci_writel(reg_value, host->base + MCI_UHS_EXT);
		spin_unlock_irqrestore(&host->lock, flags);

		himci_trace(3, "%s: valid phase shift [%d, %d] Final Phase %d",
				mmc_hostname(mmc),
				start_point, end_point, tmp);
	}

	himci_writel(READ_THRESHOLD_SIZE, host->base + MCI_READ_THRESHOLD_SIZE);
	himci_writel(ALL_INT_CLR, host->base + MCI_RINTSTS);

	return 0;
}

static void hi_mci_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct himci_host *host = mmc_priv(mmc);
	unsigned int tmp_reg;
	u32 ctrl;

	himci_trace(2, "begin");
	himci_assert(mmc);
	himci_assert(ios);
	himci_assert(host);

	himci_trace(3, "ios->power_mode = %d ", ios->power_mode);
	switch (ios->power_mode) {
	case MMC_POWER_OFF:
		/*
		 * Set controller working voltage as 3.3V before power off.
		 */
		ctrl = himci_readl(host->base + MCI_UHS_REG);
		ctrl &= ~HI_SDXC_CTRL_VDD_180;
		himci_trace(3, "set voltage %d[addr 0x%x]", ctrl, MCI_UHS_REG);
		himci_writel(ctrl, host->base + MCI_UHS_REG);

		hi_mci_ctrl_power(host, POWER_OFF);
		break;
	case MMC_POWER_UP:
	case MMC_POWER_ON:
		hi_mci_ctrl_power(host, POWER_ON);
		break;
	}
	himci_trace(3, "ios->clock = %d ", ios->clock);
	if (ios->clock) {

		hi_mci_control_cclk(host, DISABLE);
		hi_mci_set_cclk(host, ios->clock);
		hi_mci_control_cclk(host, ENABLE);

		/* speed mode check ,if it is DDR50 set DDR mode*/
		if ((ios->timing == MMC_TIMING_UHS_DDR50)) {
			ctrl = himci_readl(host->base + MCI_UHS_REG);
			if (!(HI_SDXC_CTRL_DDR_REG & ctrl)) {
				ctrl |= HI_SDXC_CTRL_DDR_REG;
				himci_writel(ctrl, host->base + MCI_UHS_REG);
			}
		}
	} else {
		hi_mci_control_cclk(host, DISABLE);
		if ((ios->timing != MMC_TIMING_UHS_DDR50)) {
			ctrl = himci_readl(host->base + MCI_UHS_REG);
			if (HI_SDXC_CTRL_DDR_REG & ctrl) {
				ctrl &= ~HI_SDXC_CTRL_DDR_REG;
				himci_writel(ctrl, host->base + MCI_UHS_REG);
			}
		}
	}

	/* set bus_width */
	himci_trace(3, "ios->bus_width = %d ", ios->bus_width);
	if (ios->bus_width == MMC_BUS_WIDTH_4) {
		tmp_reg = himci_readl(host->base + MCI_CTYPE);
		tmp_reg |= CARD_WIDTH;
		himci_writel(tmp_reg, host->base + MCI_CTYPE);
	} else {
		tmp_reg = himci_readl(host->base + MCI_CTYPE);
		tmp_reg &= ~CARD_WIDTH;
		himci_writel(tmp_reg, host->base + MCI_CTYPE);
	}
}

static void himci_enable_sdio_irq(struct mmc_host *host, int enable)
{
	struct himci_host *hi_host = mmc_priv(host);
	unsigned int reg_value;
	if (enable) {
		reg_value = readl(hi_host->base + MCI_RINTSTS);
		reg_value &= 0x10000;
		writel(reg_value, hi_host->base + MCI_RINTSTS);

		reg_value = himci_readl(hi_host->base + MCI_INTMASK);
		reg_value |= 0x10000;
		himci_writel(reg_value, hi_host->base + MCI_INTMASK);
	} else {
		reg_value = himci_readl(hi_host->base + MCI_INTMASK);
		reg_value &= ~0xffff0000;
		himci_writel(reg_value, hi_host->base + MCI_INTMASK);
	}

}


static int hi_sdio_get_cd(struct mmc_host *mmc)
{
	unsigned ret;
	struct himci_host *host = mmc_priv(mmc);

	himci_trace(2, "begin");
	ret = hi_mci_sys_card_detect(host);
	if (ret)
		return 0;
	else
		return 1;
}

static int hi_mci_get_ro(struct mmc_host *mmc)
{
	unsigned ret;
	struct himci_host *host = mmc_priv(mmc);

	himci_trace(2, "begin");
	himci_assert(mmc);

	ret = hi_mci_ctrl_card_readonly(host);

	return ret;
}

static const struct mmc_host_ops hi_mci_ops = {
	.request = hi_mci_request,
	.set_ios = hi_mci_set_ios,
	.get_ro	 = hi_mci_get_ro,
	.start_signal_voltage_switch = hi_mci_start_signal_voltage_switch,
	.execute_tuning	= hi_mci_execute_tuning,
/*	.execute_tuning	= NULL, */
	.enable_sdio_irq = himci_enable_sdio_irq,
	.get_cd = hi_sdio_get_cd,
};

static irqreturn_t hisd_irq(int irq, void *dev_id)
{
	struct himci_host *host = dev_id;
	u32 state = 0;
	u32 mstate = 0;
	int handle = 0;

	spin_lock(&host->lock);
	state = himci_readl(host->base + MCI_RINTSTS);
	spin_unlock(&host->lock);

	/* bugfix: when send soft stop to SD Card, Host will report
	   sdio interrupt, This situation needs to be avoided */
	if (host->mmc->caps & MMC_CAP_SDIO_IRQ) {
		if ((host->mmc->card != NULL)
				&& (host->mmc->card->type == MMC_TYPE_SDIO)) {
			mstate = himci_readl(host->base + MCI_INTMASK);
			if ((state & SDIO_INT_STATUS) &&
					(mstate & SDIO_INT_MASK)) {
				spin_lock(&host->lock);
				himci_writel(SDIO_INT_STATUS,
						host->base + MCI_RINTSTS);
				spin_unlock(&host->lock);
				handle = 1;
				mmc_signal_sdio_irq(host->mmc);
			}
		}
	}

	if (state & DATA_INT_MASK) {
		handle = 1;
		host->pending_events |= HIMCI_PEND_DTO_m;

		spin_lock(&host->lock);
		host->irq_status = himci_readl(host->base + MCI_RINTSTS);
		himci_writel(DATA_INT_MASK , host->base + MCI_RINTSTS);
		spin_unlock(&host->lock);

		wake_up(&host->intr_wait);
	}

	if (handle)
		return IRQ_HANDLED;

	return IRQ_NONE;
}

static int __devinit hi_mci_probe(struct platform_device *pdev)
{
	struct mmc_host *mmc;
	struct himci_host *host = NULL;
	int ret = 0, irq;

	himci_trace(2, "begin");
	himci_assert(pdev);

	mmc = mmc_alloc_host(sizeof(struct himci_host), &pdev->dev);
	if (!mmc) {
		himci_error("no mem for hi mci host controller!\n");
		ret = -ENOMEM;
		goto out;
	}
	mmc->ops = &hi_mci_ops;

	if (0 == pdev->id) {
#ifdef CONFIG_HIMCI0
		mmc->f_min = CONFIG_MMC0_CCLK_MIN;
		mmc->f_max = CONFIG_MMC0_CCLK_MAX;
		mmc->caps |= MMC_CAP_4_BIT_DATA
			| MMC_CAP_SD_HIGHSPEED
			| MMC_CAP_MMC_HIGHSPEED
			| MMC_CAP_UHS_SDR12
		/*	| MMC_CAP_SDIO_IRQ	*/
			| MMC_CAP_UHS_SDR25
			| MMC_CAP_UHS_SDR50
		/*	| MMC_CAP_UHS_DDR50	*/
			| MMC_CAP_UHS_SDR104
			;
#endif
	} else if (1 == pdev->id) {
#ifdef CONFIG_HIMCI1
		mmc->f_min = CONFIG_MMC1_CCLK_MIN;
		mmc->f_max = CONFIG_MMC1_CCLK_MAX;
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
#endif
	} else {
		himci_error("himci host id error!");
		goto out;
	}

	/* reload by this controller */
	mmc->max_blk_count = 2048;
	mmc->max_segs = 1024;
	mmc->max_seg_size = mmc->max_blk_size * mmc->max_blk_count;
	mmc->max_req_size = mmc->max_blk_size * mmc->max_blk_count;

	mmc->ocr_avail = MMC_VDD_32_33 | MMC_VDD_33_34;
	mmc->ocr = mmc->ocr_avail;
	/* mmc->pm_flags |= MMC_PM_IGNORE_PM_NOTIFY; */

	host = mmc_priv(mmc);
	host->dma_vaddr = dma_alloc_coherent(&pdev->dev, PAGE_SIZE,
			&host->dma_paddr, GFP_KERNEL);
	if (!host->dma_vaddr) {
		himci_error("no mem for himci dma!\n");
		ret = -ENOMEM;
		goto out;
	}

	host->mmc = mmc;
	if (0 == pdev->id) {
#ifdef CONFIG_HIMCI0
		host->id = 0;
		host->base = ioremap_nocache(CONFIG_HIMCI0_IOBASE, \
				HI_MCI_IO_SIZE);
#endif
	} else if (1 == pdev->id) {
#ifdef CONFIG_HIMCI1
		host->id = 1;
		host->base = ioremap_nocache(CONFIG_HIMCI1_IOBASE, \
				HI_MCI_IO_SIZE);
#endif
	} else {
		himci_error("himci host id error!");
		goto out;
	}

	if (!host->base) {
		himci_error("no mem for himci base!\n");
		ret = -ENOMEM;
		goto out;
	}

	/* enable mmc clk */
	hi_mci_sys_ctrl_init(host);

	/* enable card */
	spin_lock_init(&host->lock);
	hi_mci_init_card(host);

	host->card_status = hi_mci_sys_card_detect(host);
	init_timer(&host->timer);

	host->timer.function = hi_mci_detect_card;
	host->timer.data = (unsigned long)host;
	host->timer.expires = jiffies + detect_time;
	platform_set_drvdata(pdev, mmc);
	mmc_add_host(mmc);

	add_timer(&host->timer);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		printk(KERN_ERR "no IRQ defined!\n");
		goto out;
	}

	init_waitqueue_head(&host->intr_wait);

	host->irq = irq;
	ret = request_irq(irq, hisd_irq, 0, DRIVER_NAME, host);
	if (ret) {
		printk(KERN_ERR "request_irq error!\n");
		goto out;
	}

	return 0;
out:
	if (host) {

		del_timer(&host->timer);

		if (host->base)
			iounmap(host->base);

		if (host->dma_vaddr)
			dma_free_coherent(&pdev->dev, PAGE_SIZE,
					host->dma_vaddr, host->dma_paddr);
	}
	if (mmc)
		mmc_free_host(mmc);

	return ret;
}

static int __devexit hi_mci_remove(struct platform_device *pdev)
{
	struct mmc_host *mmc = platform_get_drvdata(pdev);

	himci_trace(2, "begin");
	himci_assert(pdev);

	platform_set_drvdata(pdev, NULL);

	if (mmc) {
		struct himci_host *host = mmc_priv(mmc);

		free_irq(host->irq, host);
		del_timer_sync(&host->timer);
		mmc_remove_host(mmc);
		hi_mci_ctrl_power(host, POWER_OFF);
		hi_mci_control_cclk(host, DISABLE);
		iounmap(host->base);
		dma_free_coherent(&pdev->dev, PAGE_SIZE, host->dma_vaddr,
				host->dma_paddr);
		mmc_free_host(mmc);
	}
	return 0;
}

#ifdef CONFIG_PM
static int hi_mci_suspend(struct platform_device *dev, pm_message_t state)
{
	struct mmc_host *mmc = platform_get_drvdata(dev);
	struct himci_host *host;
	int ret = 0;

	himci_trace(2, "begin");
	himci_assert(dev);

	if (mmc) {
		ret = mmc_suspend_host(mmc);

		host = mmc_priv(mmc);
		hi_mci_sys_ctrl_suspend(host);
	}

	himci_trace(2, "end");

	return ret;
}

static int hi_mci_resume(struct platform_device *dev)
{
	struct mmc_host *mmc = platform_get_drvdata(dev);
	struct himci_host *host;
	int ret = 0;

	himci_trace(2, "begin");
	himci_assert(dev);

	if (mmc) {
		host = mmc_priv(mmc);
		/* enable mmc clk */
		hi_mci_sys_ctrl_init(host);
		/* enable card */
		hi_mci_init_card(host);

		ret = mmc_resume_host(mmc);
	}

	himci_trace(2, "end");

	return ret;
}
#else
#define hi_mci_suspend	NULL
#define hi_mci_resume	NULL
#endif

static void hi_mci_platdev_release(struct device *dev)
{
}

#ifdef CONFIG_HIMCI0
static struct resource hi_mci0_resources[] = {
	[0] = {
		.start          = CONFIG_HIMCI0_IOBASE,
		.end            = CONFIG_HIMCI0_IOBASE + HI_MCI_IO_SIZE - 1,
		.flags          = IORESOURCE_MEM,
	},
	[1] = {
		.start          = CONFIG_HIMCI0_INTR,
		.end            = CONFIG_HIMCI0_INTR,
		.flags          = IORESOURCE_IRQ,
	},
};
#endif

#ifdef CONFIG_HIMCI1
static struct resource hi_mci1_resources[] = {
	[0] = {
		.start          = CONFIG_HIMCI1_IOBASE,
		.end            = CONFIG_HIMCI1_IOBASE + HI_MCI_IO_SIZE - 1,
		.flags          = IORESOURCE_MEM,
	},
	[1] = {
		.start          = CONFIG_HIMCI1_INTR,
		.end            = CONFIG_HIMCI1_INTR,
		.flags          = IORESOURCE_IRQ,
	},
};
#endif

static u64 himmc_dmamask = DMA_BIT_MASK(32);

#ifdef CONFIG_HIMCI0
static struct platform_device hi_mci0_device = {
	.name           = "hi_mci",
	.id             = 0,
	.dev = {
		.release      = hi_mci_platdev_release,
		.dma_mask     = &himmc_dmamask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	},
	.num_resources  = ARRAY_SIZE(hi_mci0_resources),
	.resource       = hi_mci0_resources,
};
#endif

#ifdef CONFIG_HIMCI1
static struct platform_device hi_mci1_device = {
	.name           = "hi_mci",
	.id             = 1,
	.dev = {
		.release      = hi_mci_platdev_release,
		.dma_mask     = &himmc_dmamask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	},
	.num_resources  = ARRAY_SIZE(hi_mci1_resources),
	.resource       = hi_mci1_resources,
};
#endif

static struct platform_driver hi_mci_driver = {
	.probe         = hi_mci_probe,
	.remove        = hi_mci_remove,
	.suspend       = hi_mci_suspend,
	.resume        = hi_mci_resume,
	.driver        = {
		.name          = DRIVER_NAME,
	},
};

struct platform_device *get_mci_device(unsigned int index)
{
	if ((index < 0) || (index >= MAX_MCI_HOST))
		goto out;

	if (0 == index)
#ifdef CONFIG_HIMCI0
		return &hi_mci0_device;
#else
		goto out;
#endif

	if (1 == index)
#ifdef CONFIG_HIMCI1
		return &hi_mci1_device;
#else
		goto out;
#endif

out:
	return NULL;
}

static int __init hi_mci_init(void)
{
	int ret = 0;

	himci_trace(2, "mci init begin");

#ifdef CONFIG_HIMCI0
	ret = platform_device_register(&hi_mci0_device);
	if (ret) {
		himci_error("Platform device register[himci0] is failed!");
		return ret;
	}
#endif

#ifdef CONFIG_HIMCI1
	ret = platform_device_register(&hi_mci1_device);
	if (ret) {
		himci_error("Platform device register[himci1] is failed!");
		goto register_himci1_device_failed;
	}
#endif

	ret = platform_driver_register(&hi_mci_driver);
	if (ret) {
		himci_error("Platform driver register is failed!");
		goto register_himci_driver_failed;
	}

	/* device proc entry */
	ret = mci_proc_init(MAX_MCI_HOST);
	if (ret) {
		himci_error("device proc init is failed!");
		goto register_himci_driver_failed;
	}

	return ret;

register_himci_driver_failed:
#ifdef CONFIG_HIMCI1
	platform_device_unregister(&hi_mci1_device);
#endif

register_himci1_device_failed:
#ifdef CONFIG_HIMCI0
	platform_device_unregister(&hi_mci0_device);
#endif

	return ret;
}

static void __exit hi_mci_exit(void)
{
	himci_trace(2, "begin");

	mci_proc_shutdown();

	platform_driver_unregister(&hi_mci_driver);
#ifdef CONFIG_HIMCI1
	platform_device_unregister(&hi_mci1_device);
#endif
#ifdef CONFIG_HIMCI0
	platform_device_unregister(&hi_mci0_device);
#endif
}
module_init(hi_mci_init);
module_exit(hi_mci_exit);

#ifdef MODULE
MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("MMC/SD driver for the Hisilicon MMC/SD Host Controller");
MODULE_LICENSE("GPL");
#endif
