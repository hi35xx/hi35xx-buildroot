#
# Makefile for DWC_usb3 SuperSpeed USB controller driver
#

ifneq ($(KERNELRELEASE),)

# This line is required for 2.6.37 era kernels, or else files in
# subdirectories won't compile
EXTRA_CFLAGS	+= -I$(KBUILD_EXTMOD)

ifneq ($(NOOS),)
EXTRA_CFLAGS	+= -DLINUXTEST
else
ifneq ($(SELA),)
EXTRA_CFLAGS	+= -DLINUXTEST
else
ifneq ($(PLATFORM),)
EXTRA_CFLAGS	+= -DDWC_PLATFORM_DEV
endif
endif
endif

# Uncomment these for debug messages
EXTRA_CFLAGS	+= -DDEBUG
EXTRA_CFLAGS	+= -DVERBOSE
EXTRA_CFLAGS	+= -DDEBUG_EP0

# Uncomment this, and comment out the previous 3, for minimal debug for ISOC
#EXTRA_CFLAGS	+= -DISOC_DEBUG

# Uncomment to force gasket regs to be accessed in BAR 2. Not needed if the
# PCI device ID is 0xabce.
#EXTRA_CFLAGS	+= -DDWC_BAR2_GASKET_REG

# Uncomment for OTG
#EXTRA_CFLAGS	+= -DCONFIG_USB_OTG_DWC

# Uncomment for SSIC
#EXTRA_CFLAGS	+= -DSSIC

# Uncomment this to support the UTE gadget, and also uncomment the line with
# "dwc_usb3-objs += linux/ute_if.o" below
#EXTRA_CFLAGS	+= -DDWC_UTE -I$(KBUILD_SRC)/../UTE-3.0/common -I$(KBUILD_EXTMOD)/../UTE-3.0/common

#EXTRA_CFLAGS	+= -DDWC_ISOC_INTR_MODERATION
#EXTRA_CFLAGS	+= -DDWC_TEST_ISOC_CHAIN
#EXTRA_CFLAGS	+= -DLECROY

#EXTRA_CFLAGS	+= -DDWC_STAR_9000446947_WORKAROUND
#EXTRA_CFLAGS	+= -DDWC_STAR_9000449814_WORKAROUND
#EXTRA_CFLAGS	+= -DDWC_STAR_9000459034_WORKAROUND
#EXTRA_CFLAGS	+= -DDWC_STAR_9000463548_WORKAROUND
#EXTRA_CFLAGS	+= -DDWC_STAR_9000468158_WORKAROUND
#EXTRA_CFLAGS	+= -DDWC_STAR_9000483510_WORKAROUND

#obj-m		:= dwc_usb3.o
obj-$(CONFIG_USB_HISI_UDC3)	:= dwc_usb3.o

dwc_usb3-objs	:= cil.o cil_intr.o
dwc_usb3-objs	+= pcd.o pcd_intr.o pcd_hiber.o ep0.o
#dwc_usb3-objs   += hiusb3.o

ifneq ($(NOOS),)
dwc_usb3-objs	+= no_os/no_os_init.o no_os/no_os_hiber.o no_os/no_os_ep0.o
dwc_usb3-objs	+= no_os/no_os_gadget.o no_os/no_os_src_sink_lpbk.o
else
ifneq ($(SELA),)
dwc_usb3-objs	+= sela/sela_bm_defs.o sela/sela_driver.o sela/sela_ep0.o
dwc_usb3-objs	+= sela/sela_funcs.o sela/sela_gadget.o
else
dwc_usb3-objs	+= linux_gadget.o linux_hiber.o linux_sysfs.o
#ifneq ($(PLATFORM),)
dwc_usb3-objs	+= linux_plat.o
#else
#dwc_usb3-objs	+= linux/linux_pci.o
#endif
endif
endif

# Uncomment this to support the UTE gadget, and also uncomment the line with
# "-DDWC_UTE" above
#dwc_usb3-objs	+= linux/ute_if.o

else
endif

#ifneq ($(NOOS),)
#	ln -s no_os/no_os_defs.h os_defs.h
#	ln -s no_os/no_os_dev.h os_dev.h
#else
#ifneq ($(SELA),)
#	ln -s sela/sela_defs.h os_defs.h
#	ln -s sela/sela_dev.h os_dev.h
#else
#	ln -s linux/linux_defs.h os_defs.h
#	ln -s linux/linux_dev.h os_dev.h
#	ln -s linux/ute_if.h ute_if.h
#endif
#endif
