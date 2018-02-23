################################################################################
#
# usbgpio
#
################################################################################

USBGPIO_LICENSE = GPLv2+
USBGPIO_DEPENDENCIES = libusb

define USBGPIO_BUILD_CMDS
	cp -a package/usbgpio/* $(@D)/
	cd $(@D) && $(TARGET_CXX) $(TARGET_CXXFLAGS) $(TARGET_LDFLAGS) \
	-lusb -o usbgpio usbgpio.c opendevice.c \
	-o $(@D)/usbgpio
endef

define USBGPIO_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 755 $(@D)/usbgpio $(TARGET_DIR)/usr/bin/usbgpio
endef

$(eval $(generic-package))
