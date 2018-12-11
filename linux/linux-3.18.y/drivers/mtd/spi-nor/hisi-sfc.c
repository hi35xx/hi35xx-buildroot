/*
 * HiSilicon SPI Nor Flash Controller Driver
 *
 * Copyright (c) 2015-2016 HiSilicon Technologies Co., Ltd.
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
 */
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/mtd/mtd.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/mtd/spi-nor.h>
#include <mach/platform.h>
#include <linux/mfd/hisi_fmc.h>

#define FMC_OP_DMA			0x68

struct hifmc_priv {
	u32 chipselect;
	u32 clkrate;
	struct hifmc_host *host;
};

struct hifmc_host {
	struct device *dev;
	struct mutex *lock;

	void __iomem *regbase;
	void __iomem *iobase;
	struct clk *clk;
	void *buffer;
	dma_addr_t dma_buffer;

	struct spi_nor	*nor[HIFMC_MAX_CHIP_NUM];
	struct hifmc_priv priv[HIFMC_MAX_CHIP_NUM];
	int num_chip;
};

/******************************************************************************/
static inline int wait_op_finish(struct hifmc_host *host)
{
	unsigned int reg, timeout = FMC_WAIT_TIMEOUT;

	do {
		reg = hifmc_readl(host, FMC_INT);
	} while (!(reg & FMC_INT_OP_DONE) && --timeout);

	if (!timeout) {
		dev_dbg(host->dev, "wait for operation finish timeout\n");
		return -EAGAIN;
	}

	return 0;
}

static int get_if_type(enum spi_nor_protocol mode)
{
	enum hifmc_iftype if_type;

	switch (mode) {
	case SNOR_PROTO_1_1_2:
		if_type = IF_TYPE_DUAL;
		break;
	case SNOR_PROTO_1_2_2:
		if_type = IF_TYPE_DIO;
		break;
	case SNOR_PROTO_1_1_4:
		if_type = IF_TYPE_QUAD;
		break;
	case SNOR_PROTO_1_4_4:
		if_type = IF_TYPE_QIO;
		break;
	case SNOR_PROTO_1_1_1:
	default:
		if_type = IF_TYPE_STD;
		break;
	}

	return if_type;
}

/******************************************************************************/
static void spi_nor_switch_spi_type(struct hifmc_host *host)
{
	unsigned int reg;

	reg = hifmc_readl(host, FMC_CFG);
	reg &= ~FLASH_TYPE_SEL_MASK;
	reg |= FMC_CFG_FLASH_SEL(0);
	hifmc_writel(host, FMC_CFG, reg);
}

/******************************************************************************/
static void hisi_spi_nor_init(struct hifmc_host *host)
{
	unsigned int reg;

	/* switch the flash type to spi nor */
	spi_nor_switch_spi_type(host);

	/* set the boot mode to normal */
	reg = hifmc_readl(host, FMC_CFG);
	if ((reg & FMC_CFG_OP_MODE_MASK) == FMC_CFG_OP_MODE_BOOT) {
		reg |= FMC_CFG_OP_MODE(FMC_CFG_OP_MODE_NORMAL);
		hifmc_writel(host, FMC_CFG, reg);
	}

	/* set timming */
	reg = TIMING_CFG_TCSH(CS_HOLD_TIME)
		| TIMING_CFG_TCSS(CS_SETUP_TIME)
		| TIMING_CFG_TSHSL(CS_DESELECT_TIME);
	hifmc_writel(host, FMC_SPI_TIMING_CFG, reg);
}

/******************************************************************************/
static int hisi_spi_nor_prep(struct spi_nor *nor, enum spi_nor_ops ops)
{
	int ret;
	struct hifmc_priv *priv = nor->priv;
	struct hifmc_host *host = priv->host;

	mutex_lock(host->lock);

	ret = clk_prepare_enable(host->clk);
	if (ret)
		goto out;

	ret = clk_set_rate(host->clk, priv->clkrate);
	if (ret)
		goto out;

	spi_nor_switch_spi_type(host);

	return 0;

out:
	mutex_unlock(host->lock);
	return ret;
}

/******************************************************************************/
static void hisi_spi_nor_unprep(struct spi_nor *nor, enum spi_nor_ops ops)
{
	struct hifmc_priv *priv = nor->priv;
	struct hifmc_host *host = priv->host;

	clk_disable_unprepare(host->clk);
	mutex_unlock(host->lock);
}

/******************************************************************************/
static int hisi_spi_nor_op_reg(struct spi_nor *nor, u8 opcode, int len, u8 optype)
{
	struct hifmc_priv *priv = nor->priv;
	struct hifmc_host *host = priv->host;
	u32 reg;

	reg = FMC_CMD_CMD1(opcode);
	hifmc_writel(host, FMC_CMD, reg);

	reg = FMC_DATA_NUM_CNT(len);
	hifmc_writel(host, FMC_DATA_NUM, reg);

	reg = OP_CFG_FM_CS(priv->chipselect);
	hifmc_writel(host, FMC_OP_CFG, reg);

	hifmc_writel(host, FMC_INT_CLR, 0xff);
	reg = FMC_OP_CMD1_EN;
	if (optype == FMC_OP_READ)
		reg |= FMC_OP_READ_DATA_EN;
	else
		reg |= FMC_OP_WRITE_DATA_EN;
	reg |= FMC_OP_REG_OP_START;
	hifmc_writel(host, FMC_OP, reg);

	return wait_op_finish(host);
}

/******************************************************************************/
static int hisi_spi_nor_read_reg(struct spi_nor *nor, u8 opcode, u8 *buf,
		int len)
{
	struct hifmc_priv *priv = nor->priv;
	struct hifmc_host *host = priv->host;
	int ret;

	ret = hisi_spi_nor_op_reg(nor, opcode, len, FMC_OP_READ);
	if (ret)
		return ret;

	memcpy_fromio(buf, host->iobase, len);
	return 0;
}

/******************************************************************************/
static int hisi_spi_nor_write_reg(struct spi_nor *nor, u8 opcode,
				u8 *buf, int len)
{
	struct hifmc_priv *priv = nor->priv;
	struct hifmc_host *host = priv->host;

	if (len)
		memcpy_toio(host->iobase, buf, len);

	return hisi_spi_nor_op_reg(nor, opcode, len, FMC_OP_WRITE);
}

/******************************************************************************/
static int hisi_spi_nor_dma_transfer(struct spi_nor *nor, loff_t start_off,
		dma_addr_t dma_buf, size_t len, u8 op_type)
{
	struct hifmc_priv *priv = nor->priv;
	struct hifmc_host *host = priv->host;
	u8 if_type = 0, dummy = 0;
	u32 reg;

	reg = hifmc_readl(host, FMC_CFG);
	reg &= ~SPI_NOR_ADDR_MODE_MASK;
	if (nor->addr_width == 4)
		reg |= SPI_NOR_ADDR_MODE_4BYTES;
	else
		reg |= SPI_NOR_ADDR_MODE_3BYTES;
	hifmc_writel(host, FMC_CFG, reg);
	hifmc_writel(host, FMC_ADDRL, start_off);
	hifmc_writel(host, FMC_DMA_SADDR_D0, dma_buf);
	hifmc_writel(host, FMC_DMA_LEN, FMC_DMA_LEN_SET(len));

	reg = OP_CFG_FM_CS(priv->chipselect);
	if (op_type == FMC_OP_READ) {
		if_type = get_if_type(nor->read_proto);
		dummy = nor->read_dummy >> 3;
	} else {
		if_type = get_if_type(nor->write_proto);
	}
	reg |= OP_CFG_MEM_IF_TYPE(if_type)
		| OP_CFG_DUMMY_NUM(dummy);
	hifmc_writel(host, FMC_OP_CFG, reg);

	hifmc_writel(host, FMC_INT_CLR, 0xff);
	reg = OP_CTRL_RW_OP(op_type) | OP_CTRL_DMA_OP_READY;
	if (op_type == FMC_OP_READ)
		reg |= OP_CTRL_RD_OPCODE(nor->read_opcode);
	else
		reg |= OP_CTRL_WR_OPCODE(nor->program_opcode);
	hifmc_writel(host, FMC_OP_DMA, reg);

	return wait_op_finish(host);
}

/******************************************************************************/
static int hisi_spi_nor_read(struct spi_nor *nor, loff_t from, size_t len,
		size_t *retlen, u_char *read_buf)
{
	struct hifmc_priv *priv = nor->priv;
	struct hifmc_host *host = priv->host;
	size_t offset;
	int ret;

	for (offset = 0; offset < len; offset += HIFMC_DMA_MAX_LEN) {
		size_t trans = min_t(size_t, HIFMC_DMA_MAX_LEN, len - offset);

		ret = hisi_spi_nor_dma_transfer(nor, from + offset,
				host->dma_buffer, trans, FMC_OP_READ);
		if (ret) {
			dev_warn(nor->dev, "DMA read timeout\n");
			return ret;
		}
		memcpy(read_buf + offset, host->buffer, trans);
		*retlen += trans;
	}

	return 0;
}
/******************************************************************************/
static void hisi_spi_nor_write(struct spi_nor *nor, loff_t to,
			size_t len, size_t *retlen, const u_char *write_buf)
{
	struct hifmc_priv *priv = nor->priv;
	struct hifmc_host *host = priv->host;
	int ret;

	memcpy(host->buffer, write_buf, len);
	ret = hisi_spi_nor_dma_transfer(nor, to, host->dma_buffer,
			len, FMC_OP_WRITE);
	if (ret) {
		dev_warn(nor->dev, "DMA write timeout\n");
		return;
	}
	*retlen += len;
}

/**
 * Get spi flash device information and register it as a mtd device.
 */
static int hisi_spi_nor_register(struct device_node *np,
				struct hifmc_host *host)
{
	struct device *dev = host->dev;
	struct spi_nor *nor;
	struct hifmc_priv *priv = &host->priv[host->num_chip];
	struct mtd_info *mtd;
	int ret;
	struct spi_nor_modes modes = {
		.rd_modes = SNOR_MODE_SLOW,
		.wr_modes = SNOR_MODE_1_1_1,
	};

	nor = devm_kzalloc(dev, sizeof(*nor), GFP_KERNEL);
	if (!nor)
		return -ENOMEM;

	nor->dev = dev;
	nor->flash_node = np;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	ret = of_property_read_u32(np, "reg", &priv->chipselect);
	if (ret) {
		dev_err(dev, "There's no reg property for %s\n",
			np->full_name);
		return ret;
	}

	if (priv->chipselect != host->num_chip) {
		dev_warn(dev, " The CS: %d states in device trees isn't real " \
				"chipselect on board\n, using CS: %d instead. ",
				priv->chipselect, host->num_chip);
		priv->chipselect = host->num_chip;
	}

	ret = of_property_read_u32(np, "spi-max-frequency",
			&priv->clkrate);
	if (ret) {
		dev_err(dev, "There's no spi-max-frequency property for %s\n",
			np->full_name);
		return ret;
	}
	priv->host = host;
	nor->priv = priv;

	nor->prepare = hisi_spi_nor_prep;
	nor->unprepare = hisi_spi_nor_unprep;
	nor->read_reg = hisi_spi_nor_read_reg;
	nor->write_reg = hisi_spi_nor_write_reg;
	nor->read = hisi_spi_nor_read;
	nor->write = hisi_spi_nor_write;

	modes.rd_modes |= SNOR_MODE_1_1_1
			| SNOR_MODE_1_1_2
			| SNOR_MODE_1_2_2;
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
	modes.rd_modes |= SNOR_MODE_1_1_4 | SNOR_MODE_1_4_4;
	modes.wr_modes |= SNOR_MODE_1_1_4 | SNOR_MODE_1_4_4;
#endif
	ret = spi_nor_scan(nor, NULL, &modes);
	if (ret)
		return ret;

	mtd = &nor->mtd;
	mtd->name = np->name;
	ret = mtd_device_register(mtd, NULL, 0);
	if (ret)
		return ret;

	/* current chipselect has scanned, to detect next chipselect */
	hifmc_cs_user[host->num_chip]++;
	host->nor[host->num_chip] = nor;
	host->num_chip++;
	return 0;
}

static void hisi_spi_nor_unregister_all(struct hifmc_host *host)
{
	int i;

	for (i = 0; i < host->num_chip; i++)
		mtd_device_unregister(&host->nor[i]->mtd);
}

static int hisi_spi_nor_register_all(struct hifmc_host *host)
{
	struct device *dev = host->dev;
	struct device_node *np = NULL;
	int ret;

	for_each_available_child_of_node(dev->of_node, np) {
		if (hifmc_cs_user[host->num_chip]) {
			dev_warn(dev, "Current CS(%d) is occupied.\n",
					host->num_chip);
			continue;
		}
		ret = hisi_spi_nor_register(np, host);
		if (ret)
			goto fail;

		if (host->num_chip == HIFMC_MAX_CHIP_NUM) {
			dev_warn(dev, "Flash device number exceeds the "
					"maximum chipselect number\n");
			break;
		}
	}

	return 0;

fail:
	hisi_spi_nor_unregister_all(host);
	return ret;
}

/******************************************************************************/
static int hisi_spi_nor_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct hisi_fmc *fmc = dev_get_drvdata(dev->parent);
	struct hifmc_host *host;
	int ret;

	host = devm_kzalloc(dev, sizeof(*host), GFP_KERNEL);
	if (!host)
		return -ENOMEM;

	platform_set_drvdata(pdev, host);
	host->dev = dev;

	host->regbase = fmc->regbase;
	host->iobase = fmc->iobase;
	host->clk = fmc->clk;
	host->lock = &fmc->lock;

	host->buffer = dmam_alloc_coherent(dev, HIFMC_DMA_MAX_LEN,
			&host->dma_buffer, GFP_KERNEL);
	if (!host->buffer)
		return -ENOMEM;

	clk_prepare_enable(host->clk);
	hisi_spi_nor_init(host);
	ret = hisi_spi_nor_register_all(host);
	if (ret)
		dev_warn(dev, "spi nor register fail!\n");

	clk_disable_unprepare(host->clk);

	return ret;
}

/******************************************************************************/
static int hisi_spi_nor_remove(struct platform_device *pdev)
{
	struct hifmc_host *host = platform_get_drvdata(pdev);

	hisi_spi_nor_unregister_all(host);
	clk_disable_unprepare(host->clk);
	return 0;
}

/******************************************************************************/
static void hisi_spi_nor_driver_shutdown(struct platform_device *pdev)
{
	int i;
	struct hifmc_host *host = platform_get_drvdata(pdev);

	if (!host)
		return;

	mutex_lock(host->lock);
	clk_prepare_enable(host->clk);

	spi_nor_switch_spi_type(host);
	for (i = 0; i < host->num_chip; i++)
		spi_nor_driver_shutdown(host->nor[i]);

	clk_disable_unprepare(host->clk);
	mutex_unlock(host->lock);
	dev_dbg(host->dev, "End of driver shutdown\n");
}

#ifdef CONFIG_PM
/******************************************************************************/
static int hisi_spi_nor_driver_suspend(struct platform_device *pdev,
		pm_message_t state)
{
	int i;
	struct hifmc_host *host = platform_get_drvdata(pdev);

	if (!host)
		return 0;

	mutex_lock(host->lock);
	clk_prepare_enable(host->clk);

	spi_nor_switch_spi_type(host);
	for (i = 0; i < host->num_chip; i++)
		spi_nor_suspend(host->nor[i], state);

	clk_disable_unprepare(host->clk);
	mutex_unlock(host->lock);
	dev_dbg(host->dev, "End of suspend\n");

	return 0;
}

/******************************************************************************/
static int hisi_spi_nor_driver_resume(struct platform_device *pdev)
{
	int i;
	struct hifmc_host *host = platform_get_drvdata(pdev);

	if (!host)
		return 0;

	mutex_lock(host->lock);
	clk_prepare_enable(host->clk);

	spi_nor_switch_spi_type(host);
	for (i = 0; i < host->num_chip; i++)
		spi_nor_resume(host->nor[i]);

	mutex_unlock(host->lock);
	dev_dbg(host->dev, "End of resume\n");

	return 0;
}
#endif /* End of CONFIG_PM */

/******************************************************************************/
static const struct of_device_id hisi_spi_nor_dt_ids[] = {
	{ .compatible = "hisilicon,hisi-sfc"},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, hisi_spi_nor_dt_ids);

/******************************************************************************/
static struct platform_driver hisi_spi_nor_driver = {
	.driver = {
		.name	= "hisi-sfc",
		.of_match_table = hisi_spi_nor_dt_ids,
	},
	.probe		= hisi_spi_nor_probe,
	.remove		= hisi_spi_nor_remove,
	.shutdown	= hisi_spi_nor_driver_shutdown,
#ifdef CONFIG_PM
	.suspend	= hisi_spi_nor_driver_suspend,
	.resume		= hisi_spi_nor_driver_resume,
#endif
};
module_platform_driver(hisi_spi_nor_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HiSilicon SPI Nor Flash Controller Driver");
