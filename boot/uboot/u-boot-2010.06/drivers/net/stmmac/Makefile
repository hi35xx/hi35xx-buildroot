include $(TOPDIR)/config.mk

LIB	:= $(obj)libstmmac.a

COBJS-$(CONFIG_GODNET) += sys_godnet.o
COBJS-$(CONFIG_GODARM) += sys_godarm.o
COBJS-$(CONFIG_HI3535) += sys_hi3535.o
COBJS-$(CONFIG_NET_STMMAC) \
		 += mdio.o ctrl.o stmmac.o

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
