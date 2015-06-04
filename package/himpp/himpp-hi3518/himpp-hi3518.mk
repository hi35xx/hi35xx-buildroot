################################################################################
#
# hi3518-mpp2
#
################################################################################

HIMPP_HI3518_VERSION = 1.0.9.0
HIMPP_HI3518_TARBALL = $(TOPDIR)/package/himpp/himpp-hi3518/mpp2.tgz
HIMPP_HI3518_SITE_METHOD = file
HIMPP_HI3518_SITE = $(patsubst %/,%,$(dir $(HIMPP_HI3518_TARBALL)))
HIMPP_HI3518_SOURCE = $(notdir $(HIMPP_HI3518_TARBALL))
HIMPP_HI3518_INSTALL_STAGING = YES
HIMPP_HI3518_DEPENDENCIES = linux

SENSOR_TYPE = $(call qstrip,$(BR2_PACKAGE_HIMPP_SNS_TYPE))

HIMPP_HI3518_MAKE_OPTS = ARCH=arm
HIMPP_HI3518_MAKE_OPTS += HIARCH=hi3518
HIMPP_HI3518_MAKE_OPTS += LIBC=uclibc
HIMPP_HI3518_MAKE_OPTS += CROSS=$(TARGET_CROSS)
HIMPP_HI3518_MAKE_OPTS += CROSS_COMPILE=$(TARGET_CROSS)
HIMPP_HI3518_MAKE_OPTS += LINUX_ROOT=$(LINUX_DIR)
HIMPP_HI3518_MAKE_OPTS += MPP_PATH=$(@D)/mpp2
HIMPP_HI3518_MAKE_OPTS += SENSOR_TYPE=$(SENSOR_TYPE)

define himpp_build
	pushd $(@D)/mpp2/$(1); \
	  $(MAKE1) $(HIMPP_HI3518_MAKE_OPTS); \
	popd;
endef

define himpp_build_snsdrv
	$(call himpp_build,component/isp2/sensor/$1)
endef

define himpp_build_extdrv
	$(call himpp_build,extdrv/$1)
endef

define HIMPP_HI3518_BUILD_CMDS
	pushd $(@D)/mpp2/lib; \
	for f in *.a; do \
	  $(TARGET_CC) -shared -fPIC -o $${f%.a}.so \
	               -Wl,--whole-archive $$f -Wl,--no-whole-archive; \
	done; \
	popd;
endef

###############################################################################
# build sensor driver
###############################################################################

ifeq ($(BR2_PACKAGE_HIMPP_SNSDRV_APTINA_9M034),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_snsdrv,aptina_9m034)
	SNSDRV_TO_INSTALL += libsns_9m034.so
endif

ifeq ($(BR2_PACKAGE_HIMPP_SNSDRV_APTINA_AR0130),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_snsdrv,aptina_ar0130)
	SNSDRV_TO_INSTALL += libsns_ar0130_720p.so
endif

ifeq ($(BR2_PACKAGE_HIMPP_SNSDRV_APTINA_AR0330),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_snsdrv,aptina_ar0330)
	SNSDRV_TO_INSTALL += libsns_ar0330_1080p.so
endif

ifeq ($(BR2_PACKAGE_HIMPP_SNSDRV_APTINA_MT9P006),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_snsdrv,aptina_mt9p006)
	SNSDRV_TO_INSTALL += libsns_mt9p006.so
endif

ifeq ($(BR2_PACKAGE_HIMPP_SNSDRV_HIMAX_1375),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_snsdrv,himax_1375)
	SNSDRV_TO_INSTALL += libsns_himax1375.so
endif

ifeq ($(BR2_PACKAGE_HIMPP_SNSDRV_ONMI_OV9712),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_snsdrv,ov_9712)
	SNSDRV_TO_INSTALL += libsns_ov9712.so
endif

ifeq ($(BR2_PACKAGE_HIMPP_SNSDRV_ONMI_OV9712_PLUS),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_snsdrv,ov_9712+)
	SNSDRV_TO_INSTALL += libsns_ov9712_plus.so
endif

ifeq ($(BR2_PACKAGE_HIMPP_SNSDRV_PANASONIC_MN34031),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_snsdrv,pana34031)
	SNSDRV_TO_INSTALL += libsns_mn34031_720p.so
endif

ifeq ($(BR2_PACKAGE_HIMPP_SNSDRV_PANASONIC_MN34041),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_snsdrv,pana34041)
	SNSDRV_TO_INSTALL += libsns_mn34041.so
endif

ifeq ($(BR2_PACKAGE_HIMPP_SNSDRV_PIXELPLUS_3100K),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_snsdrv,pixelplus_3100k)
	SNSDRV_TO_INSTALL += libsns_po3100k.so
endif

ifeq ($(BR2_PACKAGE_HIMPP_SNSDRV_SOI_H22),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_snsdrv,soi_h22)
	SNSDRV_TO_INSTALL += libsns_soih22.so
endif

ifeq ($(BR2_PACKAGE_HIMPP_SNSDRV_SONY_ICX692),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_snsdrv,sony_icx692)
	SNSDRV_TO_INSTALL += libsns_icx692.so
endif

ifeq ($(BR2_PACKAGE_HIMPP_SNSDRV_SONY_IMX104),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_snsdrv,sony_imx104)
	SNSDRV_TO_INSTALL += libsns_imx104.so
endif

ifeq ($(BR2_PACKAGE_HIMPP_SNSDRV_SONY_IMX122),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_snsdrv,sony_imx122)
	SNSDRV_TO_INSTALL += libsns_imx122.so
endif

ifeq ($(BR2_PACKAGE_HIMPP_SNSDRV_SONY_IMX138),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_snsdrv,sony_imx138)
	SNSDRV_TO_INSTALL += libsns_imx138.so
endif

ifeq ($(BR2_PACKAGE_HIMPP_SNSDRV_SONY_IMX236),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_snsdrv,sony_imx236)
	SNSDRV_TO_INSTALL += libsns_imx236.so
endif


###############################################################################
# build extdrv
###############################################################################

ifeq ($(BR2_PACKAGE_HIMPP_EXTDRV_GPIO_I2C),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_extdrv,gpio-i2c)
	PROGRAM_TO_INSTALL += extdrv/gpio-i2c/i2c_read \
	                      extdrv/gpio-i2c/i2c_write
	EXTDRV_TO_INSTALL += extdrv/gpio-2ic/gpioi2c.ko
endif

ifeq ($(BR2_PACKAGE_HIMPP_EXTDRV_GPIO_I2C_EX),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_extdrv,gpio-i2c-ex)
	PROGRAM_TO_INSTALL += extdrv/gpio-i2c-ex/i2c_read_ex \
	                      extdrv/gpio-i2c-ex/i2c_write_ex
	EXTDRV_TO_INSTALL += extdrv/gpio-2ic-ex/gpioi2c_ex.ko
endif

ifeq ($(BR2_PACKAGE_HIMPP_EXTDRV_HI_I2C),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_extdrv,hi_i2c)
	PROGRAM_TO_INSTALL += extdrv/hi_i2c/i2c_read \
	                      extdrv/hi_i2c/i2c_write
	EXTDRV_TO_INSTALL += extdrv/hi_i2c/hi_i2c.ko
endif

ifeq ($(BR2_PACKAGE_HIMPP_EXTDRV_PWM),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_extdrv,pwm)
	PROGRAM_TO_INSTALL += extdrv/pwm/pwm_write
	EXTDRV_TO_INSTALL += extdrv/pwm/pwm.ko
endif

ifeq ($(BR2_PACKAGE_HIMPP_EXTDRV_SSP_AD9020),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_extdrv,ssp-ad9020)
	PROGRAM_TO_INSTALL += extdrv/ssp-ad9020/ssp_read \
	                      extdrv/ssp-ad9020/ssp_write
	EXTDRV_TO_INSTALL += extdrv/ssp-ad9020/ssp_ad9020.ko
endif

ifeq ($(BR2_PACKAGE_HIMPP_EXTDRV_SSP_PANA),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_extdrv,ssp-pana)
	PROGRAM_TO_INSTALL += extdrv/ssp-pana/ssp_read \
	                      extdrv/ssp-pana/ssp_write
	EXTDRV_TO_INSTALL += extdrv/ssp-pana/ssp_pana.ko
endif

ifeq ($(BR2_PACKAGE_HIMPP_EXTDRV_SSP_SONY),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_extdrv,ssp-sony)
	PROGRAM_TO_INSTALL += extdrv/ssp-sony/ssp_read \
	                      extdrv/ssp-sony/ssp_write
	EXTDRV_TO_INSTALL += extdrv/ssp-sony/ssp_sony.ko
endif

ifeq ($(BR2_PACKAGE_HIMPP_EXTDRV_TW2865),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build_extdrv,tw2865)
	EXTDRV_TO_INSTALL += extdrv/tw2865/tw2865.ko
endif


###############################################################################
# build samples
###############################################################################

ifeq ($(BR2_PACKAGE_HIMPP_SAMPLES_AUDIO),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build,sample/audio)
	PROGRAM_TO_INSTALL += sample/audio/sample_audio
endif
ifeq ($(BR2_PACKAGE_HIMPP_SAMPLES_HIFB),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build,sample/hifb)
	PROGRAM_TO_INSTALL += sample/hifb/sample_hifb
endif
ifeq ($(BR2_PACKAGE_HIMPP_SAMPLES_IQ),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build,sample/iq)
	PROGRAM_TO_INSTALL += sample/iq/sample_iq
endif
ifeq ($(BR2_PACKAGE_HIMPP_SAMPLES_IVE),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build,sample/ive)
	PROGRAM_TO_INSTALL += sample/ive/sample_ive_canny
	PROGRAM_TO_INSTALL += sample/ive/sample_ive_detect
	PROGRAM_TO_INSTALL += sample/ive/sample_ive_FPN
	PROGRAM_TO_INSTALL += sample/ive/sample_ive_sobel_with_cached_mem
	PROGRAM_TO_INSTALL += sample/ive/sample_ive_test_memory
endif
ifeq ($(BR2_PACKAGE_HIMPP_SAMPLES_REGION),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build,sample/region)
	PROGRAM_TO_INSTALL += sample/region/sample_region
endif
ifeq ($(BR2_PACKAGE_HIMPP_SAMPLES_TDE),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build,sample/tde)
	PROGRAM_TO_INSTALL += sample/tde/sample_tde
endif
ifeq ($(BR2_PACKAGE_HIMPP_SAMPLES_VDA),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build,sample/vda)
	PROGRAM_TO_INSTALL += sample/vda/sample_vda
endif
ifeq ($(BR2_PACKAGE_HIMPP_SAMPLES_VENC),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build,sample/venc)
	PROGRAM_TO_INSTALL += sample/venc/sample_venc
endif
ifeq ($(BR2_PACKAGE_HIMPP_SAMPLES_VIO),y)
	HIMPP_HI3518_BUILD_CMDS += $(call himpp_build,sample/vio)
	PROGRAM_TO_INSTALL += sample/vio/sample_vio
endif


HIMPP_PREFIX = $(call qstrip,$(BR2_PACKAGE_HIMPP_PREFIX))

define HIMPP_HI3518_INSTALL_STAGING_CMDS
	mkdir -p $(STAGING_DIR)$(HIMPP_PREFIX)
	# install header files
	$(RM) -r $(TARGET_DIR)$(HIMPP_PREFIX)/include
	cp -a $(@D)/mpp2/include $(STAGING_DIR)$(HIMPP_PREFIX)
	# install libraries
	$(RM) -r $(TARGET_DIR)$(HIMPP_PREFIX)/lib
	cp -a $(@D)/mpp2/lib $(STAGING_DIR)$(HIMPP_PREFIX)
	# override the sensor driver libraries
	for f in $(SNSDRV_TO_INSTALL); do \
	  $(INSTALL) -D $(@D)/mpp2/component/isp2/lib/$$f \
	             $(STAGING_DIR)$(HIMPP_PREFIX)/lib/$$f \
	  || exit 1; \
	done;
endef

MPPDRV_TO_INSTALL = $(shell cd $(@D)/mpp2 && find ko/ -name *.ko)

SCRIPT_TO_INSTALL = $(shell cd $(@D)/mpp2 && find ko/ -name *.sh)
SCRIPT_TO_INSTALL += ko/load3518 ko/load3518e

MPPLIB_TO_INSTALL = $(shell cd $(@D)/mpp2 && find lib/ -name libsns*.so* -o -name \*.so\* -print0)

define HIMPP_HI3518_INSTALL_TARGET_CMDS
	# install kernel modules and scripts
	$(RM) -r $(TARGET_DIR)$(HIMPP_PREFIX)/ko
	for f in $(MPPDRV_TO_INSTALL); do \
	  $(INSTALL) -D $(@D)/mpp2/$$f \
	             $(TARGET_DIR)$(HIMPP_PREFIX)/$$f \
	  || exit 1; \
	done

	# install extdrv
	for f in $(EXTDRV_TO_INSTALL); do \
	  t=`basename $$f`; \
	  $(INSTALL) -D $(@D)/mpp2/$$f \
	             $(TARGET_DIR)$(HIMPP_PREFIX)/ko/extdrv/$t \
	  || exit 1; \
	done

	# install scripts
	for f in $(SCRIPT_TO_INSTALL); do \
	  $(INSTALL) -D $(@D)/mpp2/$$f \
	             $(TARGET_DIR)$(HIMPP_PREFIX)/$$f \
	  || exit 1; \
	  sed -r -i -e "s/himm([[:space:]]*[^[:space:]]*)/devmem \1 32/" \
	      $(TARGET_DIR)$(HIMPP_PREFIX)/$$f; \
	done

	# install libraries
	$(RM) -r $(TARGET_DIR)$(HIMPP_PREFIX)/lib
	for f in $(MPPLIB_TO_INSTALL); do \
	  $(INSTALL) -D -m 755 $(@D)/mpp2/$$f \
	             $(TARGET_DIR)$(HIMPP_PREFIX)/$$f \
	  || exit 1; \
	done

	# install sensor driver libraries
	for f in $(SNSDRV_TO_INSTALL); do \
	  $(INSTALL) -D $(@D)/mpp2/component/isp2/lib/$$f \
	             $(TARGET_DIR)$(HIMPP_PREFIX)/lib/$$f \
	  || exit 1; \
	done;

	# install samples
	$(RM) -r $(TARGET_DIR)$(HIMPP_PREFIX)/bin
	for f in $(PROGRAM_TO_INSTALL); do \
	  t=`basename $$f`; \
	  $(INSTALL) -D -m 755 $(@D)/mpp2/$$f \
	             $(TARGET_DIR)$(HIMPP_PREFIX)/bin/$$t \
	  || exit 1; \
	  $(TARGET_STRIP) -s \
	             $(TARGET_DIR)$(HIMPP_PREFIX)/bin/$$t; \
	done
endef

$(eval $(generic-package))

