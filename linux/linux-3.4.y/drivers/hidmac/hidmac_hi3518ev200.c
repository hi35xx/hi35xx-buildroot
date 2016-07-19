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
	/* periphal 0: I2C0 RX, 8bit width */
	{0, I2C0_DATA_REG, 0x99000000, 0xd000, 0},

	/* periphal 1: I2C0 TX, 8bit width */
	{1, I2C0_DATA_REG, 0x96000000, 0xc840, 0},

	/*periphal 2: I2C1 RX, 8bit width */
	{2, I2C1_DATA_REG, 0x99000000, 0xd004, 0},

	/*periphal 3: I2C1 TX, 8bit width */
	{3, I2C1_DATA_REG, 0x96000000, 0xc8c0, 0},

	/*periphal 4: UART0 RX, 8bit width */
	{4, UART0_DATA_REG, 0x99000000, 0xd008, 0},

	/*periphal 5: UART0 TX, 8bit width */
	{5, UART0_DATA_REG, 0x96000000, 0xc940, 0},

	/*periphal 6: UART1 RX, 8bit width */
	{6, UART1_DATA_REG, 0x99000000, 0xd00c, 0},

	/*periphal 7: UART1 TX, 8bit width */
	{7, UART1_DATA_REG, 0x96000000, 0xc9c0, 0},

	/*periphal 8: UART2 RX, 8bit width */
	{8, UART2_DATA_REG, 0x99000000, 0xd010, 0},

	/*periphal 9: UART2 TX, 8bit width */
	{9, UART2_DATA_REG, 0x96000000, 0xca40, 0},

	/*periphal 10: i2C2 rx, 8bit width */
	{10, I2C2_DATA_REG, 0x99000000, 0xd014, 0},

	/*periphal 11: I2C2 TX, 8bit width */
	{11, I2C2_DATA_REG, 0x96000000, 0xcac0, 0},
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
