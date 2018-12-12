/*
 * this function is only for debug.
 */
#include <asm/io.h>
#include <asm/arch/platform.h>

#include <common.h>
#include <himciv200_reg.h>

#define MMC_INT_DATA_FIFO_ERROR	(0x8E80)

#define MMC_SRC_CLK		(500 * 100 * 1000)
#define MMC_BOOT_CLK		(400 * 1000)

#define MMC_WAIT_PWR_TIME	20
#define MMC_WAIT_BDS_TIME	1000

#define ENABLE			1
#define DISABLE			0

#define MMC_INTMASK_DISABLE	0x0		/* all int disable */
#define MMC_RINTSTS_ALL_CLR	0xFFFFFFFF	/* all clear 0 */
#define MMC_IDSTS_ALL_CLR	0xFFFFFFFF	/* all clear 0 */
#define BYTE_CNT		0x100000	/* 1024k */

#define reg_get(X) readl(X)
#define reg_set(X, Y) writel((Y), (X))
#define mmc_reg_get(offset)     reg_get(mmc_base + (offset))
#define mmc_reg_set(offset, val) reg_set(mmc_base + (offset), (val))

#define MMC_FUNC(func) ({ \
		if ((func) != 0) \
			return -1; \
})

static u32 mmc_base;

static int mmc_step1(void)
{
	int i, ret = -1;

#ifdef CONFIG_ARCH_HI3536
	mmc_base = SDIO1_BASE_REG;
#endif

#ifdef CONFIG_HI3518EV200
	mmc_base = SDIO0_BASE_REG;
#endif

#if (defined CONFIG_HI3516CV300 || defined CONFIG_ARCH_HI3519 || \
		defined CONFIG_ARCH_HI3519V101 || \
		defined CONFIG_ARCH_HI3559 || defined CONFIG_ARCH_HI3556 || \
		defined CONFIG_ARCH_HI3516AV200)
	mmc_base = EMMC_BASE_REG;
#endif
	/* power off */
	mmc_reg_set(MCI_PWREN, DISABLE);

	/* reset all */
	mmc_reg_set(MCI_CTRL, CTRL_RESET | FIFO_RESET | DMA_RESET);
	for (i = 0; i < 0xf00; ++i) {
		ret = mmc_reg_get(MCI_CTRL);
		ret &= CTRL_RESET;
		ret |= FIFO_RESET;
		ret |= DMA_RESET;
		if (!ret) {
			ret = 0;
			break;
		}
	}

		/* wait */
		udelay(MMC_WAIT_PWR_TIME * 1000);

		/* power on */
		mmc_reg_set(MCI_PWREN, ENABLE);

		/* wait */
		udelay(MMC_WAIT_PWR_TIME * 1000);

		return ret;
}

static int mmc_step2(void)
{
	mmc_reg_set(MCI_INTMASK, MMC_INTMASK_DISABLE);
	mmc_reg_set(MCI_RINTSTS, MMC_RINTSTS_ALL_CLR);
	mmc_reg_set(MCI_IDSTS, MMC_IDSTS_ALL_CLR);
	return 0;
}

static int mmc_step3(void)
{
	mmc_reg_set(MCI_CTRL, INTR_EN);
	return 0;
}

static int mmc_update_clk(void)
{
	u32 val, i, j;

	for (i = 0; i < 3; ++i) {
		mmc_reg_set(MCI_CMD, START_CMD | UP_CLK_ONLY | WT_PD_CPT);

		for (j = 0; j < 0xf00; ++j) {
			val = mmc_reg_get(MCI_CMD);
			/* can send next cmd after bit 31 is 0 */
			if ((val & START_CMD) == 0)
				return 0;

			val = mmc_reg_get(MCI_RINTSTS);
			/* send stop cmd after data transfer */
			if ((val & SEND_AUTO_STOP) != 0)
				break;
		}
	}

	return -1;
}

static int mmc_step4(void)
{
	mmc_reg_set(MCI_CLKENA, DISABLE);
	MMC_FUNC(mmc_update_clk());

	mmc_reg_set(MCI_CLKDIV, MMC_SRC_CLK / (MMC_BOOT_CLK * 2));
	mmc_reg_set(MCI_CLKSRC, 0);
	MMC_FUNC(mmc_update_clk());

	mmc_reg_set(MCI_CLKENA, ENABLE);
	MMC_FUNC(mmc_update_clk());

	return 0;
}

static int mmc_step5(void)
{
	/* nearly 2.6s at 400kHz */
	mmc_reg_set(MCI_TIMEOUT,
		(mmc_reg_get(MCI_TIMEOUT) & RESPONSE_TIMEOUT) | DATA_TIMEOUT);
	return 0;
}

static int mmc_step6(void)
{
	udelay(200);
	mmc_reg_set(MCI_RESET_N, DISABLE);
	/* For eMMC, minimum is 1us but give it 10us for good measure */
	udelay(100);
	mmc_reg_set(MCI_RESET_N, ENABLE);
	/* For eMMC, minimum is 200us but give it 300us for good measure */
	udelay(1000);
	return 0;
}

static int mmc_step7(void)
{
	mmc_reg_set(MCI_BLKSIZ, MMC_BLK_SZ);
	return 0;
}

static int mmc_step8(void)
{
	/* 1024k boot is big engough */
	mmc_reg_set(MCI_BYTCNT, BYTE_CNT);
	return 0;
}

static int mmc_step9(void)
{
	u32 val;
	val = mmc_reg_get(MCI_FIFOTH);
	val = (val & ~(0xFFF<<16)) | (0x7f << 16);
	mmc_reg_set(MCI_FIFOTH, val);

	val = reg_get(REG_BASE_SCTL + REG_SYSSTAT);
#if !defined(CONFIG_HI3516CV300)
	if (val & NF_BOOTBW_MASK) {
		/* 8bit */
		mmc_reg_set(MCI_CTYPE, CARD_WIDTH_8BIT);
	} else {
#endif
		/* 4bit */
		mmc_reg_set(MCI_CTYPE, CARD_WIDTH_4BIT);
#if !defined(CONFIG_HI3516CV300)
	}
#endif

	return 0;
}

static int mmc_step10(void)
{
	/*enable_boot_ack = 0*/
	mmc_reg_set(MCI_CMD, START_CMD | ENABLE_BOOT | CARD_NUM | DATA_EXPECT);
	return 0;
}

static int mmc_wait_bds(void)
{
	u32 val;
	u32 timeout = MMC_WAIT_BDS_TIME;

	do {
		val = mmc_reg_get(MCI_RINTSTS);
		if (val & DRTO_INT_STATUS)
			break;

		udelay(1000);
	} while (timeout--);

	mmc_reg_set(MCI_RINTSTS, MMC_RINTSTS_ALL_CLR);

	if (!(val & DRTO_INT_STATUS)) {
		mmc_reg_set(MCI_CMD, START_CMD | DISABLE_BOOT);
		printf("bootmoderead:mmc_wait_bds timeout,Failed!!!\n");
		return -1;
	} else
		return 0;
}

static int mmc_init(void)
{
	int index = 0;
	unsigned int tmp_reg;

	for (index = MMC_IOMUX_START_ADDR;
			index <= MMC_IOMUX_END_ADDR; index += 4) {
#ifdef	CONFIG_ARCH_HI3536
		if (index == IOMUX_REG47)
			continue;
#endif
		tmp_reg = reg_get(IO_CONFIG_REG_BASE + index);
		tmp_reg &= MMC_IOMUX_CTRL_MASK;
		if (tmp_reg != MMC_IOMUX_CTRL) {
			printf("emmc pin_mux ebidq_gpio_sdio%d is not equal "
					"to 0x2, bootmoderead failed!!!\n",
					index);
			return -1;
		}
	}

	MMC_FUNC(mmc_step1());
	MMC_FUNC(mmc_step2());
	MMC_FUNC(mmc_step3());
	MMC_FUNC(mmc_step4());
	MMC_FUNC(mmc_step5());
	MMC_FUNC(mmc_step6());
	MMC_FUNC(mmc_step7());
	MMC_FUNC(mmc_step8());
	MMC_FUNC(mmc_step9());
	MMC_FUNC(mmc_step10());
	MMC_FUNC(mmc_wait_bds());

	return 0;
}

static void mmc_deinit(void)
{
	mmc_reg_set(MCI_CMD, START_CMD | DISABLE_BOOT);
	mmc_reg_set(MCI_CTRL, CTRL_RESET | FIFO_RESET | DMA_RESET);
	mmc_reg_set(MCI_PWREN, DISABLE);
}

static size_t mmc_read(void *ptr, size_t size)
{
	u32 val;
	u32 *tmp = (u32 *)ptr;
	size_t count;

	size = (size + 3) >> 2;
	while (size > 0) {
		do {
			val = mmc_reg_get(MCI_RINTSTS);
			if ((val & RXDR_INT_STATUS)
					|| (val & DTO_INT_STATUS))
				break;

			if (val & MMC_INT_DATA_FIFO_ERROR)
				return -1;    /* error! return */
		} while (1);

		count = ((mmc_reg_get(MCI_STATUS) >> FIFO_COUNT)
				& FIFO_COUNT_MASK);

		if (count > size)
			count = size;

		mmc_reg_set(MCI_RINTSTS, MMC_RINTSTS_ALL_CLR);

		for (; count > 0; --count) {
			*tmp++ = mmc_reg_get(MCI_FIFO_START);
			--size;
		}
		/* all finished */
		if (val & DTO_INT_STATUS)
			break;

	}
	return (size_t)(tmp - (u32 *)ptr);
}

void emmc_bootmode_read(void *ptr, unsigned int size)
{
	if (!size) {
		printf("parameter error: read count is 0.\n");
		return;
	}

	if (0 == mmc_init())
		mmc_read((void *)ptr, size);

	mmc_deinit();
	return;
}
