#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/mount.h>
#include <linux/genhd.h>
#include <linux/lzo.h>
#include <linux/vmalloc.h>
#include <asm/io.h>
#include <asm/cacheflush.h>

#include "power.h"
#include "compress.h"

extern int 
find_bootmem(const char *name, unsigned int *ofs, unsigned int *sz);

#define BOOTLOADER_ADMA_SIDE_JOB

#define __ALIGN_UP(x,y) (((x)+(y)-1) & (~((y)-1)))
#define PAGE_NR(x) (((x) + PAGE_SIZE - 1) / PAGE_SIZE)

#ifdef BOOTLOADER_ADMA_SIDE_JOB
/* guessing ratio started with (5+5)M:2M */
#define COMP_SRC_SZ_DEFAULT (SZ_1M * 5U)
#define COMP_DST_SZ_DEFAULT (SZ_1M * 2U)
#else
#define COMP_SRC_SZ_DEFAULT (SZ_1M * 9U)
#define COMP_DST_SZ_DEFAULT (SZ_1M * 3U)
#endif

extern unsigned long snapshot_buf_start;
extern unsigned long snapshot_buf_size;

#ifdef CONFIG_DEFAULT_MTD

static int hbpart_read(struct compress_writer *writer,
		sector_t page_i, sector_t page_nr, void *buf)
{
	struct block_device *bdev_orig = hib_resume_bdev;
	sector_t p;
	int ret;

	bdev_orig = hib_resume_bdev;
	hib_resume_bdev = writer->bdev;
	for (p = 0; p < page_nr; p++, page_i++, buf += PAGE_SIZE) {
		ret = hib_bio_read_page(page_i, writer->pagebuf, NULL);
		if (ret < 0) {
			hib_resume_bdev = bdev_orig;
			return ret;
		}
		memcpy(buf, writer->pagebuf, PAGE_SIZE);
	}
	hib_resume_bdev = bdev_orig;
	return (int)p;
}

static int hbpart_write(struct compress_writer *writer,
		sector_t page_i, sector_t page_nr, const void *buf)
{
	struct block_device *bdev_orig = hib_resume_bdev;
	sector_t p;
	int ret;

	bdev_orig = hib_resume_bdev;
	hib_resume_bdev = writer->bdev;
	for (p = 0; p < page_nr; p++, page_i++, buf += PAGE_SIZE) {
		memcpy(writer->pagebuf, buf, PAGE_SIZE);
		ret = hib_bio_write_page(page_i, writer->pagebuf, NULL);
		if (ret < 0) {
			hib_resume_bdev = bdev_orig;
			return ret;
		}
	}
	hib_resume_bdev = bdev_orig;
	return (int)p;
}

static int susp_writer_finish(struct compress_writer *writer)
{
	/*do nothing*/
	return 0;
}

#else

#define REG_SYSCTRL_BASE   (IO_ADDRESS(0x20050000))

static int hbpart_read(struct compress_writer *writer,
		sector_t page_i, sector_t page_nr, void *buf)
{
	sector_t p;

	sector_t total_pages;

	if (!writer)
		return -EINVAL;

	total_pages = writer->image_buf_sz/PAGE_SIZE;

	if ((page_i + page_nr) > total_pages)
		return -EINVAL;

	for (p = 0; p < page_nr; p++, page_i++, buf += PAGE_SIZE)
		memcpy(buf, (writer->image + page_i*PAGE_SIZE), PAGE_SIZE);

	return (int)p;
}

static int hbpart_write(struct compress_writer *writer,
		sector_t page_i, sector_t page_nr, const void *buf)
{
	sector_t p;

	sector_t total_pages;

	if (!writer)
		return -EINVAL;

	total_pages = writer->image_buf_sz/PAGE_SIZE;

	if ((page_i + page_nr) > total_pages) {
		printk(KERN_INFO "%s write data exceed!\n", __func__);
		return -EINVAL;
	}

	for (p = 0; p < page_nr; p++, page_i++, buf += PAGE_SIZE)
		memcpy((writer->image + page_i*PAGE_SIZE), buf, PAGE_SIZE);

	flush_cache_all();

	return (int)p;
}

static int susp_writer_finish(struct compress_writer *writer)
{
	unsigned long image_addr, image_sz;

	union sscomp_header *sscomp = &writer->sscomp;
	const struct sscomp_block *block = &sscomp->b.blocks[0];
	int total_blocks = sscomp->info.meta_blocks + sscomp->info.data_blocks;
	int total_org_size = 0;
	int total_comp_size = 0;
	int i;

#define SNAPSHOT_MAGIC  0x53414e50  /*'S''N''A''P'*/

	writel(SNAPSHOT_MAGIC, REG_SYSCTRL_BASE + 0xa4);

	/*translate to phyaddr*/
	image_addr = snapshot_buf_start +
			((unsigned long)writer->image - (unsigned long)writer);

	writel(image_addr, REG_SYSCTRL_BASE + 0xa8);

	for (i = 0; i < total_blocks; i++) {
		unsigned int aligned_comp_size;
		aligned_comp_size =
			(block->comp_size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
		total_org_size += block->org_size;
		total_comp_size += aligned_comp_size;
		printk(KERN_INFO "block[%2d]: %9d:%d(%d)\n", i,
			  block->org_size, block->comp_size, aligned_comp_size);
		block++;
	}

	printk(KERN_INFO "original:   %9d bytes\n", total_org_size);
	printk(KERN_INFO "compressed: %9d bytes\n", total_comp_size);

	image_sz = __ALIGN_UP(total_comp_size+sizeof(*sscomp), SZ_1M);
	writel(image_sz, REG_SYSCTRL_BASE + 0xac);

	return 0;
}

#endif

static int do_compress(struct compress_writer *writer,
		struct sscomp_block *block, unsigned int *src_len)
{
	void *src = (void*)writer->src;
	void *dst = (void*)writer->dst;
	size_t tmp_slen, tmp_dlen;
	const unsigned int slen = *src_len;
	int ret, nr_compressed_pages;
	bool succeed = false;
	struct compress_wrapper *compressor = writer->compressor;
	void *work_mem = (void*)writer->work_mem;

	unsigned int sb_len = COMP_SRC_SZ_DEFAULT;
	unsigned int db_len = COMP_DST_SZ_DEFAULT;

	do {
		printk(KERN_DEBUG "[comp: guessing %08u:%08u]\n",
				sb_len, db_len);
		tmp_slen = min_t(size_t, slen, sb_len);
		tmp_dlen = db_len;

		ret = compressor->compress(
			dst, &tmp_dlen, src, tmp_slen, 0, work_mem);
		if (ret != 0) {
			succeed = false;
			break;
		} else if (tmp_dlen <= db_len) {
			succeed = true;
			break;
		}

		sb_len -= SZ_1K * 512U;
		db_len += SZ_1K * 512U;
	} while (sb_len >= db_len);

	if (!succeed)
		return -EFAULT;

	printk(KERN_INFO "[comp: %08u -> %08u]\n", tmp_slen, tmp_dlen);

	block->org_size = tmp_slen;
	block->comp_size = tmp_dlen;

	nr_compressed_pages = (tmp_dlen + PAGE_SIZE - 1) / PAGE_SIZE;

	*src_len = slen - tmp_slen;
	/* backup before verifying */
	if (*src_len)
		memmove(dst + writer->dst_buf_sz - *src_len,
				src + tmp_slen, *src_len);

	/* restore after verifying */
	if (*src_len)
		memmove(src, dst + writer->dst_buf_sz - *src_len, *src_len);

	return nr_compressed_pages;
}

void put_susp_compress_writer(struct compress_writer *writer)
{
	if (!writer)
		return;

	if (writer->pagebuf)
		free_page((unsigned long)writer->pagebuf);
	writer->pagebuf = NULL;

	if (writer->bdev)
		blkdev_put(writer->bdev, FMODE_WRITE);
	writer->bdev = NULL;

	iounmap(writer);
}

struct compress_writer *get_susp_compress_writer(const char *path)
{
	struct compress_writer *writer;
	struct compress_wrapper *compressor;
	/* struct block_device *bdev; */
	/* const char *d; */
	/* char spath[64], dname[BDEVNAME_SIZE]; */
	/* dev_t device; */
	/* int ret; */

	BUILD_BUG_ON(sizeof(writer->sscomp.info) > sizeof(writer->sscomp.b.c));
	BUILD_BUG_ON(sizeof(writer->sscomp.b) > sizeof(writer->sscomp.c));
	BUILD_BUG_ON(sizeof(writer->sscomp) % PAGE_SIZE);

	compressor = hb_bdev_find_compressor_by_name(compress_method);
	if (!compressor) {
		printk(KERN_ERR "%s: could not found compressor %s\n",
				__func__, compress_method);
		return NULL;
	}

#if 0
	if (!path || path[0] == '\0') {
		printk(KERN_ERR "%s: invalid backing device path.\n", __func__);
		return NULL;
	}

	strncpy(spath, path, sizeof(spath));
	if (strncmp(spath, "/dev/block/", 11) == 0)
		strcpy(spath + 5, spath + 11);
	device = name_to_dev_t(spath);
	if (device == MKDEV(0, 0)) {
		printk(KERN_ERR "%s: could not found %s.\n", __func__, path);
		return NULL;
	}

	bdev = bdget(device);
	if (!bdev) {
		printk(KERN_ERR "%s: could not ref %s.\n", __func__, path);
		return NULL;
	}
	bdev = bdgrab(bdev);
	bdput(bdev);

	ret = blkdev_get(bdev, FMODE_WRITE, NULL);
	if (ret) {
		printk(KERN_ERR "%s: could not get %s.\n", __func__, path);
		bdput(bdev);
		return NULL;
	}

	if (strncmp(spath, "/dev/", 5) == 0)
		strcpy(spath, spath + 5);
	d = bdevname(bdev, dname);
	if (strcmp(spath, d)) {
		printk(KERN_ERR "%s: something wrong %s != %s.\n", __func__,
				spath, d);
		blkdev_put(bdev, FMODE_WRITE);
		return NULL;
	}

	ret = set_blocksize(bdev, PAGE_SIZE);
	if (ret < 0) {
		printk(KERN_ERR "%s: %s does not support PAGE_SIZE IO.\n",
				__func__, path);
		blkdev_put(bdev, FMODE_WRITE);
		return NULL;
	}
#endif

	writer = ioremap_cached(snapshot_buf_start, snapshot_buf_size);
	if (!writer) {
		/* blkdev_put(bdev, FMODE_WRITE); */
		return NULL;
	}

	printk(KERN_INFO "ioremap snapshot buffer:0x%lx to 0x%lx\n",
			 snapshot_buf_start, (unsigned long)writer);

	memset(writer, 0, sizeof(*writer));

	writer->compressor = compressor;

	writer->work_mem = __ALIGN_UP(
		(unsigned long)writer + sizeof(*writer), PAGE_SIZE);
#ifdef USE_FIXED_SMALL_CHUNK
	writer->src_buf_sz = SZ_1K * 512;
	writer->dst_buf_sz = SZ_1K * 512;
#else
	/* it should be allocated as src:dst=10M:9M fixed. */
	writer->src_buf_sz = SZ_1M * 10U;
	writer->dst_buf_sz = SZ_1M * 9U;
#endif

#ifdef CONFIG_ARCH_HI3516A
	writer->src = __ALIGN_UP(writer->work_mem + 1U, SZ_1M);
#else
	writer->src = __ALIGN_UP(writer->work_mem + 1U, SZ_1K*4);
#endif

	writer->dst = writer->src + writer->src_buf_sz;

#ifdef CONFIG_DEFAULT_MTD
	if (writer->dst + writer->dst_buf_sz >
			(unsigned long)writer + snapshot_buf_size) {
		printk(KERN_ERR "%s: not enough space\n", __func__);
		put_susp_compress_writer(writer);
		return NULL;
	}
	printk(KERN_INFO "PM: compress: using dev %s(%u:%u)\n",
			d, MAJOR(device), MINOR(device));
#else

#ifdef CONFIG_ARCH_HI3516A
	writer->image = (void *)__ALIGN_UP(
			writer->dst + writer->dst_buf_sz + 1U, SZ_1M);
#else
	writer->image = (void *)__ALIGN_UP(
			writer->dst + writer->dst_buf_sz + 1U, SZ_1K*4);
#endif

	writer->image_buf_sz = snapshot_buf_size -
		((unsigned long)writer->image - (unsigned long)writer);

	printk(KERN_INFO "PM: compress: image_buf:0x%lx, image_buf_size:0x%x\n",
			(unsigned long)writer->image, writer->image_buf_sz);
#endif

	/* writer->bdev = bdev; */
	/* writer->device = device; */

	writer->compress = do_compress;
	writer->write = hbpart_write;
	writer->read = hbpart_read;
	writer->finish = susp_writer_finish;
	writer->pagebuf = (void*)get_zeroed_page(GFP_KERNEL);

	writer->sscomp.info.comp_type = compressor->type;
	writer->sscomp.info.image_ofs = PAGE_NR(sizeof(writer->sscomp));
#ifndef USE_FIXED_SMALL_CHUNK
	writer->sscomp.info.block_type = COMP_BLK_DYNAMIC;
#else
	writer->sscomp.info.decomp_block_size = writer->src_buf_sz;
	writer->sscomp.info.block_type = COMP_BLK_FIXED;
#endif

	return writer;
}
