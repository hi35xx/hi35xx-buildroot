/*
 * RTC driver for Hisilicon BVT
 * Copyright (C) 2016 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <linux/bcd.h>
#include <linux/bitops.h>
#include <linux/log2.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rtc.h>
#include <linux/version.h>
#include <linux/io.h>
#include <linux/delay.h>

#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>

union u_spi_rw {
	struct {
		unsigned int spi_wdata		: 8; /* [7:0] */
		unsigned int spi_rdata		: 8; /* [15:8] */
		unsigned int spi_addr		: 7; /* [22:16] */
		unsigned int spi_rw		    : 1; /* [23] */
		unsigned int spi_start		: 1; /* [24] */
		unsigned int reserved		: 6; /* [30:25] */
		unsigned int spi_busy		: 1; /* [31] */
	} bits;
	unsigned int u32;
};

#define SPI_CLK_DIV			(0x000)
#define SPI_RW				(0x004)

#define SPI_WRITE		(0)
#define SPI_READ		(1)

/* RTC REG */
#define RTC_10MS_COUN	0x00
#define RTC_S_COUNT		0x01
#define RTC_M_COUNT		0x02
#define RTC_H_COUNT		0x03
#define RTC_D_COUNT_L	0x04
#define RTC_D_COUNT_H	0x05

#define RTC_MR_10MS		0x06
#define RTC_MR_S		0x07
#define RTC_MR_M		0x08
#define RTC_MR_H		0x09
#define RTC_MR_D_L		0x0A
#define RTC_MR_D_H		0x0B

#define RTC_LR_10MS		0x0C
#define RTC_LR_S		0x0D
#define RTC_LR_M		0x0E
#define RTC_LR_H		0x0F
#define RTC_LR_D_L		0x10
#define RTC_LR_D_H		0x11

#define RTC_LORD		0x12

#define RTC_IMSC		0x13
#define RTC_INT_CLR		0x14
#define RTC_INT			0x15
#define RTC_INT_RAW		0x16

#define RTC_CLK			0x17
#define RTC_POR_N		0x18
#define RTC_SAR_CTRL	0x1A
#define RTC_CLK_CFG	    0x1B

#define RTC_FREQ_H		0x51
#define RTC_FREQ_L		0x52

#define FREQ_H_DEFAULT  0x8
#define FREQ_L_DEFAULT  0x1B

#define LV_CTL_DEFAULT  0x20
#define CLK_DIV_DEFAULT 0x4
#define INT_RST_DEFAULT 0x0
#define INT_MSK_DEFAULT 0x4

#define AIE_INT_MASK       BIT(0)
#define LV_INT_MASK        BIT(1)
#define REG_LOAD_STAT      BIT(0)
#define REG_LOCK_STAT      BIT(1)
#define REG_LOCK_BYPASS    BIT(2)

#define RETRY_CNT 500

#define DATE_TO_SEC(d, h, m, s)     (s + m*60 + h*60*60 + d*24*60*60)
#define SEC_TO_DAY(s)            (s/(60*60*24))

struct hibvt_rtc {
	struct rtc_device	*rtc_dev;
	void __iomem		*regs;
	int                  rtc_irq;
};

static int hibvt_spi_write(void *spi_reg, unsigned char reg,
	unsigned char val)
{
	union u_spi_rw w_data, r_data;
	int cnt = RETRY_CNT;

	r_data.u32 = 0;
	w_data.u32 = 0;

	w_data.bits.spi_wdata = val;
	w_data.bits.spi_addr = reg;
	w_data.bits.spi_rw = SPI_WRITE;
	w_data.bits.spi_start = 0x1;

	writel(w_data.u32, (spi_reg+SPI_RW));

	do
		r_data.u32 = readl(spi_reg+SPI_RW);
	while (r_data.bits.spi_busy && (--cnt));

	if (r_data.bits.spi_busy)
		return -EIO;

	return 0;
}


static int hibvt_spi_rtc_write(void *spi_reg, unsigned char reg,
	unsigned char val)
{
	return hibvt_spi_write(spi_reg, reg, val);
}

static int hibvt_spi_read(void *spi_reg, unsigned char reg,
	unsigned char *val)
{
	union u_spi_rw w_data, r_data;
	int cnt = RETRY_CNT;

	r_data.u32 = 0;
	w_data.u32 = 0;
	w_data.bits.spi_addr = reg;
	w_data.bits.spi_rw = SPI_READ;
	w_data.bits.spi_start = 0x1;

	writel(w_data.u32, (spi_reg+SPI_RW));

	do
		r_data.u32 = readl(spi_reg+SPI_RW);
	while (r_data.bits.spi_busy && (--cnt));

	if (r_data.bits.spi_busy)
		return -EIO;

	*val = r_data.bits.spi_rdata;

	return 0;
}

static int hibvt_spi_rtc_read(void *spi_reg, unsigned char reg,
	unsigned char *val)
{
	return hibvt_spi_read(spi_reg, reg, val);
}

static int hibvt_rtc_read_time(struct device *dev, struct rtc_time *time)
{
	struct hibvt_rtc *rtc = dev_get_drvdata(dev);
	unsigned char dayl, dayh;
	unsigned char second, minute, hour;
	unsigned long seconds = 0;
	unsigned int day;
	unsigned char raw_value;
	int cnt = RETRY_CNT;
	int ret = 0;

	ret = hibvt_spi_rtc_read(rtc->regs, RTC_INT_RAW, &raw_value);
	if (ret) {
		dev_err(dev, "IO err.\n");
		return ret;
	}

	if (raw_value & LV_INT_MASK) {
		dev_err(dev,
			"low voltage detected, date/time is not reliable.\n");
		hibvt_spi_write(rtc->regs, RTC_INT_CLR, 1);
		return -EINVAL;
	}

	ret |= hibvt_spi_rtc_read(rtc->regs, RTC_LORD, &raw_value);
	if (raw_value & REG_LOCK_BYPASS)
		ret |= hibvt_spi_rtc_write(rtc->regs, RTC_LORD,
		(~(REG_LOCK_BYPASS)) & raw_value);

	ret |= hibvt_spi_rtc_read(rtc->regs, RTC_LORD, &raw_value);
	/* lock the time */
	ret |= hibvt_spi_rtc_write(rtc->regs, RTC_LORD,
	(REG_LOCK_STAT) | raw_value);
	/* wait rtc load flag */
	do {
		ret |= hibvt_spi_rtc_read(rtc->regs, RTC_LORD, &raw_value);
		msleep(20);
	} while ((ret || (raw_value & REG_LOCK_STAT)) && (--cnt));

	if (!ret && (raw_value & REG_LOCK_STAT))
		return -EBUSY;

	ret |= hibvt_spi_rtc_read(rtc->regs, RTC_S_COUNT, &second);
	ret |= hibvt_spi_rtc_read(rtc->regs, RTC_M_COUNT, &minute);
	ret |= hibvt_spi_rtc_read(rtc->regs, RTC_H_COUNT, &hour);
	ret |= hibvt_spi_rtc_read(rtc->regs, RTC_D_COUNT_L, &dayl);
	ret |= hibvt_spi_rtc_read(rtc->regs, RTC_D_COUNT_H, &dayh);

	if (ret) {
		dev_err(dev, "IO err.\n");
		return ret;
	}

	day = (dayl | (dayh << 8));
	seconds = DATE_TO_SEC(day, hour, minute, second);

	rtc_time_to_tm(seconds, time);

	return rtc_valid_tm(time);
}

static int hibvt_rtc_set_time(struct device *dev, struct rtc_time *time)
{
	struct hibvt_rtc	*rtc = dev_get_drvdata(dev);
	unsigned char ret = 0;
	unsigned int days;
	unsigned long seconds = 0;
	unsigned int cnt = RETRY_CNT;
	unsigned char raw_value = 0;

	ret = rtc_tm_to_time(time, &seconds);
	if (ret)
		return ret;
	days = SEC_TO_DAY(seconds);

	ret |= hibvt_spi_rtc_write(rtc->regs, RTC_LR_10MS, 0);
	ret |= hibvt_spi_rtc_write(rtc->regs, RTC_LR_S, time->tm_sec);
	ret |= hibvt_spi_rtc_write(rtc->regs, RTC_LR_M, time->tm_min);
	ret |= hibvt_spi_rtc_write(rtc->regs, RTC_LR_H, time->tm_hour);
	ret |= hibvt_spi_rtc_write(rtc->regs, RTC_LR_D_L, (days & 0xFF));
	ret |= hibvt_spi_rtc_write(rtc->regs, RTC_LR_D_H, (days >> 8));

	ret |= hibvt_spi_rtc_write(rtc->regs, RTC_LORD,
		(raw_value | REG_LOAD_STAT));
	/* wait rtc load flag */
	do {
		ret |= hibvt_spi_rtc_read(rtc->regs, RTC_LORD, &raw_value);
		msleep(20);
	} while ((ret || (raw_value & REG_LOAD_STAT)) && (--cnt));

	if (!ret && (raw_value & REG_LOAD_STAT))
		return -EBUSY;

	if (ret)
		dev_err(dev, "IO err.\n");

	return ret;
}

static int hibvt_rtc_read_alarm(struct device *dev,
	struct rtc_wkalrm *alrm)
{
	struct hibvt_rtc *rtc = dev_get_drvdata(dev);
	unsigned char dayl, dayh;
	unsigned char second, minute, hour;
	unsigned long seconds = 0;
	unsigned int day;
	unsigned char int_state = 0;
	int ret = 0;

	memset(alrm, 0, sizeof(struct rtc_wkalrm));

	ret |= hibvt_spi_rtc_read(rtc->regs, RTC_MR_S, &second);
	ret |= hibvt_spi_rtc_read(rtc->regs, RTC_MR_M, &minute);
	ret |= hibvt_spi_rtc_read(rtc->regs, RTC_MR_H, &hour);
	ret |= hibvt_spi_rtc_read(rtc->regs, RTC_MR_D_L, &dayl);
	ret |= hibvt_spi_rtc_read(rtc->regs, RTC_MR_D_H, &dayh);

	day = (unsigned int)(dayl | (dayh << 8));
	seconds = DATE_TO_SEC(day, hour, minute, second);

	rtc_time_to_tm(seconds, &alrm->time);

	ret |= hibvt_spi_rtc_read(rtc->regs, RTC_IMSC, &int_state);
	if (ret) {
		dev_err(dev, "IO err.\n");
		return ret;
	}

	alrm->enabled = !!(int_state & AIE_INT_MASK);
	alrm->pending = alrm->enabled;

	return 0;
}

static int hibvt_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct hibvt_rtc	*rtc = dev_get_drvdata(dev);
	unsigned int days;
	unsigned long seconds = 0;
	unsigned char val = 0;
	int ret = 0;

	rtc_tm_to_time(&alrm->time, &seconds);

	days = SEC_TO_DAY(seconds);

	ret |= hibvt_spi_rtc_write(rtc->regs, RTC_MR_10MS, 0);
	ret |= hibvt_spi_rtc_write(rtc->regs, RTC_MR_S, alrm->time.tm_sec);
	ret |= hibvt_spi_rtc_write(rtc->regs, RTC_MR_M, alrm->time.tm_min);
	ret |= hibvt_spi_rtc_write(rtc->regs, RTC_MR_H, alrm->time.tm_hour);
	ret |= hibvt_spi_rtc_write(rtc->regs, RTC_MR_D_L, (days & 0xFF));
	ret |= hibvt_spi_rtc_write(rtc->regs, RTC_MR_D_H, (days >> 8));

	ret |= hibvt_spi_rtc_read(rtc->regs, RTC_IMSC, &val);
	if (alrm->enabled)
		ret |= hibvt_spi_rtc_write(rtc->regs, RTC_IMSC,
		val | AIE_INT_MASK);
	else
		ret |= hibvt_spi_rtc_write(rtc->regs, RTC_IMSC,
		val & ~AIE_INT_MASK);

	if (ret) {
		dev_err(dev, "IO err.\n");
		return ret;
	}

	return 0;
}

static int hibvt_rtc_alarm_irq_enable(struct device *dev,
	unsigned int enabled)
{
	struct hibvt_rtc	*rtc = dev_get_drvdata(dev);
	unsigned char val = 0;
	int ret = 0;

	ret |= hibvt_spi_rtc_read(rtc->regs, RTC_IMSC, &val);
	if (enabled)
		ret |= hibvt_spi_rtc_write(rtc->regs, RTC_IMSC,
		val | AIE_INT_MASK);
	else
		ret |= hibvt_spi_rtc_write(rtc->regs, RTC_IMSC,
		val & ~AIE_INT_MASK);

	if (ret) {
		dev_err(dev, "IO err.\n");
		return ret;
	}

	return 0;
}


/*
 * interrupt function
 * do nothing. left for future
 */
static irqreturn_t hibvt_rtc_alm_interrupt(int irq, void *data)
{
	struct hibvt_rtc *rtc = (struct hibvt_rtc *)data;
	unsigned char val = 0;
	int ret = 0;

	ret |= hibvt_spi_read(rtc->regs, RTC_INT, &val);
	ret |= hibvt_spi_write(rtc->regs, RTC_INT_CLR, AIE_INT_MASK);

	if (ret) {
		dev_err(&rtc->rtc_dev->dev, "IO err.\n");
		return ret;
	}

	if (val & AIE_INT_MASK)
		rtc_update_irq(rtc->rtc_dev, 1, RTC_AF | RTC_IRQF);

	return IRQ_HANDLED;
}

#define FREQ_MAX_VAL	    3277000
#define FREQ_MIN_VAL	    3276000

static int hibvt_rtc_ioctl(struct device *dev,
	unsigned int cmd, unsigned long arg)
{
	struct hibvt_rtc	*rtc = dev_get_drvdata(dev);
	int ret = 0;

	switch (cmd) {
	case RTC_PLL_SET:
	{
		char freq_l, freq_h;
		struct rtc_pll_info pll_info;

		if (copy_from_user(&pll_info, (struct rtc_pll_info *)arg,
			sizeof(struct rtc_pll_info)))
			return -EFAULT;

		/* freq = 32700 + (freq /3052)*100 */
		if (pll_info.pll_value > FREQ_MAX_VAL
			|| pll_info.pll_value < FREQ_MIN_VAL)
			return -EINVAL;

		pll_info.pll_value = (pll_info.pll_value - 3270000)
			* 3052 / 10000;

		freq_l = (char)(pll_info.pll_value & 0xff);
		freq_h = (char)((pll_info.pll_value >> 8) & 0xf);

		ret |= hibvt_spi_rtc_write(rtc->regs, RTC_FREQ_H, freq_h);
		ret |= hibvt_spi_rtc_write(rtc->regs, RTC_FREQ_L, freq_l);

		if (ret) {
			dev_err(dev, "IO err.\n");
			return ret;
		}

		return 0;
	}
	case RTC_PLL_GET:
	{
		char freq_l, freq_h;
		struct rtc_pll_info pll_info;

		ret |= hibvt_spi_rtc_read(rtc->regs, RTC_FREQ_H, &freq_h);
		ret |= hibvt_spi_rtc_read(rtc->regs, RTC_FREQ_L, &freq_l);

		if (ret) {
			dev_err(dev, "IO err.\n");
			return ret;
		}

		pll_info.pll_value = ((freq_h & 0xf) << 8) + freq_l;
		pll_info.pll_value = 3270000
			+ (pll_info.pll_value * 10000) / 3052;

		pll_info.pll_max = FREQ_MAX_VAL;
		pll_info.pll_min = FREQ_MIN_VAL;

		if (copy_to_user((void __user *)arg,
			&pll_info, sizeof(struct rtc_pll_info)))
			return -EFAULT;

		return 0;
	}
	default:
		return -ENOIOCTLCMD;
	}
}

static const struct rtc_class_ops hibvt_rtc_ops = {
	.read_time		= hibvt_rtc_read_time,
	.set_time		= hibvt_rtc_set_time,
	.read_alarm		= hibvt_rtc_read_alarm,
	.set_alarm		= hibvt_rtc_set_alarm,
	.alarm_irq_enable	= hibvt_rtc_alarm_irq_enable,
	.ioctl          = hibvt_rtc_ioctl,
};

static int hibvt_rtc_init(struct hibvt_rtc *rtc)
{
	void *spi_reg = rtc->regs;
	int ret = 0;
	unsigned char val = 0;
	/*
	 * clk div value = (apb_clk/spi_clk)/2-1,
	 *	apb clk = 100MHz, spi_clk = 10MHz,so value= 0x4
	 */
	writel(CLK_DIV_DEFAULT, (spi_reg+SPI_CLK_DIV));

	ret |= hibvt_spi_rtc_write(spi_reg, RTC_IMSC, INT_MSK_DEFAULT);
	ret |= hibvt_spi_rtc_write(spi_reg, RTC_SAR_CTRL, LV_CTL_DEFAULT);

   
	ret |= hibvt_spi_rtc_write(spi_reg, RTC_CLK_CFG, 0x01);

    /* default FREQ COEF */
	ret |= hibvt_spi_rtc_write(spi_reg, RTC_FREQ_H, FREQ_H_DEFAULT);
	ret |= hibvt_spi_rtc_write(spi_reg, RTC_FREQ_L, FREQ_L_DEFAULT);

	ret |= hibvt_spi_rtc_read(spi_reg, RTC_INT_RAW, &val);
    //ret |= hibvt_spi_rtc_read(spi_reg, RTC_CLK_CFG, &val2);
	if (ret) {
		dev_err(&rtc->rtc_dev->dev, "IO err.\n");
		return ret;
	}
 
	if (val & LV_INT_MASK) {
		dev_err(&rtc->rtc_dev->dev,
			"low voltage detected, date/time is not reliable.\n");
		hibvt_spi_write(rtc->regs, RTC_INT_CLR, 1);
	}

	return ret;
}

static int hibvt_rtc_probe(struct platform_device *pdev)
{
	struct resource  *mem;
	struct hibvt_rtc *rtc;
	int    ret;

	rtc = devm_kzalloc(&pdev->dev, sizeof(*rtc), GFP_KERNEL);
	if (!rtc)
		return -ENOMEM;

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	rtc->regs = devm_ioremap_resource(&pdev->dev, mem);
	if (IS_ERR((const void *)rtc->regs)) {
		dev_err(&pdev->dev, "could not map I/O memory\n");
		return PTR_ERR((const void *)rtc->regs);
	}

	rtc->rtc_irq = platform_get_irq(pdev, 0);
	ret = devm_request_irq(&pdev->dev, rtc->rtc_irq,
		hibvt_rtc_alm_interrupt, 0, pdev->name, rtc);
	if (ret) {
		dev_err(&pdev->dev, "could not request irq %d\n", rtc->rtc_irq);
		return ret;
	}

	platform_set_drvdata(pdev, rtc);
	rtc->rtc_dev = devm_rtc_device_register(&pdev->dev, pdev->name,
						&hibvt_rtc_ops, THIS_MODULE);
	if (IS_ERR(rtc->rtc_dev)) {
		dev_err(&pdev->dev, "could not register rtc device\n");
		return PTR_ERR(rtc->rtc_dev);
	}

	if (hibvt_rtc_init(rtc)) {
		dev_err(&pdev->dev, "hibvt_rtc_init failed.\n");
		return -EIO;
	}

	dev_info(&pdev->dev, "RTC driver for hibvt enabled\n");

	return 0;
}

static int hibvt_rtc_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id hibvt_rtc_match[] = {
	{ .compatible = "hisilicon,hi35xvr-rtc" },
	{},
};

static struct platform_driver hibvt_rtc_driver = {
	.probe  = hibvt_rtc_probe,
	.remove = hibvt_rtc_remove,
	.driver =  { .name = "hibvt_rtc",
				.of_match_table = hibvt_rtc_match,
				},
};

module_platform_driver(hibvt_rtc_driver);

#define OSDRV_MODULE_VERSION_STRING "HISI_rtc @HiMPP"

MODULE_AUTHOR("Hisilicon");
MODULE_DESCRIPTION("Hisilicon RTC driver");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("HI_VERSION=" OSDRV_MODULE_VERSION_STRING);

