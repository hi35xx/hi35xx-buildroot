/******************************************************************************
 *    COPYRIGHT (C) 2013 Hisilicon
 *    All rights reserved.
 * ***
 *    Create 2013-08-23
 *
 ******************************************************************************/
#ifndef __DMAC_H__
#define __DMAC_H__

#define DMAC_ERROR_BASE		100
#define DMAC_CHN_SUCCESS	(DMAC_ERROR_BASE+0x10)

#ifdef CONFIG_HI_DMAC
extern int dma_driver_init(void);
extern int dmac_channelclose(unsigned int channel);
extern int dmac_channelstart(unsigned int u32channel);
extern int dmac_channel_allocate(void *pisr);

extern int dmac_start_m2p(unsigned int channel, unsigned int pmemaddr,
			unsigned int uwperipheralid,
			unsigned int uwnumtransfers,
			unsigned int next_lli_addr);
extern int dmac_m2p_transfer(unsigned int memaddr,
			unsigned int uwperipheralid, unsigned int length);
extern int dmac_channel_free(unsigned int channel);
extern int do_dma_m2p(unsigned int mem_addr, unsigned int peripheral_addr,
		unsigned int length);
extern int do_dma_p2m(unsigned int mem_addr, unsigned int peripheral_addr,
		unsigned int length);
extern int dmac_wait(int channel);
extern int dmac_start_m2m(unsigned int channel, unsigned int psource,
			unsigned int pdest, unsigned int uwnumtransfers);
extern int dmac_m2m_transfer(unsigned int source,
			unsigned int dest, unsigned int length);
extern int dmac_register_isr(unsigned int channel, void *pisr);
extern int free_dmalli_space(unsigned int *ppheadlli, unsigned int page_num);
extern int dmac_start_llim2p(unsigned int channel, unsigned int *pfirst_lli,
				unsigned int uwperipheralid);
extern int dmac_buildllim2m(unsigned int *ppheadlli, unsigned int pdest,
				unsigned int psource,
				unsigned int totaltransfersize,
				unsigned int uwnumtransfers);
extern int dmac_start_llim2m(unsigned int channel, unsigned int *pfirst_lli);
extern int allocate_dmalli_space(unsigned int *ppheadlli,
					unsigned int page_num);

extern int do_dma_llim2m_isp(unsigned int *source,
		unsigned int *dest,
		unsigned int *length,
		unsigned int num);

#else /* !CONFIG_HI_DMAC */
static inline int dma_driver_init(void) { return 0; }
static inline int dmac_channelclose(unsigned int channel) { return 0; }
static inline int dmac_channelstart(unsigned int u32channel) { return 0; }
static inline int dmac_channel_allocate(void *pisr) { return 0; }

static inline int dmac_start_m2p(unsigned int channel, unsigned int pmemaddr,
		unsigned int uwperipheralid,
		unsigned int uwnumtransfers,
		unsigned int next_lli_addr)
{ return 0; }

static inline int dmac_m2p_transfer(unsigned int memaddr,
		unsigned int uwperipheralid, unsigned int length)
{ return 0; }

static inline int dmac_channel_free(unsigned int channel) { return 0; }

int do_dma_m2p(unsigned int mem_addr, unsigned int peripheral_addr,
		unsigned int length)
{ return 0; }

static inline int do_dma_p2m(unsigned int mem_addr,
		unsigned int peripheral_addr,
		unsigned int length)
{ return 0; }

static inline int dmac_wait(int channel) { return 0; }

static inline int dmac_start_m2m(unsigned int channel, unsigned int psource,
		unsigned int pdest, unsigned int uwnumtransfers)
{ return 0; }

static inline int dmac_m2m_transfer(unsigned int source,
		unsigned int dest, unsigned int length)
{ return 0; }

static inline int dmac_register_isr(unsigned int channel, void *pisr)
{ return 0; }

static inline  int free_dmalli_space(unsigned int *ppheadlli,
		unsigned int page_num)
{ return 0; }

static inline  int dmac_start_llim2p(unsigned int channel,
		unsigned int *pfirst_lli,
		unsigned int uwperipheralid)
{ return 0; }

static inline int dmac_buildllim2m(unsigned int *ppheadlli, unsigned int pdest,
		unsigned int psource,
		unsigned int totaltransfersize,
		unsigned int uwnumtransfers)
{ return 0; }

static inline int dmac_start_llim2m(unsigned int channel,
		unsigned int *pfirst_lli)
{ return 0; }

static inline int allocate_dmalli_space(unsigned int *ppheadlli,
		unsigned int page_num)
{ return 0; }

static inline int do_dma_llim2m_isp(unsigned int *source,
		unsigned int *dest,
		unsigned int *length,
		unsigned int num)
{ return 0; }
#endif /* CONFIG_HI_DMAC */

/*structure for LLI*/
typedef struct dmac_lli {
	/*source address*/
	unsigned int src_addr;
	/*destination address*/
	unsigned int dst_addr;
	/*pointer to next LLI*/
	unsigned int next_lli;
	/*control word*/
	unsigned int lli_transfer_ctrl;
} dmac_lli;

#endif
