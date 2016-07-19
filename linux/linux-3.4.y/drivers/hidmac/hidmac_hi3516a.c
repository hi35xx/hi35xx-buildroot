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
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/io.h>

#define SYS_CRG_BASE    IO_ADDRESS(0x20030000)
#define REG_PERI_CRG54  0xd8
#define DMAC_CLK_EN     (1 << 5)
#define DMAC_SRST_REQ   (1 << 4)

/*
 *	DMA config array!
 *	DREQ, FIFO, CONTROL, CONFIG, BITWIDTH
 */
dmac_peripheral  g_peripheral[DMAC_MAX_PERIPHERALS] = {
	/* DREQ,  FIFO,   CONTROL,   CONFIG */
	/*periphal 0: UART0 RX*/
	{ 0, 0, 0, 0, 0},

	/*periphal 1: UART0 TX*/
	{ 1, 0, 0, 0, 0},

	/*periphal 2: I2C1 RX*/
	{ 2, 0x20240010, 0x99000000, 0xd004, 0},	/*  8bit width */

	/*periphal 3: I2C1 TX*/
	{ 3, 0x20240010, 0x96000000, 0xc8c0, 0},	/*  8bit width */

	/*periphal 4: SSP0 RX*/
	{ 4, 0, 0, 0, 0},

	/*periphal 5: SSP0 TX*/
	{ 5, 0, 0, 0, 0},

	/*periphal 6: I2C0 RX*/
/*	{ 6, 0x20240010, 0x99400000, 0xd00c ,0},*/	/* 32bit width */
/*	{ 6, 0x20240010, 0x99000000, 0xd00c ,0},*/	/*  8bit width */
	{ 6, 0, 0, 0, 0},

	/*periphal 7: I2C0 TX*/
/*	{ 7, 0x20240010, 0x96080000, 0xc9c0 ,0},*/	/* 32bit width */
/*	{ 7, 0x20240010, 0x96000000, 0xc9c0 ,0},*/	/*  8bit width */
	{ 7, 0, 0, 0, 0},

	/*periphal 8: I2C1 RX*/
/*	{ 8, 0x40004010, 0x99400000, 0xd010 ,0},*/	/* 32bit width */
/*	{ 8, 0x40004010, 0x99000000, 0xd010 ,0},*/	/*  8bit width */
	{ 8, 0, 0, 0, 0},

	/*periphal 9: I2C1 TX*/
/*	{ 9, 0x40004010, 0x96080000, 0xca40 ,0},*/
/*	{ 9, 0x40004010, 0x96000000, 0xca40 ,0},*/
	{ 9, 0, 0, 0, 0},

	/*periphal 10: SSP1 RX*/
	{ 10, 0, 0, 0, 0},

	/*periphal 11: SSP1 TX*/
	{ 11, 0, 0, 0, 0},
};

void hidmac_clk_en(void)
{
	unsigned int tmp;

	tmp = readl(SYS_CRG_BASE + REG_PERI_CRG54);
	tmp |= DMAC_CLK_EN;
	writel(tmp, SYS_CRG_BASE + REG_PERI_CRG54);
}

void hidmac_unreset(void)
{
	unsigned int tmp;

	tmp = readl(SYS_CRG_BASE + REG_PERI_CRG54);
	tmp &= ~DMAC_SRST_REQ;
	writel(tmp, SYS_CRG_BASE + REG_PERI_CRG54);
}
