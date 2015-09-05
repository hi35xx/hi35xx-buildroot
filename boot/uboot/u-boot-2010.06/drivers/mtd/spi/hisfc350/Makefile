include $(TOPDIR)/config.mk

LIB     := $(obj)libhisfcv350.a

COBJS-$(CONFIG_SPI_FLASH_HISFC350)     += hisfc350.o  hisfc350_spi_ids.o

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
