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

PLATFORM_RELFLAGS += -fno-strict-aliasing -fno-common -ffixed-r8
#		     -msoft-float

# Make ARMv5 to allow more compilers to work, even though its v7a.
PLATFORM_RELFLAGS += -fno-aggressive-loop-optimizations
PLATFORM_CPPFLAGS += -march=armv5te
PLATFORM_CPPFLAGS += -mno-unaligned-access
# =========================================================================
#
# Supply options according to compiler version
#
# =========================================================================
PLATFORM_RELFLAGS +=$(call cc-option,-mshort-load-bytes,\
		    $(call cc-option,-malignment-traps,))
