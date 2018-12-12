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

#ifndef HISNFC100H
#define HISNFC100H

/******************************************************************************/
/* These macroes are for debug only, reg option is slower then dma option */
#undef HISNFC100_SUPPORT_REG_READ
/* #define HISNFC100_SUPPORT_REG_READ */

#undef HISNFC100_SUPPORT_REG_WRITE
/* #define HISNFC100_SUPPORT_REG_WRITE */

/*****************************************************************************/
#define HISNFC100_CFG					0x00
#define HISNFC100_CFG_BOOT_MODE(_mode)			((_mode) << 17)
#define HISNFC100_CFG_DEVICE_INTERNAL_ECC_ENABLE	(1 << 16)
#define HISNFC100_CFG_FLASH_SIZE_CS1(_size)		(((_size) & 0xf) << 10)
#define HISNFC100_CFG_FLASH_SIZE_CS0(_size)		(((_size) & 0xf) << 6)
#define HISNFC100_CFG_ADDR_MODE(_mode)			((_mode) << 5)
#define HISNFC100_CFG_ECC_TYPE(type)			(((type) & 0x3) << 3)
#define HISNFC100_CFG_PAGE_SIZE(size)			((size) << 2)
#define HISNFC100_CFG_DEVICE_TYPE(_type)		((_type) << 1)
#define HISNFC100_CFG_OP_MODE(_mode)			((_mode) << 0)

#define BOOT_MODE_MASK				(0x1 << 17)

#define FLASH_SIZE_CS1_SHIFT			10
#define FLASH_SIZE_CS1_MASK			(0xf << 10)

#define FLASH_SIZE_CS0_SHIFT			6
#define FLASH_SIZE_CS0_MASK			(0xf << 6)

#define ECC_TYPE_SHIFT				3
#define ECC_TYPE_MASK				(0x3 << 3)

#define PAGE_SIZE_SHIFT				2
#define PAGE_SIZE_MASK				(0x1 << 2)

#define DEVICE_TYPE_SHIFT			1
#define DEVICE_TYPE_MASK			(0x1 << 1)
#define DEVICE_TYPE_NOR_FLASH			0
#define DEVICE_TYPE_NAND_FLASH			1

#define OP_MODE_MASK				(0x1 << 0)
#define OP_MODE_BOOT				0
#define OP_MODE_NORMAL				1

/*****************************************************************************/
#define HISNFC100_GLOBAL_CFG			0x04
#define HISNFC100_GLOBAL_CFG_WP_ENABLE		(1 << 5)

/*****************************************************************************/
#define HISNFC100_TIMING_CFG			0x08
#define HISNFC100_TIMING_CFG_TCSH(_n)		(((_n) & 0xf) << 8)
#define HISNFC100_TIMING_CFG_TCSS(_n)		(((_n) & 0xf) << 4)
#define HISNFC100_TIMING_CFG_TSHSL(_n)		((_n) & 0xf)

#define CS_HOLD_TIME				0x6
#define CS_SETUP_TIME				0x6
#define CS_DESELECT_TIME			0xf

/*****************************************************************************/
#define HISNFC100_INT				0x0c
#define HISNFC100_INT_OP_DONE			(1 << 0)

/*****************************************************************************/
#define HISNFC100_INT_CLR			0x14
#define HISNFC100_INT_CLR_DMA_ERR		(1 << 5)
#define HISNFC100_INT_CLR_OP_FAIL		(1 << 2)
#define HISNFC100_INT_CLR_OP_DONE		(1 << 0)

#define HISNFC100_INT_CLR_ALL			0x7f

/*****************************************************************************/
#define HISNFC100_OPCODE			0x18

/*****************************************************************************/
#define HISNFC100_OP_ADDR			0x1c
#define HISNFC100_OP_ADDRH_BLOCK_MASK(_page)	(((_page) & 0xffff) << 16)
#define HISNFC100_OP_ADDRL_BLOCK_MASK(_page)	((_page) & 0xffc0)

#define READ_ID_ADDR				0x00
#define PROTECTION_ADDR				0xa0
#define FEATURE_ADDR				0xb0
#define STATUS_ADDR				0xc0

/*****************************************************************************/
#define HISNFC100_OP				0x20
#define HISNFC100_OP_SEL_CS(_cs)		((_cs) << 11)
#define HISNFC100_OP_ADDR_NUM(_n)		(((_n) & 0x7) << 8)
#define HISNFC100_OP_DUMMY_CMD_EN(_en)		((_en) << 7)
#define HISNFC100_OP_DUMMY_ADDR_EN(_en)		((_en) << 6)
#define HISNFC100_OP_OPCODE_EN(_en)		((_en) << 5)
#define HISNFC100_OP_ADDR_EN(_en)		((_en) << 4)
#define HISNFC100_OP_DATE_WRITE_EN(_en)		((_en) << 3)
#define HISNFC100_OP_DATE_READ_EN(_en)		((_en) << 2)
#define HISNFC100_OP_STATUS_READ_EN(_en)	((_en) << 1)
#define HISNFC100_OP_START			(1 << 0)

#define STD_OP_ADDR_NUM				3
#define READ_ID_ADDR_NUM			1

#define FEATURES_OP_ADDR_NUM			1

/*****************************************************************************/
#define HISNFC100_DATA_NUM			0x24
#define HISNFC100_DATA_NUM_CNT(_n)		((_n) & 0x1fff)

#define FEATURES_DATE_NUM			1

#define READ_OOB_BB_LEN				1

/*****************************************************************************/
#define HISNFC100_OP_CFG			0x28
#define HISNFC100_OP_CFG_DIR_TRANS_ENABLE	(1 << 11)
#define HISNFC100_OP_CFG_RD_OP_SEL(_type)	(((_type) & 0x3) << 9)
#define HISNFC100_OP_CFG_MEM_IF_TYPE(_type)	(((_type) & 0x7) << 6)
#define HISNFC100_OP_CFG_DUMMY_CMD_NUM(_no)	(((_no) & 0x7) << 3)
#define HISNFC100_OP_CFG_DUMMY_ADDR_NUM(_no)	(((_no) & 0x7) << 0)

#define RD_OP_READ_PAGE				0x0
#define RD_OP_READ_DATE				0x1
#define RD_OP_READ_OOB				0x2

/*****************************************************************************/
#define HISNFC100_ADDRH				0x2c
#define HISNFC100_ADDRH_SET(_addr)		((_addr) & 0xff)

/*****************************************************************************/
#define HISNFC100_ADDRL				0x30

/*****************************************************************************/
#define HISNFC100_OP_CTRL			0x34
#define HISNFC100_OP_CTRL_RD_OPCODE(_code)	(((_code) & 0xff) << 16)
#define HISNFC100_OP_CTRL_WR_OPCODE(_code)	(((_code) & 0xff) << 8)
#define HISNFC100_OP_CTRL_CS_OP(_cs)		((_cs) << 3)
#define HISNFC100_OP_CTRL_OP_TYPE(_type)	((_type) << 2)
#define HISNFC100_OP_CTRL_RW_OP(_op)		((_op) << 1)
#define HISNFC100_OP_CTRL_OP_READY		(1 << 0)

#define OP_TYPE_REG				0
#define OP_TYPE_DMA				1

#define RW_OP_READ				0
#define RW_OP_WRITE				1

/*****************************************************************************/
#define HISNFC100_DMA_CTRL			0x3c

#define HISNFC100_DMA_CTRL_ALL_ENABLE		0x7

/*****************************************************************************/
#define HISNFC100_DMA_SADDR_D			0x40

/*****************************************************************************/
#define HISNFC100_DMA_SADDR_OOB			0x44

/*****************************************************************************/
#define HISNFC100_DMA_LEN			0x48
#define HISNFC100_DMA_LEN_SET(_len)		((_len) & 0xfffffff)

/*****************************************************************************/
#define HISNFC100_STATUS			0x54

#define GET_OP					0
#define SET_OP					1

#define PROTECTION_BRWD_MASK			(1 << 7)
#define PROTECTION_BP3_MASK			(1 << 6)
#define PROTECTION_BP2_MASK			(1 << 5)
#define PROTECTION_BP1_MASK			(1 << 4)
#define PROTECTION_BP0_MASK			(1 << 3)

#define ANY_BP_ENABLE(_val)		((PROTECTION_BP3_MASK & _val) \
					|| (PROTECTION_BP2_MASK & _val) \
					|| (PROTECTION_BP1_MASK & _val) \
					|| (PROTECTION_BP0_MASK & _val))

#define ALL_BP_MASK				(PROTECTION_BP3_MASK \
						| PROTECTION_BP2_MASK \
						| PROTECTION_BP1_MASK \
						| PROTECTION_BP0_MASK)

#define FEATURE_ECC_ENABLE			(1 << 4)
#define FEATURE_QE_ENABLE			(1 << 0)

#define STATUS_ECC_MASK				(0x3 << 4)
#define STATUS_P_FAIL_MASK			(1 << 3)
#define STATUS_E_FAIL_MASK			(1 << 2)
#define STATUS_WEL_MASK				(1 << 1)
#define STATUS_OIP_MASK				(1 << 0)

/*****************************************************************************/
#define HISNFC100_VERSION			0x68

/*****************************************************************************/
#define REG_CNT_HIGH_BLOCK_NUM_SHIFT		10

#define REG_CNT_BLOCK_NUM_MASK			0x3ff
#define REG_CNT_BLOCK_NUM_SHIFT			22

#define REG_CNT_PAGE_NUM_MASK			0x3f
#define REG_CNT_PAGE_NUM_SHIFT			16

#define REG_CNT_WRAP_MASK			0xf
#define REG_CNT_WRAP_SHIFT			12

#define REG_CNT_ECC_OFFSET_MASK			0xfff
#define REG_CNT_ECC_8BIT_OFFSET			1054
#define REG_CNT_ECC_16BIT_OFFSET		1056
#define REG_CNT_ECC_24BIT_OFFSET		1082

/*****************************************************************************/
#define SPI_NAND_MAX_PAGESIZE			4096
#define SPI_NAND_MAX_OOBSIZE			256

#define HISNFC100_BUFFER_LEN	(SPI_NAND_MAX_PAGESIZE + SPI_NAND_MAX_OOBSIZE)

/* DMA address align with 32 bytes. */
#define HISNFC100_DMA_ALIGN			32

#define HISNFC100_CHIP_DELAY			25

#define HISNFC100_ADDR_CYCLE_MASK		0x2

/*****************************************************************************/
struct hisfc_cmd_option {
	unsigned char chipselect;
	unsigned char command;
	unsigned char last_cmd;
	unsigned char address_h;
	unsigned int address_l;
	unsigned int date_num;
	unsigned short option;
	unsigned short op_config;
};

struct hisnfc_host;

struct hisnfc_host {
	struct nand_chip *chip;
	struct mtd_info  *mtd;
	struct hisnfc_op spi[CONFIG_HISNFC100_MAX_CHIP];
	struct hisfc_cmd_option cmd_option;

	void __iomem *iobase;
	void __iomem *regbase;

	unsigned int NFC_CFG;

	unsigned int offset;

	struct device *dev;

	unsigned int addr_cycle;
	unsigned int addr_value[2];
	unsigned int cache_addr_value[2];
	unsigned int column;
	unsigned int block_page_mask;

	unsigned int dma_buffer;
	unsigned int dma_oob;

	unsigned int ecctype;
	unsigned int pagesize;
	unsigned int oobsize;

	/* This is maybe an un-aligment address, only for malloc or free */
	char *buforg;
	char *buffer;

	int add_partition;

	/* BOOTROM read two bytes to detect the bad block flag */
#define HINFC_BAD_BLOCK_POS              0
	unsigned char *bbm;  /* nand bad block mark */
	unsigned short *epm;  /* nand empty page mark */

	unsigned int uc_er;

	void (*set_system_clock)(struct spi_op_info *op, int clk_en);

	void (*send_cmd_pageprog)(struct hisnfc_host *host);
	void (*send_cmd_status)(struct hisnfc_host *host);
	void (*send_cmd_readstart)(struct hisnfc_host *host);
	void (*send_cmd_erase)(struct hisnfc_host *host);
	void (*send_cmd_readid)(struct hisnfc_host *host);
	void (*send_cmd_reset)(struct hisnfc_host *host);
};

/*****************************************************************************/
#define hisfc_read(_host, _reg) \
	readl((unsigned)((char *)_host->regbase + (_reg)))

#define hisfc_write(_host, _reg, _value) \
	writel((unsigned)(_value), (unsigned)((char *)_host->regbase + (_reg)))

/*****************************************************************************/
#define DBG_BUG(fmt, args...) do { \
	printf("%s(%d): BUG: " fmt, __FILE__, __LINE__, ##args); \
	while (1) \
		; \
} while (0)

/*****************************************************************************/
#define HISNFC100_WAIT_TIMEOUT 10000000

#define HISNFC100_CMD_WAIT_CPU_FINISH(_host) do { \
	unsigned regval, timeout = HISNFC100_WAIT_TIMEOUT; \
	do { \
		regval = hisfc_read((_host), HISNFC100_OP); \
		--timeout; \
	} while ((regval & HISNFC100_OP_START) && timeout); \
	if (!timeout) \
		DBG_BUG("hisnfc100 wait cmd cpu finish timeout!\n"); \
} while (0)

/*****************************************************************************/
#define HISNFC100_DMA_WAIT_INT_FINISH(_host) do { \
	unsigned regval, timeout = HISNFC100_WAIT_TIMEOUT; \
	do { \
		regval = hisfc_read((_host), HISNFC100_INT); \
		--timeout; \
	} while ((!(regval & HISNFC100_INT_OP_DONE) && timeout)); \
	if (!timeout) \
		DBG_BUG("hisnfc100 wait dma int finish timeout!\n"); \
} while (0)

/*****************************************************************************/
#define HISNFC100_DMA_WAIT_CPU_FINISH(_host) do { \
	unsigned regval, timeout = HISNFC100_WAIT_TIMEOUT; \
	do { \
		regval = hisfc_read((_host), HISNFC100_OP_CTRL); \
		--timeout; \
	} while ((regval & HISNFC100_OP_CTRL_OP_READY) && timeout); \
	if (!timeout) \
		DBG_BUG("dma wait cpu finish timeout\n"); \
} while (0)

/*****************************************************************************/
int hisnfc100_host_init(struct hisnfc_host *host);

void hisnfc100_nand_init(struct nand_chip *chip);

int hisnfc100_ecc_probe(struct mtd_info *mtd, struct nand_chip *chip,
			struct nand_flash_dev_ex *flash_dev_ex);

/******************************************************************************/
#endif /* HISNFC100H */

