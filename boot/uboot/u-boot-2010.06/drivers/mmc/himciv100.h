#ifndef _HIMCI_V100_H_
#define _HIMCI_V100_H_

#define POWER_ON          1
#define POWER_OFF         0

#define CARD_UNPLUGED     1
#define CARD_PLUGED       0

#define ENABLE            1
#define DISABLE           0

#define HI_MCI_DETECT_TIMEOUT	(HZ/2)

#define HI_MCI_REQUEST_TIMEOUT	(5 * HZ)

#define MAX_RETRY_COUNT		1000000

#define MMC_CLK			50000000
#define MMC_CCLK_MAX		50000000
#define MMC_CCLK_MIN		60000

/* Base address of SD card register */
#define HI_MCI_INTR		(48+32)

#define HI_MCI_DEBUG		DISABLE

#if HI_MCI_DEBUG
extern int debug_type;
#define HIMCI_DEBUG_TYPE	(HIMCI_DEBUG_TYPE_REG | \
				HIMCI_DEBUG_TYPE_FUN | \
				HIMCI_DEBUG_TYPE_CMD | \
				HIMCI_DEBUG_TYPE_INFO | \
				HIMCI_DEBUG_TYPE_ERR)

#define HIMCI_DEBUG_TYPE_REG	(0x01 << 0)
#define HIMCI_DEBUG_TYPE_FUN	(0x01 << 1)
#define HIMCI_DEBUG_TYPE_CMD	(0x01 << 2)
#define HIMCI_DEBUG_TYPE_INFO	(0x01 << 3)
#define HIMCI_DEBUG_TYPE_ERR	(0x01 << 4)

#define HIMCI_DEBUG_FMT "HIMCI_DEBUG "

#define HIMCI_DEBUG(type, msg...) do { \
	if (debug_type & type) { \
		printf(HIMCI_DEBUG_FMT "(%s) %s:%d: ", \
		get_debug_type_string(type), \
		__func__, __LINE__); \
		printf(msg); \
		printf("\n"); \
	} \
} while (0)

#define HIMCI_DEBUG_REG(msg...)	HIMCI_DEBUG(HIMCI_DEBUG_TYPE_REG, msg)
#define HIMCI_DEBUG_FUN(msg...)	HIMCI_DEBUG(HIMCI_DEBUG_TYPE_FUN, msg)
#define HIMCI_DEBUG_CMD(msg...)	HIMCI_DEBUG(HIMCI_DEBUG_TYPE_CMD, msg)
#define HIMCI_DEBUG_INFO(msg...) HIMCI_DEBUG(HIMCI_DEBUG_TYPE_INFO, msg)
#define HIMCI_DEBUG_ERR(msg...)	HIMCI_DEBUG(HIMCI_DEBUG_TYPE_ERR, msg)

#define HIMCI_ASSERT_FMT "HIMCI_ASSERT "

#define HIMCI_ASSERT(cond) do { \
	if (!(cond)) {\
		printf(HIMCI_ASSERT_FMT "%s:%d\n", __func__, __LINE__); \
		BUG(); \
	} \
} while (0)
#else
#define HIMCI_DEBUG(type, msg...)
#define HIMCI_DEBUG_REG(msg...)
#define HIMCI_DEBUG_FUN(msg...)
#define HIMCI_DEBUG_CMD(msg...)
#define HIMCI_DEBUG_INFO(msg...)
#define HIMCI_DEBUG_ERR(msg...)
#define HIMCI_ASSERT(cond)
#endif

#define HIMCI_ERROR_FMT "HIMCI_ERROR "

#define HIMCI_ERROR(s...) do { \
	printf(HIMCI_ERROR_FMT "%s:%d: ", __func__, __LINE__); \
	printf(s); \
	printf("\n"); \
} while (0)

#define himci_readl(addr)({unsigned int reg = readl((unsigned int)addr); \
	HIMCI_DEBUG_REG("readl(0x%04X) = 0x%08X", (unsigned int)addr, reg); \
	reg; })

#define himci_writel(v, addr) do { writel(v, (unsigned int)addr); \
	HIMCI_DEBUG_REG("writel(0x%04X) = 0x%08X", (unsigned int)addr,\
		 (unsigned int)(v)); \
} while (0)


struct himci_dma_des {
	unsigned long idmac_des_ctrl;
	unsigned long idmac_des_buf_size;
	unsigned long idmac_des_buf_addr;
	unsigned long idmac_des_next_addr;
};

struct himci_host {
	struct mmc       mmc;
	unsigned long    base;
	unsigned int     card_status;
	int              cmd_id;
	struct mmc_cmd *cmd;
	struct himci_dma_des *dma_des;
};

typedef union {
	unsigned int cmd_arg;
	struct cmd_bits_arg{
		unsigned int cmd_index:6;
		unsigned int response_expect:1;
		unsigned int response_length:1;
		unsigned int check_response_crc:1;
		unsigned int data_transfer_expected:1;
		unsigned int read_write:1;
		unsigned int transfer_mode:1;
		unsigned int send_auto_stop:1;
		unsigned int wait_prvdata_complete:1;
		unsigned int stop_abort_cmd:1;
		unsigned int send_initialization:1;
		unsigned int card_number:5;
		unsigned int update_clk_reg_only:1; /* bit 21 */
		unsigned int reserved:9;
		unsigned int start_cmd:1; /* HSB */
	} bits;
} cmd_arg_s;

#endif


