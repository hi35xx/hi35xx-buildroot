/*
 * (C) Copyright 2003
 * Gary Jennejohn, DENX Software Engineering, gj@denx.de.
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
#include <environment.h>
#include <command.h>
#include <malloc.h>
#include <image.h>
#include <asm/byteorder.h>
#include <asm/io.h>
#include <spi_flash.h>
#include <linux/mtd/mtd.h>
#include <fat.h>

#ifdef CONFIG_AUTO_UPDATE  /* cover the whole file */

#ifdef CONFIG_AUTO_SD_UPDATE
#ifndef CONFIG_MMC
#error "should have defined CONFIG_MMC"
#endif
#include <mmc.h>
#include "mmc_init.c"
#endif

#if defined CONFIG_AUTO_USB_UPDATE
#if !defined CONFIG_USB_OHCI && !defined CONFIG_USB_XHCI
#error "should have defined CONFIG_USB_OHCI or CONFIG_USB_XHCI"
#endif
#ifndef CONFIG_USB_STORAGE
#error "should have defined CONFIG_USB_STORAGE"
#endif
#include <usb.h>
#include "usb_init.c"
#endif

#undef AU_DEBUG
#undef debug
#ifdef	AU_DEBUG
#define debug(fmt, args...)	printf(fmt, ##args)
#else
#define debug(fmt, args...)
#endif	/* AU_DEBUG */

/* possible names of files on the medium. */
#define AU_CONFIG	"config"
/* config file's size < 1K */
#define CONFIG_MAX_SIZE	2048

#define AU_FIRMWARE	"u-boot.bin"
#define AU_KERNEL	"kernel"

int boot_medium_type =0xff;

#include "nand.h"
struct flash_layout {
	long start;
	long end;
};

struct update_medium_interface {
	char name[20];
    int (*init)(void);
	int (*erase)(unsigned long offset, unsigned long len);
	int (*write)(unsigned long offset, unsigned long len, void *buf);
	int (*write_yaffs)(unsigned long offset, unsigned long len, void *buf);
	int (*write_ext4)(unsigned long offset, unsigned long len, void *buf);
};

static void schedule_notify(unsigned long offset, unsigned long len,
		unsigned long off_start)
{
	int percent_complete = -1;

	do {
		unsigned long long n = (unsigned long long)
			(offset - off_start) * 100;
		int percent;

		do_div(n, len);
		percent = (int)n;

		/* output progress message only at whole percent
		 * steps to reduce the number of messages
		 * printed on (slow) serial consoles
		 */
		if (percent != percent_complete) {
			printf("\rOperation at 0x%lx -- %3d%% complete.",
					offset, percent);
		}
	} while (0);
}
#ifdef CONFIG_CMD_SF
static struct spi_flash *spinor_flash;
static int spinor_flash_init(void)
{
    spinor_flash = spi_flash_probe(0, 0, 0, 0);
    return 0;
}
static int spi_flash_erase_op(struct spi_flash *flash, unsigned long offset,
		unsigned long len)
{
	int ret;
	struct mtd_info_ex *spiflash_info = get_spiflash_info();
	unsigned long erase_start, erase_len, erase_step;

	erase_start = offset;
	erase_len   = len;
	erase_step  = spiflash_info->erasesize;

	while (len > 0) {
		if (len < erase_step)
			erase_step = len;

		ret = flash->erase(flash, (u32)offset, erase_step);
		if (ret)
			return 1;

		len -= erase_step;
		offset += erase_step;
		/* notify real time schedule */
		schedule_notify(offset, erase_len, erase_start);
	}
	return ret;
}

static int spinor_flash_erase(unsigned long offset, unsigned long len)
{
    return spi_flash_erase_op(spinor_flash, offset, len);
}

static int spi_flash_write_op(struct spi_flash *flash, unsigned long offset,
		unsigned long len, void *buf)
{
	int ret = 0;
	unsigned long write_start, write_len, write_step;
	char *pbuf = buf;
	struct mtd_info_ex *spiflash_info = get_spiflash_info();

	write_start = offset;
	write_len   = len;
	write_step  = spiflash_info->erasesize;

	while (len > 0) {
		if (len < write_step)
			write_step = len;

		ret = flash->write(flash, offset, write_step, pbuf);
		if (ret)
			break;

		offset += write_step;
		pbuf   += write_step;
		len    -= write_step;
		/* notify real time schedule */
		schedule_notify(offset, write_len, write_start);
	}

	return ret;
}

static int spinor_flash_write(unsigned long offset, unsigned long len, void *buf)
{
    return spi_flash_write_op(spinor_flash, offset, len, buf);
}
#endif

#ifdef CONFIG_CMD_NAND
nand_info_t *nand_flash;
static int nand_flash_init(void)
{
    nand_flash = &nand_info[0];
    return 0;
}

static int nand_flash_erase(unsigned long offset, unsigned long len)
{
    int ret;
	unsigned long erase_len, erase_step;
	nand_erase_options_t opts;
	memset(&opts, 0, sizeof(opts));

    erase_step = nand_flash->erasesize;
    erase_len = len;
	opts.length  = erase_step;
	opts.offset = offset;
	opts.quiet = 1;

	while (len > 0) {
		if (len < erase_step)
			erase_step = len;

	    ret = nand_erase_opts(nand_flash, &opts);
		if (ret)
			return 1;

		len -= erase_step;
		opts.offset += erase_step;
		/* notify real time schedule */
		schedule_notify(opts.offset, erase_len, offset);
	}
	return ret;
}

static int nand_flash_write(unsigned long offset, unsigned long len,
        void *buf)
{
	int ret = 0;
	unsigned long write_start, write_len, write_step;
	char *pbuf = buf;

	write_start = offset;
	write_len   = len;
    long long length = (long)len;
	write_step  = nand_flash->erasesize;
    size_t rw_size;

	while (length > 0) {

		rw_size = write_step;
        ret = nand_write_skip_bad(nand_flash, offset, &rw_size,
							  (u_char *)pbuf);
		if (ret)
			break;
		if (length < write_step) {
		    break;
        }

		offset += write_step;
		pbuf   += write_step;
		length -= write_step;
		/* notify real time schedule */
		schedule_notify(offset, write_len, write_start);
	}

	return ret;
}

static int nand_flash_yaffs_write(unsigned long offset, unsigned long len,
        void *buf)
{
	int ret = 0;
	size_t rw_size = len;
    ret = nand_write_yaffs_skip_bad(nand_flash, offset, &rw_size,
            (u_char *)buf);

	return ret;
}
#endif

static int mmc_save_init(void)
{
	struct mmc *mmc = find_mmc_device(0);

	if (!mmc) {
		printf("%s:find mmc device failed\n", __func__);
		return -1;
	}

	mmc_init(mmc);

	return 0;
}

static int mmc_save_write(unsigned long offset, unsigned long len, void *buf)
{
	struct mmc *mmc = find_mmc_device(0);

	if (!mmc) {
		printf("%s:find mmc device failed\n", __func__);
		return -1;
	}

	if(!mmc->block_dev.block_write) {
		printf("block_write ops is null!\n");
		return -1;
	}
	mmc->block_dev.block_write(0,(offset >> 9), (len >> 9), buf);
	return 0;
}

static int mmc_save_write_ext4(unsigned long offset, unsigned long len, void *buf)
{
	struct mmc *mmc = find_mmc_device(0);

	if (!mmc) {
		printf("%s:find mmc device failed\n", __func__);
		return -1;
	}

	extern int ext4_unsparse(struct mmc *mmc, u32 dev, const u8 *pbuf, u32 blk, u32 cnt);
	return ext4_unsparse(mmc, 0, buf, (offset >> 9), (len >> 9));
}

#define UPDATE_MEDIUM_SPINOR 0
#define UPDATE_MEDIUM_NAND   1
#define UPDATE_MEDIUM_EMMC   2

static struct update_medium_interface update_intf[3] = {
#ifdef CONFIG_CMD_SF
    {"spinor",  spinor_flash_init,  spinor_flash_erase, spinor_flash_write, NULL},
#else
	{"none",  NULL,  NULL, NULL, NULL},
#endif
#ifdef CONFIG_CMD_NAND
    {"nand",    nand_flash_init,    nand_flash_erase,   nand_flash_write,   nand_flash_yaffs_write},
#else
	{"none",  NULL,  NULL, NULL, NULL},
#endif
	{"emmc", mmc_save_init, NULL, mmc_save_write, NULL,  mmc_save_write_ext4}
};

static struct update_medium_interface *update_intf_p;

struct medium_interface {
	char name[20];
	int (*init) (void);
	void (*exit) (void);
};

#define MAX_UPDATE_INTF	3
static struct medium_interface s_intf[MAX_UPDATE_INTF] = {
#ifdef CONFIG_AUTO_SD_UPDATE
	{.name = "mmc",	.init = mmc_stor_init,	.exit = mmc_stor_exit,},
#endif
#ifdef CONFIG_AUTO_USB_UPDATE
	{.name = "usb",	.init = usb_stor_init,	.exit = usb_stor_exit,},
#endif
};

/* layout of the FLASH. ST = start address, ND = end address. */
#define AU_FL_FIRMWARE_ST	0x0
#define AU_FL_FIRMWARE_ND	0x7FFFF
#define AU_FL_KERNEL_ST		0x100000
#define AU_FL_KERNEL_ND		0x5FFFFF
#define AU_FL_ROOTFS_ST		0x600000
#define AU_FL_ROOTFS_ND		0xbFFFFF

static int au_stor_curr_dev; /* current device */

/* index of each file in the following arrays */
#define IDX_FIRMWARE	0
#define IDX_KERNEL	1
#define IDX_ROOTFS	2

/* max. number of files which could interest us */
#define AU_MAXFILES 32

/* pointers to file names */
char *aufile[AU_MAXFILES] = {
    0
};

#define NAME_MAX_LEN 0x20
char aufile_table[AU_MAXFILES][NAME_MAX_LEN] = {{0,},};
unsigned aufile_size[AU_MAXFILES] = {0};

/* sizes of flash areas for each file */
long ausize[AU_MAXFILES] = {0};

/* array of flash areas start and end addresses */
struct flash_layout aufl_layout[AU_MAXFILES] = {
	{ 0,	0, }
};

/* where to load files into memory */
#if defined(CONFIG_HI3536) || defined(CONFIG_HI3531A)
#define LOAD_ADDR ((unsigned char *)0x42000000)
#else
#define LOAD_ADDR ((unsigned char *)0x82000000)
#endif
/* the app is the largest image */
#define MAX_LOADSZ ausize[IDX_ROOTFS]

static int au_do_update(int idx, long sz)
{
	//image_header_t *hdr;
	unsigned long start, len;
	unsigned long write_len;
	int rc = 1;
	void *buf;
	char *pbuf;

	start = aufl_layout[idx].start;
	len = aufl_layout[idx].end - aufl_layout[idx].start + 1;

	/*
	 * erase the address range.
	 */
	if(boot_medium_type == BOOT_FROM_SPI || boot_medium_type == BOOT_FROM_NAND) {
		printf("%s erase...\n",update_intf_p->name);
		rc = update_intf_p->erase(start, len);
		if (rc) {
			printf("sector erase failed\n");
			return 1;
		}
	}

	buf = map_physmem((unsigned long)LOAD_ADDR, len, MAP_WRBACK);
	if (!buf) {
		puts("Failed to map physical memory\n");
		return 1;
	}

	/*
	 * write the whole file to flash;uboot and rootfs image have head
	 * kernel has head,it's head alse be writed to flash
	 */
	pbuf = buf;
	write_len = aufile_size[idx];

	/* copy the data from RAM to FLASH */
	printf("\n%s write...\n",update_intf_p->name);
	if(strstr(aufile[idx],"yaffs")){
		rc = update_intf_p->write_yaffs(start, write_len, pbuf);
	} else if (strstr(aufile[idx], "ext4")) {
		if (update_intf_p->write_ext4)
			rc = update_intf_p->write_ext4(start, len, pbuf);
	} else {
		rc = update_intf_p->write(start, write_len, pbuf);
	}
	if (rc) {
		printf("write failed, return %d\n", rc);
		return 1;
	}
	unmap_physmem(buf, len);

	return 0;
}

/*
 * If none of the update file(u-boot, kernel or rootfs) was found
 * in the medium, return -1;
 * If u-boot has been updated, return 1;
 * Others, return 0;
 */
static int update_to_flash(void)
{
	int i = 0;
	long sz;
	int res, cnt;
	int uboot_updated = 0;
	int image_found = 1;

	/* just loop thru all the possible files */
	for (i = 0; i < AU_MAXFILES && aufile[i]!=NULL; i++) {
        printf("\n");
        sz = file_fat_read(aufile[i], LOAD_ADDR, ausize[i]);
        debug("read %s sz %ld hdr %d\n",
                aufile[i], sz, sizeof(image_header_t));
        if (sz <= 0) {
            printf("%s not found!\n", aufile[i]);
            continue;
        }
        /* get file's real size */
        aufile_size[i] = sz;
        printf("size=0x%08x\n",aufile_size[i]);

        /* If u-boot had been updated, we need to
         * save current env to flash */
        if (0 == strcmp((char *)AU_FIRMWARE, aufile[i]))
            uboot_updated = 1;

        /* this is really not a good idea, but it's what the */
        /* customer wants. */
        cnt = 0;
		do {
			res = au_do_update(i, sz);
			/* let the user break out of the loop */
			if (ctrlc() || had_ctrlc()) {
				clear_ctrlc();

				break;
			}
			cnt++;
#ifdef AU_TEST_ONLY
		} while (res < 0 && cnt < 3);
		if (cnt < 3)
#else
		} while (res < 0);
#endif
	}

	if (1 == uboot_updated)
		return 1;
	if (1 == image_found)
		return 0;
}

#define ENV_MAX_LEN    CONFIG_MAX_SIZE/2

/*
 * pick up env form config file and set env
 * fail:return -1;
 * ok:  return 0;
 */
static int env_pick_up(const char* envname, char *buffer)
{
    char *str,*str_s,*str_e;
    char env[ENV_MAX_LEN];
    str = strstr(buffer, envname);
    if(!str) {
        goto env_err;
    }
    str_s = strchr(str,'\'');
    if(!str_s) {
        goto env_err;
    }
    str_e = strchr(++str_s,'\'');
    if(!str_e) {
        goto env_err;
    }
    if((unsigned)(str_e-str_s) > ENV_MAX_LEN) {
        printf("ERROR:%s too long!\n",envname);
        goto err;
    }
    strncpy(env,str_s,(size_t)(str_e-str_s));
    env[(size_t)(str_e-str_s)] = '\0';
    setenv((char *)envname, env);

    return 0;

env_err:
    printf("ERROR:%s not found!\n", envname);
err:
    return -1;
}

/*
 * pick up bootargs and bootcmd from config file and save env
 * fail:return -1;
 * ok:  return 0;
 */
static int get_env_from_sd(void)
{
    char *aufile = AU_CONFIG;
    int ret;
    long sz = file_fat_read(aufile, (void *)LOAD_ADDR, CONFIG_MAX_SIZE);
    if (sz <= 0) {
        printf("ERROR:%s not found!\n", aufile);
        goto err;
    }

    ret = env_pick_up("bootargs", (char *)LOAD_ADDR);
    if(ret<0)
        goto err;

    ret = env_pick_up("bootcmd", (char *)LOAD_ADDR);
    if(ret<0)
        goto err;

	(void)env_pick_up("minibootargs", (char *)LOAD_ADDR);

    return 0;
err:
    return -1;
}

struct mtd_part_s {
    char * str;
    int unit;
    struct mtd_part_s *next;
};

/*
 * insert node to list
 */
struct mtd_part_s * list_insert(struct mtd_part_s *head, struct mtd_part_s *node)
{
    struct mtd_part_s *p = head;
    if(!head)
        head = node;
    while(p) {
        if(p->next == NULL) {
            p->next = node;
            break;
        }
        p = p->next;
    }
    return head;
}

/*
 * sort list by str
 */
struct mtd_part_s *list_sort(struct mtd_part_s *head)
{
    int flag = 0;
    struct mtd_part_s *p,*pt1,*pt2;
    if(!head)
        return NULL;
    if(head->next==NULL)
        return head;
    do {
        flag = 0;
        if((unsigned)(head->str) >= (unsigned)(head->next->str)) {
            pt1 = head->next->next;
            pt2 = head->next;
            pt2->next = head;
            head->next = pt1;
            head = pt2;
        }
        for(p=head;p->next->next!=NULL;p=p->next) {
            if((unsigned)(p->next->str) >= (unsigned)(p->next->next->str)){
                pt1 = p->next->next->next;
                pt2 = p->next->next;
                pt2->next = p->next;
                p->next->next = pt1;
                p->next = pt2;
                flag=1;
            }
        }
    } while(flag);

    return head;
}

/*
 * get mtd parttion info list from env string
 */
struct mtd_part_s *get_mtd_parts(char *env)
{
    char *str = env;
    struct mtd_part_s * part_p;
    struct mtd_part_s * head = NULL;

    str = env;
    while((str = strstr(str, "M("))) {
        part_p = malloc(sizeof(struct mtd_part_s));
        part_p->str = str;
        part_p->unit = 1024*1024;
        part_p->next = NULL;
        head = list_insert(head, part_p);
        str++;
    }

    str = env;
    while((str = strstr(str, "m("))) {
        part_p = malloc(sizeof(struct mtd_part_s));
        part_p->str = str;
        part_p->unit = 1024*1024;
        part_p->next = NULL;
        head = list_insert(head, part_p);
        str++;
    }
    str = env;
    while((str = strstr(str, "K("))) {
        part_p = malloc(sizeof(struct mtd_part_s));
        part_p->str = str;
        part_p->unit = 1024;
        part_p->next = NULL;
        head = list_insert(head, part_p);
        str++;
    }

    str = env;
    while((str = strstr(str, "k("))) {
        part_p = malloc(sizeof(struct mtd_part_s));
        part_p->str = str;
        part_p->unit = 1024;
        part_p->next = NULL;
        head = list_insert(head, part_p);
        str++;
    }

    head = list_sort(head);

    return head;
}

int bootargs_analyze(void)
{
    char env[ENV_MAX_LEN];
    char *envp = getenv("bootargs");
    if(strlen(envp)>ENV_MAX_LEN-1) {
        printf("ERROR:bootargs is too long!\n");
        return -1;
    }
    strcpy(env, envp);
    char *str = env;
    char *str_0 = env;
    int i=0;
    long start = 0;
    struct mtd_part_s *part_p = NULL;
    part_p = get_mtd_parts(env);
    while(part_p)
    {
        if(i >= AU_MAXFILES) {
            printf("ERROR:Num of partition is more than %0d!\n", AU_MAXFILES);
            break;
        }
        str = part_p->str;
        char *str_num = str - 1;
        int j = 0;
        unsigned size = 0;
        int k, num;
        while(*str_num>='0' && *str_num<='9') {
            k=j;
            num = *str_num - '0';
            while(k) {
                num *= 10;
                k--;
            }
            size += num;
            j++;
            str_num--;
        }
        ausize[i] = size * part_p->unit;
        aufl_layout[i].start = start;
        aufl_layout[i].end = start + ausize[i] -1;
        start += ausize[i];

        str += 2;
        str_0 = strstr(str, ")");
        strncpy(aufile_table[i], str, (unsigned)(str_0 - str));
        aufile_table[i][str_0 - str] = '\0';
        aufile[i] = &(aufile_table[i][0]);
        printf("\n[%0d]=%-16s start=0x%08x end=0x%08x size=0x%08x", i, aufile_table[i],
                (unsigned int)(aufl_layout[i].start),
                (unsigned int)(aufl_layout[i].end),
                (unsigned int)ausize[i]);
        i++;
        part_p = part_p->next;
    }

    if(i==0) {
        printf("ERROR:Can't find valid partition info!\n");
        return -1;
    }
    return 0;
}

#define UBOOT_DATA_ADDR		(0x81000000)
#define UBOOT_DATA_SIZE		(0x80000)
#define UBOOT_DATA_FLAG_OFF      (0x3c)
#define UBOOT_DATA_FLAG_VAL      (0x12345678)
static int save_bootdata_to_flash(void)
{
	unsigned int rc;
	void *buf;

    rc = readl(UBOOT_DATA_ADDR + UBOOT_DATA_FLAG_OFF);
    if (UBOOT_DATA_FLAG_VAL == rc) {
        /*
         * erase the address range.
         */
        if(boot_medium_type == BOOT_FROM_SPI || boot_medium_type == BOOT_FROM_NAND) {
            printf("\n%s erase...\n",update_intf_p->name);
            rc = update_intf_p->erase(0,UBOOT_DATA_SIZE);
            if (rc) {
                printf("sector erase failed\n");
                return 1;
            }
        }

        buf = map_physmem((unsigned long)UBOOT_DATA_ADDR,
                UBOOT_DATA_SIZE, MAP_WRBACK);
        if (!buf) {
            puts("Failed to map physical memory\n");
            return 1;
        }

        /* copy the data from RAM to FLASH */
        printf("\n%s write...\n",update_intf_p->name);
        rc = update_intf_p->write(0, UBOOT_DATA_SIZE, buf);
        if (rc) {
            printf("write failed, return %d\n", rc);
            return 1;
        }

        unmap_physmem(buf, UBOOT_DATA_SIZE);
    }

	return 0;
}

/*
 * This is called by board_init() after the hardware has been set up
 * and is usable.
 * return -1, otherwise it will return 0;
 */
int do_auto_update(void)
{
	block_dev_desc_t *stor_dev;
	int old_ctrlc;
	int j;
	int state = -1;
	int dev;

	au_stor_curr_dev = -1;
	for (j = 0; j < MAX_UPDATE_INTF; j++) {
		if (0 != (unsigned int)s_intf[j].name[0]) {
			au_stor_curr_dev = s_intf[j].init();
			if (-1 == au_stor_curr_dev) {
				debug("%s storage device not found!\n",
						s_intf[j].name);
				continue;
			}

			dev = 0;
#if (defined CONFIG_ARCH_HI3559 || defined CONFIG_ARCH_HI3556)
			if (strncmp("mmc", s_intf[j].name, sizeof("mmc")) == 0)
				dev = 2;
#endif
			debug("device name %s!\n", s_intf[j].name);
			stor_dev = get_dev(s_intf[j].name, dev);
			if (NULL == stor_dev) {
				debug("Unknow device type!\n");
				continue;
			}

			if (fat_register_device(stor_dev, 1) != 0) {
				debug("Unable to use %s %d:%d for fatls\n",
						s_intf[j].name,
						au_stor_curr_dev,
						1);
				continue;
			}

			if (file_fat_detectfs() != 0) {
				debug("file_fat_detectfs failed\n");
				continue;
			}

            int reg = readl((void *)(SYS_CTRL_REG_BASE + REG_SYSSTAT));
	    if(GET_SYS_BOOT_MODE(reg)==BOOT_FROM_SPI) {
#define SFC_DEVICE_MODE_SHIFT 0x3
		    /*
		     * SFC_DEVICE_MODE:0:SPINOR 1:SPINAND
		     */
		    if(reg & (1<<SFC_DEVICE_MODE_SHIFT)) {
			    boot_medium_type = BOOT_FROM_NAND;
			    update_intf_p = &update_intf[UPDATE_MEDIUM_NAND];
		    } else {
			    boot_medium_type = BOOT_FROM_SPI;
			    update_intf_p = &update_intf[UPDATE_MEDIUM_SPINOR];
		    }
	    }
	    else if (GET_SYS_BOOT_MODE(reg)==BOOT_FROM_NAND) {
		    boot_medium_type = BOOT_FROM_NAND;
		    update_intf_p = &update_intf[UPDATE_MEDIUM_NAND];
	    } else if (GET_SYS_BOOT_MODE(reg) == BOOT_FROM_EMMC) {
		    boot_medium_type = BOOT_FROM_EMMC;
		    update_intf_p = &update_intf[UPDATE_MEDIUM_EMMC];
	    }
            /* add emmc update code */
            update_intf_p->init();

			if(get_env_from_sd()) {
                extern int bare_chip_program;
                if(bare_chip_program==1) {
                    printf("Warning:no config! only update u-boot.bin!\n");
                    save_bootdata_to_flash();//only update u-boot.bin loaded by bootrom
                    return 1;
                }
                extern int auto_update_flag;
                if(auto_update_flag==1) {
                    printf("Use old env!\n");
                }
            }
            if(bootargs_analyze()) {
                printf("ERROR:bootargs analyze fail!\n");
                continue;
            }

			/*
			 * make sure that we see CTRL-C
			 * and save the old state
			 */
			old_ctrlc = disable_ctrlc(0);

			state = update_to_flash();

			/* restore the old state */
			disable_ctrlc(old_ctrlc);

			s_intf[j].exit();

			/*
			 * no update file found
			 */
			if (-1 == state)
				continue;
			/*
			 * update files have been found on current medium,
			 * so just break here
			 */
			break;
		}
	}

    /*
     * pwoer off SD card when update finish
     */
#define MCI_PWREN		0x04
    writel(0, SDIO0_BASE_REG + MCI_PWREN);
    udelay(500*1000);
	/*
	 * If u-boot has been updated, it's better to save environment to flash
	 */
	if (1 == state) {
		env_crc_update();
		saveenv();
	}
	return 0;
}

#endif /* CONFIG_AUTO_UPDATE */
