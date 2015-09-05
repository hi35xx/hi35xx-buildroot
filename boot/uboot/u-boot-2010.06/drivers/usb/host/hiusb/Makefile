include $(TOPDIR)/config.mk

#CFLAGS += -DET_DEBUG -DDEBUG

LIB	= $(obj)libhiusb.a

COBJS-$(CONFIG_USB_OHCI) += hiusb-ohci.o
COBJS-$(CONFIG_USB_XHCI) += xhci.o xhci-mem.o xhci-ring.o
CFLAGS += -fPIC
COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

$(LIB):	$(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
