/*
 * RTC client/driver for the Hisilicon hi3518ev200 Real-Time Clock
 *
 * Based on code of hi_rtc.c by Hisilicon Co., Ltd.
 *
 * Copyright (c) 2017 Watson Xu <xuhuashan@gmail.com>
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
 * along with this program;
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/moduleparam.h>

#include <linux/io.h>
#include <linux/irq.h>
#include <linux/rtc.h>

#define CRG_BASE_ADDR		IO_ADDRESS(0x20030000)
#define PERI_CRG57		(CRG_BASE_ADDR + 0xE4)
#define BIT_TEMP_SRST_REQ	2

/* RTC Control over SPI */
#define RTC_SPI_BASE_ADDR	IO_ADDRESS(0x20060000)
#define SPI_CLK_DIV		(RTC_SPI_BASE_ADDR + 0x000)
#define SPI_RW			(RTC_SPI_BASE_ADDR + 0x004)

/* RTC temperature reg */
#define RTC_TEMP_START		(RTC_SPI_BASE_ADDR + 0x80)
#define RTC_TEMP_CRC		(RTC_SPI_BASE_ADDR + 0x84)
#define RTC_TEMP_INT_MASK	(RTC_SPI_BASE_ADDR + 0x88)
#define RTC_TEMP_INT_CLEAR	(RTC_SPI_BASE_ADDR + 0x8c)
#define RTC_TEMP_STAT		(RTC_SPI_BASE_ADDR + 0x90)
#define RTC_TEMP_INT_RAW	(RTC_SPI_BASE_ADDR + 0x94)
#define RTC_TEMP_INT_STAT	(RTC_SPI_BASE_ADDR + 0x98)
#define RTC_TEMP_VAL		(RTC_SPI_BASE_ADDR + 0x9c)

#define RTC_TEMP_IRQ_NUM	(2)

/* Define the union SPI_RW */
typedef union
{
	struct
	{
		unsigned int spi_wdata		: 8; /* [7:0] */
		unsigned int spi_rdata		: 8; /* [15:8] */
		unsigned int spi_addr		: 7; /* [22:16] */
		unsigned int spi_rw		    : 1; /* [23] */
		unsigned int spi_start		: 1; /* [24] */
		unsigned int reserved		: 6; /* [30:25] */
		unsigned int spi_busy		: 1; /* [31] */
	} bits;
	/* Define an unsigned member */
	unsigned int u32;
} U_SPI_RW;

#define SPI_WRITE		(0)
#define SPI_READ		(1)

#define RTC_IRQ			(2)

/* RTC REG */
#define RTC_10MS_COUN		0x00
#define RTC_S_COUNT		0x01
#define RTC_M_COUNT		0x02
#define RTC_H_COUNT		0x03
#define RTC_D_COUNT_L		0x04
#define RTC_D_COUNT_H		0x05

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
#define RTC_INT_MASK		0x15
#define RTC_INT_RAW		0x16

#define RTC_CLK			0x17
#define RTC_POR_N		0x18
#define RTC_SAR_CTRL		0x1A
#define RTC_ANA_CTRL		0x1B
#define TOT_OFFSET_L		0x1C
#define TOT_OFFSET_H		0x1D
#define TEMP_OFFSET		0x1E
#define OUTSIDE_TEMP		0x1F
#define INTERNAL_TEMP		0x20
#define TEMP_SEL		0x21
#define LUT1			0x22

#define RTC_FREQ_H		0x51
#define RTC_FREQ_L		0x52

#define NORMAL_TEMP_VALUE	25
#define TEMP_TO_RTC(value) (((value) + 40)*255/180)

#define TEMP_ENV_OFFSET		27
#define TEMP_OFFSET_TO_RTC(value) ((value)*255/180)
#define RETRY_CNT		100

#define FREQ_MAX_VAL		3277000
#define FREQ_MIN_VAL		3276000

static const char temp_lut_table[50] = {
	0x00, 0x00, 0x00, 0xAC, 0xB4, 0xBB, 0xC2, 0xC8, 0xCF, 0xD5,
	0xDC, 0xE2, 0xE8, 0xEE, 0xF3, 0xF9, 0xFE, 0x03, 0x08, 0x0D,
	0x12, 0x16, 0x1B, 0x1F, 0x23, 0x27, 0x2B, 0x2E, 0x32, 0x35,
	0x38, 0x3B, 0x3E, 0x41, 0x43, 0x45, 0x48, 0x4A, 0x4B, 0x4D,
	0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x54, 0x55, 0x55, 0x55
};

static DEFINE_MUTEX(hirtc_lock);

static struct timer_list temperature_timer;
static unsigned int mode = 0 /* TEMP_SEL_FIXMODE */;
static int t_second = 5;
module_param(t_second, int, 0);

static int temp_itrp;

static int spi_write(char reg, char val)
{
	U_SPI_RW w_data, r_data;

	r_data.u32 = 0;
	w_data.u32 = 0;

	w_data.bits.spi_wdata = val;
	w_data.bits.spi_addr = reg;
	w_data.bits.spi_rw = SPI_WRITE;
	w_data.bits.spi_start = 0x1;

	writel(w_data.u32, SPI_RW);

	do {
		r_data.u32 = readl(SPI_RW);
	} while (r_data.bits.spi_busy);

	return 0;
}

static int spi_rtc_write(char reg, char val)
{
	mutex_lock(&hirtc_lock);
	spi_write(reg, val);
	mutex_unlock(&hirtc_lock);

	return 0;
}

static int spi_read(char reg, char* val)
{
	U_SPI_RW w_data, r_data;

	r_data.u32 = 0;
	w_data.u32 = 0;
	w_data.bits.spi_addr = reg;
	w_data.bits.spi_rw = SPI_READ;
	w_data.bits.spi_start = 0x1;

	writel(w_data.u32, SPI_RW);

	do {
		r_data.u32 = readl(SPI_RW);
	} while (r_data.bits.spi_busy);

	*val = r_data.bits.spi_rdata;

	return 0;
}

static int spi_rtc_read(char reg, char* val)
{
	mutex_lock(&hirtc_lock);
	spi_read(reg, val);
	mutex_unlock(&hirtc_lock);

	return 0;
}

static int temp_crg_reset(void)
{
	u32 value = readl(PERI_CRG57);
	mb();
	writel(value | (1<<BIT_TEMP_SRST_REQ),  PERI_CRG57);
	mb();
	writel(value & ~(1<<BIT_TEMP_SRST_REQ), PERI_CRG57);

	return 0;
}

static int hi3518_rtc_getalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	unsigned char dayl, dayh;
	unsigned char second, minute, hour;
	unsigned long seconds = 0;
	unsigned int day;

	spi_rtc_read(RTC_MR_S, &second);
	spi_rtc_read(RTC_MR_M, &minute);
	spi_rtc_read(RTC_MR_H, &hour);
	spi_rtc_read(RTC_MR_D_L, &dayl);
	spi_rtc_read(RTC_MR_D_H, &dayh);
	day = (unsigned int)(dayl | (dayh << 8));
	pr_debug("day %d\n", day);
	seconds = second + minute*60 + hour*60*60 + day*24*60*60;

	rtc_time_to_tm(seconds, &alrm->time);

	return 0;
}

static int hi3518_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	unsigned int days;
	unsigned long seconds = 0;

	if (rtc_valid_tm(&alrm->time))
		return -EINVAL;

	rtc_tm_to_time(&alrm->time, &seconds);
	days = seconds/(60*60*24);

	spi_rtc_write(RTC_MR_10MS, 0);
	spi_rtc_write(RTC_MR_S, alrm->time.tm_sec);
	spi_rtc_write(RTC_MR_M, alrm->time.tm_min);
	spi_rtc_write(RTC_MR_H, alrm->time.tm_hour);
	spi_rtc_write(RTC_MR_D_L, (days & 0xFF));
	spi_rtc_write(RTC_MR_D_H, (days >> 8));

	return 0;
}

static int hi3518_rtc_gettime(struct device *dev, struct rtc_time *rtc_tm)
{
	unsigned char dayl, dayh;
	unsigned int day;
	unsigned char second, minute, hour;
	unsigned long seconds = 0;

	unsigned char raw_value;

	spi_rtc_read(RTC_LORD, &raw_value);
	if (raw_value & 0x4) {
		spi_rtc_write(RTC_LORD, (~(1 << 2)) & raw_value);
	}

	spi_rtc_read(RTC_LORD, &raw_value);
	spi_rtc_write(RTC_LORD, (1 << 1) | raw_value); //lock the time

	do {
		spi_rtc_read(RTC_LORD, &raw_value);
	} while (raw_value & 0x2);

	msleep(1);  // must delay 1 ms

	spi_rtc_read(RTC_S_COUNT, &second);
	spi_rtc_read(RTC_M_COUNT, &minute);
	spi_rtc_read(RTC_H_COUNT, &hour);
	spi_rtc_read(RTC_D_COUNT_L, &dayl);
	spi_rtc_read(RTC_D_COUNT_H, &dayh);
	day = (dayl | (dayh << 8));

	seconds = second + minute*60 + hour*60*60 + day*24*60*60;

	rtc_time_to_tm(seconds, rtc_tm);

	return 0;
}

static int hi3518_rtc_settime(struct device *dev, struct rtc_time *tm)
{
	unsigned char ret = 0;
	unsigned int days;
	unsigned long seconds = 0;
	unsigned int cnt = 0;

	if (rtc_valid_tm(tm))
		return -EINVAL;

	/* convert tm to seconds. */
	rtc_tm_to_time(tm, &seconds);

	days = seconds / (60 * 60 * 24);

	spi_rtc_write(RTC_LR_10MS, 0);
	spi_rtc_write(RTC_LR_S, tm->tm_sec);
	spi_rtc_write(RTC_LR_M, tm->tm_min);
	spi_rtc_write(RTC_LR_H, tm->tm_hour);
	spi_rtc_write(RTC_LR_D_L, (days & 0xFF));
	spi_rtc_write(RTC_LR_D_H, (days >> 8));

	spi_rtc_write(RTC_LORD, (ret | 0x1));

	/* wait rtc load flag */
	do {
		spi_rtc_read(RTC_LORD, &ret);
		msleep(1);
		cnt++;
	} while ((ret & 0x1) && (cnt < RETRY_CNT));

	if (cnt >= RETRY_CNT) {
		pr_err("check state error!\n");
		return -1;
	}

	/* must sleep 10ms, internal clock is 100Hz */
	msleep(10);

	return 0;
}

static void temperature_detection(unsigned long arg)
{
	int ret;
	int cnt = 0;

	pr_debug("START temperature adjust");

	if (mode == 1/*TEMP_SEL_OUTSIDE*/) {
		do {
			ret = readl(RTC_TEMP_STAT);
			udelay(10);
			cnt++;
		} while (ret && (cnt < RETRY_CNT));

		if (cnt >= RETRY_CNT) {
			/*maybe temperature capture module is error,
			  need reset */
			pr_err("RTC_TEMP_STAT not ready, please check pin configuration\n");
			temp_crg_reset();
			goto timer_again;
		}

		pr_debug("WRITE RTC_TEMP_START");

		writel(0x1, RTC_TEMP_START);
	}
	else if (mode == 2/*TEMP_SEL_INTERNAL*/) {
		char temp = TEMP_TO_RTC(25);

		spi_read(INTERNAL_TEMP, &temp);

		pr_debug("internal temp is %d\n", temp);

		/*FIXME: sub offset to get enviroment temperature*/
		//temp -= 38;  /*38=27c*255/180*/
		temp -= TEMP_OFFSET_TO_RTC(TEMP_ENV_OFFSET);
		spi_write(OUTSIDE_TEMP, temp);
	}
	else {
		pr_err("invalid temperature mode");
	}

timer_again:
	mod_timer(&temperature_timer, jiffies + msecs_to_jiffies(1000)*t_second);
}

/**
 * this function change DS1820 temperature format to native RTC format
 * OUTSIDE_TEMP value size (-40<oC>, +140<oC>), use 8bit to spec
 * DS1820 value size (-55<oC>, +120<oC>), use 9bit to spec
 */
static void set_temperature(void)
{
	unsigned short ret;
	unsigned char temp;

	ret = (unsigned short)readl(RTC_TEMP_VAL);
	pr_debug("READ DS1820 temperature value 0x%x", ret);

	/* mode 1 sample, ret > 0x800 means negative number */
	if (ret > 0x800) {
		/* change to positive number */
		ret = 4096 - ret;
		ret = ret / 2;
		/* rtc temperature lower limit -40<oC> */
		if (ret > 40) { ret = 40; }
		temp = (40 - ret) * 255 / 180;
	}
	else {
		/* rtc temperature upper limit 140<oC> */
		ret = ret / 2;
		if (ret > 140)
		{ ret = 140; }
		temp = TEMP_TO_RTC(ret);
	}

	pr_debug("WRITE RTC temperature value 0x%02x", temp);

	spi_write(OUTSIDE_TEMP, temp);
}

static irqreturn_t rtc_temp_interrupt(int irq, void *dev_id)
{
	int ret = IRQ_NONE;
	int irq_stat;

	irq_stat = readl(RTC_TEMP_INT_STAT);
	if (!irq_stat) {
		return ret;
	}

	pr_debug("irq mask");
	writel(0x01, RTC_TEMP_INT_MASK);

	irq_stat = readl(RTC_TEMP_INT_RAW);
	pr_debug("irq clear");

	writel(0x1, RTC_TEMP_INT_CLEAR);

	if (mode != 1/*TEMP_SEL_OUTSIDE*/) { goto endl; }

	if (irq_stat & 0x2) {
		/* err start again */
		mod_timer(&temperature_timer, jiffies + msecs_to_jiffies(1000)*t_second);
	}
	else {
		/* set temperature data */
		set_temperature();
	}

endl:
	pr_debug("irq unmask");
	writel(0x0, RTC_TEMP_INT_MASK);

	ret = IRQ_HANDLED;
	return ret;
}

/*
 * interrupt function
 */
static irqreturn_t rtc_alrm_interrupt(int irq, void *dev_id)
{
	struct rtc_device *rtc_dev = dev_id;

	spi_write(RTC_INT_CLR, 0x1);

	rtc_update_irq(rtc_dev, 1, RTC_AF | RTC_IRQF);

	return IRQ_HANDLED;
}

static int hi3518_rtc_proc(struct device *dev, struct seq_file *seq)
{
	return 0;
}

static int hi3518_rtc_setaie(struct device *dev, unsigned int enabled)
{
	char val = (char)!!enabled;

	spi_rtc_write(RTC_IMSC, val);

	return 0;
}

static const struct rtc_class_ops hi3518_rtc_ops = {
	.read_time	= hi3518_rtc_gettime,
	.set_time	= hi3518_rtc_settime,
	.read_alarm	= hi3518_rtc_getalarm,
	.set_alarm	= hi3518_rtc_setalarm,
	.proc		= hi3518_rtc_proc,
	.alarm_irq_enable = hi3518_rtc_setaie,
};

static struct platform_device *hi3518_rtc_pdev = NULL;
struct rtc_device *rtc_dev = NULL;

static int __init hi3518_rtc_init(void)
{
	int i,ret = 0;
	char base;

	hi3518_rtc_pdev = platform_device_register_simple("rtc-hi3518",
			-1, NULL, 0);
	if (IS_ERR(hi3518_rtc_pdev)) {
		pr_err("Failed to register platform device\n");
		return PTR_ERR(hi3518_rtc_pdev);
	}

	rtc_dev = rtc_device_register("rtc-hi3518", &hi3518_rtc_pdev->dev,
			&hi3518_rtc_ops, THIS_MODULE);
	if (IS_ERR(rtc_dev)) {
		pr_err("Failed to register rtc device\n");
		goto err_unregister_pdev;
	}

	ret = request_irq(RTC_IRQ, &rtc_alrm_interrupt, IRQF_SHARED,
			"RTC Alarm", (void*)rtc_dev);
	if(ret) {
		pr_err("IRQ%d error %d\n", RTC_IRQ, ret);
		goto err_unregister_rtcdev;
	}

	ret = request_irq(RTC_IRQ, &rtc_temp_interrupt, IRQF_SHARED,
			"RTC Temperature", (void*)&temp_itrp);
	if (ret) {
		pr_err("IRQ%d error %d\n", RTC_IRQ, ret);
		goto err_free_irq;
	}

	init_timer(&temperature_timer);
	temperature_timer.function = temperature_detection;
	temperature_timer.expires = jiffies + msecs_to_jiffies(1000) * t_second;

	/* clk div value = (apb_clk/spi_clk)/2-1, for asic ,
	   apb clk = 100MHz, spi_clk = 10MHz,so value= 0x4 */
	writel(0x4, SPI_CLK_DIV);

	/* always fixed frequency division mode */
	spi_rtc_write(TEMP_SEL, 0x06);
	spi_rtc_write(RTC_SAR_CTRL, 0x08);
	spi_rtc_write(OUTSIDE_TEMP, TEMP_TO_RTC(25));

	/* set default freq */
	spi_rtc_write(RTC_FREQ_H, 0x8);
	spi_rtc_write(RTC_FREQ_L, 0x1b);

	/* rtc analog voltage config */
	spi_rtc_write(RTC_ANA_CTRL, 0x01);

	/*init rtc temperature lut table value*/
	for (i = 0; i < ARRAY_SIZE(temp_lut_table); i++) {
		if (i < 3) {
			base = TOT_OFFSET_L;
			spi_rtc_write(base + i, temp_lut_table[i]);
		}
		else {
			base = LUT1;
			spi_rtc_write(base + i - 3, temp_lut_table[i]);
		}
	}

	/* enable temperature IRQ */
	writel(0x0, RTC_TEMP_INT_MASK);

	return 0;

err_free_irq:
	free_irq(RTC_IRQ, NULL);
err_unregister_rtcdev:
	rtc_device_unregister(rtc_dev);
err_unregister_pdev:
	platform_device_unregister(hi3518_rtc_pdev);

	return ret;
}

static void __exit hi3518_rtc_exit(void)
{
	del_timer(&temperature_timer);
	free_irq(RTC_IRQ, (void*)rtc_dev);
	free_irq(RTC_IRQ, (void*)&temp_itrp);
	rtc_device_unregister(rtc_dev);
	platform_device_unregister(hi3518_rtc_pdev);
}

module_init(hi3518_rtc_init);
module_exit(hi3518_rtc_exit);

MODULE_AUTHOR("Watson Xu <xuhuashan@gmail.com>");
MODULE_DESCRIPTION("driver for Hi3518EV200 internal RTC");
MODULE_LICENSE("GPL");
