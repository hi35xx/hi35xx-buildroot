#
# drivers/devices/hisfc300new/Makefile
#

obj-$(CONFIG_MTD_HISFC300NEW)     += hisfc300new_spi_ids.o hisfc300new.o

ifdef CONFIG_ARCH_GODNET
obj-$(CONFIG_MTD_HISFC300NEW)     += hisfc300new_godnet.o
endif

ifdef CONFIG_ARCH_GODARM
obj-$(CONFIG_MTD_HISFC300NEW)     += hisfc300new_godarm.o
endif

ifdef CONFIG_ARCH_GODCUBE
obj-$(CONFIG_MTD_HISFC300NEW)     += hisfc300new_godcube.o
endif
