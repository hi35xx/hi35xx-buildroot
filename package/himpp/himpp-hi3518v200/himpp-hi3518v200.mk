################################################################################
#
# hi3518v200-mpp
#
################################################################################

HIMPP_HI3518V200_VERSION = $(call qstrip,$(BR2_PACKAGE_HIMPP_HI3518V200_SDK_VERSION))
HIMPP_HI3518V200_SITE_METHOD = file
HIMPP_HI3518V200_SITE = $(TOPDIR)/package/himpp/himpp-hi3518v200
HIMPP_HI3518V200_SOURCE = hi3518v200-mpp-$(HIMPP_HI3518V200_VERSION).tgz
HIMPP_HI3518V200_INSTALL_STAGING = YES
HIMPP_HI3518V200_DEPENDENCIES = linux

HIMPP_HI3518V200_MAKE_OPTS  = ARCH=$(ARCH)
HIMPP_HI3518V200_MAKE_OPTS += HIARCH=hi3518e
HIMPP_HI3518V200_MAKE_OPTS += LIBC=uclibc
HIMPP_HI3518V200_MAKE_OPTS += CROSS=$(TARGET_CROSS)
HIMPP_HI3518V200_MAKE_OPTS += CROSS_COMPILE=$(TARGET_CROSS)
HIMPP_HI3518V200_MAKE_OPTS += SDK_PATH=$(@D)
HIMPP_HI3518V200_MAKE_OPTS += LINUX_ROOT=$(LINUX_DIR)

SENSOR_TYPE=OMNIVISION_OV9712_DC_720P_30FPS

define HIMPP_HI3518V200_EXTRACT_CMDS
	mkdir -p $(@D)
	$(TAR) -zxf $(DL_DIR)/$(HIMPP_HI3518V200_SOURCE) \
		--strip-components=2 -C $(@D)
endef

define HIMPP_HI3518V200_BUILD_ISP
	( cd $(@D)/component/isp; \
	  $(MAKE1) $(HIMPP_HI3518V200_MAKE_OPTS) || exit 1; \
	  for f in $$(ls ko/*.ko); do cp -vf $$f $(@D)/ko/; done; \
	  for f in $$(ls lib/*.*); do cp -vf $$f $(@D)/lib/; done; \
	) || exit 1;
endef

define HIMPP_HI3518V200_BUILD_EXTDRV
	( cd $(@D)/extdrv; \
	  $(MAKE1) $(HIMPP_HI3518V200_MAKE_OPTS); ) \
	|| exit 1
endef

define HIMPP_HI3518V200_BUILD_SAMPLES
	( cd $(@D)/sample; \
	  $(MAKE1) $(HIMPP_HI3518V200_MAKE_OPTS) \
		SENSOR_TYPE=$(SENSOR_TYPE); ) \
	|| exit 1
endef

define HIMPP_HI3518V200_BUILD_CMDS
	( cd $(@D)/lib; \
	for f in *.a; do \
	  $(TARGET_CC) -shared -fPIC -o $${f%.a}.so \
	               -Wl,--whole-archive $$f -Wl,--no-whole-archive \
	  || exit 1; \
	done; \
	);
	$(HIMPP_HI3518V200_BUILD_ISP)
	$(HIMPP_HI3518V200_BUILD_EXTDRV)
	$(HIMPP_HI3518V200_BUILD_SAMPLES)
endef


HIMPP_PREFIX = $(call qstrip,$(BR2_PACKAGE_HIMPP_PREFIX))

define HIMPP_HI3518V200_INSTALL_STAGING_CMDS
	mkdir -p $(STAGING_DIR)$(HIMPP_PREFIX)/include/hi3518v200mpp
	mkdir -p $(STAGING_DIR)$(HIMPP_PREFIX)/lib
	# install header files
	cp -a $(@D)/include/* \
	    $(STAGING_DIR)$(HIMPP_PREFIX)/include/hi3518v200mpp/
	# install libraries
	cp -a $(@D)/lib/* \
	    $(STAGING_DIR)$(HIMPP_PREFIX)/lib/
endef

MPPDRV_TO_INSTALL = $(shell cd $(@D)/ko && find -name \*.ko)
SCRIPT_TO_INSTALL = $(shell cd $(@D)/ko && find -name \*.sh)
MPPLIB_TO_INSTALL = $(shell cd $(@D) && find lib/ -name \*.so\*)

MPPDRV_TARGET_DIR = $(TARGET_DIR)$(HIMPP_PREFIX)/lib/himpp-ko

define HIMPP_TARGET_INSTALL_MPPDRV
	for f in $(MPPDRV_TO_INSTALL); do \
	  $(INSTALL) -D $(@D)/ko/$$f \
	             $(MPPDRV_TARGET_DIR)/$$f \
	  || exit 1; \
	  $(TARGET_STRIP) -R .note -g --strip-unneeded \
	             $(MPPDRV_TARGET_DIR)/$$f; \
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

define HIMPP_HI3518V200_INSTALL_TARGET_CMDS
	$(HIMPP_TARGET_INSTALL_MPPDRV)
	$(HIMPP_TARGET_INSTALL_SCRIPTS)
	$(HIMPP_TARGET_INSTALL_LIBRARIES)
	$(HIMPP_TARGET_INSTALL_PROGRAMS)
	$(INSTALL) -m 0755 -D package/himpp/himpp-hi3518v200/$(HIMPP_HI3518V200_VERSION)/load3518e.sh \
	    $(MPPDRV_TARGET_DIR)/load3518e.sh
endef

define HIMPP_HI3518V200_INSTALL_INIT_SYSV
	$(INSTALL) -m 0755 -D package/himpp/himpp-hi3518v200/S70himpp \
	    $(TARGET_DIR)/etc/init.d/S70himpp
	sed -i "s;^HIMPP_PREFIX=.*$$;HIMPP_PREFIX=$(HIMPP_PREFIX);" \
	    $(TARGET_DIR)/etc/init.d/S70himpp
endef

$(eval $(generic-package))

