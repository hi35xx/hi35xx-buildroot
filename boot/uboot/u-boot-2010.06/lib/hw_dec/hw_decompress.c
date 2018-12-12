#include <common.h>
#include <asm/io.h>
#include <cmd_bootss.h>

#include "hw_decompress.h"

#if (defined CONFIG_HI3559 || defined CONFIG_HI3556)
#include "hw_decompress_hi3559.c"
#endif

#if (defined CONFIG_HI3516CV300)
#include "hw_decompress_hi3516cv300.c"
#endif

unsigned int hw_dec_type;
unsigned int hw_dec_sop;
unsigned int hw_dec_eop;
unsigned int hw_dec_cur_blk;
#if (defined CONFIG_HI3516CV300 || defined CONFIG_HI3519 || \
		defined CONFIG_HI3519V101 || defined CONFIG_HI3559 || defined CONFIG_HI3556 || \
		defined CONFIG_HI3516AV200)
unsigned int hw_blk_total_num;
#endif


void hw_dec_sop_eop_first_set(int block_num)
{
	if (block_num == 1) {
		hw_dec_sop = 1;
		hw_dec_eop = 1;
	} else {
		hw_dec_sop = 1;
		hw_dec_eop = 0;
	}

	hw_dec_cur_blk = 0;
#if (defined CONFIG_HI3516CV300 || defined CONFIG_HI3519 || \
		defined CONFIG_HI3519V101 || defined CONFIG_HI3559 || defined CONFIG_HI3556 || \
		defined CONFIG_HI3516AV200)
	hw_blk_total_num = block_num;
#endif
}

static inline void hw_dec_work_en_set(int work_en_flg)
{
	/* Enable the emar*/
	writel(work_en_flg, HW_DEC_REG_BASE_ADDR + EAMR_WORK_EN_REG_OFST);
}

#ifdef CONFIG_HI3518EV200
static inline void hw_dec_buf0_addr_set(unsigned int addr)
{
	/* Set the buffer base addr */
	writel(addr, HW_DEC_REG_BASE_ADDR + CPRS_DARA_BUF0_BADDR);
}
#endif

static inline void hw_dec_rtn_baddr_set(unsigned int addr)
{
	writel(addr, HW_DEC_REG_BASE_ADDR + DPRS_DATA_RTN_BADDR);
}

static inline void hw_dec_dprs_data_baddr_set(unsigned int addr)
{
	writel(addr, HW_DEC_REG_BASE_ADDR + DPRS_DATA_INFO_BADDR);
}

static inline void hw_dec_data_rtn_len_set(unsigned int len)
{
	writel(len, HW_DEC_REG_BASE_ADDR + DPRS_DATA_RTN_LEN);
}

static inline void hw_dec_dprs_data_len_set(unsigned int len)
{
	writel(len, HW_DEC_REG_BASE_ADDR + DPRS_DATA_INFO_LEN);
}

static inline void hw_dec_crc_check_en(unsigned int crc_en)
{
	writel(crc_en, HW_DEC_REG_BASE_ADDR + CRC_CHECK_EN);
}

static inline void hw_dec_data_crc32_set(unsigned int crc32)
{
	writel(crc32, HW_DEC_REG_BASE_ADDR + DPRS_DATA_CRC32);
}

#ifdef CONFIG_HI3518EV200
void hw_dec_data_buf_info_set(unsigned int mode,
		unsigned int b_sop,
		unsigned int b_eop,
		unsigned int buf_id,
		unsigned int buf_len)
{
	U_DPRS_DATA_BUF_INFO buf_info;

	buf_info.u32 = readl(HW_DEC_REG_BASE_ADDR + DPRS_DATA_BUF_INFO);
	buf_info.bits.mode = mode;
	buf_info.bits.sop = b_sop;
	buf_info.bits.eop = b_eop;
	buf_info.bits.buf_id = buf_id;
	buf_info.bits.buf_len = buf_len;
	writel(buf_info.u32, HW_DEC_REG_BASE_ADDR + DPRS_DATA_BUF_INFO);
}
#endif

static inline unsigned int hw_dec_buf_status_get(void)
{
	return readl(HW_DEC_REG_BASE_ADDR + BUF_INFO);
}

static inline unsigned int hw_dec_dprs_rtn_status_get(void)
{
	return readl(HW_DEC_REG_BASE_ADDR + DPRS_RTN_INFO);
}

static inline void hw_dec_buf_status_clr(void)
{
	writel(0x1, HW_DEC_REG_BASE_ADDR + BUF_INFO_CLR);
}

static inline void hw_dec_dprs_rtn_status_clr(void)
{
	writel(0x1, HW_DEC_REG_BASE_ADDR + RLT_INFO_CLR);
}

static inline void hw_dec_intr_en_set(int blk_intr_en, int task_intr_en)
{
	U_INTR_EN intr_en;
	intr_en.bits.task_intrpt_en = task_intr_en;
	intr_en.bits.block_intrpt_en = blk_intr_en;
	writel(intr_en.u32, HW_DEC_REG_BASE_ADDR + INT_EN_REG_ADDR);
}

static inline unsigned int hw_dec_intr_status_get(void)
{
	return readl(HW_DEC_REG_BASE_ADDR + INT_STATUS_REG_ADDR);
}

static inline void hw_dec_block_intr_status_clr(void)
{
	U_INTR_CLR intr_clr;

	intr_clr.u32 = readl(HW_DEC_REG_BASE_ADDR + INT_CLEAR_REG_ADDR);
	intr_clr.bits.block_intrpt_clr = 0x1;
	writel(intr_clr.u32, HW_DEC_REG_BASE_ADDR + INT_CLEAR_REG_ADDR);
}

static inline void hw_dec_task_intr_status_clr(void)
{
	U_INTR_CLR intr_clr;

	intr_clr.u32 = readl(HW_DEC_REG_BASE_ADDR + INT_CLEAR_REG_ADDR);
	intr_clr.bits.task_intrpt_clr = 0x1;
	writel(intr_clr.u32, HW_DEC_REG_BASE_ADDR + INT_CLEAR_REG_ADDR);
}

int hw_dec_intr_proc(int irq, void *para)
{
	U_BUF_STATUS buf_status;
	U_INTR_STATUS intr_status;
	U_DPRS_RTN_STATUS dprs_status;
    int ret = 0;

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
				printf("err = 0x%x, dec_data_len = 0x%x\n",
					dprs_status.bits.err_info,
#ifdef CONFIG_HI3518EV200
					dprs_status.bits.rsv);
#else
					readl(HW_DEC_REG_BASE_ADDR
						+ DPRS_RTN_LEN));
#endif
                    ret = -2;
			}

			hw_dec_dprs_rtn_status_clr();
		}

		hw_dec_task_intr_status_clr();
        goto out;
	}

    ret = -1;
out:
	return ret;
}

#ifdef CONFIG_HI3518EV200
void hw_dec_start(unsigned int src_baddr,
		unsigned int dst_baddr,
		unsigned int dst_len,
		unsigned int crc_en,
		unsigned int crc32,
		unsigned int dec_type)
{
	hw_dec_buf0_addr_set(src_baddr);
	if (!dec_type) {
		hw_dec_rtn_baddr_set(dst_baddr);
		hw_dec_data_rtn_len_set(dst_len);
	} else {
		hw_dec_dprs_data_baddr_set(dst_baddr);
		hw_dec_dprs_data_len_set(PAGE_NR(dst_len) * 4);
	}

	hw_dec_crc_check_en(crc_en);
	hw_dec_data_crc32_set(crc32);
}
#endif

#if (defined CONFIG_HI3516CV300 || defined CONFIG_HI3519 || \
		defined CONFIG_HI3519V101 || defined CONFIG_HI3559 || defined CONFIG_HI3556 || \
		defined CONFIG_HI3516AV200)
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
	writel(src_baddr, HW_DEC_REG_BASE_ADDR + DPRS_DATA_SRC_BADDR);

	val = src_len | (hw_dec_sop << 28)
		| (hw_dec_eop << 29) | (!dec_type << 31);
	writel(val, HW_DEC_REG_BASE_ADDR + DPRS_DATA_SRC_LEN);

	hw_dec_crc_check_en(crc_en);
}
#endif

int hw_dec_wait_finish(void)
{
	int ret;
	int times = 0;

	do {
		ret = hw_dec_intr_proc(HW_DEC_INTR, NULL);
		times++;
		if (times > 100000) {
			printf("hardware decompress overtime!"
					" func:%s, line:%d\n",
					__func__, __LINE__);
			break;
		}

		udelay(1000);
	} while (-1 == ret);

	return ret;
}

#ifdef CONFIG_HI3518EV200
int hw_dec_decompress(unsigned char *dst, int *dstlen,
		unsigned char *src, int srclen,
		void *unused)
{
	int ret;
	int mode;

	hw_dec_sop_eop_first_set(1);
	hw_dec_start((unsigned int)src, (unsigned int)dst,
			*dstlen, 0, 0, hw_dec_type);

	if (hw_dec_type)
		mode = 0;
	else
		mode = 1;

	hw_dec_data_buf_info_set(mode, hw_dec_sop, hw_dec_eop, 0, srclen);
	ret = hw_dec_wait_finish();

	if (ret)
		return -1;

	return 0;
}
#endif

#if (defined CONFIG_HI3516CV300 || defined CONFIG_HI3519 || \
		defined CONFIG_HI3519V101 || defined CONFIG_HI3559 || defined CONFIG_HI3556 || \
		defined CONFIG_HI3516AV200)
int hw_dec_decompress(unsigned char *dst, int *dstlen,
			unsigned char *src, int srclen,
			void *unused)
{
	int ret;
#if (defined CONFIG_HI3559 || defined CONFIG_HI3556 || defined CONFIG_HI3516CV300)
    printf("dst=0x%08x dstlen=0x%08x,src=0x%0x srclen=0x%08x\n",(int)dst,*dstlen,(int)src,srclen);
#endif
	hw_dec_sop_eop_first_set(1);
	hw_dec_start((unsigned int)src, (unsigned int)dst,
			srclen,	*dstlen, 0, 0, hw_dec_type);

	ret = hw_dec_wait_finish();

    *dstlen = readl(HW_DEC_REG_BASE_ADDR + DPRS_RTN_LEN);

	if (ret)
		return -1;

	return 0;
}
#endif

void hw_dec_init(void)
{
#if (defined CONFIG_HI3559 || defined CONFIG_HI3556 || defined CONFIG_HI3516CV300)
    /* enable decompress clock */
    enable_decompress_clock();
#endif
	/* Init the emar interface */
	writel(0, HW_DEC_REG_BASE_ADDR + EAMR_RID_REG_OFST);
	writel(0x3, HW_DEC_REG_BASE_ADDR + EAMR_ROSD_REG_OFST);
	writel(0, HW_DEC_REG_BASE_ADDR + EAMR_WID_REG_OFST);
	writel(0x3, HW_DEC_REG_BASE_ADDR + EAMR_WOSD_REG_OFST);

	/*Enable interrupt*/
	hw_dec_intr_en_set(0x1, 0x1);

	/* Enable emar*/
	hw_dec_work_en_set(0x1);
}

void hw_dec_uinit(void)
{
	hw_dec_work_en_set(0x0);
	hw_dec_intr_en_set(0x0, 0x0);

#if (defined CONFIG_HI3559 || defined CONFIG_HI3556 || defined CONFIG_HI3516CV300)
    /* disable decompress clock */
    disable_decompress_clock();
#endif
}


