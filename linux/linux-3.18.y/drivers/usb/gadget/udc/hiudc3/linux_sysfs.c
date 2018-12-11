/** @file
 */

#include "os_defs.h"
#include "hw.h"
#include "usb.h"
#include "pcd.h"
#include "dev.h"
#include "os_dev.h"
#include "cil.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0)
# undef VERIFY_OCTAL_PERMISSIONS
# define VERIFY_OCTAL_PERMISSIONS(perms)				\
	(BUILD_BUG_ON_ZERO((perms) < 0) +				\
	 BUILD_BUG_ON_ZERO((perms) > 0777) +				\
	/* User perms >= group perms >= other perms */			\
	 BUILD_BUG_ON_ZERO(((perms) >> 6) < (((perms) >> 3) & 7)) +	\
	 BUILD_BUG_ON_ZERO((((perms) >> 3) & 7) < ((perms) & 7)) +	\
	 (perms))
#endif

#ifdef CONFIG_USB_OTG_DWC
extern int otg_start_rsp(struct usb_otg *otg);
extern int otg_end_session(struct usb_otg *otg);
#endif

/*-------------------------------------------------------------------------*/
/* Encapsulate the module parameter settings */

dwc_usb3_core_params_t dwc_usb3_module_params = {
	.burst = 1,
	.newcore = 0,
	.phy = 2,
	.wakeup = 0,
#ifdef DWC_STAR_9000446947_WORKAROUND
	.pwrctl = 0,
#else
# if defined(DWC_STAR_9000449814_WORKAROUND) || \
     defined(DWC_STAR_9000459034_WORKAROUND)
	.pwrctl = 2,
# else
	.pwrctl = 3,
# endif
#endif
	.lpmctl = 1,
	.phyctl = 1,
	.usb2mode = 0,
	.hibernate = 0,
	.hiberdisc = 1,
	.clkgatingen = 1,
	.ssdisquirk = 1,
	.nobos = 0,
	.loop = 0,
	.nump = 16,
	.newcsr = 1,
	.rxfsz = 0,
	.txfsz = { 0, },
	.txfsz_cnt = 0,
	.besl = 0,
	.baseline_besl = 0,
	.deep_besl = 0,
	.ebc = 0,
};

module_param_named(burst, dwc_usb3_module_params.burst, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(burst, "Enable gadget to set USB 3.0 max burst size "
			"(0=no, 1=yes) (default=yes)");

module_param_named(new, dwc_usb3_module_params.newcore, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(new, "Force new core behavior [rev >= 1.09a] (0=no, 1=yes) "
		      "(default=no)");

module_param_named(phy, dwc_usb3_module_params.phy, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(phy, "Select PHY type (0=RocketIO, 1=old-Synopsys, "
		      "2=TI/Synopsys-8bit-UTMI/ULPI 3=Synopsys-16bit-UTMI) "
		      "(default=2)");

module_param_named(wakeup, dwc_usb3_module_params.wakeup, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(wakeup, "Enable remote wakeup (0=no, 1=yes) (default=no)");

module_param_named(pwrctl, dwc_usb3_module_params.pwrctl, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(pwrctl, "Enable U1/U2 power states (bit0=U1, bit1=U2) "
			 "(default=U1+U2)");

module_param_named(lpmctl, dwc_usb3_module_params.lpmctl, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(lpmctl, "Enable LPM power control (0=no, 1=AppL1Res-0, "
			 "2=AppL1Res-1) (default=1)");

module_param_named(phyctl, dwc_usb3_module_params.phyctl, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(phyctl, "Enable PHY suspend (0=no, 1=yes) (default=yes)");

module_param_named(usb2mode, dwc_usb3_module_params.usb2mode, int,
		   S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(usb2mode, "Force the core to connect in USB 2.0 mode at the "
			   "given speed (0=no, 1=FS, 2=HS) (default=no)");

module_param_named(hibernate, dwc_usb3_module_params.hibernate, int,
		   S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(hibernate, "Enable hibernation mode (0=no, 1=yes) "
			    "(default=no)");

module_param_named(hiberdisc, dwc_usb3_module_params.hiberdisc, int,
		   S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(hiberdisc, "Enter hibernation when disconnected "
			    "(0=no, 1=yes) (default=yes)");

module_param_named(clkgatingen, dwc_usb3_module_params.clkgatingen, int,
		   S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(clkgatingen, "Enable clock gating (0=no, 1=yes) "
			      "(default=yes)");

module_param_named(ssdisquirk, dwc_usb3_module_params.ssdisquirk, int,
		   S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(ssdisquirk, "Enable SS_DIS Quirk (0=no, 1=yes) (default=yes)");

module_param_named(nobos, dwc_usb3_module_params.nobos, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(nobos, "Fail GetDescriptor(BOS) request at USB2 speeds "
			"(0=no, 1=yes) (default=no)");

module_param_named(nump, dwc_usb3_module_params.nump, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(nump, "Set NUMP to given value (1-16) (default=16)");

module_param_named(newcsr, dwc_usb3_module_params.newcsr, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(newcsr, "Use a newer HAPS bitfile with a 64K PCI BAR size "
			 "instead of the older 1MB PCI BAR size (0=no, 1=yes) "
			 "(default=yes)");

module_param_named(loop, dwc_usb3_module_params.loop, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(loop, "Number of times to loop in reset (for debug only)");

module_param_named(rxfsz, dwc_usb3_module_params.rxfsz, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(rxfsz, "Size of Rx FIFO in bytes");

module_param_array_named(txfsz, dwc_usb3_module_params.txfsz, int,
			 &dwc_usb3_module_params.txfsz_cnt, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(txfsz, "Size of Tx FIFOs in bytes");

module_param_named(besl, dwc_usb3_module_params.besl, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(besl, "Enable besl support");

module_param_named(baseline_besl, dwc_usb3_module_params.baseline_besl, int,
		   S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(baseline_besl, "Set the baseline besl value");

module_param_named(deep_besl, dwc_usb3_module_params.deep_besl, int,
		   S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(deep_besl, "Set the deep besl value");

module_param_named(ebc, dwc_usb3_module_params.ebc, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(ebc, "Enable EBC support on all Bulk endpoints");

static dwc_usb3_device_t *get_usb3_device(struct device *dev)
{
#ifdef DWC_PLATFORM_DEV
	struct platform_device *device =
			container_of(dev, struct platform_device, dev);
	dwc_usb3_device_t *usb3_dev = platform_get_drvdata(device);
#else
	struct pci_dev *device = container_of(dev, struct pci_dev, dev);
	dwc_usb3_device_t *usb3_dev = pci_get_drvdata(device);
#endif

	return usb3_dev;
}

static ssize_t show_wakeup(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	dwc_usb3_device_t *usb3_dev = get_usb3_device(dev);

	return sprintf(buf, "%d\n", usb3_dev->pcd.wkup_rdy);
}

static ssize_t store_wakeup(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	dwc_usb3_device_t	*usb3_dev = get_usb3_device(dev);
	int			ret;

	ret = dwc_usb3_wakeup(usb3_dev->pcd.gadget);

	return ret < 0 ? ret : count;
}

/* /sys/module/dwc_usb3/drivers/pci:dwc_usb3/nnnn:nn:nn.n/wakeup */
static DEVICE_ATTR(wakeup, 0666, show_wakeup, store_wakeup);

static ssize_t show_disrupt(struct device *dev, struct device_attribute *attr,
			    char *buf)
{
	return sprintf(buf, "%d\n", 0);
}

static ssize_t store_disrupt(struct device *dev, struct device_attribute *attr,
			     const char *buf, size_t count)
{
	dwc_usb3_device_t	*usb3_dev = get_usb3_device(dev);
	unsigned long		tmp = 0;
	ssize_t			rc;
	unsigned long		flags;

	sscanf(buf, "%ld", &tmp);
	rc = strnlen(buf, count);
	printk(USB3_DWC "disrupt: %ld ms\n", tmp);

	spin_lock_irqsave(&usb3_dev->pcd.lock, flags);
	mdelay(tmp);
	spin_unlock_irqrestore(&usb3_dev->pcd.lock, flags);
	return rc;
}

/* /sys/module/dwc_usb3/drivers/pci:dwc_usb3/nnnn:nn:nn.n/disrupt */
static DEVICE_ATTR(disrupt, 0666, show_disrupt, store_disrupt);

static ssize_t show_hiber(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	return sprintf(buf, "%d\n", 0);
}

static ssize_t store_hiber(struct device *dev, struct device_attribute *attr,
			   const char *buf, size_t count)
{
	dwc_usb3_device_t	*usb3_dev = get_usb3_device(dev);
	int			tmp = 0;
	ssize_t			rc;
	unsigned long		flags;

	sscanf(buf, "%d", &tmp);
	rc = strnlen(buf, count);
	printk(USB3_DWC "hibernate: save_state=%d\n", tmp);

	spin_lock_irqsave(&usb3_dev->pcd.lock, flags);
	dwc_enter_hibernation(&usb3_dev->pcd, tmp);
	spin_unlock_irqrestore(&usb3_dev->pcd.lock, flags);
	return rc;
}

/* /sys/module/dwc_usb3/drivers/pci:dwc_usb3/nnnn:nn:nn.n/hibernate */
static DEVICE_ATTR(hibernate, 0666, show_hiber, store_hiber);

static ssize_t show_restore(struct device *dev, struct device_attribute *attr,
			    char *buf)
{
	return sprintf(buf, "%d\n", 0);
}

static ssize_t store_restore(struct device *dev, struct device_attribute *attr,
			     const char *buf, size_t count)
{
	dwc_usb3_device_t	*usb3_dev = get_usb3_device(dev);
	int			tmp = 0;
	ssize_t			rc;
	unsigned long		flags;

	sscanf(buf, "%d", &tmp);
	rc = strnlen(buf, count);
	printk(USB3_DWC "restore: restore_state=%d\n", tmp);

	spin_lock_irqsave(&usb3_dev->pcd.lock, flags);
	tmp = dwc_exit_hibernation(&usb3_dev->pcd, tmp);
	spin_unlock_irqrestore(&usb3_dev->pcd.lock, flags);
	printk(USB3_DWC "dwc_exit_hibernation() returned %d\n", tmp);
	return rc;
}

/* /sys/module/dwc_usb3/drivers/pci:dwc_usb3/nnnn:nn:nn.n/restore */
static DEVICE_ATTR(restore, 0666, show_restore, store_restore);

#ifdef CONFIG_USB_OTG_DWC

static ssize_t store_srp(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	struct usb_phy *phy;
	struct usb_otg *otg;

	phy = usb_get_phy(USB_PHY_TYPE_USB3);
	if (IS_ERR(phy) || !phy) {
		if (!IS_ERR(phy))
			usb_put_phy(phy);
		return count;
	}

	otg = phy->otg;
	if (!otg) {
		usb_put_phy(phy);
		return count;
	}

	otg_start_srp(otg);
	usb_put_phy(phy);
	return count;
}
static DEVICE_ATTR(srp, 0222, NULL, store_srp);

static ssize_t store_end(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	struct usb_phy *phy;
	struct usb_otg *otg;

	phy = usb_get_phy(USB_PHY_TYPE_USB3);
	if (IS_ERR(phy) || !phy) {
		if (!IS_ERR(phy))
			usb_put_phy(phy);
		return count;
	}

	otg = phy->otg;
	if (!otg) {
		usb_put_phy(phy);
		return count;
	}

	otg_end_session(otg);
	usb_put_phy(phy);
	return count;
}
static DEVICE_ATTR(end, 0222, NULL, store_end);

static ssize_t store_hnp(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	dwc_usb3_device_t *usb3_dev = get_usb3_device(dev);

	if (usb3_dev->pcd.b_hnp_enable) {
		dev_info(dev, "b_hnp_enable is TRUE\n");
		usb3_dev->pcd.b_hnp_enable = 0;
		usb3_dev->pcd.wants_host = 0;
		dwc_usb3_start_hnp(&usb3_dev->pcd);
	} else {
		dev_info(dev, "b_hnp_enable is FALSE\n");
		usb3_dev->pcd.wants_host = 1;
		/* TODO if we don't receive the SET_FEATURE within 4 secs,
		 * reset this value
		 */
	}
	return count;
}
static DEVICE_ATTR(hnp, 0222, NULL, store_hnp);

static ssize_t store_rsp(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	struct usb_phy *phy;
	struct usb_otg *otg;

	phy = usb_get_phy(USB_PHY_TYPE_USB3);
	if (IS_ERR(phy) || !phy) {
		if (!IS_ERR(phy))
			usb_put_phy(phy);
		return count;
	}

	otg = phy->otg;
	if (!otg) {
		usb_put_phy(phy);
		return count;
	}

	otg_start_rsp(otg);
	usb_put_phy(phy);
	return count;
}
static DEVICE_ATTR(rsp, 0222, NULL, store_rsp);

#endif /* CONFIG_USB_OTG_DWC */

static ssize_t show_lpm_nyet(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	dwc_usb3_device_t *usb3_dev = get_usb3_device(dev);
	dwc_usb3_dev_global_regs_t __iomem *regs =
			usb3_dev->pcd.dev_global_regs;
	u32 dctl = dwc_rd32(usb3_dev, &regs->dctl);
	u32 nyet = (dctl & DWC_DCTL_LPM_NYET_THRESH_BITS) >>
			DWC_DCTL_LPM_NYET_THRESH_SHIFT;

	return sprintf(buf, "0x%x\n", nyet);
}

static ssize_t store_lpm_nyet(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{
	dwc_usb3_device_t *usb3_dev = get_usb3_device(dev);
	dwc_usb3_dev_global_regs_t __iomem *regs =
			usb3_dev->pcd.dev_global_regs;
	u32 dctl = dwc_rd32(usb3_dev, &regs->dctl);
	u32 value = simple_strtol(buf, NULL, 16);

	value = (value << DWC_DCTL_LPM_NYET_THRESH_SHIFT) &
			DWC_DCTL_LPM_NYET_THRESH_BITS;

	dctl &= ~(DWC_DCTL_LPM_NYET_THRESH_BITS);
	dctl |= value;

	dwc_wr32(usb3_dev, &regs->dctl, dctl);
	return count;
}

/* /sys/module/dwc_usb3/drivers/pci:dwc_usb3/nnnn:nn:nn.n/wakeup */
static DEVICE_ATTR(lpm_nyet, 0666, show_lpm_nyet, store_lpm_nyet);

void dwc_usb3_remove_dev_files(struct device *dev)
{
#ifdef CONFIG_USB_OTG_DWC
	device_remove_file(dev, &dev_attr_end);
	device_remove_file(dev, &dev_attr_srp);
	device_remove_file(dev, &dev_attr_rsp);
	device_remove_file(dev, &dev_attr_hnp);
#endif
	device_remove_file(dev, &dev_attr_restore);
	device_remove_file(dev, &dev_attr_hibernate);
	device_remove_file(dev, &dev_attr_disrupt);
	device_remove_file(dev, &dev_attr_wakeup);
	device_remove_file(dev, &dev_attr_lpm_nyet);
}

int dwc_usb3_create_dev_files(struct device *dev)
{
	int retval;

	retval = device_create_file(dev, &dev_attr_wakeup);
	if (retval)
		goto fail;

	retval = device_create_file(dev, &dev_attr_disrupt);
	if (retval)
		goto fail;

	retval = device_create_file(dev, &dev_attr_hibernate);
	if (retval)
		goto fail;

	retval = device_create_file(dev, &dev_attr_restore);
	if (retval)
		goto fail;

#ifdef CONFIG_USB_OTG_DWC
	retval = device_create_file(dev, &dev_attr_hnp);
	if (retval)
		goto fail;

	retval = device_create_file(dev, &dev_attr_rsp);
	if (retval)
		goto fail;

	retval = device_create_file(dev, &dev_attr_srp);
	if (retval)
		goto fail;

	retval = device_create_file(dev, &dev_attr_end);
	if (retval)
		goto fail;
#endif

	retval = device_create_file(dev, &dev_attr_lpm_nyet);
	if (retval)
		goto fail;

	return 0;

fail:
	dev_err(dev, "Failed to create one or more sysfs files!!\n");
	return retval;
}
