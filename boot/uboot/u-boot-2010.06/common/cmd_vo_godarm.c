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
//#include <hi35xx_vo.h>
#include <godarm_vo.h>

extern int set_vobg(unsigned int dev, unsigned int rgb);
extern int start_vo(unsigned int dev, unsigned int type, unsigned int sync);
extern int stop_vo(unsigned int dev);
extern int start_gx(unsigned int layer, unsigned addr, unsigned int strd, unsigned int x, unsigned int y, unsigned int w, unsigned int h);
extern int stop_gx(unsigned int layer);
extern int hdmi_display(unsigned int HdFmt, unsigned int input, unsigned int output);

int check_vo_support(unsigned int dev, unsigned int type, unsigned int sync)
{
    if (VOU_DEV_DHD0 == dev)
    {
        if (((type & (VO_INTF_LCD | VO_INTF_BT1120)) == (VO_INTF_LCD | VO_INTF_BT1120))
            || ((type & (VO_INTF_LCD | VO_INTF_VGA)) == (VO_INTF_LCD | VO_INTF_VGA)))
        {
            printf("Hd%d can't use LCD and BT1120/VGA at the same time!\n", dev);
            return -1;
        }

        if (type & (VO_INTF_CVBS | VO_INTF_YPBPR | VO_INTF_BT656 | VO_INTF_BT656_H | VO_INTF_BT656_L))
        {
            printf("Hd%d can't use CVBS、YPBPR、BT656、BT656_H、BT656_L intftype, intf is %d!\n", dev, type);
            return -1;
        }

        if ((type & ~(VO_INTF_VGA | VO_INTF_BT1120 | VO_INTF_HDMI | VO_INTF_LCD 
            | VO_INTF_CVBS | VO_INTF_BT656 | VO_INTF_BT656_H | VO_INTF_BT656_L)) || (0 == type))
        {
            printf("Hd%d unknow intf%d!\n", dev, type);
            return -1;
        }
    }
    else if ((VOU_DEV_DSD0 <= dev) && (VOU_DEV_DSD1 >= dev))
    {
        if (((type & (VO_INTF_BT656 | VO_INTF_BT656_H)) == (VO_INTF_BT656 | VO_INTF_BT656_H))
            || ((type & (VO_INTF_BT656 | VO_INTF_BT656_L)) == (VO_INTF_BT656 | VO_INTF_BT656_L)))
        {
            printf("Vo(%d) can't use VO_INTF_BT656 and VO_INTF_BT656_H(VO_INTF_BT656_L) at the same time!\n", dev);
            return -1;
        }
        
        if ((type & (VO_INTF_BT656_H | VO_INTF_BT656_L)) == (VO_INTF_BT656_H | VO_INTF_BT656_L))
        {
            printf("Vo(%d) can't use VO_INTF_BT656_H and VO_INTF_BT656_L at the same time!\n", dev);
            return -1;
        }
        
        if ((type &(~(VO_INTF_BT656 | VO_INTF_BT656_H| VO_INTF_BT656_L| VO_INTF_CVBS)))
            || (0 == type))
        {
            printf("Sd%d can't use intf(%d) except CVBS、BT656、BT656_H、BT656_L!\n", dev, type);
            return -1;
        }
    }
    else
    {
        printf("Unknow dev(%d)!\n", dev);
        return -1;
    }
        
    if ((VO_INTF_VGA & type) || (VO_INTF_BT1120 & type) || (VO_INTF_HDMI & type) || (VO_INTF_LCD & type))
    {
        if (sync < VO_OUTPUT_1080P24 || sync > VO_OUTPUT_1280x800_60)
        {
            printf("Vo%d's intfsync illegal!\n", dev);
            return -1;
        }
    }
    
    if ((VO_INTF_CVBS & type) || (VO_INTF_BT656 & type) || (VO_INTF_BT656_H & type) || (VO_INTF_BT656_L & type))
    {
        if (sync != VO_OUTPUT_PAL && sync != VO_OUTPUT_NTSC)
        {
            printf("Vo%d's intfsync illegal!\n", dev);
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
    if (dev > VOU_DEV_DSD1)
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
    if (dev > VOU_DEV_DSD1 || sync >= VO_OUTPUT_BUTT)
    {
        printf("Invalid parameter!\n");
        return -1;
    }

    if (check_vo_support(dev, type, sync))
    {
        printf("Unsupport parameter!\n");
        return -1;
    }

    start_vo(dev, type, sync);

    if (type & VO_INTF_HDMI)
    {
        hdmi_display(sync, 2, 2);
        //hdmi_display(sync, 2, 0);
    } 

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
    if (dev > VOU_DEV_DSD1)
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
    unsigned int layer, addr, strd, x, y, w, h;

    if (argc < 8)
    {
        printf("Insufficient parameter!\n");
        printf ("Usage:\n%s\n", cmdtp->usage);
        return -1;
    }

    layer = (unsigned int)simple_strtoul(argv[1], NULL, 10);
    addr  = (unsigned int)simple_strtoul(argv[2], NULL, 16);
    strd  = (unsigned int)simple_strtoul(argv[3], NULL, 10);
    x     = (unsigned int)simple_strtoul(argv[4], NULL, 10);
    y     = (unsigned int)simple_strtoul(argv[5], NULL, 10);
    w     = (unsigned int)simple_strtoul(argv[6], NULL, 10);
    h     = (unsigned int)simple_strtoul(argv[7], NULL, 10);

    if (layer >= VO_GRAPHC_BUTT || strd > (PIC_MAX_WIDTH*2)
        || x > PIC_MAX_WIDTH   || (x & 0x1)
        || y > PIC_MAX_HEIGHT  || (y & 0x1)
        || w > PIC_MAX_WIDTH   || (w & 0x1) || w < PIC_MIN_LENTH
        || h > PIC_MAX_HEIGHT  || (h & 0x1) || h < PIC_MIN_LENTH
        )
    {
        printf("Invalid parameter!\n");
        return -1;
    }

    start_gx(layer, addr, strd, x, y, w, h);

    printf("graphic layer %d opened!\n", layer);

    return 0;
}

int do_stopgx(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    unsigned int layer;

    if (argc < 2)
    {
        printf("Insufficient parameter!\n");
        printf ("Usage:\n%s\n", cmdtp->usage);
        return -1;
    }

    layer = (unsigned int)simple_strtoul(argv[1], NULL, 10);

    if (layer >= VO_GRAPHC_BUTT)
    {
        printf("Invalid parameter!\n");
        return -1;
    }

    stop_gx(layer);

    printf("graphic layer %d closed!\n", layer);

    return 0;
}



U_BOOT_CMD(
	startvo,    CFG_MAXARGS,	1,  do_startvo,
	"startvo   - open interface of vo device.\n"
	"\t- startvo [dev type sync]",
	"\nargs: [dev, type, sync]\n"
	"\t-<dev> : 0(HD0), 1~2(SD0~1)\n"
	"\t-<type>: 1(CVBS),4(VGA),8(BT.656),16(BT.1120),32(HDMI),64(LCD)\n"
	"\t\tsupport multi type eg: 52(VGA|BT.1120|HDMI)"
	"\t-<sync>:\n"
	"\t\t0(PAL),1(NTSC),2(1080P24)\n"
	"\t\t3(1080P25),4(1080P30),5(720P50),6(720P60)\n"
	"\t\t7(1080I50),8(1080I60),9(1080P50),10(1080P60)\n"
	"\t\t11(576P50),12(480P60),13(800x600),14(1024x768)\n"
	"\t\t15(1280x1024),16(1366x768),17(1440x900),18(1280x800)\n"
	);

U_BOOT_CMD(
	stopvo,    CFG_MAXARGS,	1,  do_stopvo,
	"stopvo   - close interface of vo device.\n"
	"\t- stopvo [dev]",
	"\nargs: [dev]\n"
	"\t-<dev> : 0(HD0), 1~2(SD0~1)\n"
	);

U_BOOT_CMD(
	startgx,    CFG_MAXARGS,	1,  do_startgx,
	"startgx   - open graphics layer.\n"
	"\t- startgx [layer addr stride x y w h]\n",
	"\nargs: [layer, addr, stride, x, y, w, h]\n"
	"\t-<layer>   : 0(G0), 1(G1), 2(G2)\n"
	"\t-<addr>    : picture address\n"
	"\t-<stride>  : picture stride\n"
	"\t-<x,y,w,h> : display area\n"
	);

U_BOOT_CMD(
	stopgx,    CFG_MAXARGS,	1,  do_stopgx,
	"stopgx   - close graphics layer.\n"
	"\t- stopgx [layer]",
	"\nargs: [layer]\n"
	"\t-<layer> : 0(G0), 1(G1), 2(G2)\n"
	);

U_BOOT_CMD(
	setvobg,    CFG_MAXARGS,	1,  do_vobg,
	"setvobg   - set vo backgroud color.\n"
	"\t- setvobg [dev color]",
	"\nargs: [dev, color]\n"
	"\t-<dev> : 0(HD0), 1~2(SD0~1)\n"
	"\t-<color>: rgb color space\n"
	);

