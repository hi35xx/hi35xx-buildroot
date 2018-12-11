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
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>

#define TUNING_START_PHASE	0
#define TUNING_END_PHASE	7
#define HIMCI_PHASE_SCALE	8
#define DRV_PHASE_DFLT		(0x4<<23)
#define SMPL_PHASE_DFLT		(0x1<<16)

#define GET_SYS_BOOT_MODE(_reg) (((_reg) >> 0x4) & 0x1)
#define BOOT_FROM_EMMC			0x1

#define REG_PAD_CTRL			0x12040800

#define REG_CTRL_EMMC_CLK		0xa0
#define REG_CTRL_EMMC_CMD		0x9c
#define REG_CTRL_EMMC_DATA0		0x90
#define REG_CTRL_EMMC_DATA1		0x98
#define REG_CTRL_EMMC_DATA2		0xa4
#define REG_CTRL_EMMC_DATA3		0x8c

#define EMMC_CLK_DS_1V8			0xc0
#define EMMC_CMD_DS_1V8			0x150
#define EMMC_DATA0_DS_1V8		0x1d0
#define EMMC_DATA1_DS_1V8		0x1d0
#define EMMC_DATA2_DS_1V8		0x1d0
#define EMMC_DATA3_DS_1V8		0x1d0

struct emmc_drv_cap {
	unsigned int reg_addr;
	unsigned int ds;
};

#define EMMC_DRV_CAP(ofst, v) { \
	.reg_addr = ofst, \
	.ds = v}
static  struct emmc_drv_cap emmc_ds[] = {
	EMMC_DRV_CAP(REG_CTRL_EMMC_CLK, EMMC_CLK_DS_1V8),
	EMMC_DRV_CAP(REG_CTRL_EMMC_CMD, EMMC_CMD_DS_1V8),
	EMMC_DRV_CAP(REG_CTRL_EMMC_DATA0, EMMC_DATA0_DS_1V8),
	EMMC_DRV_CAP(REG_CTRL_EMMC_DATA1, EMMC_DATA1_DS_1V8),
	EMMC_DRV_CAP(REG_CTRL_EMMC_DATA2, EMMC_DATA2_DS_1V8),
	EMMC_DRV_CAP(REG_CTRL_EMMC_DATA3, EMMC_DATA3_DS_1V8),
};

static void himci_set_drv_cap(struct himci_host *host, struct mmc_ios *ios)
{
	static void *pad_ctrl = NULL;
	unsigned int i, tmp_reg;

	if (ios->timing == MMC_TIMING_MMC_DDR52) {
		tmp_reg = himci_readl(host->base + MCI_UHS_REG_EXT);
		tmp_reg &= ~CLK_DRV_PHS_MASK;
		tmp_reg |= (0x2 << CLK_DRV_PHS_SHIFT);
		himci_writel(tmp_reg, host->base + MCI_UHS_REG_EXT);
	}

	if (ios->timing == MMC_TIMING_MMC_HS200) {
		tmp_reg = himci_readl(host->base + MCI_UHS_REG_EXT);
		tmp_reg &= ~(CLK_DRV_PHS_MASK);
		tmp_reg |= (0x3 << CLK_DRV_PHS_SHIFT);
		himci_writel(tmp_reg, host->base + MCI_UHS_REG_EXT);
	}

	if (host->mmc->caps & MMC_CAP_SD_HIGHSPEED
			|| ios->timing == MMC_TIMING_LEGACY)
		return;

	if (!pad_ctrl)
		pad_ctrl = ioremap(REG_PAD_CTRL, 0x1000);

	if (!pad_ctrl)
		return;

	for (i = 0; i < ARRAY_SIZE(emmc_ds); i++)
		himci_writel(emmc_ds[i].ds, pad_ctrl + emmc_ds[i].reg_addr);
}

static int himci_check_emmc(struct himci_host *host)
{
	struct device *dev = &(host->pdev->dev);
	unsigned int val;

	if (!host->sys_regmap) {
		host->sys_regmap = syscon_regmap_lookup_by_phandle(dev->of_node,
				"regmap");
		if (IS_ERR(host->sys_regmap)) {
			host->sys_regmap = NULL;
		    return 1;
		}
	}

	if (regmap_read(host->sys_regmap, 0x8c, &val))
		return 1;

	return !(GET_SYS_BOOT_MODE(val) == BOOT_FROM_EMMC);
}
