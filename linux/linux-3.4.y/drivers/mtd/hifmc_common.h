/******************************************************************************
 *	Flash Memory Controller Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

#ifndef __HIFMC_COMMON_H__
#define __HIFMC_COMMON_H__

/*****************************************************************************/
#define _512B					(512)
#define _1K					(1024)
#define _2K					(2048)
#define _4K					(4096)
#define _8K					(8192)
#define _16K					(16384)
#define _32K					(32768)
#define _64K					(0x10000UL)
#define _128K					(0x20000UL)
#define _256K					(0x40000UL)
#define _512K					(0x80000UL)
#define _1M					(0x100000UL)
#define _2M					(0x200000UL)
#define _4M					(0x400000UL)
#define _8M					(0x800000UL)
#define _16M					(0x1000000UL)
#define _32M					(0x2000000UL)
#define _64M					(0x4000000UL)
#define _128M					(0x8000000UL)
#define _256M					(0x10000000UL)
#define _512M					(0x20000000UL)
#define _1G					(0x40000000ULL)
#define _2G					(0x80000000ULL)
#define _4G					(0x100000000ULL)
#define _8G					(0x200000000ULL)
#define _16G					(0x400000000ULL)
#define _64G					(0x1000000000ULL)

#define ECC_TYPE_0BIT				0x0
#define ECC_TYPE_8BIT				0x1
#define ECC_TYPE_16BIT				0x2
#define ECC_TYPE_24BIT				0x3
#define ECC_TYPE_28BIT				0x4
#define ECC_TYPE_40BIT				0x5
#define ECC_TYPE_64BIT				0x6

#define PAGE_SIZE_2KB				0x0
#define PAGE_SIZE_4KB				0x1
#define PAGE_SIZE_8KB				0x2
#define PAGE_SIZE_16KB				0x3

#ifdef CONFIG_HIFMC
/*****************************************************************************/
#ifndef CONFIG_HIFMC_REG_BASE
#define CONFIG_HIFMC_REG_BASE			(0x10000000)
#warning NOT config CONFIG_HIFMC_REG_BASE, used default value, maybe invalid
#endif

#ifndef CONFIG_HIFMC_BUFFER_BASE
#define CONFIG_HIFMC_BUFFER_BASE		(0x14000000)
#warning NOT config CONFIG_HIFMC_BUFFER_BASE, used default value, maybe invalid
#endif

/*****************************************************************************/
#define HIFMC_REG_ADDRESS_LEN			0x200

#define CONFIG_HIFMC_MAX_CS_NUM			2

/*****************************************************************************/
#define FMC_MAX_READY_WAIT_JIFFIES		(2 * HZ)

/*****************************************************************************/
/* HIFMC REG */
/*****************************************************************************/
#define FMC_CFG					0x00
#define FMC_CFG_SPI_NAND_SEL(_type)		(((_size) & 0x3) << 11)
#define FMC_CFG_SPI_NOR_ADDR_MODE(_mode)	((_mode) << 10)
#define FMC_CFG_BLOCK_SIZE(_size)		(((_size) & 0x3) << 8)
#define FMC_CFG_ECC_TYPE(_type)			(((_type) & 0x7) << 5)
#define FMC_CFG_PAGE_SIZE(_size)		(((_size) & 0x3) << 3)
#define FMC_CFG_FLASH_SEL(_type)		(((_type) & 0x3) << 1)
#define FMC_CFG_OP_MODE(_mode)			((_mode) & 0x1)

#define SPI_NAND_MFR_OTHER			0x0
#define SPI_NAND_MFR_WINBOND			0x1
#define SPI_NAND_MFR_ESMT			0x2
#define SPI_NAND_MFR_MICRON			0x3

#define SPI_NAND_SEL_SHIFT			11
#define SPI_NAND_SEL_MASK			(0x3 << SPI_NAND_SEL_SHIFT)

#define SPI_NOR_ADDR_MODE_3_BYTES		0x0
#define SPI_NOR_ADDR_MODE_4_BYTES		0x1

#define SPI_NOR_ADDR_MODE_SHIFT			10
#define SPI_NOR_ADDR_MODE_MASK			(0x1 << SPI_NOR_ADDR_MODE_SHIFT)

#define BLOCK_SIZE_64_PAGE			0x0
#define BLOCK_SIZE_128_PAGE			0x1
#define BLOCK_SIZE_256_PAGE			0x2
#define BLOCK_SIZE_512_PAGE			0x3

#define BLOCK_SIZE_MASK				(0x3 << 8)

#define ECC_TYPE_SHIFT				5
#define ECC_TYPE_MASK				(0x7 << ECC_TYPE_SHIFT)

#define PAGE_SIZE_SHIFT				3
#define PAGE_SIZE_MASK				(0x3 << PAGE_SIZE_SHIFT)

#define FLASH_TYPE_SPI_NOR			0x0
#define FLASH_TYPE_SPI_NAND			0x1
#define FLASH_TYPE_NAND				0x2
#define FLASH_TYPE_DEFAULT			0x3

#define FLASH_SEL_SHIFT				1
#define FLASH_SEL_MASK				(0x3 << FLASH_SEL_SHIFT)

#define OP_MODE_BOOT				0x0
#define OP_MODE_NORMAL				0x1

#define OP_MODE_MASK				0x1

/*****************************************************************************/
#define FMC_GLOBAL_CFG				0x04
#define FMC_GLOBAL_CFG_WP_ENABLE		(1 << 6)
#define FMC_GLOBAL_CFG_RANDOMIZER_EN		(1 << 2)

/*****************************************************************************/
#define FMC_SPI_TIMING_CFG			0x08
#define TIMING_CFG_TCSH(_n)			(((_n) & 0xf) << 8)
#define TIMING_CFG_TCSS(_n)			(((_n) & 0xf) << 4)
#define TIMING_CFG_TSHSL(_n)			((_n) & 0xf)

#define CS_HOLD_TIME				0x6
#define CS_SETUP_TIME				0x6
#define CS_DESELECT_TIME			0xf

/*****************************************************************************/
#define FMC_PND_PWIDTH_CFG			0x0c
#define PWIDTH_CFG_RW_HCNT(_n)			(((_n) & 0xf) << 8)
#define PWIDTH_CFG_R_LCNT(_n)			(((_n) & 0xf) << 4)
#define PWIDTH_CFG_W_LCNT(_n)			((_n) & 0xf)

#define RW_H_WIDTH				(0xa)
#define R_L_WIDTH				(0xa)
#define W_L_WIDTH				(0xa)

/*****************************************************************************/
#define FMC_INT					0x18
#define FMC_INT_AHB_OP				(1 << 7)
#define FMC_INT_WR_LOCK				(1 << 6)
#define FMC_INT_DMA_ERR				(1 << 5)
#define FMC_INT_ERR_ALARM			(1 << 4)
#define FMC_INT_ERR_INVALID			(1 << 3)
#define FMC_INT_ERR_VALID			(1 << 2)
#define FMC_INT_OP_FAIL				(1 << 1)
#define FMC_INT_OP_DONE				(1 << 0)

/*****************************************************************************/
#define FMC_INT_EN				0x1c
#define FMC_INT_EN_AHB_OP			(1 << 7)
#define FMC_INT_EN_WR_LOCK			(1 << 6)
#define FMC_INT_EN_DMA_ERR			(1 << 5)
#define FMC_INT_EN_ERR_ALARM			(1 << 4)
#define FMC_INT_EN_ERR_INVALID			(1 << 3)
#define FMC_INT_EN_ERR_VALID			(1 << 2)
#define FMC_INT_EN_OP_FAIL			(1 << 1)
#define FMC_INT_EN_OP_DONE			(1 << 0)

/*****************************************************************************/
#define FMC_INT_CLR				0x20
#define FMC_INT_CLR_AHB_OP			(1 << 7)
#define FMC_INT_CLR_WR_LOCK			(1 << 6)
#define FMC_INT_CLR_DMA_ERR			(1 << 5)
#define FMC_INT_CLR_ERR_ALARM			(1 << 4)
#define FMC_INT_CLR_ERR_INVALID			(1 << 3)
#define FMC_INT_CLR_ERR_VALID			(1 << 2)
#define FMC_INT_CLR_OP_FAIL			(1 << 1)
#define FMC_INT_CLR_OP_DONE			(1 << 0)

#define FMC_INT_CLR_ALL				0xff

/*****************************************************************************/
#define FMC_CMD					0x24
#define FMC_CMD_CMD2(_cmd)			(((_cmd) & 0xff) << 8)
#define FMC_CMD_CMD1(_cmd)			((_cmd) & 0xff)

/*****************************************************************************/
#define FMC_ADDRH				0x28
#define FMC_ADDRH_SET(_addr)			((_addr) & 0xff)

/*****************************************************************************/
#define FMC_ADDRL				0x2c
#define FMC_ADDRL_BLOCK_H_MASK(_page)		(((_page) & 0xffff) << 16)
#define FMC_ADDRL_BLOCK_L_MASK(_page)		((_page) & 0xffc0)

#define READ_ID_ADDR				0x00
#define PROTECT_ADDR				0xa0
#define FEATURE_ADDR				0xb0
#define STATUS_ADDR				0xc0

/*****************************************************************************/
#define FMC_OP_CFG				0x30
#define OP_CFG_FM_CS(_cs)			((_cs) << 11)
#define OP_CFG_FORCE_CS_EN(_en)			((_en) << 10)
#define OP_CFG_MEM_IF_TYPE(_type)		(((_type) & 0x7) << 7)
#define OP_CFG_ADDR_NUM(_addr)			(((_addr) & 0x7) << 4)
#define OP_CFG_DUMMY_NUM(_dummy)		((_dummy) & 0xf)

#define IF_TYPE_STD				0x0
#define IF_TYPE_DUAL				0x1
#define IF_TYPE_DIO				0x2
#define IF_TYPE_QUAD				0x3
#define IF_TYPE_QIO				0x4

#define IF_TYPE_SHIFT				7
#define IF_TYPE_MASK				(0x7 << IF_TYPE_SHIFT)

#define READ_ID_ADDR_NUM			1
#define FEATURES_OP_ADDR_NUM			1
#define STD_OP_ADDR_NUM				3

/*****************************************************************************/
#define FMC_SPI_OP_ADDR				0x34

/*****************************************************************************/
#define FMC_DATA_NUM				0x38
#define FMC_DATA_NUM_CNT(_n)			((_n) & 0x3fff)

#define SPI_NOR_SR_LEN				1 /* Status Register length */
#define SPI_NOR_CR_LEN				1 /* Config Register length */
#define FEATURES_DATA_LEN			1
#define READ_OOB_BB_LEN				1
#define MAX_SPI_NOR_ID_LEN			8
#define MAX_NAND_ID_LEN				8
#define MAX_SPI_NAND_ID_LEN			3

#define PROTECT_BRWD_MASK			(1 << 7)
#define PROTECT_BP3_MASK			(1 << 6)
#define PROTECT_BP2_MASK			(1 << 5)
#define PROTECT_BP1_MASK			(1 << 4)
#define PROTECT_BP0_MASK			(1 << 3)

#define ANY_BP_ENABLE(_val)			((PROTECT_BP3_MASK & _val) \
						|| (PROTECT_BP2_MASK & _val) \
						|| (PROTECT_BP1_MASK & _val) \
						|| (PROTECT_BP0_MASK & _val))

#define ALL_BP_MASK				(PROTECT_BP3_MASK \
						| PROTECT_BP2_MASK \
						| PROTECT_BP1_MASK \
						| PROTECT_BP0_MASK)

#define FEATURE_ECC_ENABLE			(1 << 4)
#define FEATURE_QE_ENABLE			(1 << 0)

/*****************************************************************************/
#define FMC_OP					0x3c
#define FMC_OP_DUMMY_EN(_en)			((_en) << 8)
#define FMC_OP_CMD1_EN(_en)			((_en) << 7)
#define FMC_OP_ADDR_EN(_en)			((_en) << 6)
#define FMC_OP_WRITE_DATA_EN(_en)		((_en) << 5)
#define FMC_OP_CMD2_EN(_en)			((_en) << 4)
#define FMC_OP_WAIT_READY_EN(_en)		((_en) << 3)
#define FMC_OP_READ_DATA_EN(_en)		((_en) << 2)
#define FMC_OP_READ_STATUS_EN(_en)		((_en) << 1)
#define FMC_OP_REG_OP_START			1

/*****************************************************************************/
#define FMC_DMA_LEN				0x40
#define FMC_DMA_LEN_SET(_len)			((_len) & 0x0fffffff)

/*****************************************************************************/
#define FMC_DMA_AHB_CTRL			0x48
#define FMC_DMA_AHB_CTRL_DMA_PP_EN		(1 << 3)
#define FMC_DMA_AHB_CTRL_BURST16_EN		(1 << 2)
#define FMC_DMA_AHB_CTRL_BURST8_EN		(1 << 1)
#define FMC_DMA_AHB_CTRL_BURST4_EN		1

#define ALL_BURST_ENABLE			(FMC_DMA_AHB_CTRL_BURST16_EN \
						| FMC_DMA_AHB_CTRL_BURST8_EN \
						| FMC_DMA_AHB_CTRL_BURST4_EN)

#define FMC_DMA_ADDR_OFFSET			4096

/*****************************************************************************/
#define FMC_DMA_SADDR_D0			0x4c

/*****************************************************************************/
#define FMC_DMA_SADDR_D1			0x50

/*****************************************************************************/
#define FMC_DMA_SADDR_D2			0x54

/*****************************************************************************/
#define FMC_DMA_SADDR_D3			0x58

/*****************************************************************************/
#define FMC_DMA_SADDR_OOB			0x5c

/*****************************************************************************/
#define FMC_DMA_BLK_SADDR			0x60
#define FMC_DMA_BLK_SADDR_SET(_addr)		((_addr) & 0xffffff)

/*****************************************************************************/
#define FMC_DMA_BLK_LEN				0x64
#define FMC_DMA_BLK_LEN_SET(_len)		((_len) & 0xffff)

/*****************************************************************************/
#define FMC_OP_CTRL				0x68
#define OP_CTRL_RD_OPCODE(_code)		(((_code) & 0xff) << 16)
#define OP_CTRL_WR_OPCODE(_code)		(((_code) & 0xff) << 8)
#define OP_CTRL_RD_OP_SEL(_op)			(((_op) & 0x3) << 4)
#define OP_CTRL_DMA_OP(_type)			((_type) << 2)
#define OP_CTRL_RW_OP(_op)			((_op) << 1)
#define OP_CTRL_DMA_OP_READY			1

#define RD_OP_READ_ALL_PAGE			0x0
#define RD_OP_READ_OOB				0x1
#define RD_OP_BLOCK_READ			0x2

#define RD_OP_SHIFT				4
#define RD_OP_MASK				(0x3 << RD_OP_SHIFT)

#define OP_TYPE_DMA				0x0
#define OP_TYPE_REG				0x1

#define RW_OP_READ				0x0
#define RW_OP_WRITE				0x1

/*****************************************************************************/
#define FMC_OP_PARA				0x70
#define FMC_OP_PARA_RD_OOB_ONLY			(1 << 1)

/*****************************************************************************/
#define FMC_BOOT_SET				0x74
#define FMC_BOOT_SET_DEVICE_ECC_EN		(1 << 3)
#define FMC_BOOT_SET_BOOT_QUAD_EN		(1 << 1)

/*****************************************************************************/
#define FMC_STATUS				0xac

#define GET_OP					0
#define SET_OP					1

#define STATUS_ECC_MASK				(0x3 << 4)
#define STATUS_P_FAIL_MASK			(1 << 3)
#define STATUS_E_FAIL_MASK			(1 << 2)
#define STATUS_WEL_MASK				(1 << 1)
#define STATUS_OIP_MASK				(1 << 0)

/*****************************************************************************/
#define FMC_VERSION				0xbc

/* Hifmc IP version */
#define HIFMC_VER_100				(0x100)

#endif /* End of CONFIG_HIFMC */

/*****************************************************************************/
#define DISABLE					0
#define ENABLE					1

/*****************************************************************************/
/* DMA address align with 32 bytes. */
#define FMC_DMA_ALIGN				32

#define FMC_CHIP_DELAY				25

/*****************************************************************************/
#define hifmc_read(_host, _reg) \
	readl((void __iomem *)((char *)_host->regbase + (_reg)))

#define hifmc_write(_host, _reg, _value) \
	writel((u_int)(_value), (void __iomem *)((char *)_host->regbase \
				+ (_reg)))

#define GET_PAGE_INDEX(host) \
		((host->addr_value[0] >> 16) | (host->addr_value[1] << 16))

/*****************************************************************************/
#define DB_MSG(_fmt, arg...) \
	pr_info("%s(%d): " _fmt, __func__, __LINE__, ##arg);

#define DB_BUG(fmt, args...) \
	do { \
		pr_info("%s(%d): BUG: " fmt, __FILE__, __LINE__, ##args); \
		while (1) \
			; \
	} while (0)

/*****************************************************************************/
#define BT_DBG		0	/* Boot init debug print */
#define ER_DBG		0	/* Erase debug print */
#define WR_DBG		0	/* Write debug print */
#define RD_DBG		0	/* Read debug print */
#define QE_DBG		0	/* Quad Enable debug print */
#define OP_DBG		0	/* OP command debug print */
#define DMA_DB		0	/* DMA read or write debug print */
#define AC_DBG		0	/* 3-4byte Address Cycle */
#define SR_DBG		0	/* Status Register debug print */
#define CR_DBG		0	/* Config Register debug print */
#define FT_DBG		0	/* Features debug print */
#define WE_DBG		0	/* Write Enable debug print */
#define BP_DBG		0	/* Block Protection debug print */
#define EC_DBG		0	/* enable/disable ecc0 and randomizer */
#define PM_DBG		0	/* power management debug */

#define FMC_PR(_type, _fmt, arg...) \
	do { \
		if (_type) \
			DB_MSG(_fmt, ##arg) \
	} while (0)

/*****************************************************************************/
#define FMC_WAIT_TIMEOUT 0x800000

#define FMC_CMD_WAIT_CPU_FINISH(_host) \
	do { \
		unsigned regval, timeout = FMC_WAIT_TIMEOUT; \
		do { \
			regval = hifmc_read((_host), FMC_OP); \
			--timeout; \
		} while ((regval & FMC_OP_REG_OP_START) && timeout); \
		if (!timeout) \
			DB_MSG("Error: Wait cmd cpu finish timeout!\n"); \
	} while (0)

/*****************************************************************************/
#define FMC_DMA_WAIT_INT_FINISH(_host) \
	do { \
		unsigned regval, timeout = FMC_WAIT_TIMEOUT; \
		do { \
			regval = hifmc_read((_host), FMC_INT); \
			--timeout; \
		} while ((!(regval & FMC_INT_OP_DONE) && timeout)); \
		if (!timeout) { \
			debug_register_dump(); \
			DB_MSG("Error: Wait dma int finish timeout!\n"); \
		} \
	} while (0)

/*****************************************************************************/
#define FMC_DMA_WAIT_CPU_FINISH(_host) \
	do { \
		unsigned regval, timeout = FMC_WAIT_TIMEOUT; \
		do { \
			regval = hifmc_read((_host), FMC_OP_CTRL); \
			--timeout; \
		} while ((regval & OP_CTRL_DMA_OP_READY) && timeout); \
		if (!timeout) { \
			debug_register_dump(); \
			DB_MSG("Error: Wait dma cpu finish timeout!\n"); \
		} \
	} while (0)

/*****************************************************************************/
struct hifmc_cmd_op {
	unsigned char cs;
	unsigned char cmd;
	unsigned char l_cmd;
	unsigned char addr_h;
	unsigned int addr_l;
	unsigned int data_no;
	unsigned short option;
	unsigned short op_cfg;
};

/*****************************************************************************/
extern unsigned char hifmc_cs_user[];

/*****************************************************************************/
extern char *ulltostr(unsigned long long size);

extern void debug_register_dump(void);

extern int hifmc_ip_ver_check(void);

#endif /* End of __HIFMC_COMMON_H__ */

