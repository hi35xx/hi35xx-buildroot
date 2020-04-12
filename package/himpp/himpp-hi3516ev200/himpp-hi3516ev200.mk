################################################################################
#
# hi3516ev200-mpp
#
################################################################################

HIMPP_SDK_VERSION = $(call qstrip,$(BR2_PACKAGE_HIMPP_HI3516EV200_SDK_VERSION))
HIMPP_SDK_SRCDIR = $(TOPDIR)/package/himpp/himpp-hi3516ev200/

HIMPP_HI3516EV200_INSTALL_STAGING = YES
HIMPP_HI3516EV200_DEPENDENCIES = linux

HIMPP_HI3516EV200_MAKE_OPTS += CROSS=$(TARGET_CROSS)
HIMPP_HI3516EV200_MAKE_OPTS += CROSS_COMPILE=$(TARGET_CROSS)
HIMPP_HI3516EV200_MAKE_OPTS += KERNEL_ROOT=$(LINUX_DIR)

define HIMPP_HI3516EV200_EXTRACT_CMDS
	mkdir -p $(@D)
	$(TAR) -zxf $(HIMPP_SDK_SRCDIR)/$(HIMPP_SDK_VERSION)/osal.tgz \
		--strip-components=1 -C $(@D)
	$(TAR) -zxf $(HIMPP_SDK_SRCDIR)/$(HIMPP_SDK_VERSION)/mpp.tgz \
		--strip-components=1 -C $(@D)
	$(TAR) -zxf $(HIMPP_SDK_SRCDIR)/$(HIMPP_SDK_VERSION)/drv.tgz \
		--strip-components=0 -C $(@D)
	for f in `ls $(HIMPP_SDK_SRCDIR)/$(HIMPP_SDK_VERSION)/*.patch | sort`; \
	do \
		patch -d $(@D) -p1 -i $$f; \
	done
endef


define HIMPP_HI3516EV200_BUILD_DIR
	( cd $1; $(MAKE) $(HIMPP_HI3516EV200_MAKE_OPTS) -C $1 )
endef

define HIMPP_HI3516EV200_BUILD_CMDS
	( cd $(@D)/mpp/lib; \
	for f in *.a; do \
	  $(TARGET_CC) -shared -fPIC -o $${f%.a}.so \
	               -Wl,--whole-archive $$f -Wl,--no-whole-archive \
	  || exit 0; \
	done; \
	);
	$(call HIMPP_HI3516EV200_BUILD_DIR, $(@D)/osal/linux/kernel)
	$(call HIMPP_HI3516EV200_BUILD_DIR, $(@D)/mpp/component/isp)
	$(call HIMPP_HI3516EV200_BUILD_DIR, $(@D)/drv/interdrv)
	$(call HIMPP_HI3516EV200_BUILD_DIR, $(@D)/drv/extdrv)
endef


HIMPP_PREFIX = $(call qstrip,$(BR2_PACKAGE_HIMPP_PREFIX))

define HIMPP_HI3516EV200_INSTALL_STAGING_CMDS
	mkdir -p $(STAGING_DIR)$(HIMPP_PREFIX)/include/hi3516ev200mpp
	mkdir -p $(STAGING_DIR)$(HIMPP_PREFIX)/lib
	# install header files
	cp -a $(@D)/mpp/include/* \
	    $(STAGING_DIR)$(HIMPP_PREFIX)/include/hi3516ev200mpp/
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

ifeq ($(BR2_PACKAGE_HIMPP_HI3516EV200_BOARD_HI3516EV300),y)
	LOADSCRIPT = load3516ev300
else ifeq ($(BR2_PACKAGE_HIMPP_HI3516EV200_BOARD_HI3518EV300),y)
	LOADSCRIPT = load3518ev300
else
	LOADSCRIPT = load3516ev200
endif

define HIMPP_TARGET_INSTALL_SCRIPTS
	for f in $(SCRIPT_TO_INSTALL); do \
	  $(INSTALL) -D $(@D)/mpp/ko/$$f \
	             $(MPPDRV_TARGET_DIR)/$$f \
	  || exit 1; \
	done
	$(INSTALL) -m 0755 -D $(@D)/mpp/ko/$(LOADSCRIPT) \
	    $(MPPDRV_TARGET_DIR)/load3516ev200
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

define HIMPP_HI3516EV200_INSTALL_TARGET_CMDS
	$(HIMPP_TARGET_INSTALL_MPPDRV)
	$(HIMPP_TARGET_INSTALL_SCRIPTS)
	$(HIMPP_TARGET_INSTALL_LIBRARIES)
	$(HIMPP_TARGET_INSTALL_PROGRAMS)
endef

define HIMPP_HI3516EV200_INSTALL_INIT_SYSV
	$(INSTALL) -m 0755 -D package/himpp/himpp-hi3516ev200/S70himpp \
	    $(TARGET_DIR)/etc/init.d/S70himpp
	sed -i "s;^HIMPP_PREFIX=.*$$;HIMPP_PREFIX=$(HIMPP_PREFIX);" \
	    $(TARGET_DIR)/etc/init.d/S70himpp
endef

$(eval $(generic-package))
