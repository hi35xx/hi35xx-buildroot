################################################################################
#
# wsapi-fcgi
#
################################################################################

WSAPI_FCGI_VERSION = 1.6.1-1
WSAPI_FCGI_DEPENDENCIES = libfcgi
WSAPI_FCGI_SUBDIR = wsapi-$(shell echo "$(WSAPI_FCGI_VERSION)" | sed -e "s/-[0-9]$$//")
WSAPI_FCGI_LICENSE = MIT

$(eval $(luarocks-package))
