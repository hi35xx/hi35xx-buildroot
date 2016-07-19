/*******************************************************************************
  Header File to describe the DMA descriptors.
  Enhanced descriptors have been in case of DWMAC1000 Cores.

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Author: Giuseppe Cavallaro <peppe.cavallaro@st.com>
*******************************************************************************/

#ifndef __DESCS_H__
#define __DESCS_H__

struct dma_desc {
	/* Receive descriptor */
	union {
		struct {
			/* RDES0 */
			u32 reserved1:1;
			u32 crc_error:1;
			u32 dribbling:1;
			u32 mii_error:1;
			u32 receive_watchdog:1;
			u32 frame_type:1;
			u32 collision:1;
			u32 frame_too_long:1;
			u32 last_descriptor:1;
			u32 first_descriptor:1;
			u32 multicast_frame:1;
			u32 run_frame:1;
			u32 length_error:1;
			u32 partial_frame_error:1;
			u32 descriptor_error:1;
			u32 error_summary:1;
			u32 frame_length:14;
			u32 filtering_fail:1;
			u32 own:1;
			/* RDES1 */
			u32 buffer1_size:11;
			u32 buffer2_size:11;
			u32 reserved2:2;
			u32 second_address_chained:1;
			u32 end_ring:1;
			u32 reserved3:5;
			u32 disable_ic:1;
		} rx;
		struct {
			/* RDES0 */
			u32 payload_csum_error:1;
			u32 crc_error:1;
			u32 dribbling:1;
			u32 error_gmii:1;
			u32 receive_watchdog:1;
			u32 frame_type:1;
			u32 late_collision:1;
			u32 ipc_csum_error:1;
			u32 last_descriptor:1;
			u32 first_descriptor:1;
			u32 vlan_tag:1;
			u32 overflow_error:1;
			u32 length_error:1;
			u32 sa_filter_fail:1;
			u32 descriptor_error:1;
			u32 error_summary:1;
			u32 frame_length:14;
			u32 da_filter_fail:1;
			u32 own:1;
			/* RDES1 */
			u32 buffer1_size:13;
			u32 reserved1:1;
			u32 second_address_chained:1;
			u32 end_ring:1;
			u32 buffer2_size:13;
			u32 reserved2:2;
			u32 disable_ic:1;
		} erx;		/* -- enhanced -- */

		/* Transmit descriptor */
		struct {
			/* TDES0 */
			u32 deferred:1;
			u32 underflow_error:1;
			u32 excessive_deferral:1;
			u32 collision_count:4;
			u32 heartbeat_fail:1;
			u32 excessive_collisions:1;
			u32 late_collision:1;
			u32 no_carrier:1;
			u32 loss_carrier:1;
			u32 reserved1:3;
			u32 error_summary:1;
			u32 reserved2:15;
			u32 own:1;
			/* TDES1 */
			u32 buffer1_size:11;
			u32 buffer2_size:11;
			u32 reserved3:1;
			u32 disable_padding:1;
			u32 second_address_chained:1;
			u32 end_ring:1;
			u32 crc_disable:1;
			u32 reserved4:2;
			u32 first_segment:1;
			u32 last_segment:1;
			u32 interrupt:1;
		} tx;
		struct {
			/* TDES0 */
			u32 deferred:1;
			u32 underflow_error:1;
			u32 excessive_deferral:1;
			u32 collision_count:4;
			u32 vlan_frame:1;
			u32 excessive_collisions:1;
			u32 late_collision:1;
			u32 no_carrier:1;
			u32 loss_carrier:1;
			u32 payload_error:1;
			u32 frame_flushed:1;
			u32 jabber_timeout:1;
			u32 error_summary:1;
			u32 ip_header_error:1;
			u32 time_stamp_status:1;
			u32 reserved1:2;
			u32 second_address_chained:1;
			u32 end_ring:1;
			u32 checksum_insertion:2;
			u32 reserved2:1;
			u32 time_stamp_enable:1;
			u32 disable_padding:1;
			u32 crc_disable:1;
			u32 first_segment:1;
			u32 last_segment:1;
			u32 interrupt:1;
			u32 own:1;
			/* TDES1 */
			u32 buffer1_size:13;
			u32 reserved3:3;
			u32 buffer2_size:13;
			u32 reserved4:3;
		} etx;		/* -- enhanced -- */
	} des01;
	unsigned int des2;
	unsigned int des3;
};

/* Transmit checksum insertion control */
enum tdes_csum_insertion {
	cic_disabled = 0,	/* Checksum Insertion Control */
	cic_only_ip = 1,	/* Only IP header */
	cic_no_pseudoheader = 2,	/* IP header but pseudoheader
					 * is not calculated */
	cic_full = 3,		/* IP header and pseudoheader */
};

struct tnk_rx_dma_desc {
	/* Words 0-3 */
	struct dma_desc base;
	struct {
		/* Word 4 */
		uint32_t seq_num;
		/* Word 5 */
		uint32_t des5;
		/* Word 6 */
		uint32_t cid:11;
		uint32_t flags:19;
		uint32_t reserved1:2;
		/* Word 7 */
		uint32_t urg_ptr:16;
		uint32_t reserved2:16;
	} extra;
};


struct tnk_ttx_dma_desc {
	/* Word 0 */
	uint32_t ack_offset:24;
	uint32_t reserved1:7;
	uint32_t hw_own:1;
	/* Word 1 */
	uint32_t buffer_size:24;
	uint32_t reserved2:7;
	uint32_t intr_on_completion:1;
	/* Word 2 */
	uint32_t buffer_ptr;
	/* Word 3 */
	uint32_t next_desc_ptr;
};

extern const struct stmmac_desc_ops enh_desc_ops;

#endif /*  __DESCS_H__ */
