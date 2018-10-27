ifeq ($(BR2_PACKAGE_HIMPP),y)
CHIP = $(call qstrip,$(BR2_PACKAGE_HIMPP_CHIP))
SENSOR = $(call qstrip,$(BR2_PACKAGE_HIMPP_SENSOR_TYPE))
include package/himpp/himpp-$(CHIP)/himpp-$(CHIP).mk
endif
