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
 * Video decode Support
 */
#include <common.h>
#include <command.h>

#ifdef CONFIG_GODNET
#include <godnet_vo.h>
#endif

#ifdef CONFIG_GODARM
#include <godarm_vo.h>
#endif

#ifdef CONFIG_HI3535
#include <hi3535_vo.h>
#endif
#ifdef CONFIG_HI3520D
#include <hi3520d_vo.h>
#endif

extern int load_jpeg(void);
extern int jpeg_decode(void);

int do_jpgd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int ret;
#if 0
    if (argc < 4)
    {
        printf("Insufficient parameter!\n");
        printf ("Usage:\n%s\n", cmdtp->usage);
        return -1;
    }
#else
#ifdef	CONFIG_SYS_LONGHELP
    printf ("you should first set:\n%s\n", cmdtp->help);
#endif
#endif

    ret = load_jpeg();
    if (0 != ret)
    {
        printf("load jpeg err. \n");
        //todo return 0 or ret?
        return 0;
    }
    jpeg_decode();

    printf("decode jpeg!\n");

    return 0;
}


U_BOOT_CMD(
	decjpg,    CFG_MAXARGS,	1,  do_jpgd,
	"jpgd   - decode jpeg picture.\n",
	"\t- setenv jpeg_addr 0x--------\n"
	"\t- setenv jpeg_size 0x--------\n"
	"\t- setenv vobuf     0x--------\n"
	);


