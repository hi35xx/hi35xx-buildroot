/*
 * (C) Copyright 2009-2010
 * Kevin Hsu, Hisilicon inc, xujilin@huawei.com
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Video output Support
 */
#include <common.h>
#include <command.h>
#include <hi3519_vo.h>

extern int set_vobg(unsigned int dev, unsigned int rgb);
extern int start_vo(unsigned int dev, unsigned int type, unsigned int sync);
extern int stop_vo(unsigned int dev);
extern int start_gx(unsigned int addr,unsigned int x, unsigned int y);
extern int stop_gx(void);
extern int hdmi_display(unsigned int dev, unsigned int vosync, unsigned int input, unsigned int output);
extern void hdmi_stop(void);

#define VOU_DEV_MAX_NUM         1
//static int gs_aInterfaceType[VOU_DEV_MAX_NUM] = {0};

int check_vo_support(unsigned int dev, unsigned int type, unsigned int sync)
{
    if (VOU_DEV_DHD1 == dev)
    {
        if ((type & ~(VO_INTF_BT656 | VO_INTF_CVBS | VO_INTF_LCD_6BIT | VO_INTF_LCD_8BIT | VO_INTF_LCD_16BIT))
            || (0 == type))
        {
            printf("Sd%d can't use intf(%d) except BT656/CVBS/LCD_6BIT/LCD_8BIT and LCD_16BIT!\n", dev, type);
            return -1;
        }

        if ((type & ~VO_INTF_BT1120 ) && (type & ~(VO_INTF_BT656 | VO_INTF_CVBS))
            && (type & ~VO_INTF_LCD_6BIT ) && (type & ~VO_INTF_LCD_8BIT)
            && (type & ~VO_INTF_LCD_16BIT))
        {
            printf("Vo(%d), the intftype(0x%x) is wrong, only cvbs/bt656can use at the same time!\n", dev, type);
            return -1;
        }
    }
    else
    {
        printf("Unknow dev(%d)!\n", dev);
        return -1;
    }

    if ((VO_INTF_BT656 & type) || (VO_INTF_CVBS & type) )
    {
        if ((sync != VO_OUTPUT_PAL) && (sync != VO_OUTPUT_NTSC))
        {
            printf("Vo%d's intfsync %d illegal!\n", dev, sync);
            return -1;
        }
    }

    if (VO_INTF_LCD_8BIT & type)
    {
        if (VO_OUTPUT_320X240_60 != sync)
        {
            printf("Vo%d's intfsync %d illegal!\n", dev, sync);
            return -1;
        }
    }

    if (VO_INTF_LCD_6BIT & type)
    {
        if (sync < VO_OUTPUT_320X240_50 || sync > VO_OUTPUT_240X320_50)
        {
            printf("Vo%d's intfsync %d illegal!\n", dev, sync);
            return -1;
        }
    }

    if (VO_INTF_LCD_16BIT & type)
    {
        if (VO_OUTPUT_240X320_60 != sync)
        {
            printf("Vo%d's intfsync %d illegal!\n", dev, sync);
            return -1;
        }
    }

    return 0;
}

int do_vobg(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    unsigned int dev, rgb;

    if (argc < 3)
    {
        printf("Insufficient parameter!\n");
        printf ("Usage:\n%s\n", cmdtp->usage);
        return -1;
    }

    dev = (unsigned int)simple_strtoul(argv[1], NULL, 10);
    rgb = (unsigned int)simple_strtoul(argv[2], NULL, 10);
    if (VOU_DEV_DHD1 != dev)
    {
        printf("Invalid parameter!\n");
        return -1;
    }

    set_vobg(dev, rgb);
    
    printf("dev %d set background color!\n", dev);

    return 0;
}

int do_startvo(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    unsigned int dev, type, sync;

    if (argc < 4)
    {
        printf("Insufficient parameter!\n");
        printf ("Usage:\n%s\n", cmdtp->usage);
        return -1;
    }

    dev  = (unsigned int)simple_strtoul(argv[1], NULL, 10);
    type = (unsigned int)simple_strtoul(argv[2], NULL, 10);
    sync = (unsigned int)simple_strtoul(argv[3], NULL, 10);

    if (check_vo_support(dev, type, sync))
    {
        printf("Unsupport parameter!\n");
        return -1;
    }

    start_vo(dev, type, sync);

    printf("dev %d opened!\n", dev);

    return 0;
}

int do_stopvo(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    unsigned int dev;

    if (argc < 2)
    {
        printf("Insufficient parameter!\n");
        printf ("Usage:\n%s\n", cmdtp->usage);
        return -1;
    }

    dev = (unsigned int)simple_strtoul(argv[1], NULL, 10);
    if (VOU_DEV_DHD1 != dev)
    {
        printf("Invalid parameter!\n");
        return -1;
    }


    stop_vo(dev);

    printf("dev %d closed!\n", dev);

    return 0;
}

int do_startgx(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    unsigned int  x, y, addr;

    if (argc < 4)
    {
        printf("Insufficient parameter!\n");
        printf ("Usage:\n%s\n", cmdtp->usage);
        return -1;
    }

    //layer = (unsigned int)simple_strtoul(argv[1], NULL, 10);
    addr  = (unsigned int)simple_strtoul(argv[1], NULL, 16);
    //strd  = (unsigned int)simple_strtoul(argv[3], NULL, 10);
    x     = (unsigned int)simple_strtoul(argv[2], NULL, 10);
    y     = (unsigned int)simple_strtoul(argv[3], NULL, 10);
    //w     = (unsigned int)simple_strtoul(argv[6], NULL, 10);
    //h     = (unsigned int)simple_strtoul(argv[7], NULL, 10);

    if ( x > PIC_MAX_WIDTH   || (x & 0x1) || y > PIC_MAX_HEIGHT  || (y & 0x1))
    {
        printf("Invalid parameter!\n");
        return -1;
    }

    start_gx(addr, x, y);

    printf("graphic layer opened!\n");

    return 0;
}

int do_stopgx(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    //unsigned int layer;

    if (argc > 1)
    {
        printf("Insufficient parameter!\n");
        printf ("Usage:\n%s\n", cmdtp->usage);
        return -1;
    }

    stop_gx();

    printf("graphic layer closed!\n");

    return 0;
}



U_BOOT_CMD(
	startvo,    CFG_MAXARGS,	1,  do_startvo,
	"startvo   - open interface of vo device.\n"
	"\t- startvo [dev type sync]",
	"\nargs: [dev, type, sync]\n"
	);

U_BOOT_CMD(
	stopvo,    CFG_MAXARGS,	1,  do_stopvo,
	"stopvo   - close interface of vo device.\n"
	"\t- stopvo [dev]",
	"\nargs: [dev]\n"
	);

U_BOOT_CMD(
	startgx,    CFG_MAXARGS,	1,  do_startgx,
	"startgx   - open graphics layer.\n"
	"\t- startgx [addr x y]\n",
	"\nargs: [addr, x, y]\n"
	);

U_BOOT_CMD(
	stopgx,    CFG_MAXARGS,	1,  do_stopgx,
	"stopgx   - close graphics layer.\n"
	"\t- stopgx [void]",
       "\nargs: [void]\n"
	);

U_BOOT_CMD(
	setvobg,    CFG_MAXARGS,	1,  do_vobg,
	"setvobg   - set vo backgroud color.\n"
	"\t- setvobg [dev color]",
	"\nargs: [dev, color]\n"
	);


