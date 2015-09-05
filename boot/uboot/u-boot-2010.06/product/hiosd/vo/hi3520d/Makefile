include $(TOPDIR)/config.mk

LIB := libvo.a

COBJS-y += vou.o
COBJS-y += vou_drv.o
COBJS-y += vou_hal.o
COBJS-y += vou_coef_org.o

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

