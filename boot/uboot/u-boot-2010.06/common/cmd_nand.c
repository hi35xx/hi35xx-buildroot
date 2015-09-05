/*
 * Driver for NAND support, Rick Bronson
 * borrowed heavily from:
 * (c) 1999 Machine Vision Holdings, Inc.
 * (c) 1999, 2000 David Woodhouse <dwmw2@infradead.org>
 *
 * Added 16-bit nand support
 * (C) 2004 Texas Instruments
 */

#include <common.h>
#include <linux/mtd/mtd.h>
#include <command.h>
#include <watchdog.h>
#include <malloc.h>
#include <asm/byteorder.h>
#include <jffs2/jffs2.h>
#include <nand.h>

#if defined(CONFIG_CMD_MTDPARTS)

/* partition handling routines */
int mtdparts_init(void);
int id_parse(const char *id, const char **ret_id, u8 *dev_type, u8 *dev_num);
int find_dev_and_part(const char *id, struct mtd_device **dev,
		      u8 *part_num, struct part_info **part);
#endif

#define HINFC_VER_300                   (0x300)
#define HINFC_VER_301                   (0x301)
#define HINFC_VER_310                   (0x310)
#define HINFC_VER_504                   (0x504)
#define HINFC_VER_505                   (0x505)
#define HINFC_VER_600                   (0x600)

#define _512B                               (512)
#define _2K                                 (2048)
#define _4K                                 (4096)
#define _8K                                 (8192)
#define _16K                                (16384)

enum ecc_type {
	et_ecc_none    = 0x00,
	et_ecc_1bit    = 0x01,
	et_ecc_4bit    = 0x02,
	et_ecc_4bytes  = 0x02,
	et_ecc_8bytes  = 0x03,
	et_ecc_24bit1k = 0x04,
	et_ecc_40bit1k = 0x05,
};

enum page_type {
	pt_pagesize_512   = 0x00,
	pt_pagesize_2K    = 0x01,
	pt_pagesize_4K    = 0x02,
	pt_pagesize_8K    = 0x03,
	pt_pagesize_16K   = 0x04,
};



/*
 * v504: 2k1b 2k4b 2k24b 4k1b 4k4b 4k24b 8k24b 8k40b
 * v301: 2k1b            4k1b 4k4b 4k24b 8k24b
 * v300: 2k1b            4k1b 4k4b 4k24b 8k24b
 *
 */

#define HINFC_VER_VER          (0xFFF00000)

#define SET_HINFC_VER(_ver, _pagesize, _ecc) \
	((((_ver) << 20) & HINFC_VER_VER) | \
		(((_pagesize) & 0xFFFF) << 4) | ((_ecc) & 0xF))

static unsigned int hinfc300_support_yaffs2[] = {
	SET_HINFC_VER(HINFC_VER_300, _2K, et_ecc_1bit),

	SET_HINFC_VER(HINFC_VER_300, _4K, et_ecc_1bit),
	SET_HINFC_VER(HINFC_VER_300, _4K, et_ecc_4bytes),
	SET_HINFC_VER(HINFC_VER_300, _4K, et_ecc_24bit1k),

	SET_HINFC_VER(HINFC_VER_300, _8K, et_ecc_24bit1k),
	0,
};

static unsigned int hinfc301_support_yaffs2[] = {
	SET_HINFC_VER(HINFC_VER_301, _2K, et_ecc_1bit),

	SET_HINFC_VER(HINFC_VER_301, _4K, et_ecc_1bit),
	SET_HINFC_VER(HINFC_VER_301, _4K, et_ecc_4bytes),
	SET_HINFC_VER(HINFC_VER_301, _4K, et_ecc_24bit1k),

	SET_HINFC_VER(HINFC_VER_301, _8K, et_ecc_24bit1k),
	0,
};

static unsigned int hinfc504_support_yaffs2[] = {
	SET_HINFC_VER(HINFC_VER_504, _2K, et_ecc_4bit),
	SET_HINFC_VER(HINFC_VER_504, _2K, et_ecc_8bytes),
	SET_HINFC_VER(HINFC_VER_504, _2K, et_ecc_24bit1k),

	SET_HINFC_VER(HINFC_VER_301, _4K, et_ecc_1bit),
	SET_HINFC_VER(HINFC_VER_301, _4K, et_ecc_4bytes),
	SET_HINFC_VER(HINFC_VER_504, _4K, et_ecc_8bytes),
	SET_HINFC_VER(HINFC_VER_301, _4K, et_ecc_24bit1k),

	SET_HINFC_VER(HINFC_VER_301, _8K, et_ecc_24bit1k),
	SET_HINFC_VER(HINFC_VER_504, _8K, et_ecc_40bit1k),
	0,
};
/*****************************************************************************/

static unsigned int *get_support_yaffs2(unsigned int nandip)
{
	switch (nandip) {
		default:
		case HINFC_VER_300:
			return hinfc300_support_yaffs2;
		case HINFC_VER_301:
			return hinfc301_support_yaffs2;
		case HINFC_VER_504:
			return hinfc504_support_yaffs2;
	}
}
/*****************************************************************************/

static unsigned int get_yaffs2_version(unsigned int nandip, int pagesize,
	int ecc)
{
	int ix;
	unsigned int *ver = get_support_yaffs2(nandip);
	unsigned int tmp = SET_HINFC_VER(0, pagesize, ecc);

	for (ix = 0; ver[ix]; ix++) {
		if ((ver[ix] & ~HINFC_VER_VER) == tmp)
			return ver[ix];
	}

	return 0;
}
static int yaffs_tag_check(unsigned char *buffer, unsigned int writesize,
	unsigned int length)
{
	unsigned int hinfc_yaff_ver;
	unsigned int yaffs_yaff_ver;
	static char *ecctype_str[] = 
		{ "None", "1bit", "4Bytes", "8Bytes", "24bits/1K", "unknown", "unknown", "unknown"};

	/* this follow must be consistent with mkyaffs2image !!! */
	struct yaffs2_tag
	{
	#define YAFFS2_SIGN_MAGIC    "YFSS!V10"
		unsigned char magic[8];
		unsigned int nandip;
		unsigned char yaffs2ver[4];
		unsigned int pagesize; 
		unsigned int ecctype; 
	};
	
	extern unsigned int nand_ip_version;
	extern int nand_get_ecctype(void);

	struct yaffs2_tag *tags = (struct yaffs2_tag *)buffer;
	unsigned int ecctype = nand_get_ecctype();

	if (length < 512)
	{
		printf("buffer length is too short.\n");
		return -1;
	}

	if (memcmp(tags->magic, (unsigned char *)YAFFS2_SIGN_MAGIC, 8))
	{
		printf("!!! The yaffs2 filesystem image has no tag information. \n"
			"please update your mkyaffs2image tool, and remake yaffs2 filesystem image.\n");
		return -1;
	}

	if (writesize != tags->pagesize)
	{
		printf("!!! yaffs2 filesystem image pagesize(%d) is NOT consistent with hardware pagesize(%d).\n",
			tags->pagesize, writesize);
		goto fail;
	}
	
	if (ecctype != tags->ecctype)
	{
		printf("!!! yaffs2 filesystem image ecctype(%s) is NOT consistent with hardware ecctype(%s).\n",
			ecctype_str[tags->ecctype & 0xF], ecctype_str[ecctype & 0xF]);
		goto fail;
	}

	/*
	 * When write 8k40bit yaffs2 filesystem to hinfc301,
	 * It will print this error, because cpu not support.
	 */
	hinfc_yaff_ver = get_yaffs2_version(nand_ip_version, writesize,
			ecctype);
	if (!hinfc_yaff_ver) {
		printf("!!! The yaffs2 filesystem "
			"or mkyaffs2image for cpu ver(0x%X) "
			"But your demo board cpu ver(0x%X).\n",
			tags->nandip, nand_ip_version);
		goto fail1;
	}

	yaffs_yaff_ver = get_yaffs2_version(tags->nandip, writesize, ecctype);
	if (!yaffs_yaff_ver) {
		printf("!!! The yaffs2 filesystem image"
			" has invalid tag information.\n");
		goto fail1;
	}

	if (hinfc_yaff_ver != yaffs_yaff_ver) {
		printf("!!! The yaffs2 filesystem "
			"or mkyaffs2image for cpu ver(0x%X) "
			"But your demo board cpu ver(0x%X).\n",
			tags->nandip, nand_ip_version);
		goto fail1;
	}
	return 0;

fail:
	printf("Please remake yaffs2 filesystem image, "
		"make sure your yaffs2 filesystem image pagesize and ecctype is consistent with hardware config.\n");
	printf("Current hardware config, pagesize:%d, ecctype:%s\n", 
		writesize, ecctype_str[ecctype & 0xF]);

	return -1;
fail1:
	printf("1. Confirm your yaffs2 filesystem image version.\n"
			"2. Update your mkyaffs2image tool,"
			" remake yaffs2 filesystem image.\n");

	return -1;
}

static int nand_dump(nand_info_t *nand, loff_t off, int only_oob)
{
	int i;
	u_char *datbuf, *oobbuf, *p;

	datbuf = malloc(nand->writesize + nand->oobsize);
	oobbuf = malloc(nand->oobsize);
	if (!datbuf || !oobbuf) {
		puts("No memory for page buffer\n");
		return 1;
	}
	off &= ~((loff_t)nand->writesize - 1);
	struct mtd_oob_ops ops;
	memset(&ops, 0, sizeof(ops));
	ops.datbuf = datbuf;
	ops.oobbuf = oobbuf; /* must exist, but oob data will be appended to ops.datbuf */
	ops.len = nand->writesize;
	ops.ooblen = nand->oobsize;
	ops.mode = MTD_OOB_RAW;
	i = nand->read_oob(nand, off, &ops);
	if (i < 0) {
		printf("Error (%d) reading page %08llx\n", i, off);
		free(datbuf);
		free(oobbuf);
		return 1;
	}
	printf("Page %08llx dump:\n", off);
	i = nand->writesize >> 4;
	p = datbuf;

	while (i--) {
		if (!only_oob)
			printf("\t%02x %02x %02x %02x %02x %02x %02x %02x"
			       "  %02x %02x %02x %02x %02x %02x %02x %02x\n",
			       p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
			       p[8], p[9], p[10], p[11], p[12], p[13], p[14],
			       p[15]);
		p += 16;
	}
	puts("OOB:\n");
	i = nand->oobsize >> 3;
	while (i--) {
		printf("\t%02x %02x %02x %02x %02x %02x %02x %02x\n",
		       p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
		p += 8;
	}
	free(datbuf);
	free(oobbuf);

	return 0;
}

/* ------------------------------------------------------------------------- */

static inline int str2ll(char *p, loff_t *num)
{
	char *endptr;

	*num = (loff_t)simple_strtoull(p, &endptr, 16);
	return (*p != '\0' && *endptr == '\0') ? 1 : 0;
}

static int
arg_off_size(int argc, char *argv[], nand_info_t *nand, loff_t *off, loff_t *size)
{
	int idx = nand_curr_device;
#if defined(CONFIG_CMD_MTDPARTS)
	struct mtd_device *dev;
	struct part_info *part;
	u8 pnum;

	if (argc >= 1 && !(str2ll(argv[0], off))) {
		if ((mtdparts_init() == 0) &&
		    (find_dev_and_part(argv[0], &dev, &pnum, &part) == 0)) {
			if (dev->id->type != MTD_DEV_TYPE_NAND) {
				puts("not a NAND device\n");
				return -1;
			}
			*off = part->offset;
			if (argc >= 2) {
				if (!(str2ll(argv[1], size))) {
					printf("'%s' is not a number\n", argv[1]);
					return -1;
				}
				if (*size > part->size)
					*size = part->size;
			} else {
				*size = part->size;
			}
			idx = dev->id->num;
			*nand = nand_info[idx];
			goto out;
		}
	}
#endif

	if (argc >= 1) {
		if (!(str2ll(argv[0], off))) {
			printf("'%s' is not a number\n", argv[0]);
			return -1;
		}
	} else {
		*off = 0;
	}

	if (argc >= 2) {
		if (!(str2ll(argv[1], size))) {
			printf("'%s' is not a number\n", argv[1]);
			return -1;
		}
	} else {
		*size = nand->size - *off;
	}

#if defined(CONFIG_CMD_MTDPARTS)
out:
#endif
	printf("device %d ", idx);
	if (*size == nand->size)
		puts("whole chip\n");
	else
		printf("offset 0x%llx, size 0x%llx\n", *off, *size);
	return 0;
}

#ifdef CONFIG_CMD_NAND_LOCK_UNLOCK
static void print_status(ulong start, ulong end, ulong erasesize, int status)
{
	printf("%08lx - %08lx: %08lx blocks %s%s%s\n",
		start,
		end - 1,
		(end - start) / erasesize,
		((status & NAND_LOCK_STATUS_TIGHT) ?  "TIGHT " : ""),
		((status & NAND_LOCK_STATUS_LOCK) ?  "LOCK " : ""),
		((status & NAND_LOCK_STATUS_UNLOCK) ?  "UNLOCK " : ""));
}

static void do_nand_status(nand_info_t *nand)
{
	ulong block_start = 0;
	ulong off;
	int last_status = -1;

	struct nand_chip *nand_chip = nand->priv;
	/* check the WP bit */
	nand_chip->cmdfunc(nand, NAND_CMD_STATUS, -1, -1);
	printf("device is %swrite protected\n",
		(nand_chip->read_byte(nand) & 0x80 ?
		"NOT " : ""));

	for (off = 0; off < nand->size; off += nand->erasesize) {
		int s = nand_get_lock_status(nand, off);

		/* print message only if status has changed */
		if (s != last_status && off != 0) {
			print_status(block_start, off, nand->erasesize,
					last_status);
			block_start = off;
		}
		last_status = s;
	}
	/* Print the last block info */
	print_status(block_start, off, nand->erasesize, last_status);
}
#endif

static void nand_print_info(int idx)
{
	nand_info_t *nand = &nand_info[idx];
	struct nand_chip *chip = nand->priv;
	printf("Device %d: ", idx);
	if (chip->numchips > 1)
		printf("%dx ", chip->numchips);
	printf("%s, sector size %u KiB\n",
	       nand->name, nand->erasesize >> 10);
}

int do_nand(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int i, dev, ret = 0;
	ulong addr;
	size_t rw_size;
	loff_t off, size;
	char *cmd, *s;
	nand_info_t *nand;
#ifdef CONFIG_SYS_NAND_QUIET
	int quiet = CONFIG_SYS_NAND_QUIET;
#else
	int quiet = 0;
#endif
	const char *quiet_str = getenv("quiet");

	/* at least two arguments please */
	if (argc < 2)
		goto usage;

	if (quiet_str)
		quiet = simple_strtoul(quiet_str, NULL, 0) != 0;

	cmd = argv[1];

	if (strcmp(cmd, "info") == 0) {

		putc('\n');
		for (i = 0; i < CONFIG_SYS_MAX_NAND_DEVICE; i++) {
			if (nand_info[i].name)
				nand_print_info(i);
		}
		return 0;
	}

	if (strcmp(cmd, "device") == 0) {

		if (argc < 3) {
			putc('\n');
			if ((nand_curr_device < 0) ||
			    (nand_curr_device >= CONFIG_SYS_MAX_NAND_DEVICE))
				puts("no devices available\n");
			else
				nand_print_info(nand_curr_device);
			return 0;
		}
		dev = (int)simple_strtoul(argv[2], NULL, 10);
		if (dev < 0 || dev >= CONFIG_SYS_MAX_NAND_DEVICE || !nand_info[dev].name) {
			puts("No such device\n");
			return 1;
		}
		printf("Device %d: %s", dev, nand_info[dev].name);
		puts("... is now current device\n");
		nand_curr_device = dev;

#ifdef CONFIG_SYS_NAND_SELECT_DEVICE
		/*
		 * Select the chip in the board/cpu specific driver
		 */
		board_nand_select_device(nand_info[dev].priv, dev);
#endif

		return 0;
	}

	if (strcmp(cmd, "bad") != 0 && strcmp(cmd, "erase") != 0 &&
	    strncmp(cmd, "dump", 4) != 0 &&
	    strncmp(cmd, "read", 4) != 0 && strncmp(cmd, "write", 5) != 0 &&
	    strcmp(cmd, "scrub") != 0 && strcmp(cmd, "markbad") != 0 &&
	    strcmp(cmd, "biterr") != 0 &&
	    strcmp(cmd, "lock") != 0 && strcmp(cmd, "unlock") != 0 )
		goto usage;

	/* the following commands operate on the current device */
	if (nand_curr_device < 0 || nand_curr_device >= CONFIG_SYS_MAX_NAND_DEVICE ||
	    !nand_info[nand_curr_device].name) {
		puts("\nno devices available\n");
		return 1;
	}
	nand = &nand_info[nand_curr_device];

	if (strcmp(cmd, "bad") == 0) {
		printf("\nDevice %d bad blocks:\n", nand_curr_device);
		for (off = 0; off < nand->size; off += nand->erasesize)
			if (nand_block_isbad(nand, off))
				printf("  %08llx\n", off);
		return 0;
	}

	/*
	 * Syntax is:
	 *   0    1     2       3    4
	 *   nand erase [clean] [off size]
	 */
	if (strcmp(cmd, "erase") == 0 || strcmp(cmd, "scrub") == 0) {
		nand_erase_options_t opts;
		/* "clean" at index 2 means request to write cleanmarker */
		int clean = argc > 2 && !strcmp("clean", argv[2]);
		int o = clean ? 3 : 2;
		int scrub = !strcmp(cmd, "scrub");

		printf("\nNAND %s: ", scrub ? "scrub" : "erase");
		/* skip first two or three arguments, look for offset and size */
		if (arg_off_size(argc - o, argv + o, nand, &off, &size) != 0)
			return 1;

		if (off + size > nand->size) {
			printf("ERROR: erase area is out of range!\n\n");
			return 1;
		}

		if (off & (nand->erasesize - 1)) {
			printf("ERROR: erase start address is "
					"not block aligned!\n\n");
			return 1;
		}

		if (size & (nand->erasesize - 1)) {
			printf("ERROR: erase length is "
					"not block aligned!\n\n");
			return 1;
		}

		memset(&opts, 0, sizeof(opts));
		opts.offset = off;
		opts.length = size;
		opts.jffs2  = clean;
		opts.quiet  = quiet;

		if (scrub) {
			puts("Warning: "
			     "scrub option will erase all factory set "
			     "bad blocks!\n"
			     "         "
			     "There is no reliable way to recover them.\n"
			     "         "
			     "Use this command only for testing purposes "
			     "if you\n"
			     "         "
			     "are sure of what you are doing!\n"
			     "\nReally scrub this NAND flash? <y/N>\n");

			if (getc() == 'y') {
				puts("y");
				if (getc() == '\r')
					opts.scrub = 1;
				else {
					puts("scrub aborted\n");
					return -1;
				}
			} else {
				puts("scrub aborted\n");
				return -1;
			}
		}
		ret = nand_erase_opts(nand, &opts);
		printf("%s\n", ret ? "ERROR" : "OK");

		return ret == 0 ? 0 : 1;
	}

	if (strncmp(cmd, "dump", 4) == 0) {
		if (argc < 3)
			goto usage;

		s = strchr(cmd, '.');
		off = simple_strtoull(argv[2], NULL, 16);

		if (s != NULL && strcmp(s, ".oob") == 0)
			ret = nand_dump(nand, off, 1);
		else
			ret = nand_dump(nand, off, 0);

		return ret == 0 ? 1 : 0;

	}

	if (strncmp(cmd, "read", 4) == 0 || strncmp(cmd, "write", 5) == 0) {
		int read;

		if (argc < 4)
			goto usage;

		addr = simple_strtoul(argv[2], NULL, 16);

		read = strncmp(cmd, "read", 4) == 0; /* 1 = read, 0 = write */
		printf("\nNAND %s: ", read ? "read" : "write");
		if (arg_off_size(argc - 3, argv + 3, nand, &off, &size) != 0)
			return 1;

		rw_size = (size_t)size;

		if (off + rw_size > nand->size) {
			printf("ERROR: %s", read ? "read " : "write ");
			printf("area is out of range!\n\n");
			return 1;
		}

		s = strchr(cmd, '.');
		if (!s || !strcmp(s, ".jffs2") ||
		    !strcmp(s, ".e") || !strcmp(s, ".i")) {
			if (read)
				ret = nand_read_skip_bad(nand, off, &rw_size,
							 (u_char *)addr);
			else {
				if (off & (nand->writesize - 1)) {
					printf("ERROR: write start address is "
						"not page aligned!\n\n");
					return 1;
				}
				if (rw_size & (nand->writesize - 1)) {
					printf("ERROR: write length is "
						"not page aligned!\n\n");
					return 1;
				}

				ret = nand_write_skip_bad(nand, off, &rw_size,
							  (u_char *)addr);
			}
		} else if (!strcmp(s, ".oob")) {
			/* out-of-band data */
			mtd_oob_ops_t ops = {
				.oobbuf = (u8 *)addr,
				.ooblen = size,
				.mode = MTD_OOB_RAW
			};

			if (read)
				ret = nand->read_oob(nand, off, &ops);
			else
				ret = nand->write_oob(nand, off, &ops);
		} else if (s != NULL && !strcmp(s, ".yaffs")) {
			if (read)
			{
				ret = nand_read_yaffs_skip_bad(nand, off,
					&rw_size, (u_char *)addr);
			}
			else
			{
				if (yaffs_tag_check((unsigned char *)addr, nand->writesize, size))
					return 1;

				ret = nand_write_yaffs_skip_bad(nand, off, &rw_size,
					(u_char *)addr);
			}
		} else if (s != NULL && !strcmp(s, ".yaffsuc")) {
			if (read) {
				printf("not support\n");
				return 1;
			} else
				ret = nand_write_yaffs_skip_bad(nand, off,
					&rw_size, (u_char *)addr);
		} else {
			printf("Unknown nand command suffix '%s'.\n", s);
			return 1;
		}

		printf(" %llu bytes %s: %s\n", size,
		       read ? "read" : "written", ret ? "ERROR" : "OK");

		return ret == 0 ? 0 : 1;
	}

	if (strcmp(cmd, "markbad") == 0) {
		argc -= 2;
		argv += 2;

		if (argc <= 0)
			goto usage;

		while (argc > 0) {
			off = simple_strtoull(*argv, NULL, 16);

			if (nand->block_markbad(nand, off)) {
				printf("block 0x%08llx NOT marked "
					"as bad! ERROR %d\n",
					off, ret);
				ret = 1;
			} else {
				printf("block 0x%08llx successfully "
					"marked as bad\n",
					off);
			}
			--argc;
			++argv;
		}
		return ret;
	}

	if (strcmp(cmd, "biterr") == 0) {
		/* todo */
		return 1;
	}

#ifdef CONFIG_CMD_NAND_LOCK_UNLOCK
	if (strcmp(cmd, "lock") == 0) {
		int tight = 0;
		int status = 0;
		if (argc == 3) {
			if (!strcmp("tight", argv[2]))
				tight = 1;
			if (!strcmp("status", argv[2]))
				status = 1;
		}
		if (status) {
			do_nand_status(nand);
		} else {
			if (!nand_lock(nand, tight)) {
				puts("NAND flash successfully locked\n");
			} else {
				puts("Error locking NAND flash\n");
				return 1;
			}
		}
		return 0;
	}

	if (strcmp(cmd, "unlock") == 0) {
		if (arg_off_size(argc - 2, argv + 2, nand, &off, &size) < 0)
			return 1;

		if (!nand_unlock(nand, off, size)) {
			puts("NAND flash successfully unlocked\n");
		} else {
			puts("Error unlocking NAND flash, "
			     "write and erase will probably fail\n");
			return 1;
		}
		return 0;
	}
#endif

usage:
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(nand, CONFIG_SYS_MAXARGS, 1, do_nand,
	"NAND sub-system",
	"info - show available NAND devices\n"
	"nand device [dev] - show or set current device\n"
	"nand read - addr off|partition size\n"
	"nand write - addr off|partition size\n"
	"    read/write 'size' bytes starting at offset 'off'\n"
	"    to/from memory address 'addr', skipping bad blocks.\n"
	"nand erase [clean] [off size] - erase 'size' bytes from\n"
	"    offset 'off' (entire device if not specified)\n"
	"nand bad - show bad blocks\n"
	"nand dump[.oob] off - dump page\n"
	"nand scrub - really clean NAND erasing bad blocks (UNSAFE)\n"
	"nand markbad off [...] - mark bad block(s) at offset (UNSAFE)\n"
	"nand biterr off - make a bit error at offset (UNSAFE)"
#ifdef CONFIG_CMD_NAND_LOCK_UNLOCK
	"\n"
	"nand lock [tight] [status]\n"
	"    bring nand to lock state or display locked pages\n"
	"nand unlock [offset] [size] - unlock section"
#endif
);

static int nand_load_image(cmd_tbl_t *cmdtp, nand_info_t *nand,
			   loff_t offset, ulong addr, char *cmd)
{
	int r;
	char *ep, *s;
	size_t cnt;
	image_header_t *hdr;
#if defined(CONFIG_FIT)
	const void *fit_hdr = NULL;
#endif

	s = strchr(cmd, '.');
	if (s != NULL &&
	    (strcmp(s, ".jffs2") && strcmp(s, ".e") && strcmp(s, ".i"))) {
		printf("Unknown nand load suffix '%s'\n", s);
		show_boot_progress(-53);
		return 1;
	}

	printf("\nLoading from %s, offset 0x%llx\n", nand->name, offset);

	cnt = nand->writesize;
	r = nand_read_skip_bad(nand, offset, &cnt, (u_char *) addr);
	if (r) {
		puts("** Read error\n");
		show_boot_progress (-56);
		return 1;
	}
	show_boot_progress (56);

	switch (genimg_get_format ((void *)addr)) {
	case IMAGE_FORMAT_LEGACY:
		hdr = (image_header_t *)addr;

		show_boot_progress (57);
		image_print_contents (hdr);

		cnt = image_get_image_size (hdr);
		break;
#if defined(CONFIG_FIT)
	case IMAGE_FORMAT_FIT:
		fit_hdr = (const void *)addr;
		puts ("Fit image detected...\n");

		cnt = fit_get_size (fit_hdr);
		break;
#endif
	default:
		show_boot_progress (-57);
		puts ("** Unknown image type\n");
		return 1;
	}
	show_boot_progress (57);

	r = nand_read_skip_bad(nand, offset, &cnt, (u_char *) addr);
	if (r) {
		puts("** Read error\n");
		show_boot_progress (-58);
		return 1;
	}
	show_boot_progress (58);

#if defined(CONFIG_FIT)
	/* This cannot be done earlier, we need complete FIT image in RAM first */
	if (genimg_get_format ((void *)addr) == IMAGE_FORMAT_FIT) {
		if (!fit_check_format (fit_hdr)) {
			show_boot_progress (-150);
			puts ("** Bad FIT image format\n");
			return 1;
		}
		show_boot_progress (151);
		fit_print_contents (fit_hdr);
	}
#endif

	/* Loading ok, update default load address */

	load_addr = addr;

	/* Check if we should attempt an auto-start */
	if (((ep = getenv("autostart")) != NULL) && (strcmp(ep, "yes") == 0)) {
		char *local_args[2];
		extern int do_bootm(cmd_tbl_t *, int, int, char *[]);

		local_args[0] = cmd;
		local_args[1] = NULL;

		printf("Automatic boot of image at addr 0x%08lx ...\n", addr);

		do_bootm(cmdtp, 0, 1, local_args);
		return 1;
	}
	return 0;
}

int do_nandboot(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	char *boot_device = NULL;
	int idx;
	ulong addr;
	loff_t offset = 0;
#if defined(CONFIG_CMD_MTDPARTS)
	struct mtd_device *dev;
	struct part_info *part;
	u8 pnum;

	if (argc >= 2) {
		char *p = (argc == 2) ? argv[1] : argv[2];
		if (!(str2long(p, &addr)) && (mtdparts_init() == 0) &&
		    (find_dev_and_part(p, &dev, &pnum, &part) == 0)) {
			if (dev->id->type != MTD_DEV_TYPE_NAND) {
				puts("Not a NAND device\n");
				return 1;
			}
			if (argc > 3)
				goto usage;
			if (argc == 3)
				addr = simple_strtoul(argv[1], NULL, 16);
			else
				addr = CONFIG_SYS_LOAD_ADDR;
			return nand_load_image(cmdtp, &nand_info[dev->id->num],
					       part->offset, addr, argv[0]);
		}
	}
#endif

	show_boot_progress(52);
	switch (argc) {
	case 1:
		addr = CONFIG_SYS_LOAD_ADDR;
		boot_device = getenv("bootdevice");
		break;
	case 2:
		addr = simple_strtoul(argv[1], NULL, 16);
		boot_device = getenv("bootdevice");
		break;
	case 3:
		addr = simple_strtoul(argv[1], NULL, 16);
		boot_device = argv[2];
		break;
	case 4:
		addr = simple_strtoul(argv[1], NULL, 16);
		boot_device = argv[2];
		offset = simple_strtoull(argv[3], NULL, 16);
		break;
	default:
#if defined(CONFIG_CMD_MTDPARTS)
usage:
#endif
		cmd_usage(cmdtp);
		show_boot_progress(-53);
		return 1;
	}

	show_boot_progress(53);
	if (!boot_device) {
		puts("\n** No boot device **\n");
		show_boot_progress(-54);
		return 1;
	}
	show_boot_progress(54);

	idx = simple_strtoul(boot_device, NULL, 16);

	if (idx < 0 || idx >= CONFIG_SYS_MAX_NAND_DEVICE || !nand_info[idx].name) {
		printf("\n** Device %d not available\n", idx);
		show_boot_progress(-55);
		return 1;
	}
	show_boot_progress(55);

	return nand_load_image(cmdtp, &nand_info[idx], offset, addr, argv[0]);
}

U_BOOT_CMD(nboot, 4, 1, do_nandboot,
	"boot from NAND device",
	"[partition] | [[[loadAddr] dev] offset]"
);

