ifeq ($(BR2_PACKAGE_HIMPP),y)
CHIP = $(call qstrip,$(BR2_PACKAGE_HIMPP_CHIP))
include package/himpp/himpp-$(CHIP)/himpp-$(CHIP).mk
endif
