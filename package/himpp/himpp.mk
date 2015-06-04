ifeq ($(BR2_PACKAGE_HIMPP),y)
CHIP = $(call qstrip,$(BR2_PACKAGE_HIMPP_CHIP))
include package/himpp/$(CHIP)/$(CHIP).mk
endif
