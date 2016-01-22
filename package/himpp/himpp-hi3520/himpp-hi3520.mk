################################################################################
#
# hi3520-mpp
#
################################################################################

HIMPP_HI3520_VERSION = 1.0.3.0
HIMPP_HI3520_SITE_METHOD = file
HIMPP_HI3520_SITE = $(TOPDIR)/package/himpp/himpp-hi3520
HIMPP_HI3520_SOURCE = hi3520-mpp-$(HIMPP_HI3520_VERSION).tgz
HIMPP_HI3520_INSTALL_STAGING = YES
HIMPP_HI3520_DEPENDENCIES = linux

SENSOR_TYPE = $(call qstrip,$(BR2_PACKAGE_HIMPP_SNS_TYPE))

HIMPP_HI3520_MAKE_OPTS = ARCH=arm
HIMPP_HI3520_MAKE_OPTS += HIARCH=hi3520A
HIMPP_HI3520_MAKE_OPTS += LIBC=uclibc
HIMPP_HI3520_MAKE_OPTS += CROSS=$(TARGET_CROSS)
HIMPP_HI3520_MAKE_OPTS += CROSS_COMPILE=$(TARGET_CROSS)
HIMPP_HI3520_MAKE_OPTS += LINUX_ROOT=$(LINUX_DIR)
HIMPP_HI3520_MAKE_OPTS += MPP_PATH=$(@D)
HIMPP_HI3520_MAKE_OPTS += SENSOR_TYPE=$(SENSOR_TYPE)

define HIMPP_HI3520_EXTRACT_CMDS
	mkdir -p $(@D)
	$(TAR) -zxf $(DL_DIR)/$(HIMPP_HI3520_SOURCE) \
		--strip-components=2 -C $(@D)
endef

define himpp_build
	( cd $(@D)/$(1) && $(MAKE1) $(HIMPP_HI3520_MAKE_OPTS) ) || exit 1;
endef

define HIMPP_HI3520_BUILD_CMDS
	( cd $(@D)/lib_hi3520A; \
	for f in *.a; do \
	  $(TARGET_CC) -shared -fPIC -o $${f%.a}.so \
	               -Wl,--whole-archive $$f -Wl,--no-whole-archive \
	  || exit 1; \
	done; \
	);
endef

###############################################################################
# build extdrv
###############################################################################

define add_extdrv
ifeq ($$(BR2_PACKAGE_HIMPP_EXTDRV_$(1)),y)
	HIMPP_HI3520_BUILD_CMDS += $$(call himpp_build,extdrv_hi3520A/$(2))
	EXTDRV_TO_INSTALL += $$(patsubst %,extdrv_hi3520A/$(2)/%,$(3))
	PROGRAM_TO_INSTALL += $$(patsubst %,extdrv_hi3520A/$(2)/%,$(4))
endif
endef

$(eval $(call add_extdrv,ADV7179,adv7179,adv_7179.ko))
$(eval $(call add_extdrv,GPIO_I2C,gpio_i2c_8b,gpioi2c.ko,i2c_read i2c_write))
$(eval $(call add_extdrv,IRDA,hisi-irda,hi_ir.ko))
$(eval $(call add_extdrv,I2C,i2c,i2c.ko,i2c_read i2c_write))
$(eval $(call add_extdrv,NVP1918,nvp1918,nvp1918.ko,))
$(eval $(call add_extdrv,SIL9022,sil9022,sil9024.ko))
$(eval $(call add_extdrv,SSP_GV7601,ssp-GV7601,ssp.ko,ssp_read ssp_write))
$(eval $(call add_extdrv,TLV320AIC31,tlv320aic31,tlv_320aic31.ko))
$(eval $(call add_extdrv,TW2865,tw2865,tw2865.ko,))
$(eval $(call add_extdrv,TW2868,tw2868,tw2868.ko,))
$(eval $(call add_extdrv,TW2960,tw2960,tw2960.ko,))


###############################################################################
# build samples
###############################################################################

define add_sample
ifeq ($$(BR2_PACKAGE_HIMPP_SAMPLES_$(call qstrip,$(1))),y)
	HIMPP_HI3520_BUILD_CMDS += $$(call himpp_build,sample/hi3520A/$(2))
	PROGRAM_TO_INSTALL += $$(patsubst %,sample/hi3520A/$(2)/%,$(3))
endif
endef

$(eval $(call add_sample,AUDIO,audio,sample_audio))
$(eval $(call add_sample,HIFB,hifb,sample_hifb))
$(eval $(call add_sample,IT,it,sample_it))
$(eval $(call add_sample,IVE,ive,sample_ive_detect sample_ive_test_memory))
$(eval $(call add_sample,REGION,region,sample_region))
$(eval $(call add_sample,TDE,tde,sample_tde sample_tde_colorkey))
$(eval $(call add_sample,VDA,vda,sample_vda))
$(eval $(call add_sample,VDEC,vdec,sample_vdec))
$(eval $(call add_sample,VENC,venc,sample_venc))
$(eval $(call add_sample,VIO,vio,sample_vio))


HIMPP_PREFIX = $(call qstrip,$(BR2_PACKAGE_HIMPP_PREFIX))

extdrv-header-$(BR2_PACKAGE_HIMPP_EXTDRV_NVP1918) += nvp1918/nvp1918.h
extdrv-header-$(BR2_PACKAGE_HIMPP_EXTDRV_TW2865) += tw2865/tw2865.h
extdrv-header-$(BR2_PACKAGE_HIMPP_EXTDRV_TW2868) += tw2868/tw2868.h
extdrv-header-$(BR2_PACKAGE_HIMPP_EXTDRV_TW2960) += tw2960/tw2960.h

define HIMPP_HI3520_INSTALL_STAGING_CMDS
	mkdir -p $(STAGING_DIR)$(HIMPP_PREFIX)/include/hi3520mpp
	mkdir -p $(STAGING_DIR)$(HIMPP_PREFIX)/lib
	# install header files
	cp -a $(@D)/include_hi3520A/* \
	    $(STAGING_DIR)$(HIMPP_PREFIX)/include/hi3520mpp/
	# install header files for extdrv
	for f in $(extdrv-header-y); do \
	    $(INSTALL) -m 0664 $(@D)/extdrv_hi3520A/$$f \
	               $(STAGING_DIR)$(HIMPP_PREFIX)/include/hi3520mpp/; \
	done
	# install libraries
	cp -a $(@D)/lib_hi3520A/* \
	    $(STAGING_DIR)$(HIMPP_PREFIX)/lib/
endef

MPPDRV_TO_INSTALL = $(shell cd $(@D)/ko_hi3520A && find -name \*.ko)
SCRIPT_TO_INSTALL = $(shell cd $(@D)/ko_hi3520A && find -name \*.sh)
MPPLIB_TO_INSTALL = $(shell cd $(@D) && find lib_hi3520A/ -name \*.so\*)
MPPDRV_TARGET_DIR = $(TARGET_DIR)$(HIMPP_PREFIX)/lib/himpp-ko

define HIMPP_TARGET_INSTALL_MPPDRV
	for f in $(MPPDRV_TO_INSTALL); do \
	  $(INSTALL) -D $(@D)/ko_hi3520A/$$f \
	             $(MPPDRV_TARGET_DIR)/$$f \
	  || exit 1; \
	  $(TARGET_STRIP) -R .note -g --strip-unneeded \
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
	  $(INSTALL) -D $(@D)/ko_hi3520A/$$f \
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

define HIMPP_HI3520_INSTALL_TARGET_CMDS
	$(HIMPP_TARGET_INSTALL_MPPDRV)
	$(HIMPP_TARGET_INSTALL_EXTDRV)
	$(HIMPP_TARGET_INSTALL_SCRIPTS)
	$(HIMPP_TARGET_INSTALL_LIBRARIES)
	$(HIMPP_TARGET_INSTALL_PROGRAMS)
	$(INSTALL) -m 0755 -D package/himpp/himpp-hi3520/load3520.sh \
	    $(MPPDRV_TARGET_DIR)/load3520.sh
endef

define HIMPP_HI3520_INSTALL_INIT_SYSV
	$(INSTALL) -m 0755 -D package/himpp/himpp-hi3520/S25himpp \
	    $(TARGET_DIR)/etc/init.d/S25himpp
	sed -i "s;^HIMPP_PREFIX=.*$$;HIMPP_PREFIX=$(HIMPP_PREFIX);" \
	    $(TARGET_DIR)/etc/init.d/S25himpp
endef

$(eval $(generic-package))

