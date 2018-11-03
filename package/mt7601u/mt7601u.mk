################################################################################
#
# mt7601u
#
################################################################################

MT7601U_SITE_METHOD = file
MT7601U_SITE = $(TOPDIR)/package/mt7601u
MT7601U_SOURCE = mt7601u.tar.gz
MT7601U_INSTALL_STAGING = YES
MT7601U_DEPENDENCIES = linux
MT7601U_LINUX_SRC = $(LINUX_DIR)/../linux-3.4.y
MT7601U_CHIPSET = 7601U
MT7601U_MODULE = $(word 1, $(MT7601U_CHIPSET))



MT7601U_MAKE_OPTS  = ARCH=$(ARCH)
MT7601U_MAKE_OPTS += CROSS_COMPILE=$(TARGET_CROSS)
MT7601U_MAKE_OPTS += PLATFORM=HI3518C
MT7601U_MAKE_OPTS += CHIPSET=$(MT7601U_CHIPSET)
MT7601U_MAKE_OPTS += LINUX_SRC=$(MT7601U_LINUX_SRC)
MT7601U_MAKE_OPTS += TARGET=LINUX
MT7601U_MAKE_OPTS += MODULE=$(MT7601U_MODULE)
MT7601U_MAKE_OPTS += OSABL=NO
MT7601U_MAKE_OPTS += RT28xx_MODE=STA

MT7601U_MAKE_OPTS += RELEASE=DPO

MT7601U_DAT_PATH = etc/Wireless/RT2870STA


MT7601U_LINUX_SRC_MODULE = $(TARGET_DIR)/lib/modules/3.4.35/kernel/net/wireless/
MT7601U_PREFIX = $(call qstrip,$(BR2_PACKAGE_MT7601U_PREFIX))
MT7601U_TARGET_DIR = $(TARGET_DIR)/$(MT7601U_PREFIX)/lib/MT7601U

define MT7601U_EXTRACT_CMDS
	mkdir -p $(@D)
	mkdir $(@D)/ko;
	$(TAR) -zxf $(DL_DIR)/$(MT7601U_SOURCE) -C $(@D)
	if [ ! -d $(LINUX_DIR)/../linux-3.4.y ]; then \
        ln -s $(LINUX_DIR)/ $(LINUX_DIR)/../linux-3.4.y; \
    fi
endef

define MT7601U_BUILD_CMDS

	( $(MAKE) -C $(@D)/tools || exit 1; \
		cp -f $(@D)/os/linux/Makefile.6 $(@D)/os/linux/Makefile; \
		$(MAKE) $(MT7601U_MAKE_OPTS) \
			RTMP_SRC_DIR=$(@D)/RT$(MT7601U_MODULE) \
			RT28xx_DIR=$(@D) \
			-C $(MT7601U_LINUX_SRC) \
		 	SUBDIRS=$(@D)/os/linux modules || exit 1; \
	 	for f in $$(ls $(@D)/os/linux/*.ko); do \
	 		$(TARGET_STRIP) -R .note -g --strip-unneeded $$f; \
	 		cp -vf $$f $(@D)/ko/; \
 		done; \
	) || exit 1;
endef

define MT7601U_TARGET_INSTALL_DRV
	if [ ! -d $(TARGET_DIR)/$(MT7601U_DAT_PATH) ]; then \
        mkdir -p $(TARGET_DIR)/$(MT7601U_DAT_PATH); \
    fi;
    cp -f $(@D)/RT2870STA.dat $(TARGET_DIR)/$(MT7601U_DAT_PATH)/;
    $(INSTALL) -d $(MT7601U_LINUX_SRC_MODULE);
    $(INSTALL) -m 644 $(@D)/ko/$(addsuffix .ko,mt$(MT7601U_MODULE)sta) \
		$(MT7601U_LINUX_SRC_MODULE);
	( $(HOST_DIR)/sbin/depmod -a 3.4.35 -b $(TARGET_DIR); \
	) || exit 1;
endef

define MT7601U_INSTALL_INIT_SYSV
	$(INSTALL) -m 0755 -D package/mt7601u/S80mt7601u \
	    $(TARGET_DIR)/etc/init.d/S80mt7601u
	sed -i "s;^MT7601U_PREFIX=.*$$;MT7601U_PREFIX=$(MT7601U_PREFIX);" \
	    $(TARGET_DIR)/etc/init.d/S80mt7601u
endef

define MT7601U_INSTALL_TARGET_CMDS
	$(MT7601U_TARGET_INSTALL_DRV)
	$(MT7601U_INSTALL_INIT_SYSV)
endef

$(eval $(generic-package))
