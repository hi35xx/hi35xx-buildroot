include $(TOPDIR)/config.mk

LIB := libhdmi.a

COBJS-y += hdmi.o
COBJS-y += de.o
COBJS-y += drv_hlviic.o
COBJS-y += eeprom.o
COBJS-y += hdmitx.o
COBJS-y += infofrm.o
COBJS-y += txvidp.o
COBJS-y += txvptbl.o
COBJS-y += vmtables.o

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

