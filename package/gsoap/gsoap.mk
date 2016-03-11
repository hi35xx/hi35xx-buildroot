################################################################################
#
## gsoap
#
#################################################################################

GSOAP_VERSION_MAJOR = 2.8
GSOAP_VERSION = $(GSOAP_VERSION_MAJOR).27
GSOAP_SOURCE = gsoap_$(GSOAP_VERSION).zip
GSOAP_SITE = http://downloads.sourceforge.net/project/gsoap2/gSOAP
GSOAP_INSTALL_STAGING = YES
GSOAP_DEPENDENCIES = host-gsoap
HOST_GSOAP_DEPENDENCIES = host-pkgconf host-flex host-bison
GSOAP_CONF_OPTS = --disable-samples --enable-ipv6
HOST_GSOAP_CONF_OPTS = --disable-samples --enable-ipv6
GSOAP_MAKE = $(MAKE1)
HOST_GSOAP_MAKE = $(MAKE1)
GSOAP_AUTORECONF = YES
GSOAP_AUTORECONF_OPTS = --install --force
GSOAP_LICENSE = GPLv2
GSOAP_LICENSE_FILES = LICENSE.txt

define HOST_GSOAP_EXTRACT_CMDS
	$(UNZIP) -d $(@D) $(DL_DIR)/$(GSOAP_SOURCE)
	mv $(@D)/gsoap-$(GSOAP_VERSION_MAJOR)/* $(@D)
	$(RM) -r $(@D)/gsoap-$(GSOAP_VERSION_MAJOR)
endef

define GSOAP_EXTRACT_CMDS
	$(UNZIP) -d $(@D) $(DL_DIR)/$(GSOAP_SOURCE)
	mv $(@D)/gsoap-$(GSOAP_VERSION_MAJOR)/* $(@D)
	$(RM) -r $(@D)/gsoap-$(GSOAP_VERSION_MAJOR)
endef

ifeq ($(BR2_PACKAGE_OPENSSL),y)
	GSOAP_DEPENDENCIES += openssl
	GSOAP_CONF_OPTS += --enable-ssl
	HOST_GSOAP_DEPENDENCIES += openssl
	HOST_GSOAP_CONF_OPTS += --enable-ssl
else
	GSOAP_CONF_OPTS += --disable-ssl
	HOST_GSOAP_CONF_OPTS += --disable-ssl
endif

ifeq ($(BR2_PACKAGE_GNUTLS),y)
	GSOAP_DEPENDENCIES += gnutls
	GSOAP_CONF_OPTS += --enable-gnutls
	HOST_GSOAP_DEPENDENCIES += gnutls
	HOST_GSOAP_CONF_OPTS += --enable-gnutls
else
	GSOAP_CONF_OPTS += --disable-gnutls
	HOST_GSOAP_CONF_OPTS += --disable-gnutls
endif

define GSOAP_REMOVE_DEV_FILES
	rm -f $(TARGET_DIR)/usr/bin/wsdl2h
	rm -f $(TARGET_DIR)/usr/bin/soapcpp2
	rm -rf $(TARGET_DIR)/usr/share/gsoap
endef
GSOAP_POST_INSTALL_TARGET_HOOKS += GSOAP_REMOVE_DEV_FILES

$(eval $(autotools-package))
$(eval $(host-autotools-package))
