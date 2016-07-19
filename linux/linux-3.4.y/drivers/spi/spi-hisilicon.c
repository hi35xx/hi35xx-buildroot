/* linux/drivers/spi/spi-hisilicon.c
 *
 * HISILICON SPI Controller driver
 *
 * Copyright (c) 2014 Hisilicon Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * History:
 *      16-September-2014 create this file
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi-hisilicon.h>

/* ********** spi reg offset define *************** */
#define REG_SPI_CR0		0x00
#define SPI_CR0_SCR_SHIFT	(8)
#define SPI_CR0_SPH_SHIFT	(7)
#define SPI_CR0_SPO_SHIFT	(6)
#define SPI_CR0_FRF_SHIFT	(4)
#define SPI_CR0_DSS_SHIFT	(0)
#define SPI_CR0_SCR		(0xff << 8) /* clkout=clk/(cpsdvsr*(scr+1)) */
#define SPI_CR0_SPH		(0x1 << 7) /* spi phase */
#define SPI_CR0_SPO		(0x1 << 6) /* spi clk polarity */
#define SPI_CR0_FRF		(0x3 << 4) /* frame format set */
#define SPI_CR0_DSS		(0xf << 0) /* data bits width */

#define REG_SPI_CR1		0x04
#define SPI_CR1_WAIT_EN_SHIFT	(15)
#define SPI_CR1_WAIT_VAL_SHIFT	(8)
#define SPI_CR1_ALT_SHIFT	(6)
#define SPI_CR1_BIG_END_SHIFT	(4)
#define SPI_CR1_MS_SHIFT	(2)
#define SPI_CR1_SSE_SHIFT	(1)
#define SPI_CR1_LBN_SHIFT	(0)
#define SPI_CR1_WAIT_EN		(0x1 << 15)
#define SPI_CR1_WAIT_VAL	(0x7f << 8)

/* alt mode:spi enable csn is select; spi disable csn is cancel */
#define SPI_CR1_ALT		(0x1 << 6)
#define SPI_CR1_BIG_END		(0x1 << 4) /* big end or little */
#define SPI_CR1_MS		(0x1 << 2) /* master-slave mode */
#define SPI_CR1_SSE		(0x1 << 1) /* spi enable set */
#define SPI_CR1_LBN		(0x1 << 0) /* loopback mode */

#define REG_SPI_DR		0x08

#define REG_SPI_SR		0x0c
#define SPI_SR_BSY_SHIFT	(4)
#define SPI_SR_RFF_SHIFT	(3)
#define SPI_SR_RNE_SHIFT	(2)
#define SPI_SR_TNF_SHIFT	(1)
#define SPI_SR_TFE_SHIFT	(0)
#define SPI_SR_BSY		(0x1 << 4)/* spi busy flag */
#define SPI_SR_RFF		(0x1 << 3)/* Whether to send fifo is full */
#define SPI_SR_RNE		(0x1 << 2)/* Whether to send fifo is no empty */
#define SPI_SR_TNF		(0x1 << 1)/* Whether to send fifo is no full */
#define SPI_SR_TFE		(0x1 << 0)/* Whether to send fifo is empty */

#define REG_SPI_CPSR		0x10
#define SPI_CPSR_CPSDVSR_SHIFT	(0)
#define SPI_CPSR_CPSDVSR	(0xff << 0)  /* even 2~254 */

#define REG_SPI_IMSC		0x14
#define REG_SPI_RIS		0x18
#define REG_SPI_MIS		0x1c
#define REG_SPI_ICR		0x20

/* ********************** spi cs ******************************* */
#define SPI_CS_ACTIVE		0
#define SPI_CS_INACTIVE		1

#define MAX_WAIT	256

#define SCR_MAX		255
#define SCR_MIN		0
#define CPSDVSR_MAX	254
#define CPSDVSR_MIN	2

/*-------------------------------------------------------------------------*/

/**
 * struct hi_spi_driver_data - Runtime info holder for SPI driver.
 * @spd: Platform specific data for the controller this driver manages.
 * @master: Pointer to the SPI Protocol master.
 * @spi_n_reg_vbase: BUS virtual address of SPI controller regs.
 * @max_speed: spi transfer output max speed hz.
 * @max_speed: spi transfer output min speed hz.
 * @cur_mode: Stores the active configuration of the controller.
 * @cur_bits_per_word: Stores the active bits per word settings.
 * @cur_speed: Stores the active xfer clock speed.
 */
struct hi_spi_driver_data {
	struct platform_device			*pdev;
	struct hi_spi_platform_data		*spd;
	struct spi_master			*master;
	void  __iomem				*spi_n_reg_vbase;
	unsigned long				max_speed;
	unsigned long				min_speed;
	unsigned int				cur_speed;
	unsigned int				cur_mode;
	unsigned int				cur_bits_per_word;
};

/*-------------------------------------------------------------------------*/

static void hi_spi_enable(struct hi_spi_driver_data *sdd)
{
	unsigned int value;

	value = readl(sdd->spi_n_reg_vbase + REG_SPI_CR1);

	value |= SPI_CR1_SSE;
	writel(value, sdd->spi_n_reg_vbase + REG_SPI_CR1);
}

static void hi_spi_disable(struct hi_spi_driver_data *sdd)
{
	unsigned int value;

	value = readl(sdd->spi_n_reg_vbase + REG_SPI_CR1);

	value &= ~SPI_CR1_SSE;
	writel(value, sdd->spi_n_reg_vbase + REG_SPI_CR1);
}

static int hi_spi_set_cs(struct hi_spi_driver_data *sdd,
		unsigned char csn, unsigned char flag)
{
	struct hi_spi_platform_data *spd = sdd->spd;

	if (spd->cfg_cs(sdd->master->bus_num, csn))
		return -1;

	if (flag == SPI_CS_ACTIVE)
		hi_spi_enable(sdd);
	else
		hi_spi_disable(sdd);

	return 0;
}

static int hi_spi_config(struct hi_spi_driver_data *sdd)
{
	struct hi_spi_platform_data *spd = sdd->spd;
	unsigned int tmp;
	unsigned int value;
	unsigned int scr, cpsdvsr;

	hi_spi_disable(sdd);

	/* Check if we can provide the requested rate */
	if (sdd->cur_speed > sdd->max_speed)  /* Max possible */
		sdd->cur_speed = sdd->max_speed;

	/* Min possible */
	if (sdd->cur_speed < sdd->min_speed) {
		dev_err(NULL, "config: sdd->cur_speed is %d not supported!\n",
				sdd->cur_speed);
		return -EINVAL;
	}

	/* Check if we can provide the requested bits_per_word */
	if ((sdd->cur_bits_per_word < 4) || (sdd->cur_bits_per_word > 16)) {
		dev_err(NULL, "config: sdd->cur_bits_per_word is %d not supported!\n",
				sdd->cur_bits_per_word);
		return -EINVAL;
	}

	/*compute spi speed, speed=clk/(cpsdvsr*(scr+1)) */
	tmp = (spd->clk_rate) / (sdd->cur_speed);
	if (tmp < CPSDVSR_MIN) {
		cpsdvsr = CPSDVSR_MIN;
		scr = 0;
	} else if (tmp <= CPSDVSR_MAX) {
		cpsdvsr = tmp & (~0x1);
		scr = (tmp / cpsdvsr) - 1;
	} else {
		cpsdvsr = CPSDVSR_MAX;
		scr = (tmp / cpsdvsr) - 1;
	}

	/* config SPICPSR register */
	value = readl(sdd->spi_n_reg_vbase + REG_SPI_CPSR);

	value &= ~SPI_CPSR_CPSDVSR;
	value |= cpsdvsr << SPI_CPSR_CPSDVSR_SHIFT;

	writel(value, sdd->spi_n_reg_vbase + REG_SPI_CPSR);

	/* config SPICR0 register */
	value = readl(sdd->spi_n_reg_vbase + REG_SPI_CR0);

	value &= ~SPI_CR0_DSS;
	value |= (sdd->cur_bits_per_word - 1) << SPI_CR0_DSS_SHIFT;

	value &= ~SPI_CR0_FRF;

	value &= ~SPI_CR0_SPO;
	tmp = (!!(sdd->cur_mode & SPI_CPOL)) << SPI_CR0_SPO_SHIFT;
	value |= tmp;

	value &= ~SPI_CR0_SPH;
	tmp = (!!(sdd->cur_mode & SPI_CPHA)) << SPI_CR0_SPH_SHIFT;
	value |= tmp;

	value &= ~SPI_CR0_SCR;
	value |= (scr << SPI_CR0_SCR_SHIFT);

	writel(value, sdd->spi_n_reg_vbase + REG_SPI_CR0);

	/* config SPICR1 register */
	value = readl(sdd->spi_n_reg_vbase + REG_SPI_CR1);

	value &= ~SPI_CR1_LBN;
	tmp = (!!(sdd->cur_mode & SPI_LOOP)) << SPI_CR1_LBN_SHIFT;
	value |= tmp;

	value &= ~SPI_CR1_MS;

	value &= ~SPI_CR1_BIG_END;
	tmp = (!!(sdd->cur_mode & SPI_LSB_FIRST)) << SPI_CR1_BIG_END_SHIFT;
	value |= tmp;

	value &= ~SPI_CR1_ALT;
	value |= 0x1 << SPI_CR1_ALT_SHIFT;

	writel(value, sdd->spi_n_reg_vbase + REG_SPI_CR1);

	return 0;
}

/*
 * wait for send fifo empty and  spi bus no busy
 */
static int hi_spi_check_timeout(struct hi_spi_driver_data *sdd)
{
	unsigned int value;
	unsigned int tmp = 0;

	while (1) {
		value = readl(sdd->spi_n_reg_vbase + REG_SPI_SR);
		if ((value & SPI_SR_TFE) && (!(value & SPI_SR_BSY)))
			break;

		if (tmp++ > MAX_WAIT) {
			dev_err(NULL, "%s spi transfer wait timeout!\n",
					__func__);
			return -EIO;
		}

		udelay(10);
	}

	return 0;
}

static int hi_spi_flush_fifo(struct hi_spi_driver_data *sdd)
{
	unsigned int value;
	unsigned int tmp;

	tmp = hi_spi_check_timeout(sdd);
	if (tmp)
		return tmp;

	tmp = 0;
	while (1) {
		value = readl(sdd->spi_n_reg_vbase + REG_SPI_SR);
		if (!(value & SPI_SR_RNE))
			break;

		if (tmp++ > sdd->spd->fifo_size) {
			dev_err(NULL, "%s spi transfer check rx fifo wait timeout!\n",
					__func__);
			return -EIO;
		}

		readl(sdd->spi_n_reg_vbase + REG_SPI_DR);
	}

	return 0;
}

static int hi_spi_txrx8(struct hi_spi_driver_data *sdd, struct spi_transfer *st)
{
	unsigned int	len = st->len;
	unsigned int	tmp_len;
	unsigned int	count;
	const u8	*tx = st->tx_buf;
	u8		*rx = st->rx_buf;
	u8		value;
	unsigned int tmp;

	while (len) {
		if (len > sdd->spd->fifo_size)
			tmp_len = sdd->spd->fifo_size;
		else
			tmp_len = len;

		len -= tmp_len;

		/* wirte fifo */
		count = tmp_len;
		value = 0;
		while (count) {
			if (tx)
				value = *tx++;

			writel(value, sdd->spi_n_reg_vbase + REG_SPI_DR);
			hi_msg("write value:0x%x\n", value);
			count -= 1;
		}

		tmp = hi_spi_check_timeout(sdd);
		if (tmp)
			return tmp;

		/* read fifo*/
		count = tmp_len;
		value = 0;
		while (count) {
			value = readl(sdd->spi_n_reg_vbase + REG_SPI_DR);
			if (rx)
				*rx++ = value;

			hi_msg("read value:0x%x\n", value);
			count -= 1;
		}

	}
	return 0;
}

static int hi_spi_txrx16(struct hi_spi_driver_data *sdd,
		struct spi_transfer *st)
{
	unsigned int	len = st->len;
	unsigned int	tmp_len = 0;
	unsigned int	count = 0;
	const u16	*tx = st->tx_buf;
	u16		*rx = st->rx_buf;
	u16		value = 0;
	unsigned int tmp;

	while (len) {
		tmp = sdd->spd->fifo_size * 2;
		if (len > tmp)
			tmp_len = tmp;
		else
			tmp_len = len;

		len -= tmp_len;

		/* wirte fifo */
		count = tmp_len;
		value = 0;
		while (count >= 2) {
			if (tx)
				value = *tx++;

			writel(value, sdd->spi_n_reg_vbase + REG_SPI_DR);
			hi_msg("write value:0x%x\n", value);
			count -= 2;
		}

		tmp = hi_spi_check_timeout(sdd);
		if (tmp)
			return tmp;

		/* read fifo*/
		count = tmp_len;
		value = 0;
		while (count >= 2) {
			value = readl(sdd->spi_n_reg_vbase + REG_SPI_DR);
			if (rx)
				*rx++ = value;

			hi_msg("read value:0x%x\n", value);
			count -= 2;
		}

	}
	return 0;
}

static int hi_spi_txrx(struct hi_spi_driver_data *sdd,
		struct spi_transfer *transfer)
{
	int ret = 0;

	ret = hi_spi_flush_fifo(sdd);
	if (ret)
		return ret;

	if (sdd->cur_bits_per_word <= 8)
		ret = hi_spi_txrx8(sdd, transfer);
	else
		ret = hi_spi_txrx16(sdd, transfer);

	return ret;
}

/*
 * Here we only check the validity of requested configuration
 * and save the configuration in a local data-structure.
 * The controller is actually configured only just before we
 * get a message to transfer.
 */
static int hi_spi_setup(struct spi_device *spi)
{
	struct hi_spi_driver_data *sdd;

	hi_msg("\n");
	sdd = spi_master_get_devdata(spi->master);

	/* Check if we can provide the requested rate */
	if (spi->max_speed_hz > sdd->max_speed)
		spi->max_speed_hz = sdd->max_speed;

	if (spi->max_speed_hz < sdd->min_speed) {
		dev_err(&spi->dev, "setup: spi->max_speed_hz is %d not supported!\n",
				spi->max_speed_hz);
		return -EINVAL;
	}

	/* Check if we can provide the requested bits_per_word */
	if ((spi->bits_per_word < 4) || (spi->bits_per_word > 16)) {
		dev_err(&spi->dev, "setup: spi->bits_per_word is %d not supported!\n",
				spi->bits_per_word);
		return -EINVAL;
	}
	return 0;
}


static int hi_spi_transfer_one_message(struct spi_master *master,
		struct spi_message *msg)
{
	struct hi_spi_driver_data *sdd = spi_master_get_devdata(master);
	struct spi_device *spi = msg->spi;
	struct spi_transfer *transfer;
	int status = 0;
	unsigned int speed;
	unsigned char bpw;

	hi_msg("\n");

	/* If Master's(controller) state differs from that needed by Slave */
	if ((sdd->cur_speed != spi->max_speed_hz) ||
			(sdd->cur_mode != spi->mode) ||
			(sdd->cur_bits_per_word != spi->bits_per_word)) {
		unsigned int	save_speed;
		unsigned int	save_mode;
		unsigned int	save_bits_per_word;

		save_speed = sdd->cur_speed;
		save_mode = sdd->cur_mode;
		save_bits_per_word = sdd->cur_bits_per_word;

		sdd->cur_speed = spi->max_speed_hz;
		sdd->cur_mode = spi->mode;
		sdd->cur_bits_per_word = spi->bits_per_word;

		status = hi_spi_config(sdd);
		if (status) {
			sdd->cur_speed = save_speed;
			sdd->cur_mode = save_mode;
			sdd->cur_bits_per_word = save_bits_per_word;
			goto end;
		}
	}

	list_for_each_entry(transfer, &msg->transfers, transfer_list)
	{
		/* Only bpw and speed may change across transfers */
		speed = transfer->speed_hz ? : spi->max_speed_hz;
		bpw = transfer->bits_per_word ? : spi->bits_per_word;

		if (bpw > 8) {
			if (transfer->len % 2) {
				dev_err(&spi->dev, "transfer->len(%u) is invalid for bits_per_word size(%u)\n",
						transfer->len, bpw);
				status = -EIO;
				goto end;
			}
		}

		if (speed != sdd->cur_speed || bpw != sdd->cur_bits_per_word) {
			unsigned int	save_speed;
			unsigned int	save_bits_per_word;

			save_speed = sdd->cur_speed;
			save_bits_per_word = sdd->cur_bits_per_word;

			sdd->cur_speed = speed;
			sdd->cur_bits_per_word = bpw;

			status = hi_spi_config(sdd);
			if (status) {
				sdd->cur_speed = save_speed;
				sdd->cur_bits_per_word = save_bits_per_word;
				goto end;
			}
		}

		/* slave chip select */
		hi_spi_set_cs(sdd, spi->chip_select, SPI_CS_ACTIVE);

		status = hi_spi_txrx(sdd, transfer);
		if (status) {
			hi_spi_set_cs(sdd, spi->chip_select, SPI_CS_INACTIVE);
			goto end;
		}

		if (transfer->delay_usecs)
			udelay(transfer->delay_usecs);

		if (transfer->cs_change)
			hi_spi_set_cs(sdd, spi->chip_select, SPI_CS_INACTIVE);

		msg->actual_length += transfer->len;
	}

end:
	msg->status = status;

	spi_finalize_current_message(master);

	return status;
}

static int hi_spi_prepare_transfer(struct spi_master *spi)
{
	return 0;
}

static int hi_spi_unprepare_transfer(struct spi_master *spi)
{
	return 0;
}


static int hi_spi_hw_init_cfg(struct hi_spi_driver_data *sdd)
{
	struct hi_spi_platform_data *spd = sdd->spd;

	if (spd->hw_init_cfg(sdd->master->bus_num))
		return -1;

	if (hi_spi_config(sdd))
		return -1;

	return 0;
}

static int hi_spi_hw_exit_cfg(struct hi_spi_driver_data *sdd)
{
	struct hi_spi_platform_data *spd = sdd->spd;

	hi_spi_disable(sdd);

	if (spd->hw_exit_cfg(sdd->master->bus_num))
		return -1;

	return 0;
}

static int __init hi_spi_probe(struct platform_device *pdev)
{
	struct hi_spi_driver_data *sdd;
	struct hi_spi_platform_data *spd;
	struct spi_master *master;
	struct resource	*mem_res;
	int ret;

	hi_msg("\n");

	if (pdev->id < 0) {
		dev_err(&pdev->dev, "Invalid platform device id-%d\n",
				pdev->id);
		return -ENODEV;
	}

	if (pdev->dev.platform_data == NULL) {
		dev_err(&pdev->dev, "platform_data missing!\n");
		return -ENODEV;
	}
	spd = pdev->dev.platform_data;

	/* Check for availability of necessary resource */
	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (mem_res == NULL) {
		dev_err(&pdev->dev, "Unable to get SPI MEM resource\n");
		return -ENXIO;
	}

	master = spi_alloc_master(&pdev->dev,
				sizeof(struct hi_spi_driver_data));
	if (master == NULL) {
		dev_err(&pdev->dev, "Unable to allocate SPI Master\n");
		return -ENOMEM;
	}

	platform_set_drvdata(pdev, master);

	master->bus_num = pdev->id;
	master->setup = hi_spi_setup;
	master->prepare_transfer_hardware = hi_spi_prepare_transfer;
	master->transfer_one_message = hi_spi_transfer_one_message;
	master->unprepare_transfer_hardware = hi_spi_unprepare_transfer;
	master->num_chipselect = spd->num_cs;

	/* the spi->mode bits understood by this driver: */
	master->mode_bits = SPI_MODE_0;

	sdd = spi_master_get_devdata(master);

	sdd->pdev = pdev;
	sdd->spd = spd;
	sdd->master = master;
	sdd->spi_n_reg_vbase = ioremap_nocache(mem_res->start,
			resource_size(mem_res));
	if (sdd->spi_n_reg_vbase == NULL) {
		dev_err(&pdev->dev, "Unable to remap IO\n");
		ret = -ENXIO;
		goto error_end0;
	}

	sdd->max_speed = (spd->clk_rate) / ((SCR_MIN + 1) * CPSDVSR_MIN);
	sdd->min_speed = (spd->clk_rate) / ((SCR_MAX + 1) * CPSDVSR_MAX);
	sdd->cur_speed = (sdd->max_speed) / 10;
	sdd->cur_mode = SPI_MODE_0;
	sdd->cur_bits_per_word = 8;

	hi_spi_hw_init_cfg(sdd);	/* Setup Deufult Mode */

	ret = spi_register_master(master);
	if (ret) {
		dev_err(&pdev->dev, "cannot register SPI master\n");
		ret = -EBUSY;
		goto error_end0;
	}

	dev_notice(&pdev->dev,
			"with %d chip select slaves attached\n",
			master->num_chipselect);
	return 0;

error_end0:
	platform_set_drvdata(pdev, NULL);

	return ret;
}

static int hi_spi_remove(struct platform_device *pdev)
{
	struct spi_master *master = platform_get_drvdata(pdev);
	struct hi_spi_driver_data *sdd = spi_master_get_devdata(master);

	hi_msg("\n");

	spi_unregister_master(master);

	hi_spi_hw_exit_cfg(sdd);

	platform_set_drvdata(pdev, NULL);

	return 0;
}

#ifdef CONFIG_PM
static int hi_spi_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct spi_master *master = platform_get_drvdata(pdev);
	struct hi_spi_driver_data *sdd = spi_master_get_devdata(master);

	hi_msg("\n");

	spi_master_suspend(master);

	hi_spi_hw_exit_cfg(sdd);

	return 0;
}

static int hi_spi_resume(struct platform_device *pdev)
{
	struct spi_master *master = platform_get_drvdata(pdev);
	struct hi_spi_driver_data *sdd = spi_master_get_devdata(master);

	hi_msg("\n");

	hi_spi_hw_init_cfg(sdd);

	spi_master_resume(master);

	return 0;
}
#else
#define hi_spi_suspend	NULL
#define	hi_spi_resume	NULL
#endif /* CONFIG_PM */


static struct platform_driver hi_spi_platform_driver = {
	.probe		= hi_spi_probe,
	.remove		= hi_spi_remove,
	.suspend	= hi_spi_suspend,
	.resume		= hi_spi_resume,
	.driver		= {
		.name	= "hi-spi-master",
		.owner	= THIS_MODULE,
	},
};

static struct resource hi_spi0_resource[] = {
	[0] = DEFINE_RES_MEM(CONFIG_HI_SPI0_IO_BASE, CONFIG_HI_SPI0_IO_SIZE),
};

static struct hi_spi_platform_data hi_spi0_pd = {
	.fifo_size	= CONFIG_HI_SPI0_FIFO_SIZE,
};

static struct platform_device hi_spi0_platform_device = {
	.name           = "hi-spi-master",
	.id             = 0,
	.num_resources  = ARRAY_SIZE(hi_spi0_resource),
	.resource       = hi_spi0_resource,
};

static struct resource hi_spi1_resource[] = {
	[0] = DEFINE_RES_MEM(CONFIG_HI_SPI1_IO_BASE, CONFIG_HI_SPI1_IO_SIZE),
};

static struct hi_spi_platform_data hi_spi1_pd = {
	.fifo_size	= CONFIG_HI_SPI1_FIFO_SIZE,
};

static struct platform_device hi_spi1_platform_device = {
	.name           = "hi-spi-master",
	.id             = 1,
	.num_resources  = ARRAY_SIZE(hi_spi1_resource),
	.resource       = hi_spi1_resource,
};


static struct platform_device *hi_spi_platform_devices[] = {
	&hi_spi0_platform_device,
	&hi_spi1_platform_device,
};

static struct hi_spi_platform_data *hi_spi_pds[] = {
	&hi_spi0_pd,
	&hi_spi1_pd,
};

static int __init hi_spi_init(void)
{
	int ret;
	int i;

	hi_msg("compile time:%s  %s\n", __DATE__, __TIME__);

	ret = platform_driver_register(&hi_spi_platform_driver);
	if (ret) {
		dev_err(NULL, "%s platform_driver_register() error!\n",
				__func__);
		goto error_end0;
	}
	hi_msg("\n");

	for (i = 0; i < ARRAY_SIZE(hi_spi_platform_devices); i++) {
		ret = hi_spi_set_platdata(hi_spi_pds[i],
				hi_spi_platform_devices[i]);
		if (ret) {
			dev_err(NULL, "%s hi_spi_set_platdata() error!\n",
					__func__);
			goto error_end0;
		}
	}

	ret = platform_add_devices(hi_spi_platform_devices,
			ARRAY_SIZE(hi_spi_platform_devices));
	if (ret) {
		dev_err(NULL, "%s platform_add_devices() error!\n", __func__);
		goto error_end1;
	}

	return 0;

error_end1:
	platform_driver_unregister(&hi_spi_platform_driver);

error_end0:
	return ret;
}
subsys_initcall(hi_spi_init);

static void __exit hi_spi_exit(void)
{
	unsigned char i;

	hi_msg("\n");

	for (i = 0; i < ARRAY_SIZE(hi_spi_platform_devices); i++)
		platform_device_unregister(hi_spi_platform_devices[i]);

	platform_driver_unregister(&hi_spi_platform_driver);
}
module_exit(hi_spi_exit);

MODULE_AUTHOR("BVT OSDRV");
MODULE_DESCRIPTION("HISILICON SPI Bus driver");
MODULE_LICENSE("GPL");
