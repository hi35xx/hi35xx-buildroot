/*
 *
 * Copyright (c) 2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/clk.h>
#include <linux/reset.h>
#include <mach/io.h>
#include <linux/hidmac.h>

#include "hi_dmac.h"
#include "hidmac_hi3516cv300.h"

#define RX	0
#define TX	1

static int dmac_channel[CHANNEL_NUM] = {0, 1, 2, 3};

int g_channel_status[CHANNEL_NUM];

/* #define DEBUG */

#define DEBUG
#ifdef DEBUG
#define dma_err printk
#else
#define dma_err(fmt, ...) do {} while (0)
#endif

/*
 *Define Memory range
 */
mem_addr mem_num[MEM_MAX_NUM] = {
	{DDRAM_ADRS, DDRAM_SIZE},
	{FLASH_BASE, FLASH_SIZE}
};

typedef void REG_ISR(int *p_dma_chn, int *p_dma_status);
REG_ISR *function[CHANNEL_NUM];

struct hidmac_host {
	struct clk *clk;
	struct reset_control *rstc;
	void __iomem *regbase;

	int irq;
};

void __iomem *dma_regbase;
unsigned int pllihead[2];

#define CLR_INT(i)	((*(unsigned int *)(dma_regbase+0x008)) = (1 << i))

/*
 *	memory address validity check
 *
static int mem_check_valid(unsigned int addr)
{
	unsigned int cnt;

	for (cnt = 0; cnt < MEM_MAX_NUM; cnt++) {
		if ((addr >= mem_num[cnt].addr_base) &&
			(addr <= (mem_num[cnt].addr_base + mem_num[cnt].size)))
			return 0;
	}

	return -1;
} */

/*
 * dmac interrupt handle function
 */
irqreturn_t dmac_isr(int irq, void *dev_id)
{
	struct hidmac_host *dma = dev_id;
	unsigned int channel_status;
	unsigned int channel_tc_status, channel_err_status;
	unsigned int i;

	/*read the status of current interrupt */
	dmac_readw(dma->regbase + DMAC_INTSTATUS, channel_status);

	/*decide which channel has trigger the interrupt*/
	for (i = 0; i < DMAC_MAX_CHANNELS; i++) {
		if ((((channel_status >> i) & 0x1) == 0x01)) {
			/* [HSCP201306240006],l00181524,20130625 */
			/* The INT status should be read first then clear it */
			/* CLR_INT(i); */
			dmac_readw(dma->regbase + DMAC_INTTCSTATUS, channel_tc_status);
			dmac_readw(dma->regbase + DMAC_INTERRORSTATUS, channel_err_status);
			CLR_INT(i);
			/*??HSCP201403110002?? l00183122 20140723*/
			if (g_channel_status[i] == DMAC_CHN_VACANCY
						&& (function[i]) == NULL) {
				if ((0x01 == ((channel_tc_status >> i) & 0x01)))
					dmac_writew(dma->regbase + DMAC_INTTCCLEAR,
								(0x01 << i));
				else if ((0x01 == ((channel_err_status
								>> i)&0x01)))
					dmac_writew(dma->regbase + DMAC_INTERRCLR,
								(0x01 << i));
				continue;
			}

			/* save the current channel transfer */
			/* status to g_channel_status[i] */
			if ((0x01 == ((channel_tc_status >> i) & 0x01))) {
				g_channel_status[i] = DMAC_CHN_SUCCESS;
				dmac_writew(dma->regbase + DMAC_INTTCCLEAR, (0x01 << i));
			} else if ((0x01 == ((channel_err_status >> i)&0x01))) {
				g_channel_status[i] = -DMAC_CHN_ERROR;
				dmac_writew(dma->regbase + DMAC_INTERRCLR, (0x01 << i));
			} else {
				pr_err("Isr Error in DMAC_IntHandeler");
				pr_err("%d! channel\n", i);
			}

			if ((function[i]) != NULL)
				function[i](&i, &g_channel_status[i]);
		}
	}

	return IRQ_RETVAL(1);
}

/*
 *	update the state of channels
 */
#define HI_DMA_UPDATE_TIMEOUT  (5 * HZ)
static int dma_update_status(unsigned int channel)
{

	unsigned int channel_status;
	unsigned int channel_tc_status[3];
	unsigned int channel_err_status[3];
	unsigned int i = channel, j;
	unsigned long update_jiffies_timeout;

	update_jiffies_timeout = jiffies + HI_DMA_UPDATE_TIMEOUT;

	while (1) {
		for (j = 0; j < 3; j++) {
			dmac_readw(dma_regbase + DMAC_INTTCSTATUS, channel_status);
			channel_tc_status[j] = (channel_status >> i) & 0x01;
			dmac_readw(dma_regbase + DMAC_INTERRORSTATUS, channel_status);
			channel_err_status[j] = (channel_status >> i) & 0x01;
		}

		if ((channel_tc_status[0] == 0x1) &&
			(channel_tc_status[1] == 0x1) &&
				(channel_tc_status[2] == 0x1)) {
			g_channel_status[i] = DMAC_CHN_SUCCESS;
			dmac_writew(dma_regbase + DMAC_INTTCCLEAR, (0x01 << i));
			break;
		} else if ((channel_err_status[0] == 0x1) &&
			(channel_err_status[1] == 0x1) &&
				(channel_err_status[2] == 0x1)) {
			g_channel_status[i] = -DMAC_CHN_ERROR;
			dma_err("Error in DMAC %d finish!\n", i);
			dmac_writew(dma_regbase + DMAC_INTERRCLR, (0x01 << i));
			break;
		}

		if (!time_before(jiffies, update_jiffies_timeout)) {
			dma_err("Timeout in DMAC %d!\n", i);
			g_channel_status[i] = -DMAC_CHN_TIMEOUT;
			break;
		}
	}

	return g_channel_status[i];
}


/*
 *	check the state of channels
 */
static int dmac_check_over(unsigned int channel)
{
	int status = 0;

	if (-DMAC_CHN_ERROR == g_channel_status[channel]) {
		dma_err("Error transfer %d finished\n", channel);
		dmac_writew(dma_regbase + DMAC_CxCONFIG(channel), DMAC_CxDISABLE);
		g_channel_status[channel] = DMAC_CHN_VACANCY;
		status = -DMAC_CHN_ERROR;
	} else if (DMAC_NOT_FINISHED == g_channel_status[channel])
		status = DMAC_NOT_FINISHED;
	else if (DMAC_CHN_ALLOCAT == g_channel_status[channel])
		status = DMAC_CHN_ALLOCAT;
	else if (DMAC_CHN_VACANCY == g_channel_status[channel])
		status = DMAC_CHN_VACANCY;
	else if (-DMAC_CHN_TIMEOUT == g_channel_status[channel]) {
		dma_err("transfer %d timeout!\n", channel);
		status = -DMAC_CHN_TIMEOUT;
	} else if (DMAC_CHN_SUCCESS == g_channel_status[channel])
		/*The transfer of Channel %d has finished successfully!*/
		status = DMAC_CHN_SUCCESS;
	else {
		dmac_writew(dma_regbase + DMAC_CxCONFIG(channel), DMAC_CxDISABLE);
		g_channel_status[channel] = DMAC_CHN_VACANCY;
		status = -DMAC_CHN_ERROR;
	}
	return status;
}

spinlock_t my_lcok = __SPIN_LOCK_UNLOCKED(old_style_spin_init);
unsigned long flags;

/*
 *	allocate channel.
 */
int dmac_channel_allocate(void *pisr)
{
	unsigned int i, channelinfo;

	for (i = 0; i < CHANNEL_NUM; i++)
		dmac_check_over(dmac_channel[i]);

	dmac_readw(dma_regbase + DMAC_ENBLDCHNS, channelinfo);
	channelinfo = channelinfo & 0x00ff;

	for (i = 0; i < CHANNEL_NUM; i++) {
		if (g_channel_status[dmac_channel[i]] == DMAC_CHN_VACANCY) {
			channelinfo = channelinfo >> dmac_channel[i];
			if (0x00 == (channelinfo & 0x01)) {
				/*clear the interrupt in this channel */
				dmac_writew(dma_regbase + DMAC_INTERRCLR,
						(0x01 << dmac_channel[i]));
				dmac_writew(dma_regbase + DMAC_INTTCCLEAR,
						(0x01 << dmac_channel[i]));

				g_channel_status[dmac_channel[i]]
						= DMAC_CHN_ALLOCAT;
				return dmac_channel[i];
			}
		}
	}

	dma_err("no to alloc\n");
	return -EINVAL;
}
EXPORT_SYMBOL(dmac_channel_allocate);

int dmac_register_isr(unsigned int channel, void *pisr)
{
	if (channel > CHANNEL_NUM - 1) {
		dma_err("channel which choosed %d is error !\n", channel);
		return -1;
	}

	if (g_channel_status[channel] != DMAC_CHN_VACANCY) {
		dma_err("dma chn %d is in used!\n", channel);
		return -1;
	}

	/*clear the interrupt in this channel */
	dmac_writew(dma_regbase + DMAC_INTERRCLR, (0x01 << channel));
	dmac_writew(dma_regbase + DMAC_INTTCCLEAR, (0x01 << channel));

	function[channel] = (void *)pisr;
	g_channel_status[channel] = DMAC_CHN_ALLOCAT;

	return 0;
}
EXPORT_SYMBOL(dmac_register_isr);

/*
 *	free channel
 */
int dmac_channel_free(unsigned int channel)
{
	g_channel_status[channel] = DMAC_CHN_VACANCY;
	return 0;
}
EXPORT_SYMBOL(dmac_channel_free);

static unsigned int dmac_check_request(unsigned int peripheral_addr,
					int direction)
{
	int i;
	/* check request pipe with peripheral_addr */
	for (i = direction; i < DMAC_MAX_PERIPHERALS; i = i + 2) {
		if (g_peripheral[i].peri_addr == peripheral_addr)
			return i;
	}

	dma_err("Invalid devaddr\n");

	return -1;
}

/*
 *	init dmac register
 *	clear interrupt flags
 *	called by dma_driver_init
 */
int dmac_init(struct hidmac_host *dma)
{
	unsigned int i, tempvalue;
	int ret;

	clk_prepare_enable(dma->clk);
	reset_control_deassert(dma->rstc);

	dmac_readw(dma->regbase + DMAC_CONFIG, tempvalue);
	if (tempvalue == 0) {
		dmac_writew(dma->regbase + DMAC_CONFIG,
				DMAC_CONFIG_VAL);
		dmac_writew(dma->regbase + DMAC_INTTCCLEAR, 0xFF);
		dmac_writew(dma->regbase + DMAC_INTERRCLR, 0xFF);
		for (i = 0; i < DMAC_MAX_CHANNELS; i++) {
			dmac_writew(dma->regbase + DMAC_CxCONFIG(i),
				DMAC_CxDISABLE);
			function[i] = NULL;
		}
	}

	/* creat LLI */
	/* alloc space for dma lli, as the source is uncontinuous, so... */
	ret = allocate_dmalli_space(pllihead, 1);
	if (ret < 0)
		return -1;

	if (request_irq(dma->irq, dmac_isr, 0, "hi_dma", dma)) {
		dma_err("DMA Irq %d request failed\n", dma->irq);
		free_dmalli_space(pllihead, 1);
		return -1;
	}

	return 0;
}


/*
 *	alloc_dma_lli_space
 *	output:
 *             ppheadlli[0]: memory physics address
 *             ppheadlli[1]: virtual address
 *
 */
int allocate_dmalli_space(unsigned int *ppheadlli, unsigned int page_num)
{
	dma_addr_t dma_phys;
	void *dma_virt;

	dma_virt = dma_alloc_coherent(NULL, page_num*PAGE_SIZE,
					&dma_phys, GFP_DMA | __GFP_WAIT);
	if (NULL == dma_virt) {
		dma_err("can't get dma mem from system\n");
		return -1;
	}

	ppheadlli[0] = (unsigned int)(dma_phys);
	ppheadlli[1] = (unsigned int)(dma_virt);

	return 0;
}
EXPORT_SYMBOL(allocate_dmalli_space);

/*
 *	free_dma_lli_space
 */
int free_dmalli_space(unsigned int *ppheadlli, unsigned int page_num)
{
	dma_addr_t dma_phys;
	unsigned int dma_virt;

	dma_phys = (dma_addr_t)(ppheadlli[0]);
	dma_virt = ppheadlli[1];

	dma_free_coherent(NULL, page_num*PAGE_SIZE,
				(void *)dma_virt, dma_phys);

	ppheadlli[0] = 0;
	ppheadlli[1] = 0;
	return 0;
}
EXPORT_SYMBOL(free_dmalli_space);

/*
 * config register for memory to memory DMA transfer without LLI
 * note:
 * it is necessary to call dmac_channelstart for channel enable
 */
int dmac_start_m2m(unsigned int  channel, unsigned int psource,
			unsigned int pdest, unsigned int uwnumtransfers)
{
	unsigned int uwchannel_num, tmp_trasnsfer;

	if (uwnumtransfers > (MAXTRANSFERSIZE << 2)) {
		dma_err("Invalidate transfer size,size=%x\n", uwnumtransfers);
		return -EINVAL;
	}

	uwchannel_num = channel;

	if ((uwchannel_num == DMAC_CHANNEL_INVALID)
			|| (uwchannel_num > CHANNEL_NUM)) {
		pr_err("failure of DMAC channel allocation in M2M function!\n");
		return -EFAULT;
	}

	/* dmac_writew (DMAC_CxCONFIG(uwchannel_num), DMAC_CxDISABLE); */
	dmac_writew(dma_regbase + DMAC_CxSRCADDR(uwchannel_num), psource);
	dmac_writew(dma_regbase + DMAC_CxDESTADDR(uwchannel_num), pdest);
	dmac_writew(dma_regbase + DMAC_CxLLI(uwchannel_num), 0);
	tmp_trasnsfer = (uwnumtransfers >> 2) & 0xfff;
	tmp_trasnsfer = tmp_trasnsfer | (DMAC_CxCONTROL_M2M & (~0xfff));
	dmac_writew(dma_regbase + DMAC_CxCONTROL(uwchannel_num), tmp_trasnsfer);
	dmac_writew(dma_regbase + DMAC_CxCONFIG(uwchannel_num), DMAC_CxCONFIG_M2M);

	return 0;
}
EXPORT_SYMBOL(dmac_start_m2m);

/*
 *	channel enable
 *	start a dma transfer immediately
 */
int dmac_channelstart(unsigned int u32channel)
{

	unsigned int reg_value;

	if (u32channel >= DMAC_MAX_CHANNELS) {
		dma_err("channel larger %d\n", DMAC_MAX_CHANNELS);
		return -EINVAL;
	}

	g_channel_status[u32channel] = DMAC_NOT_FINISHED;
	dmac_readw(dma_regbase + DMAC_CxCONFIG(u32channel), reg_value);
	dmac_writew(dma_regbase + DMAC_CxCONFIG(u32channel),
			(reg_value | DMAC_CHANNEL_ENABLE));

	return 0;
}
EXPORT_SYMBOL(dmac_channelstart);

/*
 *	wait for transfer end
 */
int dmac_wait(int channel)
{
	int ret_result;

	if (channel < 0)
		return -1;

	while (1) {
		ret_result = dma_update_status(channel);
		if (ret_result == -DMAC_CHN_ERROR) {
			dma_err("Transfer Error.\n");
			return -1;
		} else  if (ret_result == DMAC_NOT_FINISHED)
			udelay(10);
		else if (ret_result == DMAC_CHN_SUCCESS)
			return DMAC_CHN_SUCCESS;
		else if (ret_result == DMAC_CHN_VACANCY)
			return DMAC_CHN_SUCCESS;
		else if (ret_result == -DMAC_CHN_TIMEOUT) {
			dma_err("Timeout.\n");
			dmac_writew(dma_regbase + DMAC_CxCONFIG(channel), DMAC_CxDISABLE);
			g_channel_status[channel] = DMAC_CHN_VACANCY;
			return -1;
		}
	}
}
EXPORT_SYMBOL(dmac_wait);

/*
 *	buile LLI for memory to memory DMA transfer
 */
int dmac_buildllim2m_isp(unsigned int *ppheadlli, unsigned int *psource,
			unsigned int *pdest, unsigned int *length,
			unsigned int lli_num)
{
	unsigned int address, phy_address;
	unsigned int j;

	if (ppheadlli != NULL) {
		phy_address = (unsigned int)(ppheadlli[0]);
		dma_debug("phy_address: 0x%X\n",phy_address);
		address = (unsigned int)(ppheadlli[1]);
		dma_debug("address: 0x%X\n",address);
		for (j = 0; j < lli_num; j++) {
			dma_debug("psource[%d]: 0x%X\n", j, psource[j]);
			dmac_writew(address, psource[j]);
			address += 4;
			phy_address += 4;
			dma_debug("pdest[%d]: 0x%X\n", j, pdest[j]);
			dmac_writew(address, pdest[j]);
			address += 4;
			phy_address += 4;

			/* if the last node, next_lli_addr = 0*/
			if (j == (lli_num - 1))
				dmac_writew(address, 0);
			else
				dmac_writew(address,
					(((phy_address + 8) & (~0x03))
					 | DMAC_CxLLI_LM));

			address += 4;
			phy_address += 4;

			if (j == (lli_num - 1)) {
				dmac_writew(address, ((DMAC_CxCONTROL_LLIM2M_ISP
					&(~0xfff)) | (length[j])
					| 0x80000000));
			} else {
				dmac_writew(address,
					(((DMAC_CxCONTROL_LLIM2M_ISP&(~0xfff)) |
					 (length[j])) & 0x7fffffff));
			}

			address += 4;
			phy_address += 4;
		}
	}

	return 0;
}
EXPORT_SYMBOL(dmac_buildllim2m_isp);

/*
 *	buile LLI for memory to memory DMA transfer
 */
int dmac_buildllim2m(unsigned int *ppheadlli, unsigned int pdest,
			unsigned int psource, unsigned int totaltransfersize,
				unsigned int uwnumtransfers)
{
	unsigned int lli_num = 0;
	unsigned int last_lli = 0;
	unsigned int address , phy_address, srcaddr, denstaddr;
	unsigned int j;

	lli_num = (totaltransfersize / uwnumtransfers);

	if ((totaltransfersize % uwnumtransfers) != 0)
		last_lli = 1, ++lli_num;

	if (ppheadlli != NULL) {
		phy_address = (unsigned int)(ppheadlli[0]);
		address = (unsigned int)(ppheadlli[1]);
		for (j = 0; j < lli_num; j++) {
			srcaddr = (psource + (j*uwnumtransfers));
			dmac_writew(address, srcaddr);
			address += 4;
			phy_address += 4;
			denstaddr = (pdest + (j*uwnumtransfers));
			dmac_writew(address, denstaddr);
			address += 4;
			phy_address += 4;
			if (j == (lli_num - 1))
				dmac_writew(address, 0);
			else
				dmac_writew(address,
					(((phy_address + 8) & (~0x03))
					 | DMAC_CxLLI_LM));

			address += 4;
			phy_address += 4;

			if ((j == (lli_num - 1)) && (last_lli == 0))
				dmac_writew(address, ((DMAC_CxCONTROL_LLIM2M
					&(~0xfff)) | (uwnumtransfers >> 2)
					| 0x80000000));
			else if ((j == (lli_num - 1)) && (last_lli == 1))
				dmac_writew(address, ((DMAC_CxCONTROL_LLIM2M
					& (~0xfff)) | ((totaltransfersize
					% uwnumtransfers) >> 2) | 0x80000000));
			else
				dmac_writew(address,
					(((DMAC_CxCONTROL_LLIM2M&(~0xfff)) |
					 (uwnumtransfers >> 2)) & 0x7fffffff));

			address += 4;
			phy_address += 4;
		}
	}

	return 0;
}
EXPORT_SYMBOL(dmac_buildllim2m);

/*
 *	disable channel
 *	used before the operation of register configuration
 */
int dmac_channelclose(unsigned int channel)
{
	unsigned int reg_value, count;

	if (channel >= DMAC_MAX_CHANNELS) {
		dma_err("channel larger than total.\n");
		return -EINVAL;
	}

	dmac_readw(dma_regbase + DMAC_CxCONFIG(channel), reg_value);

#define CHANNEL_CLOSE_IMMEDIATE
#ifdef CHANNEL_CLOSE_IMMEDIATE
	reg_value &= 0xFFFFFFFE;
	dmac_writew(dma_regbase + DMAC_CxCONFIG(channel) , reg_value);
#else
	reg_value |= DMAC_CONFIGURATIONx_HALT_DMA_ENABLE;
	/*ignore incoming dma request*/
	dmac_writew(dma_regbase + DMAC_CxCONFIG(channel), reg_value);
	dmac_readw(dma_regbase + DMAC_CxCONFIG(channel), reg_value);
	/*if FIFO is empty*/
	while ((reg_value & DMAC_CONFIGURATIONx_ACTIVE)
			== DMAC_CONFIGURATIONx_ACTIVE)
		dmac_readw(dma_regbase + DMAC_CxCONFIG(channel), reg_value);
	reg_value &= 0xFFFFFFFE;
	dmac_writew(dma_regbase + DMAC_CxCONFIG(channel), reg_value);
#endif

	dmac_readw(dma_regbase + DMAC_ENBLDCHNS, reg_value);
	reg_value = reg_value & 0x00ff;
	count = 0;
	while (((reg_value >> channel) & 0x1) == 1) {
		dmac_readw(dma_regbase + DMAC_ENBLDCHNS, reg_value);
		reg_value = reg_value & 0x00ff;
		if (count++ > 10000) {
			dma_err("close failure.\n");
			return -1;
		}
	}

	return 0;
}
EXPORT_SYMBOL(dmac_channelclose);

/*
 *	load configuration from LLI for memory to memory
 */
int dmac_start_llim2m(unsigned int channel, unsigned int *pfirst_lli)
{
	unsigned int uwchannel_num;
	dmac_lli  plli;
	unsigned int first_lli;

	if (NULL == pfirst_lli) {
		dma_err("Invalidate LLI head!\n");
		return -EFAULT;
	}

	uwchannel_num = channel;
	if ((uwchannel_num == DMAC_CHANNEL_INVALID) ||
				(uwchannel_num > 7)) {
		dma_err("failure of DMAC channel allocation in");
		dma_err("LLIM2M function,channel=%x!\n ", uwchannel_num);
		return -EINVAL;
	}

	memset(&plli, 0, sizeof(plli));
	first_lli = (unsigned int)pfirst_lli[1];
	dmac_readw(first_lli, plli.src_addr);
	dmac_readw(first_lli+4, plli.dst_addr);
	dmac_readw(first_lli+8, plli.next_lli);
	dmac_readw(first_lli+12, plli.lli_transfer_ctrl);

	dmac_channelclose(uwchannel_num);
	dmac_writew(dma_regbase + DMAC_INTTCCLEAR, (0x1 << uwchannel_num));
	dmac_writew(dma_regbase + DMAC_INTERRCLR, (0x1 << uwchannel_num));
	dmac_writew(dma_regbase + DMAC_SYNC, 0x0);

	dmac_writew(dma_regbase + DMAC_CxCONFIG(uwchannel_num),
			DMAC_CxDISABLE);
	dmac_writew(dma_regbase + DMAC_CxSRCADDR(uwchannel_num),
			(unsigned int)(plli.src_addr));
	dmac_writew(dma_regbase + DMAC_CxDESTADDR(uwchannel_num),
			(unsigned int)(plli.dst_addr));
	dmac_writew(dma_regbase + DMAC_CxLLI(uwchannel_num),
			(unsigned int)(plli.next_lli));
	dmac_writew(dma_regbase + DMAC_CxCONTROL(uwchannel_num),
			(unsigned int)(plli.lli_transfer_ctrl));
	dmac_writew(dma_regbase + DMAC_CxCONFIG(uwchannel_num),
			DMAC_CxCONFIG_LLIM2M);

	return 0;
}
EXPORT_SYMBOL(dmac_start_llim2m);

/*
 *	load configuration from LLI for memory and peripheral
 */
int dmac_start_llim2p(unsigned int channel, unsigned int *pfirst_lli,
				unsigned int uwperipheralid)
{
	unsigned int uwchannel_num;
	dmac_lli plli;
	unsigned int first_lli;
	unsigned int temp = 0;

	if (NULL == pfirst_lli) {
		dma_err("Invalidate LLI head!\n");
		return -EINVAL;
	}
	uwchannel_num = channel;
	if ((uwchannel_num == DMAC_CHANNEL_INVALID) ||
			(uwchannel_num > CHANNEL_NUM)) {
		dma_err("failure of DMAC channel allocation in");
		dma_err("LLIM2P function, channel=%x!\n ", uwchannel_num);
		return -EINVAL;
	}

	memset(&plli, 0, sizeof(plli));
	first_lli = (unsigned int)pfirst_lli[1];
	dmac_readw(first_lli, plli.src_addr);
	dmac_readw(first_lli+4, plli.dst_addr);
	dmac_readw(first_lli+8, plli.next_lli);
	dmac_readw(first_lli+12, plli.lli_transfer_ctrl);

	dmac_channelclose(uwchannel_num);
	dmac_writew(dma_regbase + DMAC_INTTCCLEAR, (0x1<<uwchannel_num));
	dmac_writew(dma_regbase + DMAC_INTERRCLR, (0x1<<uwchannel_num));
	dmac_writew(dma_regbase + DMAC_SYNC, 0x0);

	dmac_readw(dma_regbase + DMAC_CxCONFIG(uwchannel_num), temp);
	dmac_writew(dma_regbase + DMAC_CxCONFIG(uwchannel_num),
			temp|DMAC_CxDISABLE);
	dmac_writew(dma_regbase + DMAC_CxSRCADDR(uwchannel_num),
			plli.src_addr);
	dmac_writew(dma_regbase + DMAC_CxDESTADDR(uwchannel_num),
			plli.dst_addr);
	dmac_writew(dma_regbase + DMAC_CxLLI(uwchannel_num),
			plli.next_lli);
	dmac_writew(dma_regbase + DMAC_CxCONTROL(uwchannel_num),
			plli.lli_transfer_ctrl);

	return 0;
}
EXPORT_SYMBOL(dmac_start_llim2p);

/*
 *	enable memory and peripheral dma transfer
 *	note:
 *	it is necessary to call dmac_channelstart to enable channel
 */
int dmac_start_m2p(unsigned int channel, unsigned int memaddr,
		unsigned int uwperipheralid, unsigned int uwnumtransfers,
		unsigned int next_lli_addr)
{

	unsigned int uwtrans_control = 0;
	unsigned int addtmp, tmp;
	unsigned int uwdst_addr = 0, uwsrc_addr = 0;
	unsigned int uwwidth;
	int uwchannel_num;

	addtmp = memaddr;

	if ((uwperipheralid > 15)) {
		dma_err("Invalid peripheral id%x\n", uwperipheralid);
		return -EINVAL;
	}

	uwchannel_num = (int)channel;
	if ((uwchannel_num == DMAC_CHANNEL_INVALID)
		|| (uwchannel_num > CHANNEL_NUM) || (uwchannel_num < 0)) {
		dma_err("failure alloc\n");
		return -EFAULT;
	}

	/* must modified with different peripheral */
	uwwidth = g_peripheral[uwperipheralid].transfer_width;

	/* check transfer direction *
	 * even number-->TX, odd number-->RX*/
	uwsrc_addr = memaddr;
	uwdst_addr = (unsigned int)(g_peripheral[uwperipheralid].peri_addr);

	tmp = uwnumtransfers >> uwwidth;
	if (tmp & (~0x0fff)) {
		dma_err("Invalidate size%x\n", uwnumtransfers);
		return -EINVAL;
	}

	tmp = tmp & 0xfff;
	uwtrans_control = tmp |
		(g_peripheral[uwperipheralid].transfer_ctrl & (~0xfff));
	dmac_writew(dma_regbase + DMAC_INTTCCLEAR, (0x1<<(unsigned int)uwchannel_num));
	dmac_writew(dma_regbase + DMAC_INTERRCLR, (0x1<<(unsigned int)uwchannel_num));
	dmac_writew(dma_regbase + DMAC_CxSRCADDR(uwchannel_num), (unsigned int)uwsrc_addr);
	dmac_writew(dma_regbase + DMAC_CxDESTADDR(uwchannel_num), (unsigned int)uwdst_addr);
	dmac_writew(dma_regbase + DMAC_CxCONTROL(uwchannel_num),
			(unsigned int)uwtrans_control);
	dmac_writew(dma_regbase + DMAC_CxCONFIG(uwchannel_num),
			(g_peripheral[uwperipheralid].transfer_cfg));

	return 0;
}

/*
 *	enable memory and peripheral dma transfer
 *	note:
 *	it is necessary to call dmac_channelstart to enable channel
 */
int dmac_start_p2m(unsigned int channel, unsigned int memaddr,
		unsigned int uwperipheralid, unsigned int uwnumtransfers,
		unsigned int next_lli_addr)
{
	unsigned int uwtrans_control = 0;
	unsigned int addtmp, tmp;
	unsigned int uwdst_addr = 0, uwsrc_addr = 0;
	unsigned int uwwidth;
	int uwchannel_num;

	addtmp = memaddr;

	if ((uwperipheralid > 15)) {
		dma_err("Invalid peripheral id%x\n", uwperipheralid);
		return -EINVAL;
	}

	uwchannel_num = (int)channel;
	if ((uwchannel_num == DMAC_CHANNEL_INVALID)
		|| (uwchannel_num > 3) || (uwchannel_num < 0)) {
		dma_err("failure alloc\n");
		return -EFAULT;
	}

	/* must modified with different peripheral */
	uwwidth = g_peripheral[uwperipheralid].transfer_width;

	/* check transfer direction *
	 * even number-->TX, odd number-->RX*/
	uwsrc_addr = (unsigned int)(g_peripheral[uwperipheralid].peri_addr);
	uwdst_addr = memaddr;

	tmp = uwnumtransfers >> uwwidth;
	if (tmp & (~0x0fff)) {
		dma_err("Invalidate size%x\n", uwnumtransfers);
		return -EINVAL;
	}

	tmp = tmp & 0xfff;
	uwtrans_control = tmp |
		(g_peripheral[uwperipheralid].transfer_ctrl & (~0xfff));
	dmac_writew(dma_regbase + DMAC_INTTCCLEAR, (0x1<<(unsigned int)uwchannel_num));
	dmac_writew(dma_regbase + DMAC_INTERRCLR, (0x1<<(unsigned int)uwchannel_num));
	dmac_writew(dma_regbase + DMAC_CxSRCADDR(uwchannel_num),
			(unsigned int)uwsrc_addr);
	dmac_writew(dma_regbase + DMAC_CxDESTADDR(uwchannel_num),
			(unsigned int)uwdst_addr);
	dmac_writew(dma_regbase + DMAC_CxCONTROL(uwchannel_num),
			(unsigned int)uwtrans_control);
	dmac_writew(dma_regbase + DMAC_CxCONFIG(uwchannel_num),
			(g_peripheral[uwperipheralid].transfer_cfg));

	return 0;
}

/*
 *	execute memory to memory dma transfer without LLI
 */
int dmac_m2m_transfer(unsigned int source, unsigned int dest,
						unsigned int length)
{
	unsigned int ulchnn, dma_size = 0;
	unsigned int dma_count, left_size;

	left_size = length;
	dma_count = 0;
	ulchnn = dmac_channel_allocate(NULL);

	ulchnn = 2;

	dma_err("use channel %d\n", ulchnn);

	while ((left_size >> 2) >= 0xffc) {
		dma_size   = 0xffc;
		left_size -= (dma_size << 2);
		dma_err("left_size is %x.", left_size);
		dmac_start_m2m(ulchnn, (unsigned int)(source
			+ dma_count * (dma_size << 2)),
			(unsigned int)(dest + dma_count * (dma_size << 2)),
					(dma_size << 2));
		if (dmac_channelstart(ulchnn) != 0) {
			dma_err("start channel error...\n");
			return -1;
		}

		if (dmac_wait(ulchnn) != DMAC_CHN_SUCCESS) {
			dma_err("dma transfer error...\n");
			return -1;
		}

		dma_count++;
	}

	dmac_start_m2m(ulchnn, (source + dma_count * (dma_size << 2)),
			(dest + dma_count * (dma_size << 2)), (left_size << 2));

	if (dmac_channelstart(ulchnn) != 0)
		return -1;

	if (dmac_wait(ulchnn) != DMAC_CHN_SUCCESS)
		return -1;

	return 0;
}
EXPORT_SYMBOL(dmac_m2m_transfer);

/*
 *	execute memory to peripheral dma transfer without LLI
 */
int dmac_m2p_transfer(unsigned int memaddr, unsigned int uwperipheralid,
			unsigned int length)
{
	unsigned int ulchnn, dma_size = 0;
	unsigned int dma_count, left_size;
	unsigned int uwwidth;

	left_size = length;
	dma_count = 0;

	ulchnn = dmac_channel_allocate(NULL);
	if (DMAC_CHANNEL_INVALID == ulchnn)
		return -1;

	uwwidth = g_peripheral[uwperipheralid].transfer_width;

	while ((left_size >> uwwidth) >= 0xffc) {
		dma_size = 0xffc;
		left_size -= (dma_size << uwwidth);

		if (dmac_start_m2p(ulchnn,
			(unsigned int)(memaddr + dma_count * dma_size),
			uwperipheralid, (dma_size << uwwidth), 0) < 0)
			return -1;

		if (dmac_channelstart(ulchnn) != 0)
			return -1;

		if (dmac_wait(ulchnn) != DMAC_CHN_SUCCESS)
			return -1;

		dma_count++;
	}

	pr_debug("memaddr=0x%x\n", (unsigned int)(memaddr
					+ dma_count * dma_size));

	if (dmac_start_m2p(ulchnn,
		(unsigned int)(memaddr + dma_count * dma_size),
			uwperipheralid, left_size, 0) < 0)
		return -1;

	if (dmac_channelstart(ulchnn) != 0)
		return -1;

	return ulchnn;
}

/*
 *	execute memory to peripheral dma transfer without LLI
 */
int dmac_p2m_transfer(unsigned int memaddr, unsigned int uwperipheralid,
			unsigned int length)
{
	unsigned int ulchnn, dma_size = 0;
	unsigned int dma_count, left_size;
	unsigned int uwwidth;

	left_size = length;
	dma_count = 0;

	ulchnn = dmac_channel_allocate(NULL);
	if (DMAC_CHANNEL_INVALID == ulchnn)
		return -1;

	uwwidth = g_peripheral[uwperipheralid].transfer_width;

	while ((left_size >> uwwidth) >= 0xffc) {
		dma_size = 0xffc;
		left_size -= (dma_size << uwwidth);

		if (dmac_start_p2m(ulchnn,
			(unsigned int)(memaddr + dma_count * dma_size),
			uwperipheralid, (dma_size << uwwidth), 0) < 0)
			return -1;

		if (dmac_channelstart(ulchnn) != 0)
			return -1;

		if (dmac_wait(ulchnn) != DMAC_CHN_SUCCESS)
			return -1;

		dma_count++;
	}

	pr_debug("memaddr=0x%x\n", (unsigned int)(memaddr
					+ dma_count * dma_size));

	if (dmac_start_p2m(ulchnn,
		(unsigned int)(memaddr + dma_count * dma_size),
			uwperipheralid, left_size, 0) < 0)
		return -1;

	if (dmac_channelstart(ulchnn) != 0)
		return -1;

	return ulchnn;
}

/*
 * memory to memory dma transfer with LLI
 *
 * @source
 * @dest
 * @length
 * @num
 * */
int do_dma_llim2m_isp(unsigned int *source,
		unsigned int *dest,
		unsigned int *length,
		unsigned int num)
{
	unsigned int chnn;
	int ret = 0;

	/* the dma channel is default using 2 */
	chnn = 2;

	ret = dmac_buildllim2m_isp(pllihead, source, dest, length, num);

	if (ret) {
		dma_err("build lli error...\n");
		return -1;
	}

	/* dmac_register_isr(chnn, dmac_channel_close); */
	ret = dmac_start_llim2m(chnn, pllihead);
	if (ret)
		return -1;

	if (dmac_channelstart(chnn) != 0) {
		dma_err("start channel error...\n");
		return -1;
	}

	return ret;
}
EXPORT_SYMBOL(do_dma_llim2m_isp);

int do_dma_m2p(unsigned int memaddr, unsigned int peripheral_addr,
		unsigned int length)
{
	int ret = 0;
	int uwperipheralid;

	uwperipheralid = dmac_check_request(peripheral_addr, TX);
	if (uwperipheralid < 0) {
		dma_err("m2p:Invalid devaddr\n");
		return -1;
	}

	ret = dmac_m2p_transfer(memaddr, uwperipheralid, length);
	if (ret == -1) {
		dma_err("m2p:trans err\n");
		return -1;
	}

	return ret;
}

int do_dma_p2m(unsigned int memaddr, unsigned int peripheral_addr,
		unsigned int length)
{
	int ret = -1;
	int uwperipheralid;

	uwperipheralid = dmac_check_request(peripheral_addr, RX);
	if (uwperipheralid < 0) {
		dma_err("p2m:Invalid devaddr.\n");
		return -1;
	}

	ret = dmac_p2m_transfer(memaddr, uwperipheralid, length);
	if (ret == -1) {
		dma_err("p2m:trans err\n");
		return -1;
	}

	return ret;
}

/*
 *	Apply DMA interrupt resource
 *	init channel state
 */
static int hi_dmac_probe(struct platform_device *platdev)
{
	unsigned int i;
	struct hidmac_host *dma;
	struct resource *res;
	int ret;

	dma = devm_kzalloc(&platdev->dev, sizeof(*dma), GFP_KERNEL);
	if (!dma)
		return -ENOMEM;

	res = platform_get_resource(platdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&platdev->dev, "no mmio resource\n");
		return -ENODEV;
	}

	dma->regbase = devm_ioremap_resource(&platdev->dev, res);
	if (IS_ERR(dma->regbase))
		return PTR_ERR(dma->regbase);

	dma->clk = devm_clk_get(&platdev->dev, NULL);
	if (IS_ERR(dma->clk))
		return PTR_ERR(dma->clk);

	dma->rstc = devm_reset_control_get(&platdev->dev, "dma-reset");
	if (IS_ERR(dma->rstc))
		return PTR_ERR(dma->rstc);

	dma->irq = platform_get_irq(platdev, 0);
	if (unlikely(dma->irq < 0))
		return -ENODEV;

	dma_regbase = dma->regbase;

	ret = dmac_init(dma);
	if (ret)
		return -ENODEV;

	platform_set_drvdata(platdev, dma);

	for (i = 0; i < DMAC_MAX_CHANNELS; i++)
		g_channel_status[i] = DMAC_CHN_VACANCY;

	return ret;
}

static int hi_dmac_remove(struct platform_device *platdev)
{
	int i;
	struct hidmac_host *dma = platform_get_drvdata(platdev);

	clk_disable_unprepare(dma->clk);

	for (i = 0; i < DMAC_MAX_CHANNELS; i++)
		g_channel_status[i] = DMAC_CHN_VACANCY;

	free_dmalli_space(pllihead, 1);

	return 0;
}

static int hi_dmac_suspend(struct platform_device *platdev,
		pm_message_t state)
{
	int i;
	struct hidmac_host *dma = platform_get_drvdata(platdev);

	clk_prepare_enable(dma->clk);

	for (i = 0; i < DMAC_MAX_CHANNELS; i++)
		g_channel_status[i] = DMAC_CHN_VACANCY;

	clk_disable_unprepare(dma->clk);

	return 0;
}

static int hi_dmac_resume(struct platform_device *platdev)
{
	int i;
	struct hidmac_host *dma = platform_get_drvdata(platdev);
	unsigned int tempvalue;

	clk_prepare_enable(dma->clk);
	reset_control_deassert(dma->rstc);

	dmac_readw(dma->regbase + DMAC_CONFIG, tempvalue);
	if (tempvalue == 0) {
		dmac_writew(dma->regbase + DMAC_CONFIG,
				DMAC_CONFIG_VAL);
		dmac_writew(dma->regbase + DMAC_INTTCCLEAR, 0xFF);
		dmac_writew(dma->regbase + DMAC_INTERRCLR, 0xFF);
		for (i = 0; i < DMAC_MAX_CHANNELS; i++) {
			dmac_writew(dma->regbase + DMAC_CxCONFIG(i),
				DMAC_CxDISABLE);
			function[i] = NULL;
		}
	}

	for (i = 0; i < DMAC_MAX_CHANNELS; i++)
		g_channel_status[i] = DMAC_CHN_VACANCY;

	return 0;
}

static const struct of_device_id hisi_dmac_dt_ids[] = {
	{ .compatible = "hisilicon,hisi-dmac"},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, hisi_dmac_dt_ids);

static struct platform_driver hisi_dmac_driver = {
	.driver = {
		.name   = "hisi-dmac",
		.of_match_table = hisi_dmac_dt_ids,
	},
	.probe      = hi_dmac_probe,
	.remove     = hi_dmac_remove,
	.suspend    = hi_dmac_suspend,
	.resume     = hi_dmac_resume,
};

module_platform_driver(hisi_dmac_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hisilicon");
MODULE_DESCRIPTION("HiSilicon DMA Controller driver");
