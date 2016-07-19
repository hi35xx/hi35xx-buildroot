#include <common.h>
#include <asm/io.h>
#include <cmd_bootss.h>

#include "hw_decompress.h"

unsigned int hw_dec_type;
unsigned int hw_dec_sop;
unsigned int hw_dec_eop;
unsigned int hw_dec_cur_blk;

void hw_dec_sop_eop_reset(int blk_total_num)
{
	hw_dec_cur_blk++;
	if (blk_total_num - hw_dec_cur_blk > 1) {
		hw_dec_sop = 0;
		hw_dec_eop = 0;
	} else {
		hw_dec_sop = 0;
		hw_dec_eop = 1;
	}
}

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
}

static inline void hw_dec_work_en_set(int work_en_flg)
{
	/* Enable the emar*/
	writel(work_en_flg, HW_DEC_REG_BASE_ADDR + EAMR_WORK_EN_REG_OFST);
}

static inline void hw_dec_buf0_addr_set(unsigned int addr)
{
	/* Set the buffer base addr */
	writel(addr, HW_DEC_REG_BASE_ADDR + CPRS_DARA_BUF0_BADDR);
}

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
						dprs_status.bits.rsv);
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
			times = 0;
			break;
		}

		udelay(1000);
	} while (-1 == ret);

	return ret;
}

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

void hw_dec_init(void)
{
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
}


