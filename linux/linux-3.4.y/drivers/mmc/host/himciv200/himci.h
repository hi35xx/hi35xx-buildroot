#ifndef _HI_MCI_H_
#define _HI_MCI_H_

extern int trace_level;
#define HIMCI_TRACE_LEVEL  5
/*
   0 - all message
   1 - dump all register read/write
   2 - flow trace
   3 - timeouut err and protocol err
   */

#define HIMCI_TRACE_FMT KERN_INFO
/* #define HIMCI_TRACE_FMT KERN_DEBUG */

#define POWER_ON         1
#define POWER_OFF        0
#define FORCE_ENABLE     1
#define FORCE_DISABLE    0

#define CARD_UNPLUGED     1
#define CARD_PLUGED       0

#define ENABLE            1
#define DISABLE           0

#define HI_MCI_DETECT_TIMEOUT (HZ / 2)

#define HI_MCI_REQUEST_TIMEOUT  (5 * HZ)

#define MAX_RETRY_COUNT   100

#define MMC_CLK           50000000
#define MMC_CCLK_MAX      50000000
#define MMC_CCLK_MIN      60000

/* Base address of SD card register */
#define HI_MCI_INTR               (48+32)

#define himci_trace(level, msg...) do { \
	if ((level) >= trace_level) { \
		pr_err(HIMCI_TRACE_FMT "%s:%d: ", __func__, __LINE__); \
		pr_err(msg); \
		pr_err("\n"); \
	} \
} while (0)

#define himci_assert(cond) do { \
	if (!(cond)) {\
		pr_err("Assert:himci:%s:%d\n", \
				__func__, \
				__LINE__); \
		BUG(); \
	} \
} while (0)

#define himci_error(s...) do { \
	pr_err("himci:%s:%d: ", __func__, __LINE__); \
	pr_err(s); \
	pr_err("\n"); \
} while (0)

#define himci_readl(addr) ({unsigned int reg = readl(IOMEM(addr)); \
	himci_trace(1, "readl(0x%04X) = 0x%08X", (unsigned int)addr, reg); \
	reg; })

#define himci_writel(v, addr) do { writel(v, IOMEM(addr)); \
	himci_trace(1, "writel(0x%04X) = 0x%08X", (unsigned int)addr, \
			(unsigned int)(v)); \
} while (0)

struct himci_des {
	unsigned long idmac_des_ctrl;
	unsigned long idmac_des_buf_size;
	unsigned long idmac_des_buf_addr;
	unsigned long idmac_des_next_addr;
};

struct himci_slot {
	struct mmc_host		*mmc;
	struct himci_host	*host;
	struct timer_list	timer;
	unsigned int		card_status;
	unsigned int		power_status;
	unsigned int		phase_inited;
	unsigned int		port;
	unsigned int		hclk; /* record host clk */
	unsigned int		cclk; /* record card clk */
	unsigned int		clk_div;   /* current clk divide */
	/* parameter must reset when card switch */
	unsigned int		CRG_sel;   /* CRG clk select */
	unsigned int		phase_smp; /* sampling clk phase shift */
	unsigned int		phase_drv; /* drive clk phase shift */
	unsigned int		hs400_mod; /* hs400 mode flag */
};

struct himci_host {
	struct mmc_host    *mmc;
	struct himci_slot   *slot;
	spinlock_t          lock;
	struct mmc_request  *mrq;
	struct mmc_command  *cmd;
	struct mmc_data     *data;
	void __iomem        *base;
	resource_size_t		crg_phy_addr;
	struct scatterlist  *dma_sg;

	unsigned int dma_sg_num;
	unsigned int dma_dir;

	dma_addr_t         dma_paddr;
	unsigned int       *dma_vaddr;
	unsigned int       irq;
	unsigned int		irq_status;
	unsigned int		is_tuning;
	wait_queue_head_t  intr_wait;
	unsigned long      pending_events;
	int devid;
	unsigned int	   claimed;
	unsigned int	   used;
	unsigned int	   card_num;
	unsigned int       cur_port;
	unsigned int	   hclk;

#define HIMCI_PEND_DTO_B  (0)
#define HIMCI_PEND_DTO_M  (1 << HIMCI_PEND_DTO_B)
};

union cmd_arg_u {
	unsigned int cmd_arg;
	struct cmd_bits_arg {
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
		unsigned int read_ceata_device:1;
		unsigned int ccs_expected:1;
		unsigned int enable_boot:1;
		unsigned int expect_boot_ack:1;
		unsigned int disable_boot:1;
		unsigned int boot_mode:1;
		unsigned int volt_switch:1;
		unsigned int use_hold_reg:1;
		unsigned int reserved:1;
		unsigned int start_cmd:1; /* HSB */
	} bits;
};

extern struct kset *devices_kset;
struct platform_device *get_mci_device(unsigned int index);
#endif
