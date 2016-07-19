/************************************************************************
* Copyright (C), 1988-2012, Huawei Tech. Co,.Ltd
* File name: cmd_bootss.h
* Author:     Version: 0.0.1    Date: 2012.09.05
* Description: upgrade module header file.
*
* History:
* Date          Who          Case                Description
* 2013.07.31    y00175887    DTS2013072205801    fastboot feature add
**************************************************************************/
#ifndef _CMD_BOOTSS_H_
#define _CMD_BOOTSS_H_

/* These are same definition as in linux 2.6.11 kernel. */
#define SWSUSP_SIG "S1SUSPEND"
#define PAGE_SIZE	4096
#define PAGE_OFFSET	(0xc0000000UL)
#define PHYS_OFFSET	(0x80000000UL)
#define MAP_PAGE_ENTRIES        (PAGE_SIZE / sizeof(sector_t) - 1)

#define SWAP_SIGNATURE_SIZE 10
#define UTS_SIZE 65
typedef u64 sector_t;

typedef struct {
	unsigned long val;
} swp_entry_t;

struct swsusp_header {
	char reserved[PAGE_SIZE - 20 - sizeof(sector_t) - sizeof(int)];
	sector_t image;		/*Offset holding the first sector
				  *offset of the snapshot */
	unsigned int flags;     /* Flags to pass to the "boot" kernel */
	char    orig_sig[SWAP_SIGNATURE_SIZE];
	char    sig[SWAP_SIGNATURE_SIZE];
};

struct new_utsname {
	char sysname[UTS_SIZE];
	char nodename[UTS_SIZE];
	char release[UTS_SIZE];
	char version[UTS_SIZE];
	char machine[UTS_SIZE];
	char domainname[UTS_SIZE];
};

struct swsusp_info {
	struct new_utsname      uts;
	u32                     version_code;
	unsigned long           num_physpages;
	int                     cpus;
	unsigned long           image_pages;
	unsigned long           pages;
	unsigned long           size;
} __attribute__((aligned(PAGE_SIZE)));

#define __virt_to_phys(x)	((x) - PAGE_OFFSET + PHYS_OFFSET)
#define __phys_to_virt(x)	((x) - PHYS_OFFSET + PAGE_OFFSET)

#define __ALIGN_UP(x, y) (((x) + (y) - 1) & (~((y) - 1)))
#define PAGE_ALIGN(x) __ALIGN_UP(x, PAGE_SIZE)
#define PAGE_NR(x) (((x)+PAGE_SIZE-1)/PAGE_SIZE)

void comp_save_snapshot_image(void);

int set_mtdparts_info(char *parts_info);
char *get_mtdparts_info(void);

#endif /* _CMD_BOOTSS_H_ */
