#
# Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
#
# This program is free software; you can redistribute  it and/or modify it
# under  the terms of  the GNU General Public License as published by the
# Free Software Foundation;  either version 2 of the  License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

include $(TOPDIR)/config.mk

LIB	= $(obj)lib$(CPU).a

START	:= start.o
COBJS	:= chip.o
COBJS	+= lowlevel_init_v300.o
COBJS	+= emmc_boot.o
COBJS	+= cpu.o
SOBJS	+= uart.o

SRCS    := $(START:.o=.S) $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS    := $(addprefix $(obj),$(SOBJS) $(COBJS))
START	:= $(addprefix $(obj),$(START))

all:	$(obj).depend $(START) $(LIB)

$(LIB):	$(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
