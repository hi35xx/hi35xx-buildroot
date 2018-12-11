#include"hw_decompress.h"
#if defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556)
#include"hw_decompress_hi3559.h"
#endif

static inline void udelay(unsigned long loops)
{
	__asm__ volatile ("1:\n"
			"subs %0, %1, #1\n"
			"bne 1b" : "=r" (loops) : "0" (loops));
}

unsigned int hw_dec_type = 0;
unsigned int hw_dec_sop = 0;
unsigned int hw_dec_eop = 0;
unsigned int hw_dec_cur_blk = 0;
unsigned int hw_blk_total_num = 0;

void hw_dec_sop_eop_first_set(int block_num)
{
	if (block_num == 1) {
		hw_dec_sop = 1;
		hw_dec_eop = 1;
	} else {
		hw_dec_sop = 1;
		hw_dec_eop = 0;
	}

	hw_dec_type = 0;
	hw_dec_cur_blk = 0;
	hw_blk_total_num = block_num;
}

static inline void hw_dec_work_en_set(unsigned int work_en_flg)
{
	/* Enable the emar*/
	writel(work_en_flg, (void *)(HW_DEC_REG_BASE_ADDR + EAMR_WORK_EN_REG_OFST));
}

static inline void hw_dec_rtn_baddr_set(unsigned int addr)
{
	writel(addr, (void *)(HW_DEC_REG_BASE_ADDR + DPRS_DATA_RTN_BADDR));
}

static inline void hw_dec_dprs_data_baddr_set(unsigned int addr)
{
	writel(addr, (void *)(HW_DEC_REG_BASE_ADDR + DPRS_DATA_INFO_BADDR));
}

static inline void hw_dec_data_rtn_len_set(unsigned int len)
{
	writel(len, (void *)(HW_DEC_REG_BASE_ADDR + DPRS_DATA_RTN_LEN));
}

static inline void hw_dec_dprs_data_len_set(unsigned int len)
{
	writel(len, (void *)(HW_DEC_REG_BASE_ADDR + DPRS_DATA_INFO_LEN));
}

static inline void hw_dec_crc_check_en(unsigned int crc_en)
{
	writel(crc_en, (void *)(HW_DEC_REG_BASE_ADDR + CRC_CHECK_EN));
}

static inline void hw_dec_data_crc32_set(unsigned int crc32)
{
	writel(crc32, (void *)(HW_DEC_REG_BASE_ADDR + DPRS_DATA_CRC32));
}

static inline unsigned int hw_dec_buf_status_get(void)
{
	return readl((void *)(HW_DEC_REG_BASE_ADDR + BUF_INFO));
}

static inline unsigned int hw_dec_dprs_rtn_status_get(void)
{
	return readl((void *)(HW_DEC_REG_BASE_ADDR + DPRS_RTN_INFO));
}

static inline void hw_dec_buf_status_clr(void)
{
	writel(0x1, (void *)(HW_DEC_REG_BASE_ADDR + BUF_INFO_CLR));
}

static inline void hw_dec_dprs_rtn_status_clr(void)
{
	writel(0x1, (void *)(HW_DEC_REG_BASE_ADDR + RLT_INFO_CLR));
}

static inline void hw_dec_intr_en_set(int blk_intr_en, int task_intr_en)
{
	U_INTR_EN intr_en;
	intr_en.bits.task_intrpt_en = task_intr_en;
	intr_en.bits.block_intrpt_en = blk_intr_en;
	writel(intr_en.u32, (void *)(HW_DEC_REG_BASE_ADDR + INT_EN_REG_ADDR));
}

static inline unsigned int hw_dec_intr_status_get(void)
{
	return readl((void *)(HW_DEC_REG_BASE_ADDR + INT_STATUS_REG_ADDR));
}

static inline void hw_dec_block_intr_status_clr(void)
{
	U_INTR_CLR intr_clr;

	intr_clr.u32 = readl((void *)(HW_DEC_REG_BASE_ADDR + INT_CLEAR_REG_ADDR));
	intr_clr.bits.block_intrpt_clr = 0x1;
	writel(intr_clr.u32, (void *)(HW_DEC_REG_BASE_ADDR + INT_CLEAR_REG_ADDR));
}

static inline void hw_dec_task_intr_status_clr(void)
{
	U_INTR_CLR intr_clr;

	intr_clr.u32 = readl((void *)(HW_DEC_REG_BASE_ADDR + INT_CLEAR_REG_ADDR));
	intr_clr.bits.task_intrpt_clr = 0x1;
	writel(intr_clr.u32, (void *)(HW_DEC_REG_BASE_ADDR + INT_CLEAR_REG_ADDR));
}

int hw_dec_intr_proc(int irq, void *para)
{
	U_BUF_STATUS buf_status;
	U_INTR_STATUS intr_status;
	U_DPRS_RTN_STATUS dprs_status;

	intr_status.u32 = hw_dec_intr_status_get();
	if (intr_status.bits.block_intrpt) {
		buf_status.u32 = hw_dec_buf_status_get();
		if (buf_status.bits.aval_flg)
			hw_dec_buf_status_clr();

		hw_dec_block_intr_status_clr();
	}

	if (intr_status.bits.task_intrpt) {
		dprs_status.u32 = hw_dec_dprs_rtn_status_get();
		if (dprs_status.bits.aval_flg) {
			if (dprs_status.bits.err_info) {
				// printk("err = 0x%x, dec_data_len = 0x%x\n",
					// dprs_status.bits.err_info,
					// readl(HW_DEC_REG_BASE_ADDR
						// + DPRS_RTN_LEN));
			}
			hw_dec_dprs_rtn_status_clr();
		}

		hw_dec_task_intr_status_clr();
		return 0;
	}

	return -1;
}

void hw_dec_start(unsigned int src_baddr,
			unsigned int dst_baddr,
			unsigned int src_len,
			unsigned int dst_len,
			unsigned int crc_en,
			unsigned int crc32,
			unsigned int dec_type)
{
	unsigned int val;

	if (hw_dec_sop) {
		if (!dec_type) {
			/* set the parameters of output buffer */
			hw_dec_rtn_baddr_set(dst_baddr);
			hw_dec_data_rtn_len_set(dst_len);
		} else {
			/* set the parameter of output buffer */
			hw_dec_dprs_data_baddr_set(dst_baddr);
			hw_dec_dprs_data_len_set(PAGE_NR(dst_len) * 4);
		}
	}

	/* set the parameter of input buffer */
	writel(src_baddr, (void *)(HW_DEC_REG_BASE_ADDR + DPRS_DATA_SRC_BADDR));

	val = src_len | (hw_dec_sop << 28)
		| (hw_dec_eop << 29) | (!dec_type << 31);
	writel(val, (void *)(HW_DEC_REG_BASE_ADDR + DPRS_DATA_SRC_LEN));

	hw_dec_crc_check_en(crc_en);
}

int hw_dec_wait_finish(void)
{
	int ret;
	int times = 0;

	do {
		ret = hw_dec_intr_proc(HW_DEC_INTR, NULL);
		times++;
		if (times > 100000) {
			// printk("hardware decompress overtime!"
					// " func:%s, line:%d\n",
					// __func__, __LINE__);
			times = 0;
			break;
		}

		udelay(1000);
	} while (-1 == ret);

	return ret;
}

void hw_gzip_init(void)
{
	enable_decompress_clock();
	/* Init the emar interface */
	writel(0, (void *)(HW_DEC_REG_BASE_ADDR + EAMR_RID_REG_OFST));
	writel(0x3, (void *)(HW_DEC_REG_BASE_ADDR + EAMR_ROSD_REG_OFST));
	writel(0, (void *)(HW_DEC_REG_BASE_ADDR + EAMR_WID_REG_OFST));
	writel(0x3, (void *)(HW_DEC_REG_BASE_ADDR + EAMR_WOSD_REG_OFST));

	/*Enable interrupt*/
	hw_dec_intr_en_set(0x1, 0x1);

	/* Enable emar*/
	hw_dec_work_en_set(0x1);
}

int hw_gzip_dec(unsigned int dst, int *dstlen, unsigned int src, int srclen)
{
	int ret;

	hw_dec_sop_eop_first_set(1);

	hw_dec_start(src, dst, srclen, *dstlen, 0, 0, hw_dec_type);

	ret = hw_dec_wait_finish();
	if (ret)
		return -1;

    //*dstlen = readl(HW_DEC_REG_BASE_ADDR + DPRS_RTN_LEN);

	return 0;
}

void hw_gzip_exit(void)
{
	/*Disable emar*/
	hw_dec_work_en_set(0x0);

	/*Disable interrupt*/
	hw_dec_intr_en_set(0x0, 0x0);
	disable_decompress_clock();
}


int do_decompress(u8 *input, int len, u8 *output, void (*error)(char *x))
{
    unsigned int src_pa = (unsigned long)input;
    unsigned int dst_pa = (unsigned long)output;
    int dstlen = 0x800000;

    hw_gzip_init();

    hw_gzip_dec(dst_pa, &dstlen, src_pa, len);

    hw_gzip_exit();

    return 0;
}
