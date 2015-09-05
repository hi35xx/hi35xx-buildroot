/*
 * Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

#include <common.h>
#include <command.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <version.h>

#include <asm/io.h>

/*****************************************************************************/

int do_getinfo(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *cmd;
	struct mtd_info_ex *mtd_info = NULL;
	unsigned int len, ix;

	if(argc < 2)
		goto usage;

	cmd = argv[1];
	if (strcmp(cmd, "bootmode") == 0) {
		switch (get_boot_media())
		{
			default:
			case BOOT_MEDIA_UNKNOW:
				printf("Boot from unknown device,"
					" please check your hardware config.\n");
				return -1;
				
			case BOOT_MEDIA_NAND:
				printf("nand\n");
				break;
				
			case BOOT_MEDIA_SPIFLASH:
				printf("spi\n");
				break;
				
			case BOOT_MEDIA_EMMC:
				printf("emmc\n");
				break;

			case BOOT_MEDIA_DDR:
				printf("ddr\n");
				break;
		}
		return 0;
	}
	
	if (strcmp(cmd, "version") == 0) {
		unsigned long boot_version = BOOT_VERSION;
		printf("version: %lx.%lx.%lx\n",
			((boot_version >> 16) & 0xFF),
			((boot_version >> 8) & 0xFF),
			(boot_version  & 0xFF));

		return 0;
	}

#ifdef CONFIG_CMD_NAND
	if (strcmp(cmd, "nand") == 0) {
		mtd_info = get_nand_info();
		goto print_info;
	}
#endif

#ifdef CONFIG_CMD_SF
	if (strcmp(cmd, "spi") == 0) {
		mtd_info = get_spiflash_info();
		goto print_info;
	}
#endif

usage:
	cmd_usage(cmdtp);
	return -1;

print_info:

	if (mtd_info == NULL || mtd_info->type == 0) {// no find spi/nand.
		printf("no find ");
		if (*cmd == 's') {
			printf("spi");
		} else if (*cmd == 'n') {
			printf("nand");
		}
		printf("\n");
	} else {
		printf("Block:%sB ", ultohstr(mtd_info->erasesize));
		printf("Chip:%sB*%d ",
			ultohstr(mtd_info->chipsize),
			mtd_info->numchips);

		if (*cmd == 's') {
			// printf("addrcycle:%u ", mtd_info->addrcycle);
		} else if (*cmd == 'n') {
			printf("Page:%sB ", ultohstr(mtd_info->pagesize));
			printf("OOB:%sB ", ultohstr(mtd_info->oobsize));
			printf("ECC:%s ", get_ecctype_str((mtd_info->ecctype & 0x7)));
		}
		printf("\nID:");

		len = (mtd_info->id_length > 8 ? 8 : mtd_info->id_length);
		for (ix = 0; ix < len; ix++) {
			printf("0x%02X ", mtd_info->ids[ix]);
		}
		printf("\nName:\"%s\"\n", mtd_info->name);
	}

	return 0;
}

U_BOOT_CMD(
	getinfo, CONFIG_SYS_MAXARGS, 1, do_getinfo,
	"print hardware information",
	"bootmode - get start memeory type e.g. nand/spi etc\n"
	"getinfo nand - get nand flash information\n"
	"getinfo spi - get spi flash information\n"
	"getinfo version - get system version\n"
);

