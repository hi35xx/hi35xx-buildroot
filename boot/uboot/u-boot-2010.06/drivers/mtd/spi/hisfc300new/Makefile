include $(TOPDIR)/config.mk

LIB     := $(obj)libhisfcv300new.a

COBJS-$(CONFIG_HISFC_FLASH_GODBOX)      += hisfc300new_spi_ids.o hisfc300new.o hisfc300new_godbox.o
COBJS-$(CONFIG_HISFC_FLASH_GODEYES)     += hisfc300new_spi_ids.o hisfc300new.o hisfc300new_godeyes.o
COBJS-$(CONFIG_HISFC_FLASH_GODNET)      += hisfc300new_spi_ids.o hisfc300new.o hisfc300new_godnet.o
COBJS-$(CONFIG_HISFC_FLASH_GODCUBE)     += hisfc300new_spi_ids.o hisfc300new.o hisfc300new_godcube.o
COBJS-$(CONFIG_HISFC_FLASH_GODARM)      += hisfc300new_spi_ids.o hisfc300new.o hisfc300new_godarm.o

COBJS   := $(COBJS-y)
SRCS    := $(COBJS:.o=.c)
OBJS    := $(addprefix $(obj),$(COBJS))

all:    $(LIB)

$(LIB): $(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
