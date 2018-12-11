/*
 * himci.c - hisilicon MMC Host driver
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#define pr_fmt(fmt) "himci: " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/scatterlist.h>

#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/card.h>
#include <linux/mmc/core.h>
#include <linux/mmc/sd.h>
#include <linux/slab.h>

#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/spinlock.h>

#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/freezer.h>
#include <asm/dma.h>
#include <asm/irq.h>
#include <linux/sizes.h>
#include <mach/io.h>

#include <linux/io.h>
#include <linux/of.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>

#include "himci_reg.h"
#include "himci.h"
#include "himci_dbg.h"
#include "himci_acl.h"
#include "himci_proc.h"

/*************************************************************************/
#ifdef CONFIG_ARCH_HI3516CV300
#include "himci_hi3516cv300.c"
#endif

#ifdef CONFIG_ARCH_HI3519
#include "himci_hi3519.c"
#endif

#ifdef CONFIG_ARCH_HI3519V101
#include "himci_hi3519v101.c"
#endif

#ifdef CONFIG_ARCH_HI3516AV200
#include "himci_hi3516av200.c"
#endif

#if defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556)
#include "himci_hi3559.c"
#endif

#if defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556)
	#define PERF_OPT_RATIO 8
	#define MAX_SEGS 4096
#else
	#define PERF_OPT_RATIO 1
	#define MAX_SEGS 1024
#endif

/*************************************************************************/
#define DRIVER_NAME "himci"

#ifdef CONFIG_DETECT_CARD_TIME
static unsigned int detect_time = HZ*CONFIG_DETECT_CARD_TIME/1000;
#else
static unsigned int detect_time = HI_MCI_DETECT_TIMEOUT;
#endif

static unsigned int retry_count = MAX_RETRY_COUNT;
static unsigned int request_timeout = HI_MCI_REQUEST_TIMEOUT;
int trace_level = HIMCI_TRACE_LEVEL;
unsigned int slot_index = 0;
struct himci_host *mci_host[HIMCI_SLOT_NUM] = {NULL};

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

/* reset MMC host controller */
static void himciv200_sys_reset(struct himci_host *host)
{
	unsigned int reg_value;
	unsigned long flags;

	local_irq_save(flags);

	reg_value = himci_readl(host->base + MCI_BMOD);
	reg_value |= BMOD_SWR;
	himci_writel(reg_value, host->base + MCI_BMOD);
	mdelay(10);

	reg_value = himci_readl(host->base + MCI_BMOD);
	reg_value |= BURST_16 | BURST_INCR;
	himci_writel(reg_value, host->base + MCI_BMOD);

	reg_value = himci_readl(host->base + MCI_CTRL);
	reg_value |= CTRL_RESET | FIFO_RESET | DMA_RESET;
	himci_writel(reg_value, host->base + MCI_CTRL);

	local_irq_restore(flags);
}

static void himci_ctrl_power(struct himci_host *host,
		unsigned int flag, unsigned int force)
{
	unsigned int port;

	himci_trace(2, "begin");

	port = host->cur_port;

	if (host->power_status != flag || force == FORCE_ENABLE) {
		unsigned int reg_value;

		if (flag == POWER_OFF) {
			reg_value = himci_readl(host->base + MCI_RESET_N);
			reg_value &= ~(MMC_RST_N << port);
			himci_writel(reg_value, host->base + MCI_RESET_N);
		}

		reg_value = himci_readl(host->base + MCI_PWREN);
		if (flag == POWER_OFF)
			reg_value &= ~(0x1 << port);
		else
			reg_value |= (0x1 << port);

		himci_writel(reg_value, host->base + MCI_PWREN);

		if (flag == POWER_ON) {
			reg_value = himci_readl(host->base + MCI_RESET_N);
			reg_value |= (MMC_RST_N << port);
			himci_writel(reg_value, host->base + MCI_RESET_N);
		}

		if (in_interrupt())
			mdelay(100);
		else
			msleep(100);

		host->power_status = flag;
	}
}

static void himciv200_host_power(struct himci_host *host, unsigned int power_on,
		unsigned int force)
{
	if (host->power_status != power_on || force) {
		if (!power_on) {
			himci_writel(0, host->base + MCI_RESET_N);
			himci_writel(0, host->base + MCI_PWREN);
		} else {
			himci_writel(1, host->base + MCI_PWREN);
			himci_writel(1, host->base + MCI_RESET_N);
		}

		mdelay(100);

		host->power_status = power_on;
	}
}

static void himci_idma_start(struct himci_host *host)
{
	unsigned int tmp;

	himci_trace(2, "begin");

	tmp = himci_readl(host->base + MCI_BMOD);
	tmp |= BMOD_DMA_EN;
	himci_writel(tmp, host->base + MCI_BMOD);
}

/**********************************************
 *1: card off
 *0: card on
 ***********************************************/
static unsigned int himci_sys_card_detect(struct himci_host *host)
{
	unsigned int card_status;

	card_status = himci_readl(host->base + MCI_CDETECT);

	/* eMMC need't detect, but we check if eMMC controller available */
	if (!(host->mmc->caps & MMC_CAP_SD_HIGHSPEED))
		return himci_check_emmc(host);

	card_status &= HIMCI_CARD0;
	return card_status;
}

/**********************************************
 *1: card readonly
 *0: card read/write
 ***********************************************/
static unsigned int himci_ctrl_card_readonly(struct himci_host *host)
{
	unsigned int card_value = himci_readl(host->base + MCI_WRTPRT);
	unsigned int port = host->cur_port;

	return card_value & (HIMCI_CARD0 << port);
}

static int himci_wait_cmd(struct himci_host *host)
{
	int wait_retry_count = 0;
	unsigned int reg_data = 0;
	unsigned long flags;

	while (1) {
		/*
		   Check if CMD::start_cmd bit is clear.
		   start_cmd = 0 means MMC Host controller has loaded registers
		   and next command can be loaded in.
		 */
		reg_data = himci_readl(host->base + MCI_CMD);
		if ((reg_data & START_CMD) == 0)
			return 0;

		/* Check if Raw_Intr_Status::HLE bit is set. */
		spin_lock_irqsave(&host->lock, flags);
		reg_data = himci_readl(host->base + MCI_RINTSTS);
		if (reg_data & HLE_INT_STATUS) {
			reg_data |= HLE_INT_STATUS;
			himci_writel(reg_data, host->base + MCI_RINTSTS);
			spin_unlock_irqrestore(&host->lock, flags);

			himci_trace(5, "Other CMD is running,"
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

static void himci_control_cclk(struct himci_host *host, unsigned int flag)
{
	unsigned int reg;
	union cmd_arg_u cmd_reg;
	unsigned int port = host->cur_port;

	himci_trace(2, "begin");
	himci_assert(host);

	reg = himci_readl(host->base + MCI_CLKENA);
	if (flag == ENABLE) {
		reg |= (CCLK_ENABLE << port);
		reg |= (0x10000 << port);
	} else {
		reg &= ~(CCLK_ENABLE << port);
		reg &= ~(0x10000 << port);
	}

	himci_writel(reg, host->base + MCI_CLKENA);
	cmd_reg.cmd_arg = himci_readl(host->base + MCI_CMD);
	cmd_reg.bits.start_cmd = 1;
	cmd_reg.bits.card_number = port;
	cmd_reg.bits.cmd_index = 0;
	cmd_reg.bits.data_transfer_expected = 0;
	cmd_reg.bits.update_clk_reg_only = 1;
	cmd_reg.bits.response_expect = 0;
	cmd_reg.bits.send_auto_stop = 0;
	cmd_reg.bits.wait_prvdata_complete = 0;
	cmd_reg.bits.check_response_crc = 0;
	himci_writel(cmd_reg.cmd_arg, host->base + MCI_CMD);
	if (himci_wait_cmd(host) != 0)
		himci_trace(3, "disable or enable clk is timeout!");
}

static void himci_set_cclk(struct himci_host *host, unsigned int cclk)
{
	unsigned int reg_value = 0;
	union cmd_arg_u clk_cmd;
	unsigned int port = host->cur_port;
	unsigned int hclk;
	unsigned int clk_rate;

	himci_trace(2, "begin");
	himci_assert(host);
	himci_assert(cclk);

	clk_rate = cclk > MMC_CRG_MIN ? cclk : MMC_CRG_MIN;
	clk_set_rate(host->clk, clk_rate);

	hclk = clk_get_rate(host->clk);

	/*
	 * set card clk divider value,
	 * clk_divider = Fmmcclk/(Fmmc_cclk * 2)
	 */
	reg_value = hclk / (cclk * 2);
	if ((hclk % (cclk * 2)) && (hclk > cclk))
		reg_value++;
	if (reg_value > 0xFF)
		reg_value = 0xFF;

	host->hclk = hclk;
	host->cclk = reg_value ? (hclk / (reg_value * 2)) : hclk;
	himci_writel(reg_value, host->base + MCI_CLKDIV);
	clk_cmd.cmd_arg = himci_readl(host->base + MCI_CMD);
	clk_cmd.bits.start_cmd = 1;
	clk_cmd.bits.card_number = port;
	clk_cmd.bits.update_clk_reg_only = 1;
	clk_cmd.bits.cmd_index = 0;
	clk_cmd.bits.data_transfer_expected = 0;
	clk_cmd.bits.response_expect = 0;
	himci_writel(clk_cmd.cmd_arg, host->base + MCI_CMD);
	if (himci_wait_cmd(host) != 0)
		himci_trace(3, "set card clk divider is failed!");
}

static void himci_pre_init_host(struct himci_host *host)
{
#if !defined(CONFIG_ARCH_HI3516CV300)
	unsigned int shift[] = {13, 5, 21};
	unsigned int reg;

	reg = himci_readl(PERI_CRG49);
	reg |= 0x1 << shift[host->devid];
	himci_writel(reg, PERI_CRG49);

	if (!(host->mmc->caps & MMC_CAP_SD_HIGHSPEED)) {
		himci_writel(0x10, EMMC_DLL_CTRL);
		reg = himci_readl(PERI_CRG49);
		reg |= 0x1 << 22;
		himci_writel(reg, PERI_CRG49);
	}
#endif
}

static void himci_init_host(struct himci_host *host)
{
	unsigned int tmp_reg = 0;

	himci_trace(3, "begin init card");
	himci_assert(host);

	host->error_count = 0;
	/* controller config gpio */
	himci_writel(CMD_OUT_EN_FIX_BYPASS | DTO_FIX_BYPASS,
			host->base + MCI_GPIO);

	himciv200_sys_reset(host);

	himci_pre_init_host(host);

	/* set drv/smpl phase shift */
	tmp_reg |= SMPL_PHASE_DFLT | DRV_PHASE_DFLT;
	himci_writel(tmp_reg, host->base + MCI_UHS_REG_EXT);

	/* set card read threshold */
	himci_writel(RW_THRESHOLD_SIZE, host->base + MCI_CARDTHRCTL);

	/* clear MMC host intr */
	himci_writel(ALL_INT_CLR, host->base + MCI_RINTSTS);
	tmp_reg = himci_readl(host->base + MCI_IDSTS);
	tmp_reg |= ALL_ADMA_INT_CLR;
	himci_writel(tmp_reg, host->base + MCI_IDSTS);

	/* MASK MMC all host intr */
	tmp_reg = himci_readl(host->base + MCI_INTMASK);
	tmp_reg &= ~ALL_INT_MASK;
	himci_writel(tmp_reg, host->base + MCI_INTMASK);

	/* enable inner DMA mode and close intr of MMC host controller */
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

	/* ADMA3: disable ADMA Func */
	himci_writel(0x0, host->base + ADMA_CTRL);

	/* ADMA3: set entire des addr and quene deepth  */
	himci_writel(host->entire_paddr, host->base + ADMA_Q_ADDR); /* */
	himci_writel(ADMA_QUEUE_DEEPTH, host->base + ADMA_Q_DEEPTH);

	/* ADMA3: reset queue read/write ptr */
	tmp_reg = himci_readl(host->base + ADMA_CTRL);
	tmp_reg |= RDPTR_MOD_EN;
	himci_writel(tmp_reg, host->base + ADMA_CTRL);
	himci_writel(0x0, host->base + ADMA_Q_RDPTR);
	himci_writel(0x0, host->base + ADMA_Q_WRPTR);
	tmp_reg = himci_readl(host->base + ADMA_CTRL);
	tmp_reg &= ~RDPTR_MOD_EN;
	himci_writel(tmp_reg, host->base + ADMA_CTRL);

	/* ADMA3: set queue timeout value */
	himci_writel(0xffffffff, host->base + ADMA_Q_TO);

	/* ADMA3: enable ADMA intr */
	tmp_reg = (CMD_LOCK_ERR | OWNBIT_ERR | QUEUE_OVERFLOW
			| RESP_CHECK_ERR | PACKET_INT | PACKET_TO_INT
			| AUTO_STOP_ERR | QUEUE_FULL | CES | DU | FBE);

	himci_writel(tmp_reg, host->base + MCI_IDINTEN);
	/* ADMA3: clear ADMA3 intr */
	tmp_reg = (CMD_LOCK_ERR | OWNBIT_ERR | QUEUE_OVERFLOW
			| RESP_CHECK_ERR | PACKET_INT | PACKET_TO_INT
			| AUTO_STOP_ERR | QUEUE_FULL | QUEUE_EMPTY | CES
			| DU | FBE);
	himci_writel(tmp_reg, host->base + MCI_IDSTS);

	/* ADMA3: enable ADMA Func */
	tmp_reg = PACKET_INT_EN | ADMA3_EN;
	himci_writel(tmp_reg, host->base + ADMA_CTRL);

	himci_idma_start(host);
	host->mmc->status = MMC_HOST_OK;
}

void himci_sw_reset(struct mmc_host *mmc)
{
	struct himci_host *host = mmc_priv(mmc);
	int i;

	clk_prepare_enable(host->clk);
	himci_init_host(host);

	for (i = 0; i < ADMA_QUEUE_DEEPTH ; i++)
		host->pmrq[i] = NULL;
}

static void himci_detect_card(unsigned long arg)
{
	struct himci_host *host = (struct himci_host *)arg;
	unsigned int i, curr_status, status[3], detect_retry_count = 0;

	himci_assert(host);

	while (1) {
		for (i = 0; i < 3; i++) {
			status[i] = himci_sys_card_detect(host);
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
	if (curr_status != host->card_status) {
		himci_trace(2, "begin card_status = %d\n", host->card_status);
		host->card_status = curr_status;
		if (curr_status != CARD_UNPLUGED) {
			himci_init_host(host);
			pr_info("card connected!\n");
		} else {
			pr_info("card disconnected!\n");
			host->mmc->status = MMC_HOST_ERR;
		}

		mmc_detect_change(host->mmc, 0);
	}
err:
	mod_timer(&host->timer, jiffies + detect_time);
}

static int himci_prep_data(struct himci_host *host, struct mmc_data *data)
{
	unsigned int sg_phyaddr, sg_length;
	unsigned int i, ret = 0;
	unsigned int data_size;
	unsigned int max_des, des_cnt;
	struct himci_dma_des *dma_des;
	struct himci_dma_des *des;
	unsigned int *dma_paddr;
	unsigned int dma_dir;

	himci_trace(2, "begin");
	himci_assert(host);
	himci_assert(data);

	host->data = data;

	if (data->flags & MMC_DATA_READ)
		dma_dir = DMA_FROM_DEVICE;
	else
		dma_dir = DMA_TO_DEVICE;

	host->dma_sg = data->sg;
	host->dma_sg_num = dma_map_sg(mmc_dev(host->mmc), data->sg,
				      data->sg_len, dma_dir);
	himci_assert(host->dma_sg_num);
	himci_trace(2, "sg_num is %d\n", host->dma_sg_num);

	data_size = data->blksz * data->blocks;
	if (data_size > (DMA_BUFFER * MAX_DMA_DES)) {
		himci_error("mci request data_size is too big!\n");
		ret = -1;
		goto out;
	}

	himci_trace(2, "host->dma_paddr is 0x%08X,host->dma_addr is 0x%08X\n",
		    (unsigned int)host->dma_paddr,
		    (unsigned int)host->dma_addr);

	max_des = PERF_OPT_RATIO * PAGE_SIZE / sizeof(struct himci_dma_des) - 1;

	/* dma descriptor is followed by cmd descriptor */
	dma_des = (struct himci_dma_des *)(host->wr_cmd_des + 1);
	dma_paddr = (unsigned int *)(host->cmd_paddr + PERF_OPT_RATIO * PAGE_SIZE * host->wr_pos
			+ sizeof(struct himci_cmd_des));

	des = dma_des;
	des_cnt = 0;

	for (i = 0; i < host->dma_sg_num; i++) {
		sg_length = sg_dma_len(&data->sg[i]);
		sg_phyaddr = sg_dma_address(&data->sg[i]);
		himci_trace(2, "sg[%d] sg_length is 0x%08X,"
			" sg_phyaddr is 0x%08X\n",
			i,
			(unsigned int)sg_length, (unsigned int)sg_phyaddr);
		while (sg_length) {
			des[des_cnt].idmac_des_ctrl =
				DMA_DES_OWN | DMA_DES_NEXT_DES;
			des[des_cnt].idmac_des_buf_addr = sg_phyaddr;
			/* idmac_des_next_addr is paddr for dma */
			des[des_cnt].idmac_des_next_addr = (unsigned long)
				(dma_paddr + (des_cnt + 1) * 4);

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
				des_cnt, (unsigned int)&des[des_cnt]);
			himci_trace(2, "des[%d].idmac_des_ctrl is 0x%08X",
				des_cnt, (unsigned int)
				des[des_cnt].idmac_des_ctrl);
			himci_trace(2, "des[%d].idmac_des_buf_size is 0x%08X",
				des_cnt, (unsigned int)
				des[des_cnt].idmac_des_buf_size);
			himci_trace(2, "des[%d].idmac_des_buf_addr 0x%08X",
				des_cnt,
				(unsigned int)des[des_cnt].idmac_des_buf_addr);
			himci_trace(2, "des[%d].idmac_des_next_addr is 0x%08X",
				des_cnt, (unsigned int)
				des[des_cnt].idmac_des_next_addr);
			des_cnt++;
		}

		himci_assert(des_cnt < max_des);
	}
	des[0].idmac_des_ctrl |= DMA_DES_FIRST_DES;
	des[des_cnt - 1].idmac_des_ctrl |= DMA_DES_LAST_DES;
	des[des_cnt - 1].idmac_des_next_addr = 0;
	himci_trace(2, "des[%d].idmac_des_ctrl is 0x%08X",
			des_cnt-1, (unsigned int)des[des_cnt-1].idmac_des_ctrl);
out:
	return ret;
}

static int himci_prep_cmd(struct himci_host *host, struct mmc_command *cmd,
		struct mmc_data *data)
{
	volatile union cmd_arg_u cmd_regs;
	unsigned int port = host->cur_port;
	struct himci_cmd_des *cmd_des;

	himci_trace(2, "begin");
	himci_assert(host);
	himci_assert(cmd);

	host->cmd = cmd;

	cmd_des = host->wr_cmd_des;
	cmd_des->arg = cmd->arg;

	himci_trace(3, "arg_reg 0x%x, val 0x%x\n", MCI_CMDARG, cmd->arg);
	cmd_regs.cmd_arg = DEFAULT_CMD_VALUE;
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

#ifdef CONFIG_SEND_AUTO_STOP
	if (host->mrq->stop)
		cmd_regs.bits.send_auto_stop = 1;
#endif

	if (cmd->opcode == MMC_STOP_TRANSMISSION) {
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
	case MMC_RSP_R1 & (~MMC_RSP_CRC):
		cmd_regs.bits.response_expect = 1;
		cmd_regs.bits.response_length = 0;
		cmd_regs.bits.check_response_crc = 0;
		break;
	default:
		host->cmd->error = -EINVAL;
		himci_error("himci: unhandled response type %02x\n",
				mmc_resp_type(cmd));
		return -EINVAL;
	}

	himci_trace(4, "cmd->opcode = %d cmd->arg = 0x%X\n",
			cmd->opcode, cmd->arg);

	if (cmd->opcode == SD_SWITCH_VOLTAGE)
		cmd_regs.bits.volt_switch = 1;
	else
		cmd_regs.bits.volt_switch = 0;

	cmd_regs.bits.send_initialization = 1;
	cmd_regs.bits.card_number = port;
	cmd_regs.bits.cmd_index = cmd->opcode;
	cmd_regs.bits.start_cmd = 1;
	cmd_regs.bits.update_clk_reg_only = 0;

	cmd_des->cmd = cmd_regs.cmd_arg;

	himci_trace(4, "cmd_reg 0x%x, val 0x%x\n", MCI_CMD, cmd_regs.cmd_arg);

	return 0;
}

static void himci_data_done(struct mmc_host *host,
		struct mmc_request *mrq, int err)
{
	struct mmc_data *data = mrq->data;
	unsigned int dma_dir;

	if (!data)
		return;
	if (data->flags & MMC_DATA_READ)
		dma_dir = DMA_FROM_DEVICE;
	else
		dma_dir = DMA_TO_DEVICE;
	dma_unmap_sg(mmc_dev(host), data->sg, data->sg_len, dma_dir);

	if (!data->error)
		data->bytes_xfered = data->blocks * data->blksz;
	else
		data->bytes_xfered = 0;

}

static void himci_finish_request(struct himci_host *host,
				  struct mmc_request *mrq)
{
	himci_trace(2, "begin");
	himci_assert(host);
	himci_assert(mrq);

	himci_data_done(host->mmc, mrq, 0);
	mmc_request_done(host->mmc, mrq);
}

static void himci_cmd_done(struct mmc_request *mrq, unsigned int stat)
{
	struct mmc_command *cmd = mrq->cmd;

	himci_trace(2, "begin");
	himci_assert(cmd);

	if (stat & RTO_INT_STATUS) {
		cmd->error = -ETIMEDOUT;
		himci_trace(3, "irq cmd status stat = 0x%x is timeout error!",
			stat);
	} else if (stat & (RCRC_INT_STATUS | RE_INT_STATUS)) {
		cmd->error = -EILSEQ;
		himci_error("irq cmd status stat = 0x%x is response error!",
			stat);
	}
}

static void himci_read_response(struct himci_host *host,
		struct mmc_command *cmd)
{
	/* Read response of the card */
	if (cmd->flags & MMC_RSP_PRESENT) {
		if (cmd->flags & MMC_RSP_136) {
			cmd->resp[3] = himci_readl(host->base + MCI_RESP0);
			cmd->resp[2] = himci_readl(host->base + MCI_RESP1);
			cmd->resp[1] = himci_readl(host->base + MCI_RESP2);
			cmd->resp[0] = himci_readl(host->base + MCI_RESP3);
		} else {
			cmd->resp[0] = himci_readl(host->base + MCI_RESP0);
			cmd->resp[1] = 0;
			cmd->resp[2] = 0;
			cmd->resp[3] = 0;
		}
	}
}

static void himci_reset_host(struct himci_host *host)
{
	unsigned int i = 0, reg_value;

	/* DMA Controller resets */
	reg_value = himci_readl(host->base + ADMA_CTRL);
	reg_value &= ~(ADMA3_EN);
	himci_writel(reg_value, host->base + ADMA_CTRL);
	reg_value = himci_readl(host->base + MCI_BMOD);
	reg_value |= BMOD_SWR;
	himci_writel(reg_value, host->base + MCI_BMOD);
	himci_writel(0xFFFFFFFF, host->base + MCI_IDSTS);

	/* ADMA3: reset queue R/W ptr */
	reg_value = himci_readl(host->base + ADMA_CTRL);
	reg_value |= (RDPTR_MOD_EN);
	reg_value &= ~(ADMA3_EN);
	himci_writel(reg_value, host->base + ADMA_CTRL);
	himci_writel(0, host->base + ADMA_Q_WRPTR);
	himci_writel(0, host->base + ADMA_Q_RDPTR);
	for (i = 0; i < ADMA_QUEUE_DEEPTH ; i++)
		host->pmrq[i] = NULL;

	himci_writel(0xFFFFFFFF, host->base + MCI_RINTSTS);
	udelay(800);

	reg_value = himci_readl(host->base + ADMA_CTRL);
	reg_value &= ~RDPTR_MOD_EN;
	reg_value |= (ADMA3_EN);
	himci_writel(reg_value, host->base + ADMA_CTRL);

	/* ADMA3: restart */
	reg_value = himci_readl(host->base + ADMA_CTRL);
	reg_value |= ADMA3_RESTART;
	himci_writel(reg_value, host->base + ADMA_CTRL);

	/* dma enable */
	himci_idma_start(host);
}

static void himci_err_check(struct himci_host *host,
		unsigned int qstat, unsigned int rstat,
		unsigned int *cmd_err, unsigned int *data_err)
{
	*cmd_err = 0;
	*data_err = 0;

	if (!(qstat & (ADMA_INT_ERR | CES)))
		return;

	if (qstat & ADMA_INT_ERR) {
		himci_trace(4, "ADMA err! qstat:%x rstat:%x\n", qstat, rstat);
		*cmd_err = -ETIMEDOUT;
	}

	if (qstat & CES) {
		himci_trace(4, "CES err! qstat:%x rstat:%x\n", qstat, rstat);
		if (rstat & RTO_INT_STATUS) {
			himci_trace(3, "response timeout error!");
			*cmd_err = -ETIMEDOUT;
		} else if (rstat & (RCRC_INT_STATUS | RE_INT_STATUS)) {
			himci_trace(3, "response crc error!");
			*cmd_err = -EILSEQ;
		}

		if (rstat & (HTO_INT_STATUS | DRTO_INT_STATUS)) {
			himci_trace(3, "data transfer timeout error!");
			*data_err = -ETIMEDOUT;
		} else if (rstat & (EBE_INT_STATUS | SBE_INT_STATUS |
					FRUN_INT_STATUS | DCRC_INT_STATUS)) {
			himci_trace(3, "data transfer error!");
			*data_err = -EILSEQ;
		}
	}

	if (*cmd_err || *data_err)
		himci_reset_host(host);
}

static int himci_wait_voltage_switch(struct himci_host *host)
{
	unsigned int cmd_retry_count = 0;
	unsigned long cmd_jiffies_timeout;
	unsigned int cmd_irq_reg = 0;
	struct mmc_command *cmd = host->cmd;
	unsigned long flags;

	himci_trace(2, "begin");
	himci_assert(host);
	himci_assert(cmd);

	himci_trace(3, "wait voltage switch...");
	cmd_jiffies_timeout = jiffies + request_timeout;
	while (1) {
		do {
			spin_lock_irqsave(&host->lock, flags);
			cmd_irq_reg = himci_readl(host->base + MCI_RINTSTS);
			if (cmd_irq_reg & VOLT_SWITCH_INT_STATUS) {
				himci_writel(VOLT_SWITCH_INT_STATUS,
						host->base + MCI_RINTSTS);
				spin_unlock_irqrestore(&host->lock, flags);
				himci_cmd_done(host->mrq, cmd_irq_reg);
				goto switch_succeed;
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
				cmd->resp[i] = himci_readl(host->base
						+ MCI_RESP0 + i * 0x4);
			}
			cmd->error = -ETIMEDOUT;
			himci_trace(3, "wait cmd request complete is timeout!");
			return -1;
		}

		schedule();
	}
switch_succeed:
	/* bugfix: synopsys will trigger sdio intr when send cmd11,
	 * it's a bug and must be cleared
	 */
	himci_writel(SDIO_INT_STATUS, host->base + MCI_RINTSTS);
	return 0;
}

/* Prepare the entire descriptor */
static void himci_prep_entire(struct himci_host *host,
		struct mmc_request *mrq)
{
	struct himci_entire_des	*ent_des;
	struct himci_cmd_des	*cmd_des;
	unsigned int wr_pos;

	wr_pos = himci_readl(host->base + ADMA_Q_WRPTR);
	mrq->wr_pos = wr_pos;		/* only for ADMA3 R/W */
	host->pmrq[wr_pos] = mrq;	/* save send mrq */
	host->wr_pos = wr_pos;

	himci_trace(3, "write des to position %d", wr_pos);
	/* select entire descriptor */
	ent_des = (struct himci_entire_des *)(host->entire_addr);
	ent_des = ent_des + wr_pos;
	host->wr_ent_des = ent_des;

	/* config entire descriptor */
	ent_des->cmd_des_addr = host->cmd_paddr + PERF_OPT_RATIO * PAGE_SIZE * wr_pos;

	if ((host->cmd->flags & MMC_CMD_NON_BLOCKING)
			|| (mrq->cmd->opcode == SD_SWITCH_VOLTAGE)) {
		/* non-blocking command needn't issue intr */
		himci_trace(2, "command is non-blocking");
		ent_des->ctrl = 0x80000000;
	} else {
		himci_trace(2, "command is block");
		ent_des->ctrl = 0xa0000000;
	}
	himci_trace(2, "ent des addr: 0x%x", host->entire_paddr
			+ sizeof(struct himci_entire_des) * wr_pos);

	/* select command descriptor */
	cmd_des = (struct himci_cmd_des *)(host->cmd_addr);
	cmd_des = cmd_des + (PERF_OPT_RATIO * 256 * wr_pos);
	host->wr_cmd_des = cmd_des;
}

static void himci_request(struct mmc_host *mmc, struct mmc_request *mrq)
{
	struct himci_host *host = mmc_priv(mmc);
	unsigned int byte_cnt = 0;
	unsigned int status;
	int ret = 0, i = 0;
	struct himci_cmd_des	*cmd_des;
	unsigned int rd_pos;

	himci_trace(2, "begin");
	himci_assert(mmc);
	himci_assert(mrq);
	himci_assert(host);

	host->mrq = mrq;
	host->cmd = mrq->cmd;
	host->data = mrq->data;

	/* prepare entire des */
	himci_prep_entire(host, mrq);
	cmd_des = host->wr_cmd_des;

	/* prepare data des */
	if (mrq->data) {
		ret = himci_prep_data(host, mrq->data);
		if (ret) {
			mrq->data->error = ret;
			himci_trace(3, "data setup is error!");
			goto request_done;
		}

		byte_cnt = mrq->data->blksz * mrq->data->blocks;
		cmd_des->blk_sz	= mrq->data->blksz;
		cmd_des->byte_cnt = byte_cnt;
		himci_trace(3, "blk_sz:%d blk_cnt:%d",
				mrq->data->blksz, mrq->data->blocks);
	} else {
		cmd_des->blk_sz	= 0;
		cmd_des->byte_cnt = 0;
	}

	/* prepare command des */
	ret = himci_prep_cmd(host, mrq->cmd, mrq->data);
	if (ret)
		goto request_done;

	if (mmc->caps & MMC_CAP_SD_HIGHSPEED) {
		status = himci_sys_card_detect(host);
		if (status == CARD_UNPLUGED) {
			for (i = 0; i < ADMA_QUEUE_DEEPTH ; i++)
				host->pmrq[i] = NULL;
			host->cmd->error = -ETIMEDOUT;
			if (host->cmd->flags & MMC_CMD_TYPE_RW)
				host->mmc->status = MMC_HOST_ERR;
			goto request_done;
		}
	}

	/* update des write pointer */
	host->wr_pos++;
	host->wr_pos %= ADMA_QUEUE_DEEPTH;
	himci_writel(host->wr_pos, host->base + ADMA_Q_WRPTR);

	if (host->cmd->opcode == SD_SWITCH_VOLTAGE) {
		ret = himci_wait_voltage_switch(host);
		if (ret)
			himci_trace(5, "voltage switch failed!");
		rd_pos = host->wr_pos ? host->wr_pos - 1
			: ADMA_QUEUE_DEEPTH - 1;
		host->pmrq[rd_pos] = NULL;
		goto request_done;
	}

	if (!(host->cmd->flags & MMC_CMD_TYPE_RW)) {
		long time;

		time = wait_event_timeout(host->intr_wait, host->cmd_done, HZ);
		if (time <= 0) {
			unsigned int rstat;
			unsigned int qstat;
			unsigned int cmd_err;
			unsigned int data_err;

			rstat = himci_readl(host->base + MCI_RINTSTS);
			qstat = himci_readl(host->base + MCI_IDSTS);
			himci_err_check(host, qstat, rstat,
					&cmd_err, &data_err);
			mrq->cmd->error = -ETIMEDOUT;
			himci_trace(5, "CMD%u wait event timeout", mrq->cmd->opcode);
		}

		host->cmd_done = 0;
		rd_pos = host->wr_pos ? host->wr_pos - 1
			: ADMA_QUEUE_DEEPTH - 1;
		host->pmrq[rd_pos] = NULL;
		himci_read_response(host, mrq->cmd);
		goto request_done;
	}
	return;

request_done:
	himci_finish_request(host, mrq);
}

static int himci_do_voltage_switch(struct himci_host *host,
						 struct mmc_ios *ios)
{
	struct mmc_host *mmc = host->mmc;
	u32 ctrl;
	unsigned int port = host->cur_port;
	/*
	 * We first check whether the request is to set signalling voltage
	 * to 3.3V. If so, we change the voltage to 3.3V and return quickly.
	 */
	ctrl = himci_readl(host->base + MCI_UHS_REG);
	if (ios->signal_voltage == MMC_SIGNAL_VOLTAGE_330) {
		/* Set 1.8V Signal Enable in the MCI_UHS_REG to 1 */
		himci_trace(3, "switch voltage 330");
		ctrl &= ~(HI_SDXC_CTRL_VDD_180 << port);
		himci_writel(ctrl, host->base + MCI_UHS_REG);

		/* Wait for 5ms */
		usleep_range(5000, 5500);

		/* 3.3V regulator output should be stable within 5ms */
		ctrl = himci_readl(host->base + MCI_UHS_REG);
		if (!(ctrl & (HI_SDXC_CTRL_VDD_180 << port))) {
			return 0;
		} else {
			himci_error(": Switching to 3.3V ");
			himci_error("signalling voltage failed\n");
			return -EIO;
		}
	} else if (!(ctrl & (HI_SDXC_CTRL_VDD_180 << port)) &&
		(ios->signal_voltage == MMC_SIGNAL_VOLTAGE_180)) {
		/* Stop SDCLK */
		himci_trace(3, "switch voltage 180");
		himci_control_cclk(host, DISABLE);

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
			himci_control_cclk(host, ENABLE);
			usleep_range(1000, 1500);

			if (mmc->caps2 & MMC_CAP2_HS200) {
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
				himci_writel(VOLT_SWITCH_INT_STATUS | CD_INT_STATUS,
						host->base + MCI_RINTSTS);
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

		himci_ctrl_power(host, POWER_OFF, FORCE_DISABLE);
		/* Wait for 1ms as per the spec */
		usleep_range(1000, 1500);
		himci_ctrl_power(host, POWER_ON, FORCE_DISABLE);

		himci_control_cclk(host, DISABLE);
		/* Wait for 1ms as per the spec */
		usleep_range(1000, 1500);
		himci_control_cclk(host, ENABLE);

		himci_error(": Switching to 1.8V signalling ");
		himci_error("voltage failed, retrying with S18R set to 0\n");
		return -EAGAIN;
	} else
		/* No signal voltage switch required */
		return 0;
}

static int himci_start_signal_voltage_switch(struct mmc_host *mmc,
					      struct mmc_ios *ios)
{
	struct himci_host *host = mmc_priv(mmc);
	int err;

	err = himci_do_voltage_switch(host, ios);
	return err;
}

static int himci_send_stop(struct mmc_host *host)
{
	struct mmc_command cmd = {0};
	int err;

	cmd.opcode = MMC_STOP_TRANSMISSION;
	cmd.flags = MMC_RSP_SPI_R1B | MMC_RSP_R1B | MMC_CMD_AC;
	err = mmc_wait_for_cmd(host, &cmd, 0);
	return err;
}

#if defined(CONFIG_ARCH_HI3516CV300)
static void himci_edge_tuning_enable(struct himci_host *host)
{
	unsigned int val;

	val = himci_readl(host->base + MCI_TUNING_CTRL);
	val |= HW_TUNING_EN;
	himci_writel(val, host->base + MCI_TUNING_CTRL);
}

static void himci_edge_tuning_disable(struct himci_host *host)
{
	unsigned int val;

	val = himci_readl(host->base + MCI_TUNING_CTRL);
	val &= ~HW_TUNING_EN;
	himci_writel(val, host->base + MCI_TUNING_CTRL);
}

static void himci_set_sap_phase(struct himci_host *host, u32 phase)
{
	unsigned int val;
	unsigned int phase_a;

	phase_a = phase >= 2 ? phase - 2 : phase + 6;
	val = himci_readl(host->base + MCI_UHS_REG_EXT);
	val &= ~(CLK_SMPL_PHS_MASK | CLK_SMPLA_PHS_MASK);
	val |= (phase << CLK_SMPL_PHS_SHIFT) | (phase_a << CLK_SMPLA_PHS_SHIFT);
	himci_writel(val, host->base + MCI_UHS_REG_EXT);
	val = himci_readl(host->base + MCI_UHS_REG_EXT);
}

static int himci_execute_edge_tuning(struct mmc_host *mmc, u32 opcode)
{
	struct himci_host *host = mmc_priv(mmc);
	unsigned int index, val;
	unsigned int found = 0, prev_found = 0, prev_point = 0;
	unsigned int start_point = NOT_FOUND, end_point = NOT_FOUND;
	unsigned int phase = 0;

	himci_trace(3, "begin");

	himci_edge_tuning_enable(host);

	for (index = 0; index < HIMCI_PHASE_SCALE; index++) {
		himci_set_sap_phase(host, index);

		mmc_send_tuning(mmc, opcode, NULL);

		himci_send_stop(mmc);

		val = himci_readl(host->base + MCI_TUNING_CTRL);
		found = val & FOUND_EDGE;

		himci_trace(3, "try phase:%02d, found:0x%x\n", index, found);

		if (prev_found && !found) {
			end_point = prev_point;
		} else if (!prev_found && found) {
			if (index != 0)
				start_point = index;
		}
		if ((start_point != NOT_FOUND) && (end_point != NOT_FOUND))
			goto scan_out;

		prev_point = index;
		prev_found = found;
		found = 0;
	}

scan_out:
	if ((start_point == NOT_FOUND) && (end_point == NOT_FOUND)) {
		himci_trace(5, "%s: no valid phase shift! use default",
				mmc_hostname(mmc));
		return 0;
	}

	if (start_point == NOT_FOUND)
		start_point = end_point;

	if (end_point == NOT_FOUND)
		end_point = start_point;

	pr_info("tuning %s: found edge on (s:%d, e:%d)",
			mmc_hostname(mmc), start_point, end_point);

	if (start_point > end_point)
		end_point += HIMCI_PHASE_SCALE;

	phase = ((start_point + end_point) / 2) % HIMCI_PHASE_SCALE;

	phase += HIMCI_PHASE_SCALE / 2;
	phase %= HIMCI_PHASE_SCALE;

	himci_set_sap_phase(host, phase);

	himci_edge_tuning_disable(host);

	himci_writel(ALL_INT_CLR, host->base + MCI_RINTSTS);

	pr_info("determing final phase %d\n", phase);

	return 0;
}
#else
#if defined(CONFIG_ARCH_HI3519V101) || defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556) || \
	defined(CONFIG_ARCH_HI3516AV200)
static int himci_set_sd_drv_strength(struct mmc_host *mmc,
		unsigned int drv_type)
{
	struct mmc_request mrq = {NULL};
	struct mmc_command cmd = {0};
	struct mmc_data data = {0};
	struct scatterlist sg;
	u8 *status;

	status = kmalloc(64, GFP_KERNEL);
	if (!status)
			return -ENOMEM;

	drv_type &= 0xF;

	mrq.cmd = &cmd;
	mrq.data = &data;

	cmd.opcode = SD_SWITCH;
	cmd.arg = 0x1 << 31 | 0x00FFFFFF;
	cmd.arg &= ~(0xF << 8);
	cmd.arg |= drv_type << 8;
	cmd.flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;

	data.blksz = 64;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;
	data.sg = &sg;
	data.sg_len = 1;

	sg_init_one(&sg, status, 64);

	mmc_wait_for_req(mmc, &mrq);

	kfree(status);
	if (cmd.error)
		    return cmd.error;
	if (data.error)
		    return data.error;

	return 0;
}

static int himci_set_mmc_drv_strength(struct mmc_host *mmc,
		unsigned int drv_type)
{
	int err;
	struct mmc_command cmd = {0};
	unsigned int value;

	drv_type &= 0xF;
	value = EXT_CSD_TIMING_HS200 | (drv_type << 4);

	cmd.opcode = MMC_SWITCH;
	cmd.arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
		(EXT_CSD_HS_TIMING << 16) |
		(value << 8) |
		EXT_CSD_CMD_SET_NORMAL;
	cmd.flags = MMC_CMD_AC | MMC_RSP_SPI_R1B | MMC_RSP_R1B;

	err = mmc_wait_for_cmd(mmc, &cmd, 0);

	return err;
}
#endif
static void himci_set_sap_phase(struct himci_host *host,
		u32 phase, u32 hs400_tuning)
{
	unsigned int reg_value;
	unsigned long flags;

	spin_lock_irqsave(&host->lock, flags);
	if (hs400_tuning) {
		phase = (phase < 8) ? (8 - phase) : phase;
		reg_value = himci_readl(EMMC_DLL_CTRL);
		reg_value &= ~0xF;
		reg_value |= phase;
		himci_writel(reg_value, EMMC_DLL_CTRL);
		host->dll_phase = phase;
	} else {
		reg_value = himci_readl(host->base + MCI_UHS_REG_EXT);
		reg_value &= ~CLK_SMPL_PHS_MASK;
		reg_value |= (phase << CLK_SMPL_PHS_SHIFT);
		himci_writel(reg_value, host->base + MCI_UHS_REG_EXT);
		host->phase = phase;
	}
	spin_unlock_irqrestore(&host->lock, flags);
}

/*
 * The procedure of tuning the phase shift of sampling clock
 *
 * 1.Set a phase shift of 0бу on cclk_in_sample
 * 2.Send the Tuning command to the card
 * 3.increase the phase shift value of cclk_in_sample until the
 *   correct sampling point is received such that the host does not
 *   see any of the errors.
 * 4.Mark this phase shift value as the starting point of the sampling
 *   window.
 * 5.increase the phase shift value of cclk_in_sample until the host
 *   sees the errors starting to come again or the phase shift value
 *   reaches 360бу.
 * 6.Mark the last successful phase shift value as the ending
 *   point of the sampling window.
 *
 *     A window is established where the tuning block is matched.
 * For example, for a scenario where the tuning block is received
 * correctly for a phase shift window of 90буand 180бу, then an appropriate
 * sampling point is established as 135бу. Once a sampling point is
 * established, no errors should be visible in the tuning block.
 *
 */
static int himci_execute_tuning(struct mmc_host *mmc, u32 opcode)
{
	struct himci_host *host;
	unsigned int index, count;
	unsigned int err = 0;
	unsigned int found = 0; /* identify if we have found a valid phase */
	unsigned int start_point;
	unsigned int end_point;
	unsigned int prev_err = NOT_FOUND;
	unsigned int raise_point = NOT_FOUND;
	unsigned int fall_point = NOT_FOUND;
	unsigned int hs400_tuning;
	int phase, ret, send_cmd_num = 1;
#if defined(CONFIG_ARCH_HI3519V101) || defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556) || \
	defined(CONFIG_ARCH_HI3516AV200)
	int retry = 0;
#endif

	if (opcode == MMC_SEND_EXT_CSD) {
		start_point = HS400_TUNING_START_PHASE;
		end_point = HS400_TUNING_END_PHASE;
		hs400_tuning = 1;
	} else {
		start_point = TUNING_START_PHASE;
		end_point = TUNING_END_PHASE;
		hs400_tuning = 0;
	}

	host = mmc_priv(mmc);

	himci_trace(3, "start sd3.0 phase tuning...");

#if defined(CONFIG_ARCH_HI3519V101) || defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556) || \
	defined(CONFIG_ARCH_HI3516AV200)
	if (!hs400_tuning && (mmc->caps & MMC_CAP_UHS_SDR104))
		himci_set_sd_drv_strength(mmc, SD_STRENGTH_TYPE_C);
try_again:
#endif

	for (index = start_point; index <= end_point; index++) {
		/* set sample clk phase shift */
		himci_set_sap_phase(host, index, hs400_tuning);

		count = 0;
		do {
			if (hs400_tuning) {
				ret = mmc_send_dll_tuning(mmc);
			} else {
				ret = mmc_send_tuning(mmc, opcode, NULL);
				himci_send_stop(mmc); /* send soft_stop tail */
			}

			if (ret) {
				himci_trace(3, "send tuning CMD%u fail! phase:%d err:%d\n",
						opcode, index, ret);
				err = 1;
				break;
			}
			count++;
		} while (count < send_cmd_num);

		if (!err)
			found = 1;	/* found a valid phase */

		if (index > start_point) {
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

#if defined(CONFIG_ARCH_HI3519V101) || defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556) || \
	defined(CONFIG_ARCH_HI3516AV200)
	if ((NOT_FOUND == raise_point) && (NOT_FOUND == fall_point)
			&& !hs400_tuning && !retry) {
		if (mmc->caps & MMC_CAP_UHS_SDR104)
			himci_set_sd_drv_strength(mmc, SD_STRENGTH_TYPE_D);
		else if (mmc->caps2 & MMC_CAP2_HS200)
			himci_set_mmc_drv_strength(mmc, MMC_STRENGTH_TYPE_3);

		prev_err = NOT_FOUND;
		send_cmd_num = 40;
		retry = 1;
		goto try_again;
	}
#endif

tuning_out:
	if (!found) {
		himci_trace(5, "%s: no valid phase shift! use default",
				mmc_hostname(mmc));
		if (!hs400_tuning)
			himci_set_sap_phase(host, DEFAULT_SMPL_PHASE, hs400_tuning);
	} else {
		himci_trace(3, "Tuning finished!!");

		if (NOT_FOUND == raise_point)
			raise_point = start_point;
		if (NOT_FOUND == fall_point)
			fall_point = end_point;

		if (fall_point < raise_point) {
#ifdef CONFIG_ARCH_HI3519
			if (fall_point - start_point > end_point - raise_point)
				phase = (fall_point + start_point) / 2;
			else
				phase = (end_point + raise_point) / 2;
#else
			phase = (raise_point + fall_point) / 2;
			phase = phase - (HIMCI_PHASE_SCALE / 2);
			phase = (phase < 0) ? (HIMCI_PHASE_SCALE + phase) : phase;
#endif
		} else
			phase = (raise_point + fall_point) / 2;

		himci_set_sap_phase(host, phase, hs400_tuning);

		pr_info("%stuning %s: valid phase shift [%d, %d] Final Phase %d\n",
			hs400_tuning ? "HS400 " : "", mmc_hostname(mmc),
			raise_point, fall_point, phase);
	}

#if defined(CONFIG_ARCH_HI3519V101) || defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556) || \
	defined(CONFIG_ARCH_HI3516AV200)
	if (retry) {
		if (mmc->caps & MMC_CAP_UHS_SDR104)
			himci_set_sd_drv_strength(mmc, SD_STRENGTH_TYPE_C);
		else if (mmc->caps2 & MMC_CAP2_HS200)
			himci_set_mmc_drv_strength(mmc, MMC_STRENGTH_TYPE_0);
	}
#endif
	return 0;
}
#endif

static void himci_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct himci_host *host = mmc_priv(mmc);
	unsigned int tmp_reg;
	unsigned int port = host->cur_port;
	u32 ctrl = 0;

	himci_trace(2, "begin");
	himci_assert(mmc);
	himci_assert(ios);
	himci_assert(host);

	himci_trace(3, "ios->power_mode = %d ", ios->power_mode);
	if (!ios->clock)
		himci_control_cclk(host, DISABLE);

	switch (ios->power_mode) {
	case MMC_POWER_OFF:
		himci_ctrl_power(host, POWER_OFF, FORCE_DISABLE);
		break;
	case MMC_POWER_UP:
	case MMC_POWER_ON:
		himci_ctrl_power(host, POWER_ON, FORCE_DISABLE);
		break;
	}
	himci_trace(3, "ios->clock = %d ", ios->clock);
	if (ios->clock) {
		himci_control_cclk(host, DISABLE);
		himci_set_cclk(host, ios->clock);
		himci_control_cclk(host, ENABLE);

		/* speed mode check ,if it is DDR50 set DDR mode */
		if (ios->timing == MMC_TIMING_UHS_DDR50) {
			ctrl = himci_readl(host->base + MCI_UHS_REG);
			if (!((HI_SDXC_CTRL_DDR_REG << port) & ctrl)) {
				ctrl |= (HI_SDXC_CTRL_DDR_REG << port);
				himci_writel(ctrl, host->base + MCI_UHS_REG);
			}
		}

		if (ios->timing == MMC_TIMING_MMC_HS400) {
			ctrl = himci_readl(host->base + MCI_EMMC_DDR_REG);
			if (!(HI_EMMC_HS400_MODE & ctrl)) {
				ctrl |= HI_EMMC_HS400_MODE;
				himci_writel(ctrl,
					host->base + MCI_EMMC_DDR_REG);
			}
		}
	} else {
		himci_control_cclk(host, DISABLE);
		if (ios->timing != MMC_TIMING_UHS_DDR50) {
			ctrl = himci_readl(host->base + MCI_UHS_REG);
			if ((HI_SDXC_CTRL_DDR_REG << port) & ctrl) {
				ctrl &= ~(HI_SDXC_CTRL_DDR_REG << port);
				himci_writel(ctrl, host->base + MCI_UHS_REG);
			}
		}

		if (ios->timing != MMC_TIMING_MMC_HS400) {
			ctrl = himci_readl(host->base + MCI_EMMC_DDR_REG);
			if (HI_EMMC_HS400_MODE & ctrl) {
				ctrl &= ~HI_EMMC_HS400_MODE;
				himci_writel(ctrl,
					host->base + MCI_EMMC_DDR_REG);
			}
		}
	}

	himci_set_drv_cap(host, ios);

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
}

static void himci_enable_sdio_irq(struct mmc_host *mmc, int enable)
{
	struct himci_host *host = mmc_priv(mmc);
	unsigned int reg_value;

	reg_value = himci_readl(host->base + MCI_INTMASK);
	if (enable)
		reg_value |= SDIO_INT_MASK;
	else
		reg_value &= ~SDIO_INT_MASK;
	himci_writel(reg_value, host->base + MCI_INTMASK);
}

static int himci_get_card_detect(struct mmc_host *mmc)
{
	unsigned ret;
	struct himci_host *host = mmc_priv(mmc);

	himci_trace(2, "begin");
	ret = himci_sys_card_detect(host);

	if (ret)
		return 0;
	else
		return 1;
}

static int himci_get_ro(struct mmc_host *mmc)
{
	unsigned ret;
	struct himci_host *host = mmc_priv(mmc);

	himci_trace(2, "begin");
	himci_assert(mmc);

	ret = himci_ctrl_card_readonly(host);

	return ret;
}

static unsigned int himci_get_rdptr(struct mmc_host *mmc)
{
	unsigned int cur;
	struct himci_host *host = mmc_priv(mmc);

	himci_trace(2, "begin");
	himci_assert(mmc);

	cur = himci_readl(host->base + ADMA_Q_RDPTR);

	return cur;
}

static void himci_hw_reset(struct mmc_host *mmc)
{
	unsigned int reg_value;
	struct himci_host *host = mmc_priv(mmc);
	unsigned int port = host->cur_port;

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

static int himci_select_drv_strength(unsigned int max_dtr,
		int host_drv, int card_drv)
{
	if ((host_drv & SD_DRIVER_TYPE_C) &&
			(card_drv & SD_DRIVER_TYPE_C))
		return SD_STRENGTH_TYPE_C;
	else
		return SD_STRENGTH_TYPE_B;
}

static int himci_card_info_save(struct mmc_host *mmc)
{
	struct mmc_card *card = mmc->card;
	struct himci_host * host= mmc_priv(mmc);
	struct card_info * c_info = &host->c_info;

	if (!card) {
		memset(c_info,0,sizeof(struct card_info));
		c_info->card_connect = CARD_DISCONNECT;
		goto out;
	}

	c_info->card_type = card->type;
	c_info->card_state = card->state;

	c_info->timing = mmc->ios.timing;
	c_info->card_support_clock = mmc->ios.clock;

	c_info->sd_bus_speed = card->sd_bus_speed;

	memcpy(c_info->ssr, card->c_ssr, ARRAY_SIZE(c_info->ssr));

	c_info->card_connect = CARD_CONNECT;
out:
	return 0;
}

static const struct mmc_host_ops himci_ops = {
	.request = himci_request,
	.post_req = himci_data_done,
	.set_ios = himci_set_ios,
	.get_rd = himci_get_rdptr,
	.get_ro = himci_get_ro,
	.start_signal_voltage_switch = himci_start_signal_voltage_switch,
#if defined(CONFIG_ARCH_HI3516CV300)
	.execute_tuning	= himci_execute_edge_tuning,
#else
	.execute_tuning	= himci_execute_tuning,
#endif
	.enable_sdio_irq = himci_enable_sdio_irq,
	.select_drive_strength = himci_select_drv_strength,
	.hw_reset = himci_hw_reset,
	.sw_reset = himci_sw_reset,
	.get_cd = himci_get_card_detect,
	.card_info_save = himci_card_info_save,
};

static void himci_check_sdio_irq(struct himci_host *host, unsigned int rstat)
{
	u32 mstate;

	if ((host->mmc->card != NULL)
			&& (host->mmc->card->type == MMC_TYPE_SDIO)) {
		mstate = himci_readl(host->base + MCI_INTMASK);
		if ((rstat & SDIO_INT_STATUS) && (mstate & SDIO_INT_MASK)) {
			spin_lock(&host->lock);
			himci_writel(SDIO_INT_STATUS,
					host->base + MCI_RINTSTS);
			spin_unlock(&host->lock);
			mmc_signal_sdio_irq(host->mmc);
		}
	}
}

static unsigned int himci_get_stable_rd_pos(struct himci_host *host,
		unsigned int qstat, unsigned int rstat)
{
	unsigned int rd_pos;
	unsigned int fsm_stat;
	volatile unsigned int new_pos;
	volatile unsigned int new_fsm;
	unsigned int count = 0;
	int try = 1000;

	rd_pos = himci_readl(host->base + ADMA_Q_RDPTR);
	fsm_stat = himci_readl(host->base + MCI_IDSTS);
	while (try > 0) {
		new_pos = himci_readl(host->base + ADMA_Q_RDPTR);
		new_fsm = himci_readl(host->base + MCI_IDSTS);
		if ((rd_pos == new_pos) && (fsm_stat == new_fsm)) {
			count++;
		} else {
			rd_pos = new_pos;
			fsm_stat = new_fsm;
			count = 0;
		}

		if (count == 3)
			break;
		try--;
	}

	/* when error occur, whether the read ptr jump next is depend on
	 * host's state machine when CES && ADMA3_FSM=4 && FSM!=0,
	 *rdptr don't jump next
	 */
	if (!(qstat & PACKET_TO_INT)) {
		if (!((qstat & CES) &&
			(((fsm_stat >> ADMA3_FSM_SHIFT) & 0xF) == 4) &&
			((fsm_stat >> FSM_SHIFT) & 0xF)))
			rd_pos = rd_pos ? (rd_pos - 1)
				: (ADMA_QUEUE_DEEPTH - 1);
	}

	return rd_pos;
}

static irqreturn_t hisd_irq(int irq, void *dev_id)
{
	struct himci_host *host = dev_id;
	struct mmc_request  *mrq;
	u32 rd_pos = 0;
	u32 rstat = 0;
	u32 qstat = 0;
	int cmd_err = 0;
	int data_err = 0;

	rstat = himci_readl(host->base + MCI_RINTSTS); /* read RAW intr */
	qstat = himci_readl(host->base + MCI_IDSTS); /* clear ADMA3 intr */
	himci_writel(qstat, host->base + MCI_IDSTS);
	himci_trace(3, "queue state:0x%x raw state:0x%x", qstat, rstat);

	if (!(qstat & ADMA_INT_ALL) && !(rstat & SDIO_INT_STATUS)) {
		himci_trace(5, "##### no irq, should never be here! ######");
		return IRQ_NONE;
	}

	/* bugfix: when send soft stop to SD Card, Host will report
	   sdio interrupt, This situation needs to be avoided */
	if (host->mmc->caps & MMC_CAP_SDIO_IRQ)
		himci_check_sdio_irq(host, rstat);

	rd_pos = himci_get_stable_rd_pos(host, qstat, rstat);

	mrq = host->pmrq[rd_pos];
	himci_err_check(host, qstat, rstat, &cmd_err, &data_err);

	host->pmrq[rd_pos] = NULL;
	if (!mrq || !mrq->cmd) {
		himci_trace(4, "##################");
		himci_trace(4, "queue state:0x%x raw state:0x%x", qstat, rstat);
		himci_trace(4, "current rd:%d", rd_pos);
		return IRQ_HANDLED;
	}

	if (cmd_err || data_err) {
		if (mrq->cmd->flags & MMC_CMD_TYPE_RW) {
			himci_trace(5, "R/W CMD%u error, qstat:0x%x rstat:0x%x",
					mrq->cmd->opcode, qstat, rstat);
			host->error_count++;
			host->mmc->status = MMC_HOST_ERR;
		}
		mrq->cmd->error = cmd_err;
		if (mrq->data)
			mrq->data->error = data_err;
	}

	/* finish each mrq */
	if (mrq->cmd->flags & MMC_CMD_TYPE_RW) {
		himci_read_response(host, mrq->cmd);
		mmc_request_done(host->mmc, mrq);
	} else {
		host->cmd_done = 1;
		wake_up(&host->intr_wait);
	}

	return IRQ_HANDLED;
}

static int himci_of_parse(struct device_node *np, struct mmc_host *mmc)
{
	struct himci_host *host = mmc_priv(mmc);
	int ret = mmc_of_parse(mmc);
	int len;

	if (ret)
		return ret;

	if (of_property_read_u32(np, "min-frequency", &mmc->f_min))
		mmc->f_min = MMC_CCLK_MIN;

	if (of_property_read_u32(np, "devid", &host->devid))
		return -EINVAL;

	if (of_find_property(np, "cap-mmc-hw-reset", &len))
		mmc->caps |= MMC_CAP_HW_RESET;

	return 0;
}

static int __init himci_probe(struct platform_device *pdev)
{
	struct mmc_host *mmc;
	struct himci_host *host = NULL;
	struct resource *host_ioaddr_res = NULL;
	int ret = 0, irq;
	struct device_node *np = pdev->dev.of_node;

	himci_trace(2, "begin");
	pr_info("mmc host probe\n");
	himci_assert(pdev);

	mmc = mmc_alloc_host(sizeof(struct himci_host), &pdev->dev);
	if (!mmc) {
		himci_error("no mem for hi mci host controller!\n");
		ret = -ENOMEM;
		goto out;
	}

	platform_set_drvdata(pdev, mmc);

	mmc->ops = &himci_ops;

	host_ioaddr_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (NULL == host_ioaddr_res) {
		himci_error("no ioaddr rescources config!\n");
		ret = -ENODEV;
		goto out;
	}

	if (himci_of_parse(np, mmc)) {
		himci_error("failed to parse mmc dts!\n");
		ret = -EINVAL;
		goto out;
	}

	/* reload by this controller */
	mmc->max_blk_count = 2048;
	mmc->max_segs = MAX_SEGS;
	mmc->max_seg_size = mmc->max_blk_size * mmc->max_blk_count;
	mmc->max_req_size = mmc->max_blk_size * mmc->max_blk_count;
	mmc->ocr_avail = MMC_VDD_32_33 | MMC_VDD_33_34;
	mmc->status = MMC_HOST_OK;

	host = mmc_priv(mmc);
	mci_host[slot_index++] = host;
	pdev->id = host->devid;
	host->pdev = pdev;
	host->mmc = mmc;

	/* ADMA3 buffer alloc */
	host->entire_addr = dma_alloc_coherent(&pdev->dev, PAGE_SIZE,
			&host->entire_paddr, GFP_KERNEL);
	if (!host->entire_addr) {
		himci_error("no mem for himci dma!\n");
		ret = -ENOMEM;
		goto out;
	}

	host->cmd_addr = dma_alloc_coherent(&pdev->dev,
			PERF_OPT_RATIO * PAGE_SIZE * ADMA_QUEUE_DEEPTH,
			&host->cmd_paddr, GFP_KERNEL);
	if (!host->cmd_addr) {
		himci_error("no mem for himci dma!\n");
		ret = -ENOMEM;
		goto out;
	}

	host->base = devm_ioremap_resource(&pdev->dev, host_ioaddr_res);
	if (IS_ERR_OR_NULL(host->base)) {
		himci_error("no mem for himci base!\n");
		ret = -ENOMEM;
		goto out;
	}

	spin_lock_init(&host->lock);

	host->clk = devm_clk_get(&pdev->dev, "mmc_clk");
	if (IS_ERR_OR_NULL(host->clk)) {
		himci_error("get clock fail.\n");
		ret = PTR_ERR(host->clk);
		goto out;
	}

	clk_prepare_enable(host->clk);

	host->power_status = POWER_OFF;
	host->cur_port = 0;

	/* enable card */
	himci_init_host(host);
	host->card_status = himci_sys_card_detect(host);

	if (mmc->caps & MMC_CAP_SD_HIGHSPEED) {
		init_timer(&host->timer);
		host->timer.function = himci_detect_card;
		host->timer.data = (unsigned long)host;
		host->timer.expires = jiffies + detect_time;
		add_timer(&host->timer);
	}

	host->cmd_done = 0;
	init_waitqueue_head(&host->intr_wait);
	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		goto out;

	host->irq = irq;
	ret = request_irq(irq, hisd_irq, 0, DRIVER_NAME, host);
	if (ret)
		goto out;

	mmc_add_host(mmc);
	return 0;
out:
	if (host) {
		if (mmc->caps & MMC_CAP_SD_HIGHSPEED)
			del_timer(&host->timer);

		if (host->base)
			devm_iounmap(&pdev->dev, host->base);

		if (host->entire_addr)
			dma_free_coherent(&pdev->dev, PAGE_SIZE,
					host->entire_addr, host->entire_paddr);
		if (host->cmd_addr)
			dma_free_coherent(&pdev->dev,
					PERF_OPT_RATIO * PAGE_SIZE * ADMA_QUEUE_DEEPTH,
					host->cmd_addr, host->cmd_paddr);
	}
	if (mmc)
		mmc_free_host(mmc);
	return ret;
}

static int __exit himci_remove(struct platform_device *pdev)
{
	struct mmc_host *mmc = platform_get_drvdata(pdev);

	himci_trace(2, "begin");
	himci_assert(pdev);

	platform_set_drvdata(pdev, NULL);

	if (mmc) {
		struct himci_host *host = mmc_priv(mmc);

		mmc_remove_host(mmc);
		free_irq(host->irq, host);
		if (mmc->caps & MMC_CAP_SD_HIGHSPEED)
			del_timer_sync(&host->timer);
		himci_ctrl_power(host, POWER_OFF, FORCE_DISABLE);
		himci_control_cclk(host, DISABLE);
		devm_iounmap(&pdev->dev, host->base);
		dma_free_coherent(&pdev->dev, PAGE_SIZE,
				host->entire_addr, host->entire_paddr);
		dma_free_coherent(&pdev->dev,
				PERF_OPT_RATIO * PAGE_SIZE * ADMA_QUEUE_DEEPTH,
				host->cmd_addr, host->cmd_paddr);
		mmc_free_host(mmc);
	}
	return 0;
}

static void himci_shutdown(struct platform_device *pdev)
{
	struct mmc_host *mmc = platform_get_drvdata(pdev);

	himci_trace(3, "shutdown");
	if (mmc) {
		unsigned int val;
		struct himci_host *host = mmc_priv(mmc);

		/* bugfix: host reset can trigger error intr */
		himci_writel(0, host->base + MCI_IDINTEN);
		himci_writel(0, host->base + MCI_INTMASK);

		val = himci_readl(host->base + MCI_CTRL);
		val |= CTRL_RESET | FIFO_RESET | DMA_RESET;
		himci_writel(val, host->base + MCI_CTRL);

		himciv200_host_power(host, POWER_OFF, FORCE_DISABLE);
	}
}

#ifdef CONFIG_PM
static int himciv200_pltm_suspend(struct platform_device *pdev,
		pm_message_t state)
{
	struct mmc_host *mmc = platform_get_drvdata(pdev);
	struct himci_host *host;
	int ret = 0;

	if (mmc) {
		host = mmc_priv(mmc);
		if (mmc->caps & MMC_CAP_SD_HIGHSPEED)
			del_timer_sync(&host->timer);

		if (__clk_is_enabled(host->clk))
			clk_disable_unprepare(host->clk);
	}

	return ret;
}
/******************************************************************************/

static int himciv200_pltm_resume(struct platform_device *pdev)
{
	struct mmc_host *mmc = platform_get_drvdata(pdev);
	struct himci_host *host;
	int ret = 0;

	if (mmc) {
		host = mmc_priv(mmc);

		if (!__clk_is_enabled(host->clk))
			clk_prepare_enable(host->clk);

		himci_init_host(host);

		if (mmc->caps & MMC_CAP_SD_HIGHSPEED)
			add_timer(&host->timer);
	}

	return ret;
}
#else
#define himciv200_pltm_suspend    NULL
#define himciv200_pltm_resume     NULL
#endif

void himci_mmc_rescan(int slot)
{
	struct mmc_host *mmc;
	struct himci_host *host;

	host = mci_host[slot];
	if (!host || !host->mmc) {
		himci_trace(5, "mmc%d: invalid slot!\n", slot);
		return;
	}

	mmc = host->mmc;
	if (mmc->caps & MMC_CAP_SD_HIGHSPEED)
		del_timer_sync(&host->timer);

	mmc_remove_host(mmc);

	mmc_add_host(mmc);

	if (mmc->caps & MMC_CAP_SD_HIGHSPEED)
		add_timer(&host->timer);
}
EXPORT_SYMBOL(himci_mmc_rescan);

static const struct of_device_id
himciv200_match[] __maybe_unused = {
	{.compatible = "hisilicon,hi3516cv300-himciv200"},
	{.compatible = "hisilicon,hi3519-himciv200"},
	{.compatible = "hisilicon,hi3516av200-himciv200"},
	{.compatible = "hisilicon,hi3559-himciv200"},
	{.compatible = "hisilicon,hi3556-himciv200"},
	{},
};

static struct platform_driver himci_driver = {
	.probe = himci_probe,
	.remove = himci_remove,
	.shutdown = himci_shutdown,
	.suspend = himciv200_pltm_suspend,
	.resume = himciv200_pltm_resume,
	.driver = {
		   .name = DRIVER_NAME,
		   .owner = THIS_MODULE,
		   .of_match_table = of_match_ptr(himciv200_match),
		   },
};

static int __init himci_init(void)
{
	int ret;

	himci_trace(2, "begin");

	/*
	 * We should register SDIO1 first to make sure that
	 * the eMMC device,which connected to SDIO1 is mmcblk0.
	 */

	ret = platform_driver_register(&himci_driver);
	if (ret) {
		platform_driver_unregister(&himci_driver);
		himci_error("Himci driver register failed!");
		return ret;
	}

	/* device proc entry */
	ret = mci_proc_init(HIMCI_SLOT_NUM);
	if (ret)
		himci_error("device proc init is failed!");

	return ret;
}

static void __exit himci_exit(void)
{
	himci_trace(2, "begin");

	mci_proc_shutdown();

	platform_driver_unregister(&himci_driver);
}

module_init(himci_init);
module_exit(himci_exit);

#ifdef MODULE
MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("MMC/SD driver for the Hisilicon MMC/SD Host Controller");
MODULE_LICENSE("GPL");
#endif
