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

#
# image should be loaded at 0x01000000
#
TEXT_BASE = 0x88400000

PLATFORM_CPPFLAGS += -march=armv5te -mno-unaligned-access -DTEXT_BASE=$(TEXT_BASE)
