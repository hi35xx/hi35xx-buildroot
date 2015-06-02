/******************************************************************************
*    Copyright (c) 2009-2010 by czy.
*    All rights reserved.
* ***
*
******************************************************************************/

#ifndef HISFC300H
#define HISFC300H
/******************************************************************************/

#ifndef CONFIG_HISFC300_REG_BASE_ADDRESS
#  define CONFIG_HISFC300_REG_BASE_ADDRESS            (0x60020000)
#  warning NOT config CONFIG_HISFC300_REG_BASE_ADDRESS, \
	used default value, maybe invalid.
#endif /* CONFIG_HISFC300_REG_BASE_ADDRESS */

#ifndef CONFIG_HISFC300_BUFFER_BASE_ADDRESS
#  define CONFIG_HISFC300_BUFFER_BASE_ADDRESS         (0x26000000)
#  warning NOT config CONFIG_HISFC300_BUFFER_BASE_ADDRESS, \
	used default value, maybe invalid.
#endif /* CONFIG_HISFC300_BUFFER_BASE_ADDRESS */

#ifndef CONFIG_HISFC300_PERIPHERY_REGBASE
#  define CONFIG_HISFC300_PERIPHERY_REGBASE           (0x101F5000)
#  warning NOT config CONFIG_HISFC300_PERIPHERY_REGBASE, \
	used default value, maybe invalid.
#endif /* CONFIG_HISFC300_PERIPHERY */

#ifndef CONFIG_HISFC300_CHIP_NUM
#  define CONFIG_HISFC300_CHIP_NUM                    (2)
#  warning NOT config CONFIG_HISFC300_CHIP_NUM, \
	used default value, maybe invalid.
#endif /* CONFIG_HISFC300_CHIP_NUM */

/*****************************************************************************/
#define HISFC300_REG_BASE_LEN                              (0x100)
#define HISFC300_PERIPHERY_REG_LEN                         (0x100)

#define HISFC300_BUFFER_BASE_LEN               (0x4000000) /* 64MB */

#define HISFC300_MAX_READY_WAIT_JIFFIES        (40 * HZ)
/*****************************************************************************/
#define HISFC200_REG_BUF_SIZE             (32)
#define HISFC200_REG_BUF_MASK             (0x1F)
#define HISFC200_MEM_READ_SLICE           (512)

#define HISFC200_CONFIG                                               (0x0000)
#define HISFC200_CONFIG_MODE3                (1 << 0)
#define HISFC200_CONFIG_IF_TYPE(_n)          (((_n) & 0x07) << 4)
#define HISFC200_CONFIG_DUMMY_BYTE(_n)       (((_n) & 0x01) << 7)
#define HISFC200_CONFIG_WIP_LOCATE(_n)       (((_n) & 0x07) << 8)
#define HISFC200_CONFIG_ADDR_MODE_4B         (1 << 11)
#define HISFC200_CONFIG_RD_DELAY(_n)         (((_n) & 0x03) << 12)
#define HISFC200_CONFIG_PREFETCH_CNT(_n)     (((_n) & 0x03) << 14)
#define HISFC200_CONFIG_RD_INS(_ins)         (((_ins) & 0xFF) << 16)
#define HISFC200_CONFIG_WR_INS(_ins)         (((_ins) & 0xFF) << 24)

#define HISFC200_CMD                                                  (0x0004)
#define HISFC200_CMD_DATA_CNT(_n)            (((_n - 1) & 0x1F) << 16)
#define HISFC200_CMD_DUMMY_CNT(_n)           (((_n) & 0x07) << 12)
#define HISFC200_CMD_ADDR_EN                 (1 << 8)
#define HISFC200_CMD_WP_EN                   (1 << 4)
#define HISFC200_CMD_RDDATA_EN               (1 << 3)
#define HISFC200_CMD_WRDATA_EN               (1 << 2)
#define HISFC200_CMD_CS(_cs)                 (((_cs) & 0x01) << 1)
#define HISFC200_CMD_START                   (1 << 0)

#define HISFC200_INSTR                                                (0x0008)

#define HISFC200_ADDR                                                 (0x000C)
#define HISFC200_ADDR_MASK                   (0x3FFFFFFF)

#define HISFC200_DATABUFFER1                                          (0x0010)

#define HISFC200_CS0CONFIG                                            (0x0030)
#define HISFC200_CS1CONFIG                                            (0x0034)
#define HISFC200_CSxCONFIG_CHIPSIZE(_n)      ((_n) & 0x0F)

#define HISFC200_CS0BASEADDR                                          (0x0038)
#define HISFC200_CS1BASEADDR                                          (0x003C)

#define HISFC200_TIMING                                               (0x0044)
#define HISFC200_TIMING_TSHSL(_n)            ((_n) & 0xF)
#define HISFC200_TIMING_TSHWL(_n)            (((_n) & 0xF) << 4)
#define HISFC200_TIMING_TCSS(_n)             (((_n) & 0x7) << 8)
#define HISFC200_TIMING_TCSH(_n)             (((_n) & 0x7) << 12)
#define HISFC200_TIMING_TRPD(_n)             (((_n) & 0xFFF) << 16)

/*****************************************************************************/
#define HISFC300_VERSION                                              (0x01F8)

/*****************************************************************************/

struct hisfc_spi {
	char *name;
	int chipselect;
	unsigned long long chipsize;
	unsigned int erasesize;
	void __iomem *iobase;

	unsigned int addrcycle;
	struct spi_operation  read[1];
	struct spi_operation  write[1];
	struct spi_operation  erase[MAX_SPI_OP];
};

struct hisfc_host {
	struct mtd_info   mtd[1];
	void __iomem     *iobase;
	void __iomem     *regbase;
	struct device    *dev;
	struct mutex      lock;
	void __iomem     *cfgreg;

	int add_partition;
	int num_chip;
	struct hisfc_spi spi[CONFIG_HISFC300_CHIP_NUM+1];

	int (*suspend)(struct platform_device *pltdev, pm_message_t state);
	int (*resume)(struct platform_device *pltdev);
};

#define MTD_TO_HOST(_mtd)               ((struct hisfc_host *)(_mtd))
/*****************************************************************************/

#define hisfc_read(_host, _reg) \
	readl((char *)_host->regbase + (_reg))

#define hisfc_write(_host, _reg, _value) \
	writel((_value), (char *)_host->regbase + (_reg))

/*****************************************************************************/

#if 1

#define DBG_MSG(_fmt, arg...)
#define DBG_WARN(_fmt, arg...)

#else

#define DBG_MSG(_fmt, arg...) \
	printk(KERN_INFO "%s(%d): " _fmt, __FILE__, __LINE__, ##arg);

#define DBG_WARN(_fmt, arg...) \
	printk(KERN_INFO "%s(%d): " _fmt, __FILE__, __LINE__, ##arg);

#endif

#define DBG_BUG(fmt, args...) do {\
	printk("%s(%d): BUG !!! " fmt, __FILE__, __LINE__, ##args); \
	while (1) \
		; \
} while (0)

/*****************************************************************************/
extern void hisfc200_set_system_clock(struct hisfc_host *host, struct spi_operation *op, int clk_en);
extern void hisfc200_get_best_clock(unsigned int *_clock);

/******************************************************************************/
#endif /* HISFC300H */
