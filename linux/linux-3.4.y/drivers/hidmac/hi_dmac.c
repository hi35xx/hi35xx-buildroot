/*
 * Copyright (c) 2014 Hisilicon Co., Ltd.
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 *
 * History:
 *      17-August-2006 create this file
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
#include <linux/io.h>
#include <linux/hidmac.h>
#include "hi_dmac.h"

#ifdef CONFIG_ARCH_HI3516A
#include "hidmac_hi3516a.h"
#include "hidmac_hi3516a.c"
#endif

#ifdef CONFIG_ARCH_HI3518EV200
#include "hidmac_hi3518ev200.h"
#include "hidmac_hi3518ev200.c"
#endif

void *reg_dmac_base_va;
#define IO_DMAC_ADDRESS(x) (reg_dmac_base_va + ((x)-(DMAC_BASE_REG)))

#define RX	0
#define TX	1

static int dmac_channel[CHANNEL_NUM] = {0, 1, 2, 3};

#define CLR_INT(i)	((*(unsigned int *)IO_DMAC_ADDRESS\
				(DMAC_BASE_REG+0x008)) = (1 << i))

int g_channel_status[CHANNEL_NUM];

/* #define DEBUG */

#define DEBUG
#ifdef DEBUG
#define dma_err printk
#else
#define dma_err(fmt, ...) do {} while (0);
#endif

/*
 *Define Memory range
 */
mem_addr mem_num[MEM_MAX_NUM] = {
	{DDRAM_ADRS, DDRAM_SIZE},
	{FLASH_BASE, FLASH_SIZE}
};

typedef void REG_ISR(int *p_dma_chn, int *p_dma_status) ;
REG_ISR *function[CHANNEL_NUM];

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
	unsigned int channel_status;
	unsigned int channel_tc_status, channel_err_status;
	unsigned int i;

	/*read the status of current interrupt */
	dmac_readw(DMAC_INTSTATUS, channel_status);

	/*decide which channel has trigger the interrupt*/
	for (i = 0; i < DMAC_MAX_CHANNELS; i++) {
		if ((((channel_status >> i) & 0x1) == 0x01)) {
			/* [HSCP201306240006],l00181524,20130625 */
			/* The INT status should be read first then clear it */
			/* CLR_INT(i); */
			dmac_readw(DMAC_INTTCSTATUS, channel_tc_status);
			dmac_readw(DMAC_INTERRORSTATUS, channel_err_status);
			CLR_INT(i);
			/*¡¾HSCP201403110002¡¿ l00183122 20140723*/
			if (g_channel_status[i] == DMAC_CHN_VACANCY
						&& (function[i]) == NULL) {
				if ((0x01 == ((channel_tc_status >> i) & 0x01)))
					dmac_writew(DMAC_INTTCCLEAR,
								(0x01 << i));
				else if ((0x01 == ((channel_err_status
								>> i)&0x01)))
					dmac_writew(DMAC_INTERRCLR,
								(0x01 << i));
				continue;
			}

			/* save the current channel transfer */
			/* status to g_channel_status[i] */
			if ((0x01 == ((channel_tc_status >> i) & 0x01))) {
				g_channel_status[i] = DMAC_CHN_SUCCESS;
				dmac_writew(DMAC_INTTCCLEAR, (0x01 << i));
			} else if ((0x01 == ((channel_err_status >> i)&0x01))) {
				g_channel_status[i] = -DMAC_CHN_ERROR;
				dmac_writew(DMAC_INTERRCLR, (0x01 << i));
			} else
				printk(KERN_ERR "Isr Error in DMAC_IntHandeler"\
						"%d! channel\n", i);

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
			dmac_readw(DMAC_INTTCSTATUS, channel_status);
			channel_tc_status[j] = (channel_status >> i) & 0x01;
			dmac_readw(DMAC_INTERRORSTATUS, channel_status);
			channel_err_status[j] = (channel_status >> i) & 0x01;
		}

		if ((channel_tc_status[0] == 0x1) &&
			(channel_tc_status[1] == 0x1) &&
				(channel_tc_status[2] == 0x1)) {
			g_channel_status[i] = DMAC_CHN_SUCCESS;
			dmac_writew(DMAC_INTTCCLEAR, (0x01 << i));
			break;
		} else if ((channel_err_status[0] == 0x1) &&
			(channel_err_status[1] == 0x1) &&
				(channel_err_status[2] == 0x1)) {
			g_channel_status[i] = -DMAC_CHN_ERROR;
			dma_err("Error in DMAC %d finish!\n", i);
			dmac_writew(DMAC_INTERRCLR, (0x01 << i));
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
	if (-DMAC_CHN_ERROR == g_channel_status[channel]) {
		dma_err("Error transfer %d finished\n", channel);
		dmac_writew(DMAC_CxCONFIG(channel), DMAC_CxDISABLE);
		g_channel_status[channel] = DMAC_CHN_VACANCY;
		return -DMAC_CHN_ERROR;
	} else if (DMAC_NOT_FINISHED == g_channel_status[channel])
		return DMAC_NOT_FINISHED;
	else if (DMAC_CHN_ALLOCAT == g_channel_status[channel])
		return DMAC_CHN_ALLOCAT;
	else if (DMAC_CHN_VACANCY == g_channel_status[channel])
		return DMAC_CHN_VACANCY;
	else if (-DMAC_CHN_TIMEOUT == g_channel_status[channel]) {
		dma_err("transfer %d timeout!\n", channel);
		return -DMAC_CHN_TIMEOUT;
	} else if (DMAC_CHN_SUCCESS == g_channel_status[channel])
		/*The transfer of Channel %d has finished successfully!*/
		return DMAC_CHN_SUCCESS;
	else {
		dmac_writew(DMAC_CxCONFIG(channel), DMAC_CxDISABLE);
		g_channel_status[channel] = DMAC_CHN_VACANCY;
		return -DMAC_CHN_ERROR;
	}
}

spinlock_t my_lcok = __SPIN_LOCK_UNLOCKED(old_style_spin_init);
unsigned long flags;

/*
 *	allocate channel.
 */
int  dmac_channel_allocate(void *pisr)
{
	unsigned int i, channelinfo;
	for (i = 0; i < CHANNEL_NUM; i++)
		dmac_check_over(dmac_channel[i]);

	dmac_readw(DMAC_ENBLDCHNS, channelinfo);
	channelinfo = channelinfo & 0x00ff;

	for (i = 0; i < CHANNEL_NUM; i++) {
		if (g_channel_status[dmac_channel[i]] == DMAC_CHN_VACANCY) {
			channelinfo = channelinfo >> dmac_channel[i];
			if (0x00 == (channelinfo & 0x01)) {
				/*clear the interrupt in this channel */
				dmac_writew(DMAC_INTERRCLR,
						(0x01 << dmac_channel[i]));
				dmac_writew(DMAC_INTTCCLEAR,
						(0x01 << dmac_channel[i]));

				g_channel_status[dmac_channel[i]]
						= DMAC_CHN_ALLOCAT;
				dma_err("allocate channel is %d......\n", i);
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
	if ((channel < 0) || (channel > CHANNEL_NUM)) {
		dma_err("channel which choosed %d is error !\n", channel);
		return -1;
	}

	if (g_channel_status[channel] != DMAC_CHN_VACANCY) {
		dma_err("dma chn %d is in used!\n", channel);
		return -1;
	}

	/*clear the interrupt in this channel */
	dmac_writew(DMAC_INTERRCLR, (0x01 << channel));
	dmac_writew(DMAC_INTTCCLEAR, (0x01 << channel));

	function[channel] = (void *)pisr;
	g_channel_status[channel] = DMAC_CHN_ALLOCAT;

	return 0;
}
EXPORT_SYMBOL(dmac_register_isr);

/*
 *	free channel
 */
int  dmac_channel_free(unsigned int channel)
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
 *	clear interupt flags
 *	called by dma_driver_init
 */
int  dmac_init(void)
{
	unsigned int i, tempvalue;

	hidmac_clk_en();

	hidmac_unreset();

	dmac_readw(DMAC_CONFIG, tempvalue);
	if (tempvalue == 0) {
		dmac_writew(DMAC_CONFIG, DMAC_CONFIG_VAL);
		dmac_writew(DMAC_INTTCCLEAR, 0xFF);
		dmac_writew(DMAC_INTERRCLR, 0xFF);
		for (i = 0; i < DMAC_MAX_CHANNELS; i++)
			dmac_writew(DMAC_CxCONFIG(i), DMAC_CxDISABLE);
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
	void *dma_virt ;
	unsigned int *address;
	address = ppheadlli;

	dma_virt = dma_alloc_coherent(NULL, page_num*PAGE_SIZE,
					&dma_phys, GFP_DMA | __GFP_WAIT);
	if (NULL == dma_virt) {
		dma_err("can't get dma mem from system\n");
		return -1;
	}

	address[0] = (unsigned int)(dma_phys);
	address[1] = (unsigned int)(dma_virt);
	return 0;
}
EXPORT_SYMBOL(allocate_dmalli_space);

/*
 *	free_dma_lli_space
 */
int free_dmalli_space(unsigned int *ppheadlli, unsigned int page_num)
{
	dma_addr_t dma_phys;
	unsigned int dma_virt ;

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
 * config register for memory to memory DMA tranfer without LLI
 * note:
 * it is necessary to call dmac_channelstart for channel enable
 */
int dmac_start_m2m(unsigned int  channel, unsigned int psource,
			unsigned int pdest, unsigned int uwnumtransfers)
{
	unsigned int uwchannel_num, tmp_trasnsfer, addtmp;

	/*check input paramet*/
	addtmp = psource;
	addtmp = pdest;

	if (uwnumtransfers > (MAXTRANSFERSIZE << 2)) {
		dma_err("Invalidate transfer size,size=%x\n", uwnumtransfers);
		return -EINVAL;
	}

	uwchannel_num = channel;

	if ((uwchannel_num == DMAC_CHANNEL_INVALID) || (uwchannel_num >
								CHANNEL_NUM)) {
		printk(KERN_INFO "failure of DMAC channel"\
			"allocation in M2M function!\n");
		return -EFAULT;
	}

	/* dmac_writew (DMAC_CxCONFIG(uwchannel_num), DMAC_CxDISABLE); */
	dmac_writew(DMAC_CxSRCADDR(uwchannel_num), psource);
	dmac_writew(DMAC_CxDESTADDR(uwchannel_num), pdest);
	dmac_writew(DMAC_CxLLI(uwchannel_num), 0);
	tmp_trasnsfer = (uwnumtransfers >> 2) & 0xfff;
	tmp_trasnsfer = tmp_trasnsfer | (DMAC_CxCONTROL_M2M & (~0xfff));
	dmac_writew(DMAC_CxCONTROL(uwchannel_num), tmp_trasnsfer);
	dmac_writew(DMAC_CxCONFIG(uwchannel_num), DMAC_CxCONFIG_M2M);

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
	dmac_readw(DMAC_CxCONFIG(u32channel), reg_value);
	dmac_writew(DMAC_CxCONFIG(u32channel),
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
			dmac_writew(DMAC_CxCONFIG(channel), DMAC_CxDISABLE);
			g_channel_status[channel] = DMAC_CHN_VACANCY;
			return -1;
		}
	}
}
EXPORT_SYMBOL(dmac_wait);

/*
 *	buile LLI for memory to memory DMA tranfer
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

	dmac_readw(DMAC_CxCONFIG(channel), reg_value);

#define CHANNEL_CLOSE_IMMEDIATE
#ifdef CHANNEL_CLOSE_IMMEDIATE
	reg_value &= 0xFFFFFFFE;
	dmac_writew(DMAC_CxCONFIG(channel) , reg_value);
#else
	reg_value |= DMAC_CONFIGURATIONx_HALT_DMA_ENABLE;
	/*ignore incoming dma request*/
	dmac_writew(DMAC_CxCONFIG(channel), reg_value);
	dmac_readw(DMAC_CxCONFIG(channel), reg_value);
	/*if FIFO is empty*/
	while ((reg_value & DMAC_CONFIGURATIONx_ACTIVE)
			== DMAC_CONFIGURATIONx_ACTIVE)
		dmac_readw(DMAC_CxCONFIG(channel), reg_value);
	reg_value &= 0xFFFFFFFE;
	dmac_writew(DMAC_CxCONFIG(channel), reg_value);
#endif

	dmac_readw(DMAC_ENBLDCHNS, reg_value);
	reg_value = reg_value & 0x00ff;
	count = 0;
	while (((reg_value >> channel) & 0x1) == 1) {
		dmac_readw(DMAC_ENBLDCHNS, reg_value);
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
		dma_err("failure of DMAC channel allocation in"\
			"LLIM2M function,channel=%x!\n ", uwchannel_num);
		return -EINVAL;
	}

	memset(&plli, 0, sizeof(plli));
	first_lli = (unsigned int)pfirst_lli[1];
	dmac_readw(first_lli, plli.src_addr);
	dmac_readw(first_lli+4, plli.dst_addr);
	dmac_readw(first_lli+8, plli.next_lli);
	dmac_readw(first_lli+12, plli.lli_transfer_ctrl);

	dmac_channelclose(uwchannel_num);
	dmac_writew(DMAC_INTTCCLEAR, (0x1 << uwchannel_num));
	dmac_writew(DMAC_INTERRCLR, (0x1 << uwchannel_num));
	dmac_writew(DMAC_SYNC, 0x0);

	dmac_writew(DMAC_CxCONFIG(uwchannel_num), DMAC_CxDISABLE);
	dmac_writew(DMAC_CxSRCADDR(uwchannel_num),
			(unsigned int)(plli.src_addr));
	dmac_writew(DMAC_CxDESTADDR(uwchannel_num),
			(unsigned int)(plli.dst_addr));
	dmac_writew(DMAC_CxLLI(uwchannel_num), (unsigned int)(plli.next_lli));
	dmac_writew(DMAC_CxCONTROL(uwchannel_num),
			(unsigned int)(plli.lli_transfer_ctrl));
	dmac_writew(DMAC_CxCONFIG(uwchannel_num), DMAC_CxCONFIG_LLIM2M);

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
		dma_err(" failure of DMAC channel allocation in"\
			"LLIM2P function, channel=%x!\n ", uwchannel_num);
		return -EINVAL;
	}

	memset(&plli, 0, sizeof(plli));
	first_lli = (unsigned int)pfirst_lli[1];
	dmac_readw(first_lli, plli.src_addr);
	dmac_readw(first_lli+4, plli.dst_addr);
	dmac_readw(first_lli+8, plli.next_lli);
	dmac_readw(first_lli+12, plli.lli_transfer_ctrl);

	dmac_channelclose(uwchannel_num);
	dmac_writew(DMAC_INTTCCLEAR, (0x1<<uwchannel_num));
	dmac_writew(DMAC_INTERRCLR, (0x1<<uwchannel_num));
	dmac_writew(DMAC_SYNC, 0x0);

	dmac_readw(DMAC_CxCONFIG(uwchannel_num), temp);
	dmac_writew(DMAC_CxCONFIG(uwchannel_num), temp|DMAC_CxDISABLE);
	dmac_writew(DMAC_CxSRCADDR(uwchannel_num), plli.src_addr);
	dmac_writew(DMAC_CxDESTADDR(uwchannel_num), plli.dst_addr);
	dmac_writew(DMAC_CxLLI(uwchannel_num), plli.next_lli);
	dmac_writew(DMAC_CxCONTROL(uwchannel_num), plli.lli_transfer_ctrl);

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
	dma_err("(in)uwdst_addr = %#x\n", (unsigned int)uwdst_addr);
	dmac_writew(DMAC_INTTCCLEAR, (0x1<<uwchannel_num));
	dmac_writew(DMAC_INTERRCLR, (0x1<<uwchannel_num));
	dmac_writew(DMAC_CxSRCADDR(uwchannel_num), (unsigned int)uwsrc_addr);
	dmac_writew(DMAC_CxDESTADDR(uwchannel_num), (unsigned int)uwdst_addr);
	dmac_writew(DMAC_CxCONTROL(uwchannel_num),
			(unsigned int)uwtrans_control);
	dmac_writew(DMAC_CxCONFIG(uwchannel_num),
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
	dma_err("(in)uwdst_addr = %#x\n", (unsigned int)uwdst_addr);
	dmac_writew(DMAC_INTTCCLEAR, (0x1<<uwchannel_num));
	dmac_writew(DMAC_INTERRCLR, (0x1<<uwchannel_num));
	dmac_writew(DMAC_CxSRCADDR(uwchannel_num), (unsigned int)uwsrc_addr);
	dmac_writew(DMAC_CxDESTADDR(uwchannel_num), (unsigned int)uwdst_addr);
	dmac_writew(DMAC_CxCONTROL(uwchannel_num),
			(unsigned int)uwtrans_control);
	dmac_writew(DMAC_CxCONFIG(uwchannel_num),
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
	if (DMAC_CHANNEL_INVALID == ulchnn)
		return -1;

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

	dma_err("memaddr=0x%x\n", (unsigned int)(memaddr
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

	dma_err("memaddr=0x%x\n", (unsigned int)(memaddr
					+ dma_count * dma_size));

	if (dmac_start_p2m(ulchnn,
		(unsigned int)(memaddr + dma_count * dma_size),
			uwperipheralid, left_size, 0) < 0)
		return -1;

	if (dmac_channelstart(ulchnn) != 0)
		return -1;

	return ulchnn;
}


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

	reg_dmac_base_va = (void *)IO_ADDRESS(DMAC_BASE_REG);

	dmac_init();

	for (i = 0; i < DMAC_MAX_CHANNELS; i++)
		g_channel_status[i] = DMAC_CHN_VACANCY;

	return 0;
}

static int hi_dmac_remove(struct platform_device *platdev)
{
	int i;

	for (i = 0; i < DMAC_MAX_CHANNELS; i++)
		g_channel_status[i] = DMAC_CHN_VACANCY;

	return 0;
}

static int hi_dmac_suspend(struct platform_device *dev, pm_message_t state)
{
	int i;

	for (i = 0; i < DMAC_MAX_CHANNELS; i++)
		g_channel_status[i] = DMAC_CHN_VACANCY;

	return 0;
}

static int hi_dmac_resume(struct platform_device *dev)
{
	int i;

	dmac_init();

	for (i = 0; i < DMAC_MAX_CHANNELS; i++)
		g_channel_status[i] = DMAC_CHN_VACANCY;

	return 0;
}

struct platform_device hi_dmac_device = {
	.name           = "hisilicon-dmac",
	.id             = 0,
};

static struct platform_driver hi_dmac_driver = {
	.probe          = hi_dmac_probe,
	.remove         = hi_dmac_remove,
	.suspend        = hi_dmac_suspend,
	.resume         = hi_dmac_resume,
	.driver         = {
		.owner  = THIS_MODULE,
		.name   = "hisilicon-dmac",
	},
};

int __init dma_driver_init(void)
{
	int ret = 0;

	ret = platform_device_register(&hi_dmac_device);
	if (ret) {
		pr_err("register netdevice device failed!");
		goto _error_register_device;
	}

	ret = platform_driver_register(&hi_dmac_driver);
	if (ret) {
		pr_err("register netdevice driver failed!");
		goto _error_register_driver;
	}

	return ret;

_error_register_driver:
	platform_device_unregister(&hi_dmac_device);
_error_register_device:
	return -1;
}

static void __exit dma_driver_exit(void)
{
	platform_driver_unregister(&hi_dmac_driver);

	platform_device_unregister(&hi_dmac_device);
}

module_init(dma_driver_init);
module_exit(dma_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hisilicon");

