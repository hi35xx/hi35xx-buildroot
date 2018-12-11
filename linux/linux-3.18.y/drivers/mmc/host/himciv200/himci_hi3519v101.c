/*
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <mach/platform.h>

#define PERI_CRG49			IO_ADDRESS(0x120100c4)
#define EMMC_DLL_CTRL		IO_ADDRESS(0x12030140)

#define TUNING_START_PHASE      0
#define TUNING_END_PHASE        15
#define HIMCI_PHASE_SCALE       16
#define DRV_PHASE_DFLT         (0x6<<23)
#define SMPL_PHASE_DFLT        (0x0<<16)

#define REG_PAD_CTRL			0x12040800
#define REG_CTRL_SDIO0_CCLK		0xc4
#define REG_CTRL_SDIO0_CCMD		0xc8
#define REG_CTRL_SDIO0_CDATA0	0xcc
#define REG_CTRL_SDIO0_CDATA1	0xd0
#define REG_CTRL_SDIO0_CDATA2	0xd4
#define REG_CTRL_SDIO0_CDATA3	0xd8

#define REG_CTRL_SDIO1_CCLK		0xe8
#define REG_CTRL_SDIO1_CCMD		0xec
#define REG_CTRL_SDIO1_CDATA0	0xf0
#define REG_CTRL_SDIO1_CDATA1	0xf4
#define REG_CTRL_SDIO1_CDATA2	0xf8
#define REG_CTRL_SDIO1_CDATA3	0xfc

#define REG_CTRL_EMMC_CLK		0x1ac
#define REG_CTRL_EMMC_CMD		0x1c8
#define REG_CTRL_EMMC_DATA0		0x1b0
#define REG_CTRL_EMMC_DATA1		0x1cc
#define REG_CTRL_EMMC_DATA2		0x1b4
#define REG_CTRL_EMMC_DATA3		0x1bc
#define REG_CTRL_EMMC_DATA4		0x1c0
#define REG_CTRL_EMMC_DATA5		0x1a8
#define REG_CTRL_EMMC_DATA6		0x1dc
#define REG_CTRL_EMMC_DATA7		0x1b8

#define EMMC_CLK_DS_3V3			0xd0
#define EMMC_CMD_DS_3V3			0xd0
#define EMMC_DATA0_DS_3V3		0xd0
#define EMMC_DATA1_DS_3V3		0xd0
#define EMMC_DATA2_DS_3V3		0xd0
#define EMMC_DATA3_DS_3V3		0xd0
#define EMMC_DATA4_DS_3V3		0xd0
#define EMMC_DATA5_DS_3V3		0xd0
#define EMMC_DATA6_DS_3V3		0xd0
#define EMMC_DATA7_DS_3V3		0xd0

#define EMMC_CLK_DS_1V8			0xc0
#define EMMC_CMD_DS_1V8			0xc0
#define EMMC_DATA0_DS_1V8		0xc0
#define EMMC_DATA1_DS_1V8		0xc0
#define EMMC_DATA2_DS_1V8		0xc0
#define EMMC_DATA3_DS_1V8		0xc0
#define EMMC_DATA4_DS_1V8		0xc0
#define EMMC_DATA5_DS_1V8		0xc0
#define EMMC_DATA6_DS_1V8		0xc0
#define EMMC_DATA7_DS_1V8		0xc0

struct sdio_drv_cap {
	unsigned int reg_addr[2];
	unsigned int ds;
};

#define SDIO_DRV_CAP(ofst1, ofst2, v) { \
	.reg_addr[0] = (REG_PAD_CTRL + ofst1), \
	.reg_addr[1] = (REG_PAD_CTRL + ofst2), \
	.ds = v}
static struct sdio_drv_cap sdio_ds_hs[] = {
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CCLK, REG_CTRL_SDIO1_CCLK, 0x1c0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CCMD, REG_CTRL_SDIO1_CCMD, 0x1e0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CDATA0, REG_CTRL_SDIO1_CDATA0, 0x1e0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CDATA1, REG_CTRL_SDIO1_CDATA1, 0x1e0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CDATA2, REG_CTRL_SDIO1_CDATA2, 0x1e0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CDATA3, REG_CTRL_SDIO1_CDATA3, 0x1e0)
};

static struct sdio_drv_cap sdio_ds_ddr50[] = {
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CCLK, REG_CTRL_SDIO1_CCLK, 0x1b0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CCMD, REG_CTRL_SDIO1_CCMD, 0x1c0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CDATA0, REG_CTRL_SDIO1_CDATA0, 0x1c0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CDATA1, REG_CTRL_SDIO1_CDATA1, 0x1c0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CDATA2, REG_CTRL_SDIO1_CDATA2, 0x1c0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CDATA3, REG_CTRL_SDIO1_CDATA3, 0x1c0)
};

static struct sdio_drv_cap sdio_ds_sdr50[] = {
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CCLK, REG_CTRL_SDIO1_CCLK, 0x80),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CCMD, REG_CTRL_SDIO1_CCMD, 0x1c0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CDATA0, REG_CTRL_SDIO1_CDATA0, 0x1c0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CDATA1, REG_CTRL_SDIO1_CDATA1, 0x1c0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CDATA2, REG_CTRL_SDIO1_CDATA2, 0x1c0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CDATA3, REG_CTRL_SDIO1_CDATA3, 0x1c0)
};

static struct sdio_drv_cap sdio_ds_sdr104[] = {
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CCLK, REG_CTRL_SDIO1_CCLK, 0x0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CCMD, REG_CTRL_SDIO1_CCMD, 0xc0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CDATA0, REG_CTRL_SDIO1_CDATA0, 0xc0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CDATA1, REG_CTRL_SDIO1_CDATA1, 0xc0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CDATA2, REG_CTRL_SDIO1_CDATA2, 0xc0),
	SDIO_DRV_CAP(REG_CTRL_SDIO0_CDATA3, REG_CTRL_SDIO1_CDATA3, 0xc0)
};

struct emmc_drv_cap {
	unsigned int reg_addr;
	unsigned int ds[2];
};

#define EMMC_DRV_CAP(ofst, v1, v2) { \
	.reg_addr = (REG_PAD_CTRL + ofst), \
	.ds[0] = v1, \
	.ds[1] = v2}
static  struct emmc_drv_cap emmc_ds[] = {
	EMMC_DRV_CAP(REG_CTRL_EMMC_CLK, EMMC_CLK_DS_3V3, EMMC_CLK_DS_1V8),
	EMMC_DRV_CAP(REG_CTRL_EMMC_CMD, EMMC_CMD_DS_3V3, EMMC_CMD_DS_1V8),
	EMMC_DRV_CAP(REG_CTRL_EMMC_DATA0, EMMC_DATA0_DS_3V3, EMMC_DATA0_DS_1V8),
	EMMC_DRV_CAP(REG_CTRL_EMMC_DATA1, EMMC_DATA1_DS_3V3, EMMC_DATA1_DS_1V8),
	EMMC_DRV_CAP(REG_CTRL_EMMC_DATA2, EMMC_DATA2_DS_3V3, EMMC_DATA2_DS_1V8),
	EMMC_DRV_CAP(REG_CTRL_EMMC_DATA3, EMMC_DATA3_DS_3V3, EMMC_DATA3_DS_1V8),
	EMMC_DRV_CAP(REG_CTRL_EMMC_DATA4, EMMC_DATA4_DS_3V3, EMMC_DATA4_DS_1V8),
	EMMC_DRV_CAP(REG_CTRL_EMMC_DATA5, EMMC_DATA5_DS_3V3, EMMC_DATA5_DS_1V8),
	EMMC_DRV_CAP(REG_CTRL_EMMC_DATA6, EMMC_DATA6_DS_3V3, EMMC_DATA6_DS_1V8),
	EMMC_DRV_CAP(REG_CTRL_EMMC_DATA7, EMMC_DATA7_DS_3V3, EMMC_DATA7_DS_1V8),
};

static void himci_set_pin_drv_cap(struct himci_host *host, unsigned int flag,
		unsigned char timing)
{
	struct sdio_drv_cap *sdio_ds;
	unsigned int i;

	if (host->devid == 2) {
		for (i = 0; i < ARRAY_SIZE(emmc_ds); i++)
			himci_writel(emmc_ds[i].ds[flag], IO_ADDRESS(emmc_ds[i].reg_addr));
	} else {
		if (timing == MMC_TIMING_UHS_SDR104)
			sdio_ds = sdio_ds_sdr104;
		else if (timing ==  MMC_TIMING_UHS_SDR50)
			sdio_ds = sdio_ds_sdr50;
		else if (timing ==  MMC_TIMING_UHS_DDR50)
			sdio_ds = sdio_ds_ddr50;
		else
			sdio_ds = sdio_ds_hs;

		for (i = 0; i < ARRAY_SIZE(sdio_ds_hs); i++)
			himci_writel(sdio_ds[i].ds,
					IO_ADDRESS(sdio_ds[i].reg_addr[host->devid]));
	}
}

static void himci_set_drv_cap(struct himci_host *host, struct mmc_ios *ios)
{
	unsigned int tmp_reg;

	if (ios->timing == MMC_TIMING_UHS_DDR50) {
		tmp_reg = himci_readl(host->base + MCI_UHS_REG_EXT);
		tmp_reg &= ~CLK_SMPL_PHS_MASK;
		tmp_reg |= (0x4 << CLK_SMPL_PHS_SHIFT);
		himci_writel(tmp_reg, host->base + MCI_UHS_REG_EXT);
	} else if (ios->timing == MMC_TIMING_MMC_HS) {
		tmp_reg = himci_readl(host->base + MCI_UHS_REG_EXT);
		tmp_reg &= ~CLK_SMPL_PHS_MASK;
		tmp_reg |= SMPL_PHASE_DFLT;
		himci_writel(tmp_reg, host->base + MCI_UHS_REG_EXT);
	} else if (ios->timing == MMC_TIMING_MMC_HS400) {
		tmp_reg = himci_readl(host->base + MCI_UHS_REG_EXT);
		tmp_reg &= ~CLK_SMPL_PHS_MASK;
		tmp_reg |= (host->phase << CLK_SMPL_PHS_SHIFT);
		himci_writel(tmp_reg, host->base + MCI_UHS_REG_EXT);
	}

	/* set pin drive capability */
	if (ios->timing == MMC_TIMING_MMC_HS200 ||
			ios->timing == MMC_TIMING_MMC_HS400)
		himci_set_pin_drv_cap(host, 1, ios->timing);
	else
		himci_set_pin_drv_cap(host, 0, ios->timing);
}

static int himci_check_emmc(struct himci_host *host)
{
	unsigned int val;

	val = himci_readl(IO_ADDRESS(SYS_CTRL_BASE + REG_SC_STAT));

	return !(GET_SYS_BOOT_MODE(val) == BOOT_FROM_EMMC);
}
