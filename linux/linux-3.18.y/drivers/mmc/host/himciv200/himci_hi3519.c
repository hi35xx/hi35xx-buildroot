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
#define TUNING_START_PHASE      2
#define TUNING_END_PHASE        14

#define EMMC_DLL_CTRL		IO_ADDRESS(0x12030140)
#define REG_PAD_CTRL		0x12040800
#define DRV_PHASE_DFLT         (0x6<<23)
#define SMPL_PHASE_DFLT        (0x0<<16)

#define REG_CTRL_SDIO0_CCLK	IO_ADDRESS(REG_PAD_CTRL + 0x114)
#define REG_CTRL_SDIO0_CCMD	IO_ADDRESS(REG_PAD_CTRL + 0x118)
#define REG_CTRL_SDIO0_CDATA0	IO_ADDRESS(REG_PAD_CTRL + 0x11c)
#define REG_CTRL_SDIO0_CDATA1	IO_ADDRESS(REG_PAD_CTRL + 0x120)
#define REG_CTRL_SDIO0_CDATA2	IO_ADDRESS(REG_PAD_CTRL + 0x124)
#define REG_CTRL_SDIO0_CDATA3	IO_ADDRESS(REG_PAD_CTRL + 0x128)

#define REG_CTRL_SDIO1_CCLK	IO_ADDRESS(REG_PAD_CTRL + 0x138)
#define REG_CTRL_SDIO1_CCMD	IO_ADDRESS(REG_PAD_CTRL + 0x13c)
#define REG_CTRL_SDIO1_CDATA0	IO_ADDRESS(REG_PAD_CTRL + 0x140)
#define REG_CTRL_SDIO1_CDATA1	IO_ADDRESS(REG_PAD_CTRL + 0x144)
#define REG_CTRL_SDIO1_CDATA2	IO_ADDRESS(REG_PAD_CTRL + 0x148)
#define REG_CTRL_SDIO1_CDATA3	IO_ADDRESS(REG_PAD_CTRL + 0x14c)

#define REG_CTRL_EMMC_DS	IO_ADDRESS(REG_PAD_CTRL + 0x17c)
#define REG_CTRL_EMMC_CLK	IO_ADDRESS(REG_PAD_CTRL + 0x154)
#define REG_CTRL_EMMC_CMD	IO_ADDRESS(REG_PAD_CTRL + 0x170)
#define REG_CTRL_EMMC_DATA0	IO_ADDRESS(REG_PAD_CTRL + 0x158)
#define REG_CTRL_EMMC_DATA1	IO_ADDRESS(REG_PAD_CTRL + 0x174)
#define REG_CTRL_EMMC_DATA2	IO_ADDRESS(REG_PAD_CTRL + 0x15c)
#define REG_CTRL_EMMC_DATA3	IO_ADDRESS(REG_PAD_CTRL + 0x164)
#define REG_CTRL_EMMC_DATA4	IO_ADDRESS(REG_PAD_CTRL + 0x168)
#define REG_CTRL_EMMC_DATA5	IO_ADDRESS(REG_PAD_CTRL + 0x150)
#define REG_CTRL_EMMC_DATA6	IO_ADDRESS(REG_PAD_CTRL + 0x184)
#define REG_CTRL_EMMC_DATA7	IO_ADDRESS(REG_PAD_CTRL + 0x160)

#define SDIO_CCLK_DS_3V3		0x1c0
#define SDIO_CCMD_DS_3V3		0x140
#define SDIO_CDATA0_DS_3V3		0x140
#define SDIO_CDATA1_DS_3V3		0x140
#define SDIO_CDATA2_DS_3V3		0x140
#define SDIO_CDATA3_DS_3V3		0x140

#define SDIO_CCLK_DS_1V8		0x0
#define SDIO_CCMD_DS_1V8		0x120
#define SDIO_CDATA0_DS_1V8		0x120
#define SDIO_CDATA1_DS_1V8		0x120
#define SDIO_CDATA2_DS_1V8		0x120
#define SDIO_CDATA3_DS_1V8		0x120

#define EMMC_DS_DS_3V3			0x120
#define EMMC_CLK_DS_3V3			0x1c0
#define EMMC_CMD_DS_3V3			0x1d0
#define EMMC_DATA0_DS_3V3		0x1e0
#define EMMC_DATA1_DS_3V3		0x1d0
#define EMMC_DATA2_DS_3V3		0x1d0
#define EMMC_DATA3_DS_3V3		0x1e0
#define EMMC_DATA4_DS_3V3		0x1e0
#define EMMC_DATA5_DS_3V3		0x1d0
#define EMMC_DATA6_DS_3V3		0x1e0
#define EMMC_DATA7_DS_3V3		0x1e0

#define EMMC_DS_DS_1V8			0x100
#define EMMC_CLK_DS_1V8			0x160
#define EMMC_CMD_DS_1V8			0x180
#define EMMC_DATA0_DS_1V8		0x180
#define EMMC_DATA1_DS_1V8		0x180
#define EMMC_DATA2_DS_1V8		0x180
#define EMMC_DATA3_DS_1V8		0x180
#define EMMC_DATA4_DS_1V8		0x180
#define EMMC_DATA5_DS_1V8		0x180
#define EMMC_DATA6_DS_1V8		0x180
#define EMMC_DATA7_DS_1V8		0x180

static void himci_set_pin_drv_cap(struct himci_host *host, unsigned int flag)
{
	if (flag) {
		if (0 == host->devid) {
			himci_writel(SDIO_CCLK_DS_1V8, REG_CTRL_SDIO0_CCLK);
			himci_writel(SDIO_CCMD_DS_1V8, REG_CTRL_SDIO0_CCMD);
			himci_writel(SDIO_CDATA0_DS_1V8, REG_CTRL_SDIO0_CDATA0);
			himci_writel(SDIO_CDATA1_DS_1V8, REG_CTRL_SDIO0_CDATA1);
			himci_writel(SDIO_CDATA2_DS_1V8, REG_CTRL_SDIO0_CDATA2);
			himci_writel(SDIO_CDATA3_DS_1V8, REG_CTRL_SDIO0_CDATA3);
		} else if (1 == host->devid) {
			himci_writel(SDIO_CCLK_DS_1V8, REG_CTRL_SDIO1_CCLK);
			himci_writel(SDIO_CCMD_DS_1V8, REG_CTRL_SDIO1_CCMD);
			himci_writel(SDIO_CDATA0_DS_1V8, REG_CTRL_SDIO1_CDATA0);
			himci_writel(SDIO_CDATA1_DS_1V8, REG_CTRL_SDIO1_CDATA1);
			himci_writel(SDIO_CDATA2_DS_1V8, REG_CTRL_SDIO1_CDATA2);
			himci_writel(SDIO_CDATA3_DS_1V8, REG_CTRL_SDIO1_CDATA3);
		} else if (2 == host->devid) {
			himci_writel(EMMC_DS_DS_1V8, REG_CTRL_EMMC_DS);
			himci_writel(EMMC_CLK_DS_1V8, REG_CTRL_EMMC_CLK);
			himci_writel(EMMC_CMD_DS_1V8, REG_CTRL_EMMC_CMD);
			himci_writel(EMMC_DATA0_DS_1V8, REG_CTRL_EMMC_DATA0);
			himci_writel(EMMC_DATA1_DS_1V8, REG_CTRL_EMMC_DATA1);
			himci_writel(EMMC_DATA2_DS_1V8, REG_CTRL_EMMC_DATA2);
			himci_writel(EMMC_DATA3_DS_1V8, REG_CTRL_EMMC_DATA3);
			himci_writel(EMMC_DATA4_DS_1V8, REG_CTRL_EMMC_DATA4);
			himci_writel(EMMC_DATA5_DS_1V8, REG_CTRL_EMMC_DATA5);
			himci_writel(EMMC_DATA6_DS_1V8, REG_CTRL_EMMC_DATA6);
			himci_writel(EMMC_DATA7_DS_1V8, REG_CTRL_EMMC_DATA7);
		}
	} else {
		/* config Pin drive capability */
		if (0 == host->devid) {
			himci_writel(SDIO_CCLK_DS_3V3, REG_CTRL_SDIO0_CCLK);
			himci_writel(SDIO_CCMD_DS_3V3, REG_CTRL_SDIO0_CCMD);
			himci_writel(SDIO_CDATA0_DS_3V3, REG_CTRL_SDIO0_CDATA0);
			himci_writel(SDIO_CDATA1_DS_3V3, REG_CTRL_SDIO0_CDATA1);
			himci_writel(SDIO_CDATA2_DS_3V3, REG_CTRL_SDIO0_CDATA2);
			himci_writel(SDIO_CDATA3_DS_3V3, REG_CTRL_SDIO0_CDATA3);
		} else if (1 == host->devid) {
			himci_writel(SDIO_CCLK_DS_3V3, REG_CTRL_SDIO1_CCLK);
			himci_writel(SDIO_CCMD_DS_3V3, REG_CTRL_SDIO1_CCMD);
			himci_writel(SDIO_CDATA0_DS_3V3, REG_CTRL_SDIO1_CDATA0);
			himci_writel(SDIO_CDATA1_DS_3V3, REG_CTRL_SDIO1_CDATA1);
			himci_writel(SDIO_CDATA2_DS_3V3, REG_CTRL_SDIO1_CDATA2);
			himci_writel(SDIO_CDATA3_DS_3V3, REG_CTRL_SDIO1_CDATA3);
		} else if (2 == host->devid) {
			himci_writel(EMMC_DS_DS_3V3, REG_CTRL_EMMC_DS);
			himci_writel(EMMC_CLK_DS_3V3, REG_CTRL_EMMC_CLK);
			himci_writel(EMMC_CMD_DS_3V3, REG_CTRL_EMMC_CMD);
			himci_writel(EMMC_DATA0_DS_3V3, REG_CTRL_EMMC_DATA0);
			himci_writel(EMMC_DATA1_DS_3V3, REG_CTRL_EMMC_DATA1);
			himci_writel(EMMC_DATA2_DS_3V3, REG_CTRL_EMMC_DATA2);
			himci_writel(EMMC_DATA3_DS_3V3, REG_CTRL_EMMC_DATA3);
			himci_writel(EMMC_DATA4_DS_3V3, REG_CTRL_EMMC_DATA4);
			himci_writel(EMMC_DATA5_DS_3V3, REG_CTRL_EMMC_DATA5);
			himci_writel(EMMC_DATA6_DS_3V3, REG_CTRL_EMMC_DATA6);
			himci_writel(EMMC_DATA7_DS_3V3, REG_CTRL_EMMC_DATA7);
		}
	}
}

static void himci_set_drv_cap(struct himci_host *host, struct mmc_ios *ios)
{
	unsigned int tmp_reg;

	if (ios->timing == MMC_TIMING_UHS_DDR50) {
		tmp_reg = himci_readl(host->base + MCI_UHS_REG_EXT);
		tmp_reg &= ~CLK_SMPL_PHS_MASK;
		tmp_reg |= (0x4 << 16);
		himci_writel(tmp_reg, host->base + MCI_UHS_REG_EXT);
	} else if (ios->timing == MMC_TIMING_UHS_SDR104) {
		tmp_reg = himci_readl(host->base + MCI_UHS_REG_EXT);
		tmp_reg &= ~CLK_DRV_PHS_MASK;
		tmp_reg |= (0x4 << 23);
		himci_writel(tmp_reg, host->base + MCI_UHS_REG_EXT);
	}

	/* set pin drive capability */
	if (ios->timing == MMC_TIMING_MMC_HS200 ||
			ios->timing == MMC_TIMING_MMC_HS400 ||
			ios->timing == MMC_TIMING_UHS_SDR104)
		himci_set_pin_drv_cap(host, 1);
	else
		himci_set_pin_drv_cap(host, 0);
}

static int himci_check_emmc(struct himci_host *host)
{
	unsigned int val;

	val = himci_readl(IO_ADDRESS(SYS_CTRL_BASE + REG_SC_STAT));

	return !(GET_SYS_BOOT_MODE(val) == BOOT_FROM_EMMC);
}
