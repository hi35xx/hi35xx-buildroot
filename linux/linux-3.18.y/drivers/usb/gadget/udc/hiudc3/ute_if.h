/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/DWC_usb3/driver/linux/ute_if.h $
 * $Revision: #10 $
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

#ifndef _DWC_UTE_IFC_H_
#define _DWC_UTE_IFC_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file
 *
 */

extern dwc_usb3_pcd_t *dwc_usb3_get_pcd_instance(unsigned devnum);
extern int dwc_usb3_set_usb_ep_map(unsigned devnum, unsigned phys_ep_num,
				   unsigned usb_ep_num);
extern int dwc_usb3_get_usb_ep_map(unsigned devnum, unsigned phys_ep_num,
				   unsigned *usb_ep_num_ret);
extern void dwc_usb3_ute_config(dwc_usb3_device_t *usb3_dev);

extern int dwc_usb3_reset_usb_ep_map(unsigned devnum);
extern int dwc_usb3_switch_speed(unsigned devnum, int speed);
extern int dwc_usb3_get_dev_speed(unsigned devnum);

#ifdef __cplusplus
}
#endif

#endif	/* _DWC_UTE_IFC_H_ */
