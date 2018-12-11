/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/DWC_usb3/driver/linux/ute_if.c $
 * $Revision: #11 $
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

/** @file
 *
 */

#include "os_defs.h"
#include "hw.h"
#include "usb.h"
#include "pcd.h"
#include "dev.h"
#include "os_dev.h"
#include "cil.h"
#include "dwc_pcd_cfi.h"
#include "ute_if.h"

static struct cfi_ft_lst pcd_feats = {
	.hdr = {
		//.totlen		= (set at runtime),
		.cfiver			= 0x100,
		.coreID			= 4,
		//.ftcount		= (set at runtime),
	},
	.desc[0] = {		/* Rx FIFO size */
		.ftparm = {
			.fid		= UTE_FID_RXFIFO_SIZE,
			.paramcount	= 1,
			.param1		= 0xffffffff,	/* shared */
		},
		//.fdlen		= sizeof(struct cfi_feature_desc),
		.dlen			= 4,
		.bmattr			= FT_ATTR_RW,
		.ftname			= "rxfsz",
	},
	.desc[1] = {		/* Tx FIFO 0 size */
		.ftparm = {
			.fid		= UTE_FID_TXFIFO_SIZE,
			.paramcount	= 1,
			.param1		= 0,
		},
		//.fdlen		= sizeof(struct cfi_feature_desc),
		.dlen			= 4,
		.bmattr			= FT_ATTR_RW,
		.ftname			= "txfsz0",
	},
	.desc[2] = {		/* Tx FIFO 1 size */
		.ftparm = {
			.fid		= UTE_FID_TXFIFO_SIZE,
			.paramcount	= 1,
			.param1		= 1,
		},
		//.fdlen		= sizeof(struct cfi_feature_desc),
		.dlen			= 4,
		.bmattr			= FT_ATTR_RW,
		.ftname			= "txfsiz1",
	},
	.desc[3] = {		/* Tx FIFO 2 size */
		.ftparm = {
			.fid		= UTE_FID_TXFIFO_SIZE,
			.paramcount	= 1,
			.param1		= 2,
		},
		//.fdlen		= sizeof(struct cfi_feature_desc),
		.dlen			= 4,
		.bmattr			= FT_ATTR_RW,
		.ftname			= "txfsz2",
	},
	.desc[4] = {		/* Tx FIFO 3 size */
		.ftparm = {
			.fid		= UTE_FID_TXFIFO_SIZE,
			.paramcount	= 1,
			.param1		= 3,
		},
		//.fdlen		= sizeof(struct cfi_feature_desc),
		.dlen			= 4,
		.bmattr			= FT_ATTR_RW,
		.ftname			= "txfsz3",
	},
	.desc[5] = {		/* FIFO 0 physical EP mapping */
		.ftparm = {
			.fid		= UTE_FID_TXFIFO_MAP,
			.paramcount	= 1,
			.param1		= 0,
		},
		//.fdlen		= sizeof(struct cfi_feature_desc),
		.dlen			= 4,
		.bmattr			= FT_ATTR_RW,
		.ftname			= "txfmap0",
	},
	.desc[6] = {		/* FIFO 1 physical EP mapping */
		.ftparm = {
			.fid		= UTE_FID_TXFIFO_MAP,
			.paramcount	= 1,
			.param1		= 1,
		},
		//.fdlen		= sizeof(struct cfi_feature_desc),
		.dlen			= 4,
		.bmattr			= FT_ATTR_RW,
		.ftname			= "txfmap1",
	},
	.desc[7] = {		/* FIFO 2 physical EP mapping */
		.ftparm = {
			.fid		= UTE_FID_TXFIFO_MAP,
			.paramcount	= 1,
			.param1		= 2,
		},
		//.fdlen		= sizeof(struct cfi_feature_desc),
		.dlen			= 4,
		.bmattr			= FT_ATTR_RW,
		.ftname			= "txfmap2",
	},
	.desc[8] = {		/* FIFO 3 physical EP mapping */
		.ftparm = {
			.fid		= UTE_FID_TXFIFO_MAP,
			.paramcount	= 1,
			.param1		= 3,
		},
		//.fdlen		= sizeof(struct cfi_feature_desc),
		.dlen			= 4,
		.bmattr			= FT_ATTR_RW,
		.ftname			= "txfmap3",
	},
	.desc[9] = {		/* Physical OUT EP 1 USB EP mapping */
		.ftparm = {
			.fid		= UTE_FID_EP_MAP,
			.paramcount	= 1,
			.param1		= 0x01,
		},
		//.fdlen		= sizeof(struct cfi_feature_desc),
		.dlen			= 4,
		.bmattr			= FT_ATTR_RW,
		.ftname			= "epmap1-out",
	},
	.desc[10] = {		/* Physical IN EP 1 USB EP mapping */
		.ftparm = {
			.fid		= UTE_FID_EP_MAP,
			.paramcount	= 1,
			.param1		= 0x11,
		},
		//.fdlen		= sizeof(struct cfi_feature_desc),
		.dlen			= 4,
		.bmattr			= FT_ATTR_RW,
		.ftname			= "epmap1-in",
	},
	.desc[11] = {		/* Physical OUT EP 2 USB EP mapping */
		.ftparm = {
			.fid		= UTE_FID_EP_MAP,
			.paramcount	= 1,
			.param1		= 0x02,
		},
		//.fdlen		= sizeof(struct cfi_feature_desc),
		.dlen			= 4,
		.bmattr			= FT_ATTR_RW,
		.ftname			= "epmap2-out",
	},
	.desc[12] = {		/* Physical IN EP 2 USB EP mapping */
		.ftparm = {
			.fid		= UTE_FID_EP_MAP,
			.paramcount	= 1,
			.param1		= 0x12,
		},
		//.fdlen		= sizeof(struct cfi_feature_desc),
		.dlen			= 4,
		.bmattr			= FT_ATTR_RW,
		.ftname			= "epmap2-in",
	},
	.desc[13] = {		/* Physical OUT EP 3 USB EP mapping */
		.ftparm = {
			.fid		= UTE_FID_EP_MAP,
			.paramcount	= 1,
			.param1		= 0x03,
		},
		//.fdlen		= sizeof(struct cfi_feature_desc),
		.dlen			= 4,
		.bmattr			= FT_ATTR_RW,
		.ftname			= "epmap3-out",
	},
	.desc[14] = {		/* Physical IN EP 3 USB EP mapping */
		.ftparm = {
			.fid		= UTE_FID_EP_MAP,
			.paramcount	= 1,
			.param1		= 0x13,
		},
		//.fdlen		= sizeof(struct cfi_feature_desc),
		.dlen			= 4,
		.bmattr			= FT_ATTR_RW,
		.ftname			= "epmap3-in",
	},
	.desc[15] = {		/* Super Speed */
		.ftparm = {
			.fid		= UTE_FID_DEV_SPEED,
			.paramcount	= 1,
			.param1		= 0xffffffff,
		},
		//.fdlen		= sizeof(struct cfi_feature_desc),
		.dlen			= 4,
		.bmattr			= FT_ATTR_RW,
		.ftname			= "dev speed",
	},
};

static void *cficb_get_ft_list(struct cfiobject *cfi)
{
	return cfi->features;
}

static int cficb_get_ft_value(struct cfiobject *cfi,
			      struct cfi_ft_params *params,
			      unsigned long valptr)
{
	dwc_usb3_pcd_t *pcd;
	dwc_usb3_device_t *dev;
	dwc_usb3_core_global_regs_t __iomem *global_regs;
	unsigned val, index, ram_width;
	int ret = 0;

	printk(KERN_DEBUG USB3_DWC "%s(%p,%p,%1lx)\n", __func__,
	       cfi, params, valptr);
	pcd = cfi->pcd;
	printk(KERN_DEBUG USB3_DWC "pcd=%p\n", pcd);
	dev = pcd->usb3_dev;
	if (params) {
		printk(KERN_DEBUG USB3_DWC "params->param1=%1x ->fid=%1d\n",
		       params->param1, params->fid);
	} else {
		printk(KERN_ERR USB3_DWC "params=NULL!\n");
		return -EINVAL;
	}
	if (!valptr) {
		printk(KERN_ERR USB3_DWC "valptr=NULL!\n");
		return -EINVAL;
	}

	switch (params->fid) {
	case UTE_FID_RXFIFO_SIZE:
		/* get Rx FIFO size */
		/* val is FIFO size in bytes */
		if (params->param1 != 0xffffffff) {
			ret = -EINVAL;
		} else {
			global_regs = dev->core_global_regs;
			val = dwc_rd32(dev, &global_regs->grxfifosiz[0]);
			val = (val >> DWC_FIFOSZ_DEPTH_SHIFT) &
			      (DWC_FIFOSZ_DEPTH_BITS >> DWC_FIFOSZ_DEPTH_SHIFT);
			ram_width =
			     ((dev->hwparams0 >> DWC_HWP0_MDWIDTH_SHIFT) &
			      (DWC_HWP0_MDWIDTH_BITS >> DWC_HWP0_MDWIDTH_SHIFT))
			     / 8;
			val *= ram_width;
			*(unsigned *)valptr = val;
		}
		break;

	case UTE_FID_TXFIFO_SIZE:
		/* get Tx FIFO size */
		/* param1 is Tx FIFO #, val is FIFO size in bytes */
		if (params->param1 >= DWC_MAX_TX_FIFOS) {
			ret = -EINVAL;
		} else {
			global_regs = dev->core_global_regs;
			val = dwc_rd32(dev,
				&global_regs->gtxfifosiz[params->param1]);
			val = (val >> DWC_FIFOSZ_DEPTH_SHIFT) &
			      (DWC_FIFOSZ_DEPTH_BITS >> DWC_FIFOSZ_DEPTH_SHIFT);
			ram_width =
			     ((dev->hwparams0 >> DWC_HWP0_MDWIDTH_SHIFT) &
			      (DWC_HWP0_MDWIDTH_BITS >> DWC_HWP0_MDWIDTH_SHIFT))
			     / 8;
			val *= ram_width;
			*(unsigned *)valptr = val;
		}
		break;

	case UTE_FID_RXFIFO_MAP:
		/* Rx FIFO mapping not possible for USB3 core */
		ret = -EINVAL;
		break;

	case UTE_FID_TXFIFO_MAP:
		/* get Tx FIFO mapping */
		/* param1 is Tx FIFO #, val is physical EP # */
		if (params->param1 >= DWC_MAX_TX_FIFOS) {
			ret = -EINVAL;
		} else {
			/* find matching FIFO # in EP array */
			for (index = 1; index < DWC_MAX_PHYS_EP; index += 2) {
				if (pcd->txf_map[index] == params->param1)
					break;
			}
			if (index >= DWC_MAX_PHYS_EP) {
				ret = -EINVAL;
			} else {
				/* convert EP array index to physical EP # */
				/* Note: have to drop the direction bit, UTE
				 * doesn't expect it to be set
				 */
				val = (index >> 1) & 0xf;
				*(unsigned *)valptr = val;
			}
		}
		break;

	case UTE_FID_EP_MAP:
		/* get EP mapping */
		/* param1 is physical EP #, val is USB EP # */
		index = (params->param1 & 0xf) << 1 |
			((params->param1 >> 4) & 0x1);
		ret = dwc_usb3_get_usb_ep_map(0, index, &val);
		if (!ret)
			*(unsigned *)valptr = val;
		break;
	case UTE_FID_DEV_SPEED:
		/* get current device speed */
		val = dwc_usb3_get_dev_speed(0);
		*(unsigned *)valptr = val;
		break;

	default:
		ret = -EINVAL;
	}

	if (ret == 0)
		printk(KERN_DEBUG USB3_DWC "*valptr=%1x\n",
		       *(unsigned *)valptr);
	return ret;
}

static void reset_txfifosize_ft_value(struct cfiobject *cfi)
{
	int i;
	dwc_usb3_pcd_t *pcd = cfi->pcd;

	for (i = 0; i < DWC_MAX_TX_FIFOS; i++) {
		pcd->txf_size[i] = pcd->def_txf_size[i];
	}
}

static void reset_txfifomap_ft_value(struct cfiobject *cfi)
{
	int i;
	dwc_usb3_pcd_t *pcd = cfi->pcd;

	for (i = 0; i < pcd->num_in_eps + 1; i++) {
		pcd->txf_map[(i << 1) | 1] = i;
	}
}

static int cficb_reset_ft_value(struct cfiobject *cfi,
				struct cfi_ft_params *params)
{
	int ret = 0;
	dwc_usb3_pcd_t *pcd = cfi->pcd;

	printk(KERN_DEBUG USB3_DWC "%s(%p,%p)\n", __func__, cfi, params);

	if (params) {
		printk(KERN_DEBUG USB3_DWC "params->param1=%1x ->fid=%1d\n",
		       params->param1, params->fid);
	} else {
		printk(KERN_ERR USB3_DWC "params=NULL!\n");
		return -EINVAL;
	}

	switch (params->fid) {
		case UTE_FID_EP_MAP:
			if (params->param1 == 0)
				ret = dwc_usb3_reset_usb_ep_map(0);
			break;
		case UTE_FID_RXFIFO_SIZE:
			if (params->param1 == 0 && pcd->rxf_size != 0) {
				pcd->rxf_size = pcd->def_rxf_size;
				pcd->ute_change = 1;
			}
			break;
		case UTE_FID_TXFIFO_SIZE:
			if (params->param1 == 0) {
				reset_txfifosize_ft_value(cfi);
				pcd->ute_change = 1;
			}
			break;
		case UTE_FID_TXFIFO_MAP:
			if (params->param1) {
				reset_txfifomap_ft_value(cfi);
				pcd->ute_change = 1;
			}
			break;
		default:
			ret = -ENOTSUPP;
			break;
	}

	if (ret == 0)
		pcd->ute_change = 1;

	return ret;
}


static int cficb_set_ft_value(struct cfiobject *cfi,
			      struct cfi_ft_params *params,
			      unsigned long valptr)
{
	dwc_usb3_pcd_t *pcd;
	unsigned val, index;
	int ret = 0;

	printk(KERN_DEBUG USB3_DWC "%s(%p,%p,%1lx)\n", __func__,
	       cfi, params, valptr);
	pcd = cfi->pcd;
	printk(KERN_DEBUG USB3_DWC "pcd=%p\n", pcd);
	if (params) {
		printk(KERN_DEBUG USB3_DWC "params->param1=%1x ->fid=%1d\n",
		       params->param1, params->fid);
	} else {
		printk(KERN_ERR USB3_DWC "params=NULL!\n");
		return -EINVAL;
	}
	if (valptr) {
		printk(KERN_DEBUG USB3_DWC "*valptr=%1x\n",
		       *(unsigned *)valptr);
	} else {
		printk(KERN_ERR USB3_DWC "valptr=NULL!\n");
		return -EINVAL;
	}

	switch (params->fid) {
	case UTE_FID_RXFIFO_SIZE:
		/* set Rx FIFO size */
		/* val is FIFO size in bytes */
		val = *(unsigned *)valptr;
		if (params->param1 != 0xffffffff)
			ret = -EINVAL;
		else
			pcd->rxf_size = val;
		break;

	case UTE_FID_TXFIFO_SIZE:
		/* set Tx FIFO size */
		/* param1 is Tx FIFO #, val is FIFO size in bytes */
		val = *(unsigned *)valptr;
		if (params->param1 >= DWC_MAX_TX_FIFOS)
			ret = -EINVAL;
		else
			pcd->txf_size[params->param1] = val;
		break;

	case UTE_FID_RXFIFO_MAP:
		/* Rx FIFO mapping not possible for USB3 core */
		ret = -EINVAL;
		break;

	case UTE_FID_TXFIFO_MAP:
		/* set Tx FIFO mapping */
		/* param1 is Tx FIFO #, val is physical EP # */
		val = *(unsigned *)valptr;
		/* convert physical EP # to EP array index */
		/* Note: have to force the direction bit to 1, UTE
		 * doesn't set it
		 */
		index = ((val & 0xf) << 1) | 1;
		if (params->param1 >= DWC_MAX_TX_FIFOS ||
		    index >= DWC_MAX_PHYS_EP)
			ret = -EINVAL;
		else
			pcd->txf_map[index] = params->param1;
		break;

	case UTE_FID_EP_MAP:
		/* set EP mapping */
		/* param1 is physical EP #, val is USB EP # */
		val = *(unsigned *)valptr;
		index = (params->param1 & 0xf) << 1 |
			((params->param1 >> 4) & 0x1);
		ret = dwc_usb3_set_usb_ep_map(0, index, val);
		break;
	case UTE_FID_DEV_SPEED:
		index = params->param1;
		printk("UTE_FID_DEV_SPEED is received: new speed %d", index);
		ret = dwc_usb3_switch_speed(0, index);
		break;
	default:
		ret = -EINVAL;
	}

	if (ret == 0) {
		pcd->ute_change = 1;
		printk(KERN_DEBUG USB3_DWC "Set UTE change bit\n");
	}

	return ret;
}

static struct cfiobject *cfi_create_object(dwc_usb3_pcd_t *pcd)
{
	struct cfiobject *cfi;

	/* Set the ftcount field in the features header */
	pcd_feats.hdr.ftcount = 16;

	/* Calculate the total length of the feature descriptors for all
	 * features plus the features header descriptor length, and write it
	 * to the totlen field in the features header
	 */
	pcd_feats.hdr.totlen = sizeof(struct cfi_features_hdr) +
			sizeof(struct cfi_feature_desc) * pcd_feats.hdr.ftcount;

	/* Allocate the cfi object */
	cfi = kmalloc(sizeof(*cfi), GFP_KERNEL);
	if (!cfi)
		return NULL;
	memset(cfi, 0, sizeof(*cfi));

	/* Assign the PCD pointer */
	cfi->pcd = pcd;

	/* Assign the features pointer */
	cfi->features = &pcd_feats;

	/* Assign the callbacks */
	cfi->ops.get_ft_list = cficb_get_ft_list;
	cfi->ops.get_ft_value = cficb_get_ft_value;
	cfi->ops.set_ft_value = cficb_set_ft_value;
	cfi->ops.reset_ft_value = cficb_reset_ft_value;

	return cfi;
}

static void *dwc_cfi_get_object(void)
{
	struct cfiobject *cfi;
	dwc_usb3_pcd_t *pcd;

	pcd = dwc_usb3_get_pcd_instance(0);
	if (!pcd)
		return NULL;

	cfi = cfi_create_object(pcd);
	if (!cfi)
		return NULL;

	return cfi;
};
EXPORT_SYMBOL(dwc_cfi_get_object);
