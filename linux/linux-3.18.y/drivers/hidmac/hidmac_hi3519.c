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

#define SYS_CRG_BASE    IO_ADDRESS(0x12010000)
#define REG_PERI_CRG56  0xe0
#define DMAC_CLK_EN     (1 << 1)
#define DMAC_SRST_REQ   (1 << 0)

/*
 *	DMA config array!
 *	DREQ, FIFO, CONTROL, CONFIG, BITWIDTH
 */
dmac_peripheral  g_peripheral[DMAC_MAX_PERIPHERALS] = {
	/* periphal 0: UART0 RX, 8bit width */
	{0, UART0_DATA_REG, 0x99000000, 0xd000, 0},

	/* periphal 1: UART0 TX, 8bit width */
	{1, UART0_DATA_REG, 0x96000000, 0xc840, 0},

	/*periphal 2: UART1 RX, 8bit width */
	{2, UART1_DATA_REG, 0x99000000, 0xd004, 0},

	/*periphal 3: UART1 TX, 8bit width */
	{3, UART1_DATA_REG, 0x96000000, 0xc8c0, 0},

	/*periphal 4: UART2 RX, 8bit width */
	{4, UART2_DATA_REG, 0x99000000, 0xd008, 0},

	/*periphal 5: UART2 TX, 8bit width */
	{5, UART2_DATA_REG, 0x96000000, 0xc940, 0},

	/*periphal 6: UART3 RX, 8bit width */
	{6, UART3_DATA_REG, 0x99000000, 0xd00c, 0},

	/*periphal 7: UART3 TX, 8bit width */
	{7, UART3_DATA_REG, 0x96000000, 0xc9c0, 0},

	/*periphal 8: I2C0 RX, 8bit width */
	{8, I2C0_DATA_REG, 0x99000000, 0xd010, 0},

	/*periphal 9: I2C0 TX, 8bit width */
	{9, I2C0_DATA_REG, 0x96000000, 0xca40, 0},

	/*periphal 10: I2C1 RX, 8bit width */
	{10, I2C1_DATA_REG, 0x99000000, 0xd014, 0},

	/*periphal 11: I2C1 TX, 8bit width */
	{11, I2C1_DATA_REG, 0x96000000, 0xcac0, 0},

	/*periphal 12: I2C2 RX, 8bit width */
	{12, I2C2_DATA_REG, 0x99000000, 0xd018, 0},

	/*periphal 13: I2C2 TX, 8bit width */
	{13, I2C2_DATA_REG, 0x96000000, 0xcb40, 0},

	/*periphal 14: I2C3 RX, 8bit width */
	{14, I2C3_DATA_REG, 0x99000000, 0xd01c, 0},

	/*periphal 15: I2C3 TX, 8bit width */
	{15, I2C3_DATA_REG, 0x96000000, 0xcbc0, 0},
};

void hidmac_clk_en(void)
{
	unsigned int tmp;

	dmac_readw(SYS_CRG_BASE + REG_PERI_CRG56, tmp);
	tmp |= DMAC_CLK_EN;
	dmac_writew(SYS_CRG_BASE + REG_PERI_CRG56, tmp);
}

void hidmac_unreset(void)
{
	unsigned int tmp;

	dmac_readw(SYS_CRG_BASE + REG_PERI_CRG56, tmp);
	tmp &= ~DMAC_SRST_REQ;
	dmac_writew(SYS_CRG_BASE + REG_PERI_CRG56, tmp);
}
