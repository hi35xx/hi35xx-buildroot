#include <config.h>
#include <common.h>
#include <mmc.h>
#include <part.h>
#include <malloc.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <himciv200_reg.h>

#include "himciv200.h"

/*************************************************************************/

#if defined(CONFIG_ARCH_HI3536)
#include "himciv200_hi3536.c"
#endif

#if defined(CONFIG_HI3518EV200)
#include "himciv200_hi3518ev200.c"
#endif

#ifdef CONFIG_HI3516CV300
	#include "himciv200_hi3516cv300.c"
#endif

#if defined(CONFIG_ARCH_HI3519)
#include "himciv200_hi3519.c"
#endif

#if defined(CONFIG_ARCH_HI3519V101)
#include "himciv200_hi3519v101.c"
#endif

#if defined(CONFIG_ARCH_HI3516AV200)
#include "himciv200_hi3516av200.c"
#endif
#if (defined CONFIG_ARCH_HI3559 || defined CONFIG_ARCH_HI3556)
#include "himciv200_hi3559.c"
#endif
/*************************************************************************/
#if defined(CONFIG_HIMCI_V200)
#define DRIVER_NAME "HIMCI_V200"
#endif

#if HI_MCI_DEBUG
int debug_type = HIMCI_DEBUG_TYPE;

char *get_debug_type_string(int type)
{
	if (type & HIMCI_DEBUG_TYPE_REG)
		return "REG";
	else if (type & HIMCI_DEBUG_TYPE_FUN)
		return "FUN";
	else if (type & HIMCI_DEBUG_TYPE_CMD)
		return "CMD";
	else if (type & HIMCI_DEBUG_TYPE_INFO)
		return "INFO";
	else if (type & HIMCI_DEBUG_TYPE_ERR)
		return "ERR";
	else
		return "UNKNOWN";
}
#endif

static unsigned int retry_count = MAX_RETRY_COUNT;

#define MAX_DMA_DES	20480
static struct himci_dma_des hi_dma_des[MAX_DMA_DES]
	__attribute__ ((aligned(512)));

/* reset MMC host controler */
static void hi_mci_sys_reset(struct himci_host *host)
{
	unsigned int tmp_reg;
	unsigned int time_out;

	HIMCI_DEBUG_FUN("Function Call");
	HIMCI_ASSERT(host);

	tmp_reg = himci_readl(host->base + MCI_BMOD);
	tmp_reg |= BMOD_SWR;
	himci_writel(tmp_reg, host->base + MCI_BMOD);

	time_out = 1000;
	do {
		tmp_reg = himci_readl(host->base + MCI_BMOD);
		udelay(10);
	} while ((tmp_reg & BMOD_SWR) && time_out--);

	tmp_reg = himci_readl(host->base + MCI_BMOD);
	tmp_reg |= BURST_16 | BURST_INCR;
	himci_writel(tmp_reg, host->base + MCI_BMOD);

	tmp_reg = himci_readl(host->base + MCI_CTRL);
	tmp_reg |=  CTRL_RESET | FIFO_RESET | DMA_RESET;
	himci_writel(tmp_reg, host->base + MCI_CTRL);
}

static void hi_mci_ctrl_power(struct himci_host *host, int flag)
{
	int reg_value = 0;
	HIMCI_DEBUG_FUN("Function Call: flag %d", flag);
	HIMCI_ASSERT(host);

	if (flag)
		reg_value = 1 << host->port;
	else
		reg_value = 0;

	himci_writel(reg_value, host->base + MCI_PWREN);
}

static int hi_mci_wait_cmd(struct himci_host *host)
{
	unsigned int tmp_reg, wait_retry_count = 0;

	HIMCI_DEBUG_FUN("Function Call");
	HIMCI_ASSERT(host);

	do {
		tmp_reg = himci_readl(host->base + MCI_CMD);
		if ((tmp_reg & START_CMD) == 0)
			return 0;
		wait_retry_count++;
	} while (wait_retry_count < retry_count);

	HIMCI_DEBUG_INFO("CMD send timeout");

	return -1;
}

static void hi_mci_control_cclk(struct himci_host *host, unsigned int flag)
{
	unsigned int reg;
	cmd_arg_s cmd_reg;

	HIMCI_DEBUG_FUN("Function Call: flag %d", flag);
	HIMCI_ASSERT(host);

	reg = himci_readl(host->base + MCI_CLKENA);
	if (flag == ENABLE)
		reg |= CCLK_ENABLE << host->port;
	else
		reg &= ~(CCLK_ENABLE << host->port);
	himci_writel(reg, host->base + MCI_CLKENA);

	cmd_reg.cmd_arg = himci_readl(host->base + MCI_CMD);
	cmd_reg.cmd_arg &= ~MCI_CMD_MASK;
	cmd_reg.bits.start_cmd = 1;
	cmd_reg.bits.card_number = host->port;
	cmd_reg.bits.update_clk_reg_only = 1;
	himci_writel(cmd_reg.cmd_arg, host->base + MCI_CMD);

	if (hi_mci_wait_cmd(host) != 0)
		HIMCI_DEBUG_ERR("Disable or enable CLK is timeout");

}

static void hi_mci_set_cclk(struct himci_host *host, unsigned int cclk)
{
	unsigned int reg_value;
	cmd_arg_s cmd_reg;

	HIMCI_DEBUG_FUN("Function Call: cclk %d", cclk);
	HIMCI_ASSERT(host);
	HIMCI_ASSERT(cclk);

	/*set card clk divider value, clk_divider = Fmmcclk/(Fmmc_cclk * 2) */
	reg_value = 0;
	if (cclk < MMC_CLK) {
		reg_value = MMC_CLK / (cclk * 2);
		if (MMC_CLK % (cclk * 2))
			reg_value++;
		if (reg_value > 0xFF)
			reg_value = 0xFF;
	}
	himci_writel(reg_value, host->base + MCI_CLKDIV);
	host->mmc.tran_speed = reg_value ? MMC_CLK / (reg_value * 2) : MMC_CLK;

	cmd_reg.cmd_arg = himci_readl(host->base + MCI_CMD);
	cmd_reg.cmd_arg &= ~MCI_CMD_MASK;
	cmd_reg.bits.start_cmd = 1;
	cmd_reg.bits.card_number = host->port;
	cmd_reg.bits.update_clk_reg_only = 1;
	himci_writel(cmd_reg.cmd_arg, host->base + MCI_CMD);

	if (hi_mci_wait_cmd(host) != 0)
		HIMCI_DEBUG_ERR("Set card CLK divider is failed");
}
/**********************************************
 *1: card off
 *0: card on
 ***********************************************/
static unsigned int hi_mci_sys_card_detect(struct himci_host *host)
{
		unsigned int card_status;

		card_status = readl(host->base + MCI_CDETECT);
		card_status &= (HIMCI_CARD0 << host->port);

		return card_status >> host->port;
}

static void hi_mci_init_card(struct himci_host *host)
{
	unsigned int tmp_reg;

	HIMCI_DEBUG_FUN("Function Call");
	HIMCI_ASSERT(host);

	hi_mci_sys_reset(host);

	/* card reset */
	himci_writel(~(1<<host->port), host->base + MCI_RESET_N);
	__udelay(CONFIG_MMC_RESET_LOW_TIMEOUT);

	/* card power off and power on */
	hi_mci_ctrl_power(host, POWER_OFF);
	__udelay(CONFIG_MMC_POWER_OFF_TIMEOUT * 1000);
	hi_mci_ctrl_power(host, POWER_ON);
	__udelay(CONFIG_MMC_POWER_ON_TIMEROUT * 1000);

	/* card reset cancel */
	himci_writel(1<<host->port, host->base + MCI_RESET_N);
	__udelay(CONFIG_MMC_RESET_HIGH_TIMEROUT);

	/* set drv/smpl phase shift */
	tmp_reg = himci_readl(host->base + MCI_UHS_REG_EXT);
	tmp_reg &= ~(DRV_PHASE_MASK | SMPL_PHASE_MASK);
	tmp_reg |= DRV_PHASE_SHIFT | SMPL_PHASE_SHIFT;
	himci_writel(tmp_reg, host->base + MCI_UHS_REG_EXT);

	/* clear MMC host intr */
	himci_writel(ALL_INT_CLR, host->base + MCI_RINTSTS);

	/*read write threshold*/
	himci_writel(RW_THRESHOLD_SIZE, host->base + MMC_CARDTHRCTL);

	/* MASK MMC host intr */
	tmp_reg = himci_readl(host->base + MCI_INTMASK);
	tmp_reg &= ~ALL_INT_MASK;
	himci_writel(tmp_reg, host->base + MCI_INTMASK);

	/* enable inner DMA mode and close intr of MMC host controler */
	tmp_reg = himci_readl(host->base + MCI_CTRL);
	tmp_reg &= ~INTR_EN;
	tmp_reg |= USE_INTERNAL_DMA;
	himci_writel(tmp_reg, host->base + MCI_CTRL);

	/* enable dma intr */
	tmp_reg = TI | RI | NI;
	himci_writel(tmp_reg, host->base + MCI_IDINTEN);

	/* set timeout param */
	himci_writel(DATA_TIMEOUT | RESPONSE_TIMEOUT, host->base + MCI_TIMEOUT);

	/* set FIFO param */
	himci_writel(BURST_SIZE | RX_WMARK | TX_WMARK, host->base + MCI_FIFOTH);
}

static void hi_mci_idma_start(struct himci_host *host, struct mmc_data *data)
{
	unsigned int tmp_reg;

	HIMCI_DEBUG_FUN("Function Call");
	HIMCI_ASSERT(host);
	HIMCI_ASSERT(data);

	himci_writel((unsigned long)host->dma_des, host->base + MCI_DBADDR);
	HIMCI_DEBUG_INFO("host->dma_des is 0x%x", (unsigned int)host->dma_des);

	tmp_reg = himci_readl(host->base + MCI_BMOD);
	tmp_reg |= BMOD_DMA_EN;
	himci_writel(tmp_reg, host->base + MCI_BMOD);
}

static void hi_mci_idma_stop(struct himci_host *host)
{
	unsigned int tmp_reg;

	HIMCI_DEBUG_FUN("Function Call");
	HIMCI_ASSERT(host);

	tmp_reg = himci_readl(host->base + MCI_BMOD);
	tmp_reg &= ~BMOD_DMA_EN;
	himci_writel(tmp_reg, host->base + MCI_BMOD);
	himci_writel(0, host->base + MCI_BYTCNT);
	himci_writel(0, host->base + MCI_BLKSIZ);
	himci_writel(0, host->base + MCI_DBADDR);
}

static int hi_mci_setup_data(struct himci_host *host, struct mmc_data *data)
{
	struct himci_dma_des *des = host->dma_des;
	unsigned long des_cnt;
	unsigned long data_size = data->blocks * data->blocksize;
	unsigned long src = (unsigned long)data->src;

	HIMCI_DEBUG_FUN("Function Call");
	HIMCI_ASSERT(host);
	HIMCI_ASSERT(data);

	if (((data_size + 0x1000 - 1) / 0x1000) > MAX_DMA_DES) {
		HIMCI_ERROR("Data size outside the limit of DMA des, "
				"data size: 0x%08lx, limit of DMA des: 0x%08x",
				data_size, 0x1000 * MAX_DMA_DES);
		return -1;
	}

	des_cnt = 0;
	while (data_size) {
		des[des_cnt].idmac_des_ctrl = DMA_DES_OWN | DMA_DES_NEXT_DES;
		des[des_cnt].idmac_des_buf_addr = src;
		des[des_cnt].idmac_des_next_addr =
			(unsigned long)(des + des_cnt + 1);

		if (data_size >= 0x1000) {
			des[des_cnt].idmac_des_buf_size = 0x1000;
			data_size -= 0x1000;
			src += 0x1000;
		} else {
			des[des_cnt].idmac_des_buf_size = data_size;
			data_size = 0;
		}

		HIMCI_DEBUG_INFO("des[%ld] vaddr is 0x%X", des_cnt,
				 (unsigned int)&des[des_cnt]);
		HIMCI_DEBUG_INFO("des[%ld].idmac_des_ctrl is 0x%X", des_cnt,
				 (unsigned int)des[des_cnt].idmac_des_ctrl);
		HIMCI_DEBUG_INFO("des[%ld].idmac_des_buf_size is 0x%X",
				 des_cnt,
				 (unsigned int)des[des_cnt].idmac_des_buf_size);
		HIMCI_DEBUG_INFO("des[%ld].idmac_des_buf_addr 0x%X", des_cnt,
				 (unsigned int)des[des_cnt].idmac_des_buf_addr);
		HIMCI_DEBUG_INFO("des[%ld].idmac_des_next_addr is 0x%X",
			des_cnt,
			(unsigned int)des[des_cnt].idmac_des_next_addr);

		des_cnt++;
	}

	des[0].idmac_des_ctrl |= DMA_DES_FIRST_DES;
	des[des_cnt - 1].idmac_des_ctrl |= DMA_DES_LAST_DES;
	des[des_cnt - 1].idmac_des_next_addr = 0;

	return 0;
}

static int hi_mci_exec_cmd(struct himci_host *host, struct mmc_cmd *cmd,
			   struct mmc_data *data)
{
	volatile cmd_arg_s cmd_reg;

	HIMCI_DEBUG_FUN("Function Call");
	HIMCI_ASSERT(host);
	HIMCI_ASSERT(cmd);

	himci_writel(cmd->cmdarg, host->base + MCI_CMDARG);

	cmd_reg.cmd_arg = himci_readl(host->base + MCI_CMD);
	cmd_reg.cmd_arg &= ~MCI_CMD_MASK;
	if (data) {
		cmd_reg.bits.data_transfer_expected = 1;
		if (data->flags & (MMC_DATA_WRITE | MMC_DATA_READ))
			cmd_reg.bits.transfer_mode = 0;

		if (data->flags & MMC_DATA_WRITE)
			cmd_reg.bits.read_write = 1;
		else if (data->flags & MMC_DATA_READ)
			cmd_reg.bits.read_write = 0;
	} else {
		cmd_reg.bits.data_transfer_expected = 0;
		cmd_reg.bits.transfer_mode = 0;
		cmd_reg.bits.read_write = 0;
	}

	cmd_reg.bits.wait_prvdata_complete = 1;

	switch (cmd->resp_type) {
	case MMC_RSP_NONE:
		cmd_reg.bits.response_expect = 0;
		cmd_reg.bits.response_length = 0;
		cmd_reg.bits.check_response_crc = 0;
		break;
	case MMC_RSP_R1:
	case MMC_RSP_R1b:
		cmd_reg.bits.response_expect = 1;
		cmd_reg.bits.response_length = 0;
		cmd_reg.bits.check_response_crc = 1;
		break;
	case MMC_RSP_R2:
		cmd_reg.bits.response_expect = 1;
		cmd_reg.bits.response_length = 1;
		cmd_reg.bits.check_response_crc = 1;
		break;
	case MMC_RSP_R3:
		cmd_reg.bits.response_expect = 1;
		cmd_reg.bits.response_length = 0;
		cmd_reg.bits.check_response_crc = 0;
		break;
	default:
		HIMCI_ERROR("unhandled response type %02x", cmd->resp_type);
		return -1;
	}

	HIMCI_DEBUG_INFO("Send cmd of card is CMD %d", cmd->cmdidx);

	if (cmd->cmdidx == MMC_CMD_GO_IDLE_STATE)
		cmd_reg.bits.send_initialization = 1;
	else
		cmd_reg.bits.send_initialization = 0;

	cmd_reg.bits.card_number = host->port;
	cmd_reg.bits.cmd_index = cmd->cmdidx;
	cmd_reg.bits.send_auto_stop = 0;
	cmd_reg.bits.start_cmd = 1;
	cmd_reg.bits.update_clk_reg_only = 0;
	himci_writel(cmd_reg.cmd_arg, host->base + MCI_CMD);

	if (hi_mci_wait_cmd(host) != 0) {
		HIMCI_DEBUG_ERR("Send card cmd is failed");
		return -1;
	}
	return 0;
}

static int hi_mci_cmd_done(struct himci_host *host, unsigned int stat)
{
	struct mmc_cmd *cmd = host->cmd;

	HIMCI_DEBUG_FUN("Function Call: stat 0x%08x", stat);
	HIMCI_ASSERT(host);
	HIMCI_ASSERT(cmd);

	host->cmd = NULL;

	if (cmd->resp_type & MMC_RSP_PRESENT) {
		if (cmd->resp_type & MMC_RSP_136) {
			cmd->response[0] = himci_readl(host->base + MCI_RESP3);
			cmd->response[1] = himci_readl(host->base + MCI_RESP2);
			cmd->response[2] = himci_readl(host->base + MCI_RESP1);
			cmd->response[3] = himci_readl(host->base + MCI_RESP0);
			HIMCI_DEBUG_INFO("CMD Response of card is "
					"%08x %08x %08x %08x",
					 cmd->response[0], cmd->response[1],
					 cmd->response[2], cmd->response[3]);
		} else {
			cmd->response[0] = himci_readl(host->base + MCI_RESP0);
			HIMCI_DEBUG_INFO("CMD Response of card is %08x",
					 cmd->response[0]);
		}
		if (host->mmc.version && !IS_SD((&host->mmc))) {
			if ((cmd->resp_type == MMC_RSP_R1)
			    || (cmd->resp_type == MMC_RSP_R1b)) {
				if (cmd->response[0] & MMC_CS_ERROR_MASK) {
					HIMCI_DEBUG_ERR("Card status"
							" stat = 0x%x"
							" is card error!",
							cmd->response[0]);
					return -1;
				}
			}
		}
	}

	if (stat & RTO_INT_STATUS) {
		HIMCI_DEBUG_ERR("CMD status stat = 0x%x is timeout error!",
				stat);
		return TIMEOUT;
	} else if (stat & (RCRC_INT_STATUS | RE_INT_STATUS)) {
		HIMCI_DEBUG_ERR("CMD status stat = 0x%x is response error!",
				stat);
		return -1;
	}
	return 0;
}

static void hi_mci_data_done(struct himci_host *host, unsigned int stat)
{
	HIMCI_DEBUG_FUN("Function Call: stat 0x%08x", stat);
	HIMCI_ASSERT(host);

	if (stat & (HTO_INT_STATUS | DRTO_INT_STATUS)) {
		HIMCI_DEBUG_ERR("Data status stat = 0x%x is timeout error!",
				stat);
	} else if (stat & (EBE_INT_STATUS | SBE_INT_STATUS | FRUN_INT_STATUS
			   | DCRC_INT_STATUS)) {
		HIMCI_DEBUG_ERR("Data status stat = 0x%x is data error!", stat);
	}
}

static int hi_mci_wait_cmd_complete(struct himci_host *host)
{
	unsigned int wait_retry_count = 0;
	unsigned int reg_data = 0;
	int ret = 0;

	HIMCI_DEBUG_FUN("Function Call");
	HIMCI_ASSERT(host);

	do {
		reg_data = himci_readl(host->base + MCI_RINTSTS);
		if (reg_data & CD_INT_STATUS) {
			himci_writel(CD_INT_STATUS | RTO_INT_STATUS
					| RCRC_INT_STATUS | RE_INT_STATUS,
					host->base + MCI_RINTSTS);
			ret = hi_mci_cmd_done(host, reg_data);
			return ret;
		}
		udelay(100);
		wait_retry_count++;
	} while (wait_retry_count < retry_count);

	HIMCI_DEBUG_ERR("Wait cmd complete error! irq status is 0x%x",
			reg_data);

	return -1;
}

static int hi_mci_wait_data_complete(struct himci_host *host)
{
	unsigned int wait_retry_count = 0;
	unsigned int reg_data = 0;

	HIMCI_DEBUG_FUN("Function Call");
	HIMCI_ASSERT(host);

	do {
		reg_data = himci_readl(host->base + MCI_RINTSTS);
		if (reg_data & DTO_INT_STATUS) {
			hi_mci_idma_stop(host);
			hi_mci_data_done(host, reg_data);
			return 0;
		}
		udelay(100);
		wait_retry_count++;
	} while (wait_retry_count < retry_count);

	HIMCI_DEBUG_ERR("Wait cmd complete error! irq status is 0x%x",
			reg_data);

	return -1;
}

static int hi_mci_wait_card_complete(struct himci_host *host)
{
	unsigned int wait_retry_count = 0;
	unsigned int reg_data = 0;

	HIMCI_DEBUG_FUN("Function Call");
	HIMCI_ASSERT(host);

	do {
		reg_data = himci_readl(host->base + MCI_STATUS);
		if (!(reg_data & DATA_BUSY))
			return 0;
		udelay(100);
		wait_retry_count++;
	} while (wait_retry_count < retry_count);

	HIMCI_DEBUG_ERR("Wait card complete error! status is 0x%x", reg_data);

	return -1;
}

static int hi_mci_request(struct mmc *mmc, struct mmc_cmd *cmd,
			  struct mmc_data *data)
{
	struct himci_host *host = mmc->priv;
	unsigned int blk_size;
	unsigned int tmp_reg, fifo_count = 0;
	int ret = 0;

	HIMCI_DEBUG_FUN("Function Call");
	HIMCI_ASSERT(mmc);
	HIMCI_ASSERT(host);
	HIMCI_ASSERT(cmd);

	host->cmd = cmd;

	himci_writel(ALL_INT_CLR, host->base + MCI_RINTSTS);

	/* prepare data */
	if (data) {
		ret = hi_mci_setup_data(host, data);
		if (ret) {
			HIMCI_ERROR("Data setup is error!");
			goto request_end;
		}

		blk_size = data->blocks * data->blocksize;
		himci_writel(blk_size, host->base + MCI_BYTCNT);
		himci_writel(data->blocksize, host->base + MCI_BLKSIZ);

		tmp_reg = himci_readl(host->base + MCI_CTRL);
		tmp_reg |= FIFO_RESET;
		himci_writel(tmp_reg, host->base + MCI_CTRL);

		do {
			tmp_reg = himci_readl(host->base + MCI_CTRL);
			if (fifo_count > retry_count) {
				HIMCI_ERROR("FIFO reset error!");
				break;
			}
			fifo_count++;
		} while (tmp_reg & FIFO_RESET);

		/* start DMA */
		hi_mci_idma_start(host, data);
	} else {
		himci_writel(0, host->base + MCI_BYTCNT);
		himci_writel(0, host->base + MCI_BLKSIZ);
	}

	/* send command */
	ret = hi_mci_exec_cmd(host, cmd, data);
	if (ret) {
		HIMCI_ERROR("CMD execute is error!");
		goto request_end;
	}

	/* wait command send complete */
	ret = hi_mci_wait_cmd_complete(host);
	if (ret)
		goto request_end;

	/* start data transfer */
	if (data) {
		/* wait data transfer complete */
		ret = hi_mci_wait_data_complete(host);
		if (ret)
			goto request_end;

		/* wait card complete */
		ret = hi_mci_wait_card_complete(host);
		if (ret)
			goto request_end;
	}

	if (cmd->resp_type & MMC_RSP_BUSY) {
		/* wait card complete */
		ret = hi_mci_wait_card_complete(host);
		if (ret)
			goto request_end;
	}
request_end:
	/* clear MMC host intr */
	himci_writel(ALL_INT_CLR, host->base + MCI_RINTSTS);
	return ret;
}

static void hi_mci_set_ios(struct mmc *mmc)
{
	struct himci_host *host = mmc->priv;
	unsigned int tmp_reg;

	HIMCI_DEBUG_FUN("Function Call");
	HIMCI_ASSERT(mmc);
	HIMCI_ASSERT(host);

	if (mmc->clock) {
		hi_mci_control_cclk(host, DISABLE);
		hi_mci_set_cclk(host, mmc->clock);
		hi_mci_control_cclk(host, ENABLE);
	} else {
		hi_mci_control_cclk(host, DISABLE);
	}

	/* set bus_width */
	HIMCI_DEBUG_INFO("ios->bus_width = %d", mmc->bus_width);

	tmp_reg = himci_readl(host->base + MCI_CTYPE);
	tmp_reg &= ~CARD_WIDTH_MASK;

	if (mmc->bus_width == 8)
		tmp_reg |= (CARD_WIDTH_8BIT<<host->port);
	else if (mmc->bus_width == 4)
		tmp_reg |= (CARD_WIDTH_4BIT<<host->port);
	else
		tmp_reg |= (CARD_WIDTH_1BIT<<host->port);

	himci_writel(tmp_reg, host->base + MCI_CTYPE);
}

static int hi_mci_init(struct mmc *mmc)
{
	struct himci_host *host = mmc->priv;

	HIMCI_DEBUG_FUN("Function Call");

	hi_mci_init_card(host);

	return 0;
}

static void himci_shutdown(void)
{
	unsigned long base_addr;
	unsigned int value;

#ifdef CONFIG_ARCH_HI3536
	base_addr = SDIO1_BASE_REG;
#endif

#ifdef CONFIG_HI3518EV200
	base_addr = SDIO0_BASE_REG;
#endif

#if (defined CONFIG_HI3516CV300 || defined CONFIG_ARCH_HI3519 || \
		defined CONFIG_ARCH_HI3519V101 || defined CONFIG_ARCH_HI3559 || defined CONFIG_ARCH_HI3556 || \
		defined CONFIG_ARCH_HI3516AV200)
	base_addr = EMMC_BASE_REG;
#endif

	value = readl(base_addr + MCI_CTRL);
	value |= CTRL_RESET | FIFO_RESET | DMA_RESET;

	writel(value, (base_addr + MCI_CTRL));
	writel(POWER_OFF, (base_addr + MCI_PWREN));

	/* Delay 100ms, waiting for the eMMC device power off*/
	udelay(100 * 1000);
}

static int hi_mci_initialize(unsigned int dev_num)
{
	struct mmc *mmc = NULL;
	static struct himci_host *host;
	unsigned int regval;
	unsigned long base_addr = 0;
	HIMCI_DEBUG_FUN("Function Call");

	/* enable SDIO clock and clock 50MHz */
	hi_mci_sys_init(dev_num);

#ifdef CONFIG_ARCH_HI3536
	base_addr = SDIO1_BASE_REG;
#endif

#ifdef CONFIG_HI3518EV200
	base_addr = SDIO0_BASE_REG;
#endif

#if (defined CONFIG_HI3516CV300 || defined CONFIG_ARCH_HI3519 || \
		defined CONFIG_ARCH_HI3519V101 || defined CONFIG_ARCH_HI3559 || defined CONFIG_ARCH_HI3556 || \
		defined CONFIG_ARCH_HI3516AV200)
	if (dev_num == 0)
		base_addr = EMMC_BASE_REG;
	else if (dev_num == 1)
		base_addr = SDIO1_BASE_REG;
	else
		base_addr = SDIO0_BASE_REG;
#endif

	/* check controller version. */
	regval = himci_readl(base_addr +  MCI_VERID);
	if ((regval != MCI_VERID_VALUE) && (regval != MCI_VERID_VALUE2)
			&& (regval != MCI_VERID_VALUE3)) {
		printf("MMC/SD/EMMC controller version incorrect.\n");
		return -ENODEV;
	}

	host = malloc(sizeof(struct himci_host));
	if (!host)
		return -ENOMEM;

	memset(host, 0, sizeof(struct himci_host));

	mmc = &host->mmc;
	mmc->priv = host;

	host->base = base_addr;
	host->dma_des = hi_dma_des;
	host->dev_id = dev_num;
	host->card_status = hi_mci_sys_card_detect(host);
	host->port = 0;
#ifdef CONFIG_EMMC_SUPPORT
#ifdef CONFIG_EMMC_PORT
	host->port = CONFIG_EMMC_PORT;
#endif
#endif

	sprintf(mmc->name, DRIVER_NAME);
	mmc->send_cmd = hi_mci_request;
	mmc->set_ios = hi_mci_set_ios;
	mmc->init = hi_mci_init;
	mmc->host_caps = MMC_MODE_HS | MMC_MODE_HS_52MHz
#if !defined(CONFIG_HI3516CV300)
		| MMC_MODE_4BIT
		| MMC_MODE_8BIT;
#else
		| MMC_MODE_4BIT;
#endif

	mmc->voltages = MMC_VDD_32_33 | MMC_VDD_33_34;

	mmc->f_min = MMC_CCLK_MIN;
	mmc->f_max = MMC_CCLK_MAX;
#ifdef CONFIG_HIMCI_V200
	mmc->is_init = 0;
#endif

	mmc_register(mmc);

	add_shutdown(himci_shutdown);
	return 0;
}

int board_mmc_init(int dev_num)
{
	HIMCI_DEBUG_FUN("Function Call");

	return hi_mci_initialize(dev_num);
}

extern int mmc_send_ext_csd(struct mmc *mmc, char *ext_csd);
extern int mmc_switch(struct mmc *mmc, u8 set, u8 index, u8 value);

void check_ext_csd(struct mmc *mmc)
{
	char ext_csd[512];
	unsigned int tmp, tmp1, boot_part, boot_bus_width;

	HIMCI_DEBUG_FUN("Function Call");

	/* check bootcfg on est_csd */
	int err = mmc_send_ext_csd(mmc, ext_csd);
	if (err) {
		HIMCI_ERROR("Check est_csd error!");
		return;
	}

	boot_part = 0x7;

#if !defined(CONFIG_HI3516CV300)
	if (readl(REG_BASE_SCTL + REG_SYSSTAT) & NF_BOOTBW_MASK)
		boot_bus_width = EXT_CSD_BUS_WIDTH_8; /* 8bits */
	else
#endif
		boot_bus_width = EXT_CSD_BUS_WIDTH_4; /* 4bits */

	tmp = ext_csd[EXT_CSD_RST_N_FUNCTION];
	tmp1 = ext_csd[EXT_CSD_REV];
	if ((tmp1 >= 5)
	    && ((tmp & EXT_CSD_RST_N_EN_MASK) != EXT_CSD_RST_N_ENABLED)) {
		err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
				 EXT_CSD_RST_N_FUNCTION,
				 EXT_CSD_RST_N_ENABLED);
		if (err) {
			HIMCI_ERROR("Set EXT_CSD_RST_N_FUNCTION error!");
			return;
		}
	}

	tmp = ext_csd[EXT_CSD_WR_REL_PARAM];
	tmp1 = ext_csd[EXT_CSD_REV];
	if ((tmp1 >= 5)
	    && ((tmp & EXT_CSD_WR_REL_PARAM_HCR) == EXT_CSD_WR_REL_PARAM_HCR)) {
		tmp1 = ext_csd[EXT_CSD_WR_REL_SET];
		if (tmp1 != EXT_CSD_WR_REL_VALUE) {
			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
					 EXT_CSD_WR_REL_SET,
					 EXT_CSD_WR_REL_VALUE);
			if (err) {
				HIMCI_ERROR("Set EXT_CSD_WR_REL_SET error!");
				return;
			}
		}
	}

	tmp = ext_csd[EXT_CSD_BOOT_CONFIG] >> 0x3 & 0xF;
	tmp1 = ext_csd[EXT_CSD_BOOT_BUS_WIDTH] & 0x3;
	if (tmp == boot_part && tmp1 == boot_bus_width)
		return;

	if (tmp != boot_part) {
		printf("Set EXT_CSD_BOOT_CONFIG!\n");
		tmp = ext_csd[EXT_CSD_BOOT_CONFIG];
		tmp &= (~(0xF << 3));
		tmp |= (boot_part << 3);
		err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
				 EXT_CSD_BOOT_CONFIG, tmp);
		if (err) {
			HIMCI_ERROR("Set EXT_CSD_BOOT_CONFIG error!");
			return;
		}
	}

	if (tmp1 != boot_bus_width) {
		printf("Set EXT_CSD_BOOT_BUS_WIDTH to 0x%X!\n", boot_bus_width);
		tmp1 = ext_csd[EXT_CSD_BOOT_BUS_WIDTH];
		tmp1 &= (~0x3);
		tmp1 |= boot_bus_width;
		err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
				 EXT_CSD_BOOT_BUS_WIDTH, tmp1);
		if (err) {
			HIMCI_ERROR("set EXT_CSD_BOOT_BUS_WIDTH error!");
			return;
		}
	}

	err = mmc_send_ext_csd(mmc, ext_csd);
	if (err) {
		HIMCI_ERROR("Check est_csd error!");
		return;
	}

	tmp = ext_csd[EXT_CSD_BOOT_CONFIG] >> 0x3 & 0xF;
	tmp1 = ext_csd[EXT_CSD_BOOT_BUS_WIDTH] & 0x3;

	if (tmp == boot_part && tmp1 == boot_bus_width)
		printf("EXT_CSD CONFIG OK!\n");
	else
		printf("EXT_CSD CONFIG Fail!\n");
}

int select_boot_part(struct mmc *mmc, unsigned int boot_part)
{
	char ext_csd[512];
	unsigned int tmp;

	HIMCI_DEBUG_FUN("Function Call");

	int err = mmc_send_ext_csd(mmc, ext_csd);
	if (err) {
		HIMCI_ERROR("Read boot part error!");
		return -1;
	}

	boot_part = boot_part & 0x7;
	tmp = ext_csd[EXT_CSD_BOOT_CONFIG] & 0x7;
	if (tmp == boot_part)
		return 0;

	tmp = ext_csd[EXT_CSD_BOOT_CONFIG];
	tmp &= (~0x7);
	tmp |= boot_part;
	err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_BOOT_CONFIG, tmp);
	if (err) {
		HIMCI_ERROR("Select boot part error!");
		return -1;
	}

	err = mmc_send_ext_csd(mmc, ext_csd);
	if (err) {
		HIMCI_ERROR("Read boot part error!");
		return -1;
	}

	tmp = ext_csd[EXT_CSD_BOOT_CONFIG] & 0x7;
	if (tmp == boot_part)
		HIMCI_DEBUG_INFO("Select boot part OK!\n");
	else
		HIMCI_DEBUG_INFO("Select boot part Fail!\n");

	return (tmp == boot_part) ? 0 : -1;
}

void print_ext_csd(struct mmc *mmc)
{
	char ext_csd[512];
	unsigned int tmp;

	HIMCI_DEBUG_FUN("Function Call");

	int err = mmc_send_ext_csd(mmc, ext_csd);
	if (err) {
		HIMCI_ERROR("Check est_csd error!");
		return;
	}

	HIMCI_DEBUG_INFO("Extended CSD register:");
	for (tmp = 0; tmp < 512; tmp += 8)
		HIMCI_DEBUG_INFO
		    ("%03d: %02x %02x %02x %02x %02x %02x %02x %02x", tmp,
		     ext_csd[tmp], ext_csd[tmp + 1], ext_csd[tmp + 2],
		     ext_csd[tmp + 3], ext_csd[tmp + 4], ext_csd[tmp + 5],
		     ext_csd[tmp + 6], ext_csd[tmp + 7]);
}

static void print_mmcinfo(struct mmc *mmc)
{
	struct himci_host *host = mmc->priv;

	HIMCI_DEBUG_FUN("Function Call");

	if (hi_mci_sys_card_detect(host))
		printf("Device is not exit ! \n");

	printf("MMC/SD Card:\n");
	printf("    MID:         0x%x\n", mmc->cid[0] >> 24);
	printf("    Read Block:  %d Bytes\n", mmc->read_bl_len);
	printf("    Write Block: %d Bytes\n", mmc->write_bl_len);
	printf("    Chip Size:   %s Bytes (%s)\n",
	       ultohstr(mmc->capacity),
	       mmc->high_capacity ? "High Capacity" : "Low Capacity");
	printf("    Name:        \"%c%c%c%c%c\"\n",
	       mmc->cid[0] & 0xff,
	       (mmc->cid[1] >> 24),
	       (mmc->cid[1] >> 16) & 0xff,
	       (mmc->cid[1] >> 8) & 0xff,
	       mmc->cid[1] & 0xff);

	printf("    Chip Type:   %s\n"
	       "    Version:     %d.%d\n",
	       IS_SD(mmc) ? "SD" : "MMC",
	       (mmc->version >> 4) & 0xf, mmc->version & 0xf);

	printf("    Speed:       %sHz\n", ultohstr(mmc->tran_speed));
	printf("    Bus Width:   %dbit\n", mmc->bus_width);
	printf("    Boot Addr:   %d Bytes\n", CONFIG_MMC_BOOT_ADDR);
}

int print_mmc_reg(int dev_num, int show_ext_csd)
{
	struct mmc *mmc;
	unsigned int tmp;
	char ext_csd[512];

	mmc = find_mmc_device(dev_num);
	if (!mmc) {
		printf("No MCI found!!!\n");
		return -1;
	}

	printf("OCR register: %08x\n", mmc->ocr);
	printf("CID register: %08x %08x %08x %08x\n",
	       mmc->cid[0], mmc->cid[1], mmc->cid[2], mmc->cid[3]);
	printf("CSD register: %08x %08x %08x %08x\n",
	       mmc->csd[0], mmc->csd[1], mmc->csd[2], mmc->csd[3]);
	printf("RCA register: %08x\n", mmc->rca);
	if (show_ext_csd) {
		int err = mmc_send_ext_csd(mmc, ext_csd);
		if (err) {
			printf("Check est_csd error!\n");
			return -1;
		}
		printf("Extended CSD register:\n");
		for (tmp = 0; tmp < 512; tmp += 8)
			printf("%03d: %02x %02x %02x %02x"
				" %02x %02x %02x %02x\n",
			       tmp,
			       ext_csd[tmp],
			       ext_csd[tmp+1],
			       ext_csd[tmp+2],
			       ext_csd[tmp+3],
			       ext_csd[tmp+4],
			       ext_csd[tmp+5],
			       ext_csd[tmp+6],
			       ext_csd[tmp+7]);
	}
	printf("\n");
	return 0;
}

int mmc_flash_init(int dev_num)
{
	struct mmc *mmc;
	int err = 0;

	puts("\nEMMC/MMC/SD controller initialization.\n");

	mmc = find_mmc_device(dev_num);
	if (!mmc)
		return -1;

	err = mmc_init(mmc);
	if (err) {
		printf("No EMMC/MMC/SD device found !\n");
		return err;
	}

	HIMCI_DEBUG_INFO("OCR register: %08x", mmc->ocr);
	HIMCI_DEBUG_INFO("CID register: %08x %08x %08x %08x",
			mmc->cid[0], mmc->cid[1],
			mmc->cid[2], mmc->cid[3]);
	HIMCI_DEBUG_INFO("CSD register %08x %08x %08x %08x",
			mmc->csd[0], mmc->csd[1],
			mmc->csd[2], mmc->csd[3]);
	HIMCI_DEBUG_INFO("RCA register: %08x", mmc->rca);
#if HI_MCI_DEBUG
	if (debug_type & HIMCI_DEBUG_TYPE_INFO)
		print_ext_csd(mmc);
#endif

	print_mmcinfo(mmc);

	if (!IS_SD(mmc))
		check_ext_csd(mmc);

	return 0;
}
