################################################################################
#
# hi3516cv300-mpp
#
################################################################################

HIMPP_SDK_VERSION = $(call qstrip,$(BR2_PACKAGE_HIMPP_HI3516CV300_SDK_VERSION))
HIMPP_SDK_SRCDIR = $(TOPDIR)/package/himpp/himpp-hi3516cv300/

HIMPP_HI3516CV300_INSTALL_STAGING = YES
HIMPP_HI3516CV300_DEPENDENCIES = linux

HIMPP_HI3516CV300_MAKE_OPTS += CROSS=$(TARGET_CROSS)
HIMPP_HI3516CV300_MAKE_OPTS += CROSS_COMPILE=$(TARGET_CROSS)
HIMPP_HI3516CV300_MAKE_OPTS += KERNEL_ROOT=$(LINUX_DIR)

define HIMPP_HI3516CV300_EXTRACT_CMDS
	mkdir -p $(@D)
	$(TAR) -zxf $(HIMPP_SDK_SRCDIR)/$(HIMPP_SDK_VERSION)/osal.tgz \
		--strip-components=1 -C $(@D)
	$(TAR) -zxf $(HIMPP_SDK_SRCDIR)/$(HIMPP_SDK_VERSION)/mpp.tgz \
		--strip-components=1 -C $(@D)
	$(TAR) -zxf $(HIMPP_SDK_SRCDIR)/$(HIMPP_SDK_VERSION)/drv.tgz \
		--strip-components=0 -C $(@D)
	for f in `ls $(HIMPP_SDK_SRCDIR)/$(HIMPP_SDK_VERSION)/*.patch`; do \
		patch -d $(@D) -p1 -i $$f; \
	done
endef

define HIMPP_HI3516CV300_BUILD_MPP
	$(MAKE) -C $(@D)/mpp $(HIMPP_HI3516CV300_MAKE_OPTS)
	$(MAKE) -C $(@D)/mpp/component/isp $(HIMPP_HI3516CV300_MAKE_OPTS)
endef

define HIMPP_HI3516CV300_BUILD_CMDS
	( cd $(@D)/mpp/lib; \
	for f in *.a; do \
	  $(TARGET_CC) -shared -fPIC -o $${f%.a}.so \
	               -Wl,--whole-archive $$f -Wl,--no-whole-archive \
	  || exit 1; \
	done; \
	);
	$(HIMPP_HI3516CV300_BUILD_MPP)
endef


HIMPP_PREFIX = $(call qstrip,$(BR2_PACKAGE_HIMPP_PREFIX))

define HIMPP_HI3516CV300_INSTALL_STAGING_CMDS
	mkdir -p $(STAGING_DIR)$(HIMPP_PREFIX)/include/hi3516cv300mpp
	mkdir -p $(STAGING_DIR)$(HIMPP_PREFIX)/lib
	# install header files
	cp -a $(@D)/mpp/include/* \
	    $(STAGING_DIR)$(HIMPP_PREFIX)/include/hi3516cv300mpp/
	# install libraries
	cp -a $(@D)/mpp/lib/* \
	    $(STAGING_DIR)$(HIMPP_PREFIX)/lib/
endef

MPPDRV_TO_INSTALL = $(shell cd $(@D)/mpp/ko && find -name \*.ko)
SCRIPT_TO_INSTALL = $(shell cd $(@D)/mpp/ko && find -name \*.sh)
MPPLIB_TO_INSTALL = $(shell cd $(@D)/mpp && find lib/ -name \*.so\*)

MPPDRV_TARGET_DIR = $(TARGET_DIR)$(HIMPP_PREFIX)/lib/himpp-ko

define HIMPP_TARGET_INSTALL_MPPDRV
	for f in $(MPPDRV_TO_INSTALL); do \
	  $(INSTALL) -D $(@D)/mpp/ko/$$f \
	             $(MPPDRV_TARGET_DIR)/$$f \
	  || exit 1; \
	  $(TARGET_STRIP) -R .note -g --strip-unneeded \
	             $(MPPDRV_TARGET_DIR)/$$f; \
	done
endef

define HIMPP_TARGET_INSTALL_SCRIPTS
	for f in $(SCRIPT_TO_INSTALL); do \
	  $(INSTALL) -D $(@D)/mpp/ko/$$f \
	             $(MPPDRV_TARGET_DIR)/$$f \
	  || exit 1; \
	  sed -r -i -e "s/himm([[:space:]]*[^[:space:]]*)/devmem \1 32/" \
	      $(MPPDRV_TARGET_DIR)/$$f; \
	done
	$(INSTALL) -m 0755 -D package/himpp/himpp-hi3516cv300/$(HIMPP_SDK_VERSION)/load3516cv300 \
	    $(MPPDRV_TARGET_DIR)/load3516cv300
endef

define HIMPP_TARGET_INSTALL_LIBRARIES
	for f in $(MPPLIB_TO_INSTALL) $(SNSDRV_TO_INSTALL); do \
	  t=`basename $$f`; \
	  $(INSTALL) -D -m 0755 $(@D)/mpp/$$f \
	             $(TARGET_DIR)$(HIMPP_PREFIX)/lib/$$t \
	  || exit 1; \
	  $(TARGET_STRIP) --strip-all \
	             $(TARGET_DIR)$(HIMPP_PREFIX)/lib/$$t; \
	done
endef

define HIMPP_HI3516CV300_INSTALL_TARGET_CMDS
	$(HIMPP_TARGET_INSTALL_MPPDRV)
	$(HIMPP_TARGET_INSTALL_SCRIPTS)
	$(HIMPP_TARGET_INSTALL_LIBRARIES)
	$(HIMPP_TARGET_INSTALL_PROGRAMS)
endef

define HIMPP_HI3516CV300_INSTALL_INIT_SYSV
	$(INSTALL) -m 0755 -D package/himpp/himpp-hi3516cv300/S70himpp \
	    $(TARGET_DIR)/etc/init.d/S70himpp
	sed -i "s;^HIMPP_PREFIX=.*$$;HIMPP_PREFIX=$(HIMPP_PREFIX);" \
	    $(TARGET_DIR)/etc/init.d/S70himpp
endef

$(eval $(generic-package))

