/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/DWC_usb3/driver/pcd.h $
 * $Revision: #71 $
 * $Date: 2014/11/11 $
 * $Change: 2664766 $
 *
 * Synopsys SS USB3 Linux Software Driver and documentation (hereinafter,
 * "Software") is an Unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================== */
#ifndef _DWC_PCD_H_
#define _DWC_PCD_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 *
 * This file contains the structures, constants, and interfaces for
 * the Perpherial Contoller Driver (PCD).
 *
 * The Peripheral Controller Driver (PCD) for Linux will implement the
 * Gadget API, so that the existing Gadget drivers can be used. For
 * the Mass Storage function, the File-backed USB Storage Gadget (FBS)
 * driver will be used. The FBS driver supports the Control-Bulk (CB),
 * Control-Bulk-Interrupt (CBI), and Bulk-Only transports.
 *
 */

/** Maximum number of Tx FIFOs. Depends on the RTL configuration. No way to
 * probe the value at runtime
 */
#define DWC_MAX_TX_FIFOS	16

/** Maximum number of physical EPs. Depends on the RTL configuration. No way to
 * probe the value at runtime
 */
#define DWC_MAX_PHYS_EP		32

/** Maximum number of data buffers per TRB. OS/application specific */
#define DWC_MAX_DATA_BUFS	13

/** Maximum number of EPs, defined by USB spec */
#define DWC_MAX_EPS		16

/** Maxpacket size for EP0, defined by USB3 spec */
#define DWC_MAX_EP0_SIZE	512

/** Maxpacket size for any EP, defined by USB3 spec */
#define DWC_MAX_PACKET_SIZE	1024

/**
 * States of EP0
 */
typedef enum ep0_state {
	EP0_IDLE,
	EP0_IN_DATA_PHASE,
	EP0_OUT_DATA_PHASE,
	EP0_IN_WAIT_GADGET,
	EP0_OUT_WAIT_GADGET,
	EP0_IN_WAIT_NRDY,
	EP0_OUT_WAIT_NRDY,
	EP0_IN_STATUS_PHASE,
	EP0_OUT_STATUS_PHASE,
	EP0_STALL,
} ep0state_e;

typedef enum pcd_state {
	DWC_STATE_UNCONNECTED,	/* no host */
	DWC_STATE_DEFAULT,
	DWC_STATE_ADDRESSED,
	DWC_STATE_CONFIGURED,
} pcdstate_e;

struct dwc_usb3_device;
struct dwc_usb3_pcd;

/**
 * The <code>dwc_req</code> structure represents the state of a single
 * transfer request when acting in device mode. It contains the data items
 * needed for a request to be started and completed.
 */
typedef struct dwc_req {
	dwc_usb3_dma_desc_t *trb;	/**< TRB or TRB chain for this req */
	dwc_dma_t trbdma;		/**< DMA address of TRB or TRB chain */
	u32 length;			/**< total length of data bufs */
	u32 actual;			/**< actual amt of data transferred */
	u32 stream;			/**< stream # of this request */

	int flags;	/**< request flags - bits 8-31 are OS-specific */
#define DWC_PCD_REQ_ZERO	0x001
#define DWC_PCD_REQ_STARTED	0x002
#define DWC_PCD_REQ_MAP_DMA	0x100
#define DWC_PCD_REQ_IN		0x200

	int numbuf;				/**< number of data bufs */
	char *buf[DWC_MAX_DATA_BUFS];		/**< data buffers */
	dwc_dma_t bufdma[DWC_MAX_DATA_BUFS];	/**< DMA addrs of data bufs */
	u32 buflen[DWC_MAX_DATA_BUFS];		/**< length of data bufs */
} dwc_req_t;

/** DWC_usb3 request structure.
 * This structure is used to form a list of requests.
 */
typedef struct dwc_usb3_pcd_req {
	/** DWC_usb3 request data */
	dwc_req_t dwc_req;

	/* ==== The rest is OS-specific ==== */

	/** List entry for EP queue */
#if defined(__linux__) || defined(SELA_PLATFORM)
	struct list_head entry;
#else
	DWC_CIRCLEQ_ENTRY(dwc_usb3_pcd_req) entry;
#endif
	/** USB request */
	struct usb_request usb_req;
} dwc_usb3_pcd_req_t;

/**
 * The <code>dwc_ep</code> structure represents the state of a single EP
 * when acting in device mode. It contains the data items needed for an EP
 * to be activated and transfer packets.
 */
typedef struct dwc_ep {
	/** Pointer to PCD */
	struct dwc_usb3_pcd *pcd;

	/** Pointer to OUT EP register */
	dwc_usb3_dev_ep_regs_t __iomem *out_ep_reg;

	/** Pointer to IN EP register */
	dwc_usb3_dev_ep_regs_t __iomem *in_ep_reg;

	/** Physical EP number */
	u8 phys;

	/** USB EP number */
	u8 num;

	/** EP type: 0 - Control, 1 - ISOC, 2 - BULK, 3 - INTR */
	u8 type;

	/** 'bInterval' value for Isoc EPs */
	u8 intvl;

	/** Max Packet bytes */
	u16 maxpacket;

	/** 'mult' value for SS Isoc EPs */
	u8 mult;

	/** Max burst size for SS EPs (0 - 15, actual burst is 1 - 16) */
	u8 maxburst;

	/** Number of streams for SS Bulk EPs (0 - 16, actual number is 2^n) */
	u8 num_streams;

	/** Tx FIFO # for IN EPs */
	u8 tx_fifo_num;

	/** @{ */
	/** Transfer Resource Index from the Start Transfer command */
	u8 tri_out;
	u8 tri_in;
	/** @} */

	/** @{ */
	/** Status of the queue */
	unsigned int stopped		: 1;
	unsigned int disabling		: 1;
	unsigned int queue_sof		: 1;
	/** @} */

	/** @{ */
	/** Send ZLP */
	unsigned int send_zlp		: 1;

	/** Stall clear flag */
	unsigned int stall_clear_flag	: 1;

	/** True if 3-stage control transfer */
	unsigned int three_stage	: 1;

	/** True if transfer has been started on EP */
	unsigned int xfer_started	: 1;
	/** @} */

	/** EP direction 0 = OUT */
	unsigned int is_in		: 1;

	/** EP active */
	unsigned int active		: 1;

	/** True if TRB array has a link TRB at the end */
	unsigned int desc_link		: 1;

	/** DATA start PID for INTR and BULK EP */
	unsigned int data_pid_start	: 1;

	/** EP has been enabled for this configuration */
	unsigned int ena_once		: 1;

	/** EP was in stalled state when entering hibernation */
	unsigned int stalled_save	: 1;

	/** @{ */
	/** Saved parameters from the last DEPCFG for this EP. Used when
	 * resetting the sequence number
	 */
	u32 param0out;
	u32 param1out;
	u32 param0in;
	u32 param1in;
	/** @} */

	/** EP state, saved across core hibernation */
	u32 save_state;

	/** Pointer to USB EP descriptor */
	const usb_endpoint_descriptor_t *usb_ep_desc;

	/** @{ */
	/** Array of DMA descriptors (TRBs) for this EP */
	dwc_usb3_dma_desc_t *dma_desc;
	dwc_dma_t dma_desc_dma;
	int desc_size;
	int num_desc;
	/** @} */

	/** Number of DMA descriptors available */
	int desc_avail;

	/** Index to next free DMA descriptor in array */
	int desc_idx;

	/** Index to DMA descriptor that was active before hibernation */
	int hiber_desc_idx;

	/* ==== The rest is OS-specific ==== */

	/** Condition variable for EPCMD_CMPL interrupt */
	u32 condition;

	/** Queue of dwc_usb3_pcd_reqs */
#if defined(__linux__) || defined(SELA_PLATFORM)
	struct list_head queue;
#else
	DWC_CIRCLEQ_HEAD(circleq, dwc_usb3_pcd_req) queue;
#endif
} dwc_ep_t;

/** PCD EP structure.
 * This structure describes an EP, there is an array of EP pointers in the
 * PCD structure.
 */
typedef struct dwc_usb3_pcd_ep {
	/** DWC_usb3 EP data */
	dwc_ep_t dwc_ep;

	/* ==== The rest is OS-specific ==== */

	/** USB EP */
	struct usb_ep usb_ep;
} dwc_usb3_pcd_ep_t;

/** @{ */
/** PCD EP accessor functions */
#define dwc_usb3_pcd_ep_to_pcd(pcd_ep)	((pcd_ep)->dwc_ep.pcd)
#define dwc_usb3_pcd_ep_num(pcd_ep)	((pcd_ep)->dwc_ep.num)
#define dwc_usb3_pcd_ep_type(pcd_ep)	((pcd_ep)->dwc_ep.type)
#define dwc_usb3_pcd_ep_is_in(pcd_ep)	((pcd_ep)->dwc_ep.is_in)
/** @} */

struct dwc_hiber_scratchpad_array {
	u64	dma_addr[15];
};

/** DWC_usb3 PCD Structure.
 * This structure encapsulates the data for the dwc_usb3 PCD.
 */
typedef struct dwc_usb3_pcd {
	/** The DWC otg device pointer */
	struct dwc_usb3_device *usb3_dev;

	/** USB3 link state */
	int link_state;

	/** State of the device */
	pcdstate_e state;

	/** State of EP0 */
	ep0state_e ep0state;

	/** EP0 Status Request is pending */
	unsigned int ep0_status_pending		: 1;

	/** Indicates when SET CONFIGURATION Request is in process */
	unsigned int request_config		: 1;

	/** State of the Remote Wakeup Enable */
	unsigned int remote_wakeup_enable	: 1;

	/** State of the Latency Tolerance Messaging Enable */
	unsigned int ltm_enable			: 1;

	/** True if we should send an LPM notification after the status stage */
	unsigned int send_lpm			: 1;

	/** True if ready for remote wakeup request from user */
	unsigned int wkup_rdy			: 1;

	/** True if we have enabled some EPs */
	unsigned int eps_enabled		: 1;

	/** True if UTE has made some config changes */
	unsigned int ute_change			: 1;

#ifdef DWC_STAR_9000463548_WORKAROUND
	unsigned int configuring		: 1;
#endif

#ifdef CONFIG_USB_OTG_DWC
	/** Set when user writes to 'hnp' sysfs attribute */
	unsigned int wants_host			: 1;

	/** For set feature (b_hnp_enable) */
	unsigned int b_hnp_enable		: 1;
#endif
	/** EP0 */
	dwc_usb3_pcd_ep_t *ep0;

	/** Array of OUT EPs (not including EP0) */
	dwc_usb3_pcd_ep_t *out_ep[DWC_MAX_EPS - 1];

	/** Array of IN EPs (not including EP0) */
	dwc_usb3_pcd_ep_t *in_ep[DWC_MAX_EPS - 1];

	/** Pointer to device Global registers.
	 * Device Global Registers starting at offset 700h
	 */
	dwc_usb3_dev_global_regs_t __iomem *dev_global_regs;

	/** Device Logical OUT EP-Specific Registers 800h-9ECh */
	dwc_usb3_dev_ep_regs_t __iomem *out_ep_regs;

	/** Device Logical IN EP-Specific Registers 810h-9FCh */
	dwc_usb3_dev_ep_regs_t __iomem *in_ep_regs;

	/** @{ */
	/** Scratchpad buffers for hibernation support */
	void *hiber_scratchpad[15];
	struct dwc_hiber_scratchpad_array *hiber_scratchpad_array;
	dwc_dma_t hiber_scratchpad_array_dma;
	/** @} */

	/** @{ */
	/** EP0 state, saved across core hibernation */
	u32 ep0_out_save_state;
	u32 ep0_in_save_state;
	/** @} */

	/** 'dummy' request, for EP0 only */
	dwc_usb3_pcd_req_t *ep0_req;

#ifdef DWC_UTE
	/** size of Rx FIFO, requested by UTE */
	unsigned rxf_size;

	/** size of Tx FIFOs, requested by UTE */
	unsigned txf_size[DWC_MAX_TX_FIFOS];

	/** mapping of Tx FIFO for each physical EP, requested by UTE */
	unsigned txf_map[DWC_MAX_PHYS_EP];

	/** Rx FIFO default size */
	unsigned def_rxf_size;

	/** Tx FIFOs, default size */
	unsigned def_txf_size[DWC_MAX_TX_FIFOS];
#endif

	/** @{ */
	/** Thresholding enable flags and length variables */
	u16 rx_thr_en;
	u16 iso_tx_thr_en;
	u16 non_iso_tx_thr_en;
	u16 rx_thr_length;
	u16 tx_thr_length;
	/** @} */

	/** Device configuration information */
	u8 speed;	/**< Device Speed - 0:Unk 1:LS 2:FS 3:HS 4:Var 5:SS */
	u8 num_out_eps;	/**< Number # of Rx EP range: 0-15 except ep0 */
	u8 num_in_eps;	/**< Number # of Tx EP range: 0-15 except ep0 */

	/** The TEST mode to enter when test_mode_tasklet is executed */
	u8 test_mode;

	/** Count of pending Requests */
	unsigned request_pending;

	/* ==== The rest is OS-specific ==== */
#ifdef SELA_PLATFORM
	/** Limit the number of EPs to speed up simulation */
	int max_in_eps, max_out_eps;

	/** Pointers for passing event codes back to the test code */
	int *dev_event, *ep_event;
#endif
#if defined(__linux__) || defined(LINUXTEST)
	/** The spinlock for the PCD */
	spinlock_t lock;
#endif
#ifdef __linux__
	/** The associated gadget */
	struct usb_gadget *gadget;
#endif
	/** Tasklet to defer starting of TEST mode transmissions until
	 * Status Phase has been completed
	 */
	struct tasklet_struct test_mode_tasklet;

	/**
	 * Pointers to the DMA Descriptors for EP0 Control transfers
	 * (virtual and physical)
	 */

	/** @{ */
	/** Descriptor for SETUP packets */
	dwc_usb3_dma_desc_t *ep0_setup_desc;
	dwc_dma_t ep0_setup_desc_dma;
	/** @} */

	/** @{ */
	/** Descriptor for Data Out or Status Out phases */
	dwc_usb3_dma_desc_t *ep0_out_desc;
	dwc_dma_t ep0_out_desc_dma;
	/** @} */

	/** @{ */
	/** Descriptor for Data In or Status In phases */
	dwc_usb3_dma_desc_t *ep0_in_desc;
	dwc_dma_t ep0_in_desc_dma;
	/** @} */

	/** @{ */
	/** Data packet buffer used to return data for GET_STATUS and
	 *  GET_DESCRIPTOR(BOS) up to 512 bytes in length
	 */
	u8 *ep0_status_buf;
	dwc_dma_t ep0_status_buf_dma;
#define DWC_STATUS_BUF_SIZE 512
	/** @} */

	/** @{ */
	/** SETUP packet buffer for EP0 */
	union dwc_setup_pkt {
		usb_device_request_t req;
		u32 d32[2];
		char d8[8];
	} *ep0_setup_pkt;
	dwc_dma_t ep0_setup_pkt_dma;
	/** @} */
} dwc_usb3_pcd_t;

/** @{ */
/** PCD accessor functions */
#define dwc_usb3_ep0_setup_desc(pcd)		(pcd)->ep0_setup_desc
#define dwc_usb3_ep0_setup_desc_dma(pcd)	(pcd)->ep0_setup_desc_dma
#define dwc_usb3_ep0_out_desc(pcd)		(pcd)->ep0_out_desc
#define dwc_usb3_ep0_out_desc_dma(pcd)		(pcd)->ep0_out_desc_dma
#define dwc_usb3_ep0_in_desc(pcd)		(pcd)->ep0_in_desc
#define dwc_usb3_ep0_in_desc_dma(pcd)		(pcd)->ep0_in_desc_dma
#define dwc_usb3_ep0_setup_pkt(pcd)		(pcd)->ep0_setup_pkt
#define dwc_usb3_ep0_setup_pkt_dma(pcd)		(pcd)->ep0_setup_pkt_dma
/** @} */

extern void dwc_usb3_ep0_activate(dwc_usb3_pcd_t *pcd, int restore);
extern void dwc_usb3_ep_activate(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *ep,
				 int restore);
extern void dwc_usb3_stop_all_xfers(dwc_usb3_pcd_t *pcd);
extern void dwc_usb3_complete_request(dwc_usb3_pcd_t *pcd,
				      dwc_usb3_pcd_ep_t *ep, u32 event);
extern dwc_usb3_pcd_ep_t *dwc_usb3_get_out_ep(dwc_usb3_pcd_t *pcd, u32 ep_num);
extern dwc_usb3_pcd_ep_t *dwc_usb3_get_in_ep(dwc_usb3_pcd_t *pcd, u32 ep_num);
extern void dwc_usb3_handle_ep0_xfer(dwc_usb3_pcd_t *pcd, u32 event);
#ifdef DEBUG_EP0
extern void dwc_usb3_print_ep0_state(dwc_usb3_pcd_t *pcd);
#endif
extern void dwc_usb3_handle_ep_intr(dwc_usb3_pcd_t *pcd, int physep, u32 event);
extern int dwc_usb3_handle_dev_intr(dwc_usb3_pcd_t *pcd, u32 event);
extern void dwc_usb3_handle_connect_done_intr(dwc_usb3_pcd_t *pcd);
extern void dwc_enter_hibernation(dwc_usb3_pcd_t *pcd, int save_state);
extern void dwc_exit_hibernation_after_connect(dwc_usb3_pcd_t *pcd,
					       int connected);
extern int dwc_exit_hibernation(dwc_usb3_pcd_t *pcd, int restore_state);
extern int dwc_usb3_handle_pme_intr(struct dwc_usb3_device *dev);
extern void dwc_usb3_power_ctl(struct dwc_usb3_device *dev, int on);

/** @addtogroup init_api_grp */
/** @{ */
extern int dwc_usb3_pcd_init(struct dwc_usb3_device *dev);
extern void dwc_usb3_pcd_remove(struct dwc_usb3_device *dev);
/** @} */

/** @addtogroup trb_api_grp TRB API Routines
 *
 * These routines handle the allocation, deallocation, and setup of TRBs.
 */
/** @{ */
extern dwc_usb3_dma_desc_t *dwc_usb3_pcd_trb_alloc(dwc_usb3_pcd_ep_t *ep,
		int num_trbs, int trb_type, int iso_intvl, int link,
		dwc_dma_t *trbs_dma_ret);
extern void dwc_usb3_pcd_trb_free(dwc_usb3_pcd_ep_t *ep /*, int num_trbs, int link,
				  void *trbs, dwc_dma_t trbs_dma*/);
extern void dwc_usb3_pcd_fill_trbs(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *ep,
				   dwc_usb3_pcd_req_t *req);
/** @} */

/** @addtogroup ep_api_grp Endpoint API Routines
 *
 * These routines handle all the functionality required for configuring,
 * enabling, controlling, and submitting transfers to an endpoint\n\n
 * Note: For Control endpoint 0, only the submit_req, cancel_req, request_done,
 * and set_halt routines are used; the remaining functionality is handled either
 * by the @ref ep0_api_grp below or internally by the PCD.
 */
/** @{ */
extern int dwc_usb3_pcd_ep_enable(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *ep,
			const usb_endpoint_descriptor_t *ep_desc,
			const ss_endpoint_companion_descriptor_t *ep_comp);
extern int dwc_usb3_pcd_ep_disable(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *ep);
extern int dwc_usb3_pcd_ep_submit_req(dwc_usb3_pcd_t *pcd,
				      dwc_usb3_pcd_ep_t *ep,
				      dwc_usb3_pcd_req_t *req, int req_flags);
extern void dwc_usb3_pcd_ep_cancel_req(dwc_usb3_pcd_t *pcd,
				       dwc_usb3_pcd_ep_t *ep,
				       dwc_usb3_pcd_req_t *req, u32 stream);
extern void dwc_usb3_pcd_request_done(dwc_usb3_pcd_t *pcd,
				      dwc_usb3_pcd_ep_t *ep,
				      dwc_usb3_pcd_req_t *req, int status);
extern void dwc_usb3_pcd_ep_start_transfer(dwc_usb3_pcd_t *pcd,
					   dwc_usb3_pcd_ep_t *ep,
					   dwc_usb3_pcd_req_t *req, u32 event);
extern void dwc_usb3_pcd_ep_set_stall(dwc_usb3_pcd_t *pcd,
				      dwc_usb3_pcd_ep_t *ep);
extern void dwc_usb3_pcd_ep_clear_stall(dwc_usb3_pcd_t *pcd,
					dwc_usb3_pcd_ep_t *ep);
extern void dwc_usb3_pcd_ep_set_halt(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *ep,
				     int value);
/** @} */

/** @addtogroup ep0_api_grp Control Endpoint 0 API Routines
 *
 * These routines are only used for Control endpoint 0.
 */
/** @{ */
extern void dwc_usb3_pcd_do_setup(dwc_usb3_pcd_t *pcd);
extern void dwc_usb3_pcd_ep0_out_start(dwc_usb3_pcd_t *pcd);
extern void dwc_usb3_pcd_ep0_start_transfer(dwc_usb3_pcd_t *pcd,
					    dwc_usb3_pcd_req_t *req);
extern void dwc_usb3_pcd_ep0_continue_transfer(dwc_usb3_pcd_t *pcd,
					       dwc_usb3_pcd_req_t *req);
extern void dwc_usb3_pcd_ep0_data_stage(dwc_usb3_pcd_t *pcd, int length);
/** @} */

/** @addtogroup misc_api_grp Miscellaneous API Routines
 *
 * These are miscellaneous routines that don't fit into any of the other
 * categories.
 */
/** @{ */
extern dwc_usb3_pcd_ep_t *dwc_usb3_pcd_get_ep_by_addr(dwc_usb3_pcd_t *pcd,
						      u16 index);
extern int dwc_usb3_pcd_get_frame_number(dwc_usb3_pcd_t *pcd);
extern int dwc_usb3_pcd_isoc_ep_hiber_restart(dwc_usb3_pcd_t *pcd,
					      dwc_usb3_pcd_ep_t *ep);
extern void dwc_usb3_pcd_stop(dwc_usb3_pcd_t *pcd);
/** @} */

/** @addtogroup gadget_notif_grp Function Driver notification routines
 *
 * These routines receive notifications from the PCD when certain events occur
 * which the Function Driver may need to be aware of.
 */
/** @{ */
extern int dwc_usb3_gadget_connect(dwc_usb3_pcd_t *pcd, int speed);
extern int dwc_usb3_gadget_disconnect(dwc_usb3_pcd_t *pcd);
extern int dwc_usb3_gadget_suspend(dwc_usb3_pcd_t *pcd);
extern int dwc_usb3_gadget_resume(dwc_usb3_pcd_t *pcd);
extern int dwc_usb3_gadget_setup(dwc_usb3_pcd_t *pcd,
				 usb_device_request_t *ctrl);
extern int dwc_usb3_gadget_complete(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *ep,
				    dwc_usb3_pcd_req_t *pcd_req, int status);
extern void dwc_usb3_gadget_do_setup(dwc_usb3_pcd_t *pcd);
/** @} */

/** @addtogroup gadget_callbk_grp Function Driver callback routines
 *
 * The PCD calls these routines when it needs something from the Function
 * Driver.
 */
/** @{ */
extern void *dwc_usb3_gadget_alloc_dma(dwc_usb3_pcd_ep_t *ep, int size,
				       dwc_dma_t *mem_dma_ret);
extern void dwc_usb3_gadget_free_dma(dwc_usb3_pcd_ep_t *ep, int size, void *mem,
				     dwc_dma_t mem_dma);
extern dwc_usb3_pcd_req_t *dwc_usb3_gadget_get_request(dwc_usb3_pcd_t *pcd,
						       dwc_usb3_pcd_ep_t *ep);
extern void dwc_usb3_gadget_start_next_request(dwc_usb3_pcd_t *pcd,
					       dwc_usb3_pcd_ep_t *ep);
extern void dwc_usb3_gadget_isoc_ep_start(dwc_usb3_pcd_t *pcd,
					  dwc_usb3_pcd_ep_t *ep, u32 event);
extern void dwc_usb3_gadget_request_nuke(dwc_usb3_pcd_t *pcd,
					 dwc_usb3_pcd_ep_t *ep);
extern void dwc_usb3_gadget_set_ep_not_started(dwc_usb3_pcd_t *pcd,
					       dwc_usb3_pcd_ep_t *ep);
/** @} */

/* OS-specific routines called from core code */
extern void dwc_usb3_task_schedule(struct tasklet_struct *tasklet);

#ifdef CONFIG_USB_OTG_DWC
extern void dwc_usb3_start_hnp(dwc_usb3_pcd_t *pcd);
extern void dwc_usb3_host_release(dwc_usb3_pcd_t *pcd);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _DWC_PCD_H_ */
