/******************************************************************************
 *    Copyright (c) 2009-2012 by Hisi.
 *    All rights reserved.
 * ***
 *
 ******************************************************************************/

#include <asm/arch/platform.h>
#include <config.h>
#ifdef CONFIG_DDR_TRAINING_V2
#include <ddr_interface.h>
#endif

#define HI_SYS_CTL_REG	0x12020000

#define DDR0_BASE_REG	0x12068000
#define DDR0_PLL_REG    0x1206c000

static inline void delay(unsigned int num)
{
	volatile unsigned int i;

	for (i = 0; i < (100 * num); i++)
		__asm__ __volatile__("nop");
}

extern void reset_cpu(unsigned long addr);

static inline void DWB(void) /* drain write buffer */
{
}

static inline unsigned int readl(unsigned addr)
{
	unsigned int val;

	val = (*(volatile unsigned int *)(addr));
	return val;
}

static inline void writel(unsigned val, unsigned addr)
{
	DWB();
	(*(volatile unsigned *) (addr)) = (val);
	DWB();
}

#ifdef DDR_SCRAMB_ENABLE
#define REG_BASE_MISC   0x12030000
#define MISC_CTRL0_OFST         0x0
#define MISC_CTRL3_OFST         0xc
#define MISC_CTRL62_OFST        0xf8
#define MISC_CTRL63_OFST        0xfc
#define MISC_CTRL64_OFST        0x100
#define REG_BASE_DDRC                   DDRC0_REG_BASE
#define DDRC_CTRL_SREF_OFST             0x8000 + 0x0
#define DDRC_CURR_FUNC_OFST             0x8000 + 0x294
#define REG_BASE_RNG_GEN                0x120C0000
#define TRNG_DSTA_FIFO_DATA_OFST        0x204
#define TRNG_DATA_ST_OFST               0x208
#define HISEC_COM_TRNG_CTRL_OFST        0x200
#define BIT_TRNG_FIFO_DATA_CNT          0x8
#define TRNG_FIFO_DATA_CNT_MASK         0xff
#define TRNG_CTRL_DEF_VAL               0xa

 #define REG_PERI_CRG56          0x00E0
#define TRNG_CLK_ENABLE         (0x1 << 5)

static int get_random_num(void)
{
	unsigned int reg_val;


	do {
		reg_val = readl(REG_BASE_RNG_GEN + TRNG_DATA_ST_OFST);

	} while (!((reg_val >> BIT_TRNG_FIFO_DATA_CNT)
				& TRNG_FIFO_DATA_CNT_MASK));

	reg_val = readl(REG_BASE_RNG_GEN + TRNG_DSTA_FIFO_DATA_OFST);

	return reg_val;
}

static void ddr_scramb_start(unsigned int random1, unsigned int random2)
{
	writel(random1, REG_BASE_MISC + MISC_CTRL62_OFST);
	writel(random2, REG_BASE_MISC + MISC_CTRL63_OFST);
	writel(0, REG_BASE_MISC + MISC_CTRL64_OFST);
	writel(0x10, REG_BASE_MISC + MISC_CTRL64_OFST);

	delay(1000);

	writel(0, REG_BASE_MISC + MISC_CTRL62_OFST);
	writel(0, REG_BASE_MISC + MISC_CTRL63_OFST);
}

static int ddr_scramb(void)
{
#if 0
	unsigned int random_num1;
	unsigned int random_num2;
#endif
	unsigned int reg_val;

	/* open rsa and trng clock */
	reg_val = readl(CRG_REG_BASE + REG_PERI_CRG56);
	reg_val |= TRNG_CLK_ENABLE;
	writel(reg_val, CRG_REG_BASE + REG_PERI_CRG56);

	/* set ddrc to do self-refurbish */
	reg_val = 0x1;
	writel(reg_val, REG_BASE_DDRC + DDRC_CTRL_SREF_OFST);

	/* wait the status of ddrc to be sef-refurbish */
	do {
		reg_val = readl(REG_BASE_DDRC + DDRC_CURR_FUNC_OFST);
	} while (!(reg_val & 0x1));

#if 0
	/* get random number */
	random_num1 = get_random_num();
	random_num2 = get_random_num();

	if (random_num1 == random_num2) {
		return -1;
	}

	/* start ddr scrambling */
	ddr_scramb_start(random_num1, random_num2);


	/* clear random number */
	random_num1 = get_random_num();
	random_num2 = get_random_num();
	random_num1 = get_random_num();
	random_num2 = get_random_num();
#endif

	/* start ddr scrambling */
	ddr_scramb_start(0, 0);
	
	/* set ddrc to exit self-refurbish */
	reg_val = (0x1 << 1);
	writel(reg_val, REG_BASE_DDRC + DDRC_CTRL_SREF_OFST);

	/* wait the status of ddrc to be normal */
	do {
		reg_val = readl(REG_BASE_DDRC + DDRC_CURR_FUNC_OFST);
	} while (reg_val & 0x1);

	return 0;
}
#endif

#if 0
void hi3516av200_ddr_init(void)
{
	unsigned int temp1, temp2;

	temp2 = readl(HI_SYS_CTL_REG + 0xa0);
	if (temp2) {
		temp1 = readl(DDR0_BASE_REG + 0x108);
		writel((temp1 & 0xffff0000), DDR0_BASE_REG + 0x108);
		writel(temp2, DDR0_PLL_REG + 0x4);
		writel(0x2, DDR0_BASE_REG);

		while (1) {
			temp2 = readl(DDR0_PLL_REG + 0x4) & 0x1;
			if (!temp2)
				break;
		}

		writel(0x8000, DDR0_PLL_REG + 0x4);
		writel(0x0, DDR0_PLL_REG + 0x4);
		writel(temp1, DDR0_BASE_REG + 0x108);
	}

	/* for low cost*/
	/*writel(0x23201, DDR0_BASE_REG + 0x28);
	writel(0xaf501, DDR0_BASE_REG + 0x1c);
	writel(0xfe014830, DDR0_PLL_REG + 0x1e4);*/
	writel(0x2, DDR0_BASE_REG + 0x0);

	do {
		temp1 = readl(DDR0_BASE_REG + 0x294);
	} while (temp1 & 0x1);
}
#endif

#define REG_SC_GEN32 0xa0
#define REG_SC_GEN33 0xa4
#define REG_DDR_DMC0_CFG_DDRMODE (DDR0_BASE_REG + 0x50)
#define REG_DDR_DMC0_CFG_TIMING2 (DDR0_BASE_REG + 0x108)
#define REG_DDR_PHY0_TRAINCTRL   (DDR0_PLL_REG + 0x48)
#define REG_DDR_PHY0_INITCTRL    (DDR0_PLL_REG + 0x4)
#define REG_DDR_DMC0_CTRL_SREF   DDR0_BASE_REG
#define REG_DDR_DMC0_IN_SREF     (DDR0_BASE_REG + 0x294)

void ddr_set_sref(unsigned int val)
{
        int cnt = 0;

        writel(val, REG_DDR_DMC0_CTRL_SREF);
        while ((!(readl(REG_DDR_DMC0_IN_SREF) & 0x1)) && (cnt < 10000)) {
                cnt++;
        }

        if (cnt >= 10000) {
                reset_cpu(0);
        }
}


void hi3516av200_ddr_init(void)
{
	unsigned int initctrl_rank0, initctrl_rank1, rank_num;

	/* Read rank0 HW DDR training value */
	initctrl_rank0 = readl(REG_BASE_SCTL + REG_SC_GEN32);
	/* Read rank1 HW DDR training value */
	initctrl_rank1 = readl(REG_BASE_SCTL + REG_SC_GEN33);
	/* 0: one rank, 1: two rank */
	rank_num = (readl(REG_DDR_DMC0_CFG_DDRMODE) >> 20) & 0x3;

	if (initctrl_rank0 || (initctrl_rank1 && rank_num)) {
		unsigned int timing;
		unsigned int timing_backup;
		unsigned int rank_ctrl;

		/* Save the Auto-self refresh timing */
		timing = readl(REG_DDR_DMC0_CFG_TIMING2);
		timing_backup = timing & 0x7FF;
		
		/* Set Auto-self refresh timing to 0 */
		writel((timing & 0xFFFFF800), REG_DDR_DMC0_CFG_TIMING2);

		rank_ctrl = readl(REG_DDR_PHY0_TRAINCTRL);
		if (initctrl_rank0) {
			/* set bit[0:3] = 0 to training rank0 */
			writel(rank_ctrl & 0xfffffff0, REG_DDR_PHY0_TRAINCTRL);
			/* Start HW DDR training */
			writel(initctrl_rank0, REG_DDR_PHY0_INITCTRL);
			/* wait HW DDR training finish */
			do {
				initctrl_rank0 = readl(REG_DDR_PHY0_INITCTRL) & 0x7FFFF;
			} while(initctrl_rank0);
		}

		if (initctrl_rank1 && rank_num) {
			/* set bit[0:3] = 1 to training rank1 */
			writel((rank_ctrl & 0xfffffff0) | 0x1, REG_DDR_PHY0_TRAINCTRL);
			/* Start HW DDR training */
			writel(initctrl_rank1, REG_DDR_PHY0_INITCTRL);
			/* wait HW DDR training finish */
			do {
				initctrl_rank1 = readl(REG_DDR_PHY0_INITCTRL) & 0x7FFFF;
			} while(initctrl_rank1);
		}
		writel(rank_ctrl, REG_DDR_PHY0_TRAINCTRL); /* restore */

		/* Exit Auto-self refresh */
		ddr_set_sref(0x2);

		/* Restore the Auto-self refresh timing */
		timing = readl(REG_DDR_DMC0_CFG_TIMING2);
		timing = (timing & 0xFFFFF800) | timing_backup;
		writel(timing, REG_DDR_DMC0_CFG_TIMING2);
	}

	/* Clear fifo */
	initctrl_rank0 = readl(REG_DDR_PHY0_INITCTRL);
	initctrl_rank0 = initctrl_rank0 | (1<<15);
	writel(initctrl_rank0, REG_DDR_PHY0_INITCTRL);
	initctrl_rank0 = readl(REG_DDR_PHY0_INITCTRL);
	initctrl_rank0 = initctrl_rank0 & ~(1<<15);
	writel(initctrl_rank0, REG_DDR_PHY0_INITCTRL);
}

#ifdef CONFIG_SVB_ENABLE

#define HI_PMC_CTL_REG		0x120a0000
#define HPM_CORE_VOL_REG	(HI_PMC_CTL_REG + 0x0)
#define HPM_CPU_VOL_REG		(HI_PMC_CTL_REG + 0x4)
#define HPM_DDR_VOL_REG		(HI_PMC_CTL_REG + 0x8)
#define HPM_MDA_VOL_REG		(HI_PMC_CTL_REG + 0xc)
#define CYCLE_NUM		4
#define HPM_CORE_REG0	0x2c
#define HPM_CORE_REG1	0x30
#define HPM_CPU_REG0	0x3c
#define HPM_CPU_REG1	0x40
#define HPM_DDR_REG0	0x4c
#define HPM_DDR_REG1	0x50
#define HPM_MDA_REG0	0x5c
#define HPM_MDA_REG1	0x60
#define PERI_CRG_PLL1	0x4
#define PERI_CRG13	0x34
#define CPU_A17_CKSEL_MASK	0x70

#define HPM_POWER_PROFILE_REG	0x1202015c
#define FOUR_WAY_POWER	0
#define THREE_WAY_POWER	1
#define TWO_WAY_POWER	2

#define OTP_CORE_HPM_IDDQ	0x12032000
#define OTP_MDA_HPM_IDDQ	0x12032004

static unsigned hpm_value_avg(unsigned int *val)
{
	unsigned int i;
	unsigned tmp = 0;

	for (i = 0; i < 4; i++)
		tmp += val[i] >> 2;

	return tmp >> 2;
}

static void get_hpm_value(unsigned int *hpm_core, unsigned int *hpm_cpu,
		unsigned int *hpm_ddr, unsigned int *hpm_mda)
{
	int i;
	unsigned int temp;
	unsigned int core_value[4] = {0, 0, 0, 0};
	unsigned int mda_value[4] = {0, 0, 0, 0};

	for (i = 0; i < CYCLE_NUM; i++) {
		delay(100);

		/* core */
		temp = readl(HI_PMC_CTL_REG + HPM_CORE_REG0);
		core_value[1] += (temp >> 12) & 0x3ff;
		core_value[0] += temp & 0x3ff;
		temp = readl(HI_PMC_CTL_REG + HPM_CORE_REG1);
		core_value[3] += (temp >> 12) & 0x3ff;
		core_value[2] += temp & 0x3ff;

		/* mda */
		temp = readl(HI_PMC_CTL_REG + HPM_MDA_REG0);
		mda_value[1] += (temp >> 12) & 0x3ff;
		mda_value[0] += temp & 0x3ff;
		temp = readl(HI_PMC_CTL_REG + HPM_MDA_REG1);
		mda_value[3] += (temp >> 12) & 0x3ff;
		mda_value[2] += temp & 0x3ff;
	}

	*hpm_core = hpm_value_avg(core_value);
	*hpm_mda = hpm_value_avg(mda_value);
}

static void start_hpm(unsigned int *hpm_core, unsigned int *hpm_cpu,
		unsigned int *hpm_ddr, unsigned int *hpm_mda)
{
	writel(0x05000003, HI_PMC_CTL_REG + 0x28);
	writel(0x05000003, HI_PMC_CTL_REG + 0x58);

	delay(500);

	get_hpm_value(hpm_core, hpm_cpu, hpm_ddr, hpm_mda);
}

static void set_hpm_core_volt(unsigned int hpm_core_value)
{
	unsigned int otp_core_hpm_iddq;

	otp_core_hpm_iddq = (readl(OTP_CORE_HPM_IDDQ) >> 16) & 0x3f;

	if (hpm_core_value > 270) {
		if (otp_core_hpm_iddq > 9 || !otp_core_hpm_iddq)
			writel(0x3500c7, HPM_CORE_VOL_REG);/*0.89v*/
		else
			writel(0x2900c7, HPM_CORE_VOL_REG);/*0.91v*/

	} else if (hpm_core_value > 245)
		writel(0x2900c7, HPM_CORE_VOL_REG);/*0.91v*/
	else if (hpm_core_value > 220)
		writel(0x1700c7, HPM_CORE_VOL_REG);/*0.94v*/
	else
		writel(0x0b00c7, HPM_CORE_VOL_REG);/*0.96v*/
}
static void set_hpm_mda_volt(unsigned int hpm_mda_value)
{
	unsigned int otp_mda_hpm_iddq;

	otp_mda_hpm_iddq =  (readl(OTP_MDA_HPM_IDDQ) >> 16) & 0x3f;

	if (hpm_mda_value > 245) {
		if (otp_mda_hpm_iddq > 13 || !otp_mda_hpm_iddq)
			writel(0x4600c7, HPM_MDA_VOL_REG);/*0.86v*/
		else
			writel(0x3a00c7, HPM_MDA_VOL_REG);/*0.88v*/
	} else if (hpm_mda_value > 220)
		writel(0x3a00c7, HPM_MDA_VOL_REG);/*0.88v*/
	else if (hpm_mda_value > 195)
		writel(0x2200c7, HPM_MDA_VOL_REG);/*0.92v*/
	else
		writel(0x1000c7, HPM_MDA_VOL_REG);/*0.95v*/
}

static void start_svb(void)
{
	unsigned int hpm_core;
	unsigned int hpm_cpu;
	unsigned int hpm_ddr;
	unsigned int hpm_mda;
	unsigned int otp_hpm_core;
	unsigned int otp_hpm_mda;

	/*set start voltage, CORE/MDA0 is 0.9v*/
	writel(0x2f00c7, HPM_CORE_VOL_REG);
	writel(0x2e00c7, HPM_MDA_VOL_REG);
	

	start_hpm(&hpm_core, &hpm_cpu, &hpm_ddr, &hpm_mda);

	/* modify hpm value according to otp hpm value */
	otp_hpm_core = readl(OTP_CORE_HPM_IDDQ) & 0x3ff;
	if (otp_hpm_core && otp_hpm_core < hpm_core)
		hpm_core = otp_hpm_core;

	otp_hpm_mda = readl(OTP_MDA_HPM_IDDQ) & 0x3ff;
	if (otp_hpm_mda && (otp_hpm_mda + 15) < hpm_mda)
		hpm_mda = otp_hpm_mda + 15;

	set_hpm_core_volt(hpm_core);
	set_hpm_mda_volt(hpm_mda);
}
#endif

void start_ddr_training(unsigned int base)
{
#ifdef CONFIG_SVB_ENABLE
	start_svb();
#endif

	hi3516av200_ddr_init();

#ifdef CONFIG_DDR_TRAINING_V2
	int ret = 0;

	/* ddr training function */
	ret = ddr_sw_training_if(0);
	if (ret)
		reset_cpu(0);
#endif
	/*the value should config after trainning, or
	  it will cause chip compatibility problems*/
	writel(0x401, DDR0_BASE_REG + 0x28);

#ifdef DDR_SCRAMB_ENABLE	
	/* enable ddr scramb */
	ddr_scramb();
#endif
}
