################################################################################
#
# hi3518-mpp2
#
################################################################################

HIMPP_HI3518_VERSION = 1.0.A.0
HIMPP_HI3518_SITE_METHOD = file
HIMPP_HI3518_SITE = $(TOPDIR)/package/himpp/himpp-hi3518
HIMPP_HI3518_SOURCE = hi3518-mpp2-$(HIMPP_HI3518_VERSION).tgz
HIMPP_HI3518_INSTALL_STAGING = YES
HIMPP_HI3518_DEPENDENCIES = linux

SENSOR_TYPE = $(call qstrip,$(BR2_PACKAGE_HIMPP_SNS_TYPE))

HIMPP_HI3518_MAKE_OPTS = ARCH=arm
HIMPP_HI3518_MAKE_OPTS += HIARCH=hi3518
HIMPP_HI3518_MAKE_OPTS += LIBC=uclibc
HIMPP_HI3518_MAKE_OPTS += CROSS=$(TARGET_CROSS)
HIMPP_HI3518_MAKE_OPTS += CROSS_COMPILE=$(TARGET_CROSS)
HIMPP_HI3518_MAKE_OPTS += LINUX_ROOT=$(LINUX_DIR)
HIMPP_HI3518_MAKE_OPTS += MPP_PATH=$(@D)
HIMPP_HI3518_MAKE_OPTS += SENSOR_TYPE=$(SENSOR_TYPE)

define HIMPP_HI3518_EXTRACT_CMDS
	mkdir -p $(@D)
	$(TAR) -zxf $(DL_DIR)/$(HIMPP_HI3518_SOURCE) \
		--strip-components=2 -C $(@D)
endef

define himpp_build

	( cd $(@D)/$(1) && $(MAKE1) $(HIMPP_HI3518_MAKE_OPTS) ) || exit 1;

endef

define HIMPP_HI3518_BUILD_CMDS
	pushd $(@D)/lib; \
	for f in *.a; do \
	  $(TARGET_CC) -shared -fPIC -o $${f%.a}.so \
	               -Wl,--whole-archive $$f -Wl,--no-whole-archive; \
	done; \
	popd;
endef

###############################################################################
# build sensor driver
###############################################################################

define add_snsdrv
ifeq ($$(BR2_PACKAGE_HIMPP_SNSDRV_$(call qstrip,$(1))),y)
	HIMPP_HI3518_BUILD_CMDS += \
		$$(call himpp_build,component/isp2/sensor/$$(call qstrip,$(2)))
	SNSDRV_TO_INSTALL += component/isp2/lib/$$(patsubst %,libsns_%.so,$(3))
endif
endef

$(eval $(call add_snsdrv,APTINA_9M034,aptina_9m034, 9m034))
$(eval $(call add_snsdrv,APTINA_AR0130,aptina_ar0130,ar0130_720p))
$(eval $(call add_snsdrv,APTINA_AR0330,aptina_ar0330,ar0330_1080p))
$(eval $(call add_snsdrv,APTINA_MT9P006,aptina_mt9p006,mt9p006))
$(eval $(call add_snsdrv,GALAXYCORE_GC1004,galaxycore_gc1004,gc1004))
$(eval $(call add_snsdrv,HIMAX_1375,himax_1375,himax1375))
$(eval $(call add_snsdrv,ONMI_OV9712,ov_9712,ov9712))
$(eval $(call add_snsdrv,ONMI_OV9712,ov_9712+,ov9712_plus))
$(eval $(call add_snsdrv,PANASONIC_MN34031,pana34031,mn34031_720p))
$(eval $(call add_snsdrv,PANASONIC_MN34041,pana34041,mn34041))
$(eval $(call add_snsdrv,PIXELPLUS_3100K,pixelplus_3100k,po3100k))
$(eval $(call add_snsdrv,SOI_H22,soi_h22,soih22))
$(eval $(call add_snsdrv,SONY_ICX692,sony_icx692,icx692))
$(eval $(call add_snsdrv,SONY_IMX104,sony_imx104,imx104))
$(eval $(call add_snsdrv,SONY_IMX122,sony_imx122,imx122))
$(eval $(call add_snsdrv,SONY_IMX138,sony_imx138,imx138))
$(eval $(call add_snsdrv,SONY_IMX236,sony_imx236,imx236))


###############################################################################
# build extdrv
###############################################################################

define add_extdrv
ifeq ($$(BR2_PACKAGE_HIMPP_EXTDRV_$(1)),y)
	HIMPP_HI3518_BUILD_CMDS += $$(call himpp_build,extdrv/$(2))
	EXTDRV_TO_INSTALL += $$(patsubst %,extdrv/$(2)/%,$(3))
	PROGRAM_TO_INSTALL += $$(patsubst %,extdrv/$(2)/%,$(4))
endif
endef

$(eval $(call add_extdrv,GPIO_I2C,gpio-i2c,gpioi2c.ko,i2c_read i2c_write))
$(eval $(call add_extdrv,GPIO_I2C_EX,gpio-i2c-ex,gpioi2c_ex.ko,i2c_read_ex i2c_write_ex))
$(eval $(call add_extdrv,HI_I2C,hi_i2c,hi_i2c.ko,i2c_read i2c_write))
$(eval $(call add_extdrv,PWM,pwm,pwm.ko,pwm_write))
$(eval $(call add_extdrv,SSP_AD9020,ssp-ad9020,ssp_ad9020.ko,ssp_read ssp_write))
$(eval $(call add_extdrv,SSP_PANA,ssp-pana,ssp_pana.ko,ssp_read ssp_write))
$(eval $(call add_extdrv,SSP_SONY,ssp-sony,ssp_sony.ko,ssp_read ssp_write))
$(eval $(call add_extdrv,TW2865,tw2865,tw2865.ko,))


###############################################################################
# build samples
###############################################################################

define add_sample
ifeq ($$(BR2_PACKAGE_HIMPP_SAMPLES_$(call qstrip,$(1))),y)
	HIMPP_HI3518_BUILD_CMDS += $$(call himpp_build,sample/$(2))
	PROGRAM_TO_INSTALL += $$(patsubst %,sample/$(2)/%,$(3))
endif
endef

$(eval $(call add_sample,AUDIO,audio,sample_audio))
$(eval $(call add_sample,HIFB,hifb,sample_hifb))
$(eval $(call add_sample,IQ,iq,sample_iq))
$(eval $(call add_sample,IVE,ive,sample_ive_canny sample_ive_detect \
		sample_ive_FPN sample_ive_sobel_with_cached_mem \
		sample_ive_test_memory))
$(eval $(call add_sample,REGION,region,sample_region))
$(eval $(call add_sample,TDE,tde,sample_tde))
$(eval $(call add_sample,VDA,vda,sample_vda))
$(eval $(call add_sample,VENC,venc,sample_venc))
$(eval $(call add_sample,VIO,vio,sample_vio))


HIMPP_PREFIX = $(call qstrip,$(BR2_PACKAGE_HIMPP_PREFIX))

define HIMPP_HI3518_INSTALL_STAGING_CMDS
	mkdir -p $(STAGING_DIR)$(HIMPP_PREFIX)
	# install header files
	cp -a $(@D)/include $(STAGING_DIR)$(HIMPP_PREFIX)
	# tw2835
	$(INSTALL) -m 0664 $(@D)/extdrv/tw2865/tw2865.h \
                   $(STAGING_DIR)$(HIMPP_PREFIX)/include
	# install libraries
	cp -a $(@D)/lib $(STAGING_DIR)$(HIMPP_PREFIX)
	# override the sensor driver libraries
	for f in $(SNSDRV_TO_INSTALL); do \
	  $(INSTALL) -D $(@D)/$$f \
	             $(STAGING_DIR)$(HIMPP_PREFIX)/lib/$$f \
	  || exit 1; \
	done;
endef

MPPDRV_TO_INSTALL = $(shell cd $(@D)/ko && find -name \*.ko)
SCRIPT_TO_INSTALL = $(shell cd $(@D)/ko && find -name \*.sh)
MPPLIB_TO_INSTALL = $(shell cd $(@D) && find lib/ -name libsns\*.so\* -o -name \*.so\* -print)
MPPDRV_TARGET_DIR = $(TARGET_DIR)$(HIMPP_PREFIX)/lib/himpp-ko

define HIMPP_TARGET_INSTALL_MPPDRV
	for f in $(MPPDRV_TO_INSTALL); do \
	  $(INSTALL) -D $(@D)/ko/$$f \
	             $(MPPDRV_TARGET_DIR)/$$f \
	  || exit 1; \
	  $(TARGET_STRIP) --strip-unneeded \
	             $(MPPDRV_TARGET_DIR)/$$f; \
	done
endef

define HIMPP_TARGET_INSTALL_EXTDRV
	for f in $(EXTDRV_TO_INSTALL); do \
	  t=`basename $$f`; \
	  $(INSTALL) -D $(@D)/$$f \
	             $(MPPDRV_TARGET_DIR)/extdrv/$$t \
	  || exit 1; \
	  $(TARGET_STRIP) --strip-unneeded \
	             $(MPPDRV_TARGET_DIR)/extdrv/$$t; \
	done
endef

define HIMPP_TARGET_INSTALL_SCRIPTS
	for f in $(SCRIPT_TO_INSTALL); do \
	  $(INSTALL) -D $(@D)/ko/$$f \
	             $(MPPDRV_TARGET_DIR)/$$f \
	  || exit 1; \
	  sed -r -i -e "s/himm([[:space:]]*[^[:space:]]*)/devmem \1 32/" \
	      $(MPPDRV_TARGET_DIR)/$$f; \
	done
endef

define HIMPP_TARGET_INSTALL_LIBRARIES
	for f in $(MPPLIB_TO_INSTALL) $(SNSDRV_TO_INSTALL); do \
	  t=`basename $$f`; \
	  $(INSTALL) -D -m 0755 $(@D)/$$f \
	             $(TARGET_DIR)$(HIMPP_PREFIX)/lib/$$t \
	  || exit 1; \
	  $(TARGET_STRIP) --strip-all \
	             $(TARGET_DIR)$(HIMPP_PREFIX)/lib/$$t; \
	done
endef

define HIMPP_TARGET_INSTALL_PROGRAMS
	for f in $(PROGRAM_TO_INSTALL); do \
	  t=`basename $$f`; \
	  $(INSTALL) -D -m 0755 $(@D)/$$f \
	             $(TARGET_DIR)$(HIMPP_PREFIX)/bin/$$t \
	  || exit 1; \
	  $(TARGET_STRIP) --strip-all \
	             $(TARGET_DIR)$(HIMPP_PREFIX)/bin/$$t; \
	done
endef

define HIMPP_HI3518_INSTALL_TARGET_CMDS
	$(HIMPP_TARGET_INSTALL_MPPDRV)
	$(HIMPP_TARGET_INSTALL_EXTDRV)
	$(HIMPP_TARGET_INSTALL_SCRIPTS)
	$(HIMPP_TARGET_INSTALL_LIBRARIES)
	$(HIMPP_TARGET_INSTALL_PROGRAMS)
	$(INSTALL) -m 0755 -D package/himpp/himpp-hi3518/load3518.sh \
	    $(MPPDRV_TARGET_DIR)/load3518.sh
endef

define HIMPP_HI3518_INSTALL_INIT_SYSV
	$(INSTALL) -m 0755 -D package/himpp/himpp-hi3518/S25himpp \
	    $(TARGET_DIR)/etc/init.d/S25himpp
	sed -r -i -e "s;^HIMPP_PREFIX=.*$$;HIMPP_PREFIX=$(HIMPP_PREFIX);" \
	    $(TARGET_DIR)/etc/init.d/S25himpp
endef

$(eval $(generic-package))

