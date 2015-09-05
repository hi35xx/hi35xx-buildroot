

include $(TOPDIR)/config.mk

LIB	= $(obj)lib$(CPU).a

START	:= start.o
SOBJS   := lowlevel_init.o
SOBJS	+= emmc_boot.o
COBJS	:= cpu.o
COBJS	+= timer.o

SRCS	:= $(START:.o=.S) $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(SOBJS) $(COBJS))
START	:= $(addprefix $(obj),$(START))

all:	$(obj).depend $(START) $(LIB)

$(LIB):	$(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
