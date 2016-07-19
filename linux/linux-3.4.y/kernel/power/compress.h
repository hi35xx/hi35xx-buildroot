#ifndef __KEREL_POWER_COMPRESS_H_
#define __KEREL_POWER_COMPRESS_H_

struct sscomp_block {
	unsigned int org_size;
	unsigned int comp_size;
};

union sscomp_header {
	struct {
#define COMP_TYPE_LZF 1
#define COMP_TYPE_LZO 2
#define COMP_TYPE_LZMA 3
#define COMP_TYPE_BZIP2 4
#define COMP_TYPE_SNAPPY 5
#define COMP_TYPE_QUICKLZ 6
		int comp_type;

		int meta_blocks;
		int meta_pages;
		int data_blocks;
		int data_pages;
		unsigned int decomp_block_size;
		int image_ofs;
		
		int crc32;

#define SSCOMP_MAGIC_4 "SSCOMP#4"
		char magic[8];
		unsigned char sha1sum[20];

#define COMP_BLK_LEGACY 0
#define COMP_BLK_FIXED 1
#define COMP_BLK_DYNAMIC 2
                int block_type;

		unsigned long ctl_func;
		unsigned long ctl_data;
	} info __attribute__((packed));
	struct {
		unsigned char c[128];
#define MAX_COMP_BLOCKS ((4096-128)/sizeof(struct sscomp_block))
		struct sscomp_block blocks[MAX_COMP_BLOCKS];
	} b __attribute__((packed));
	unsigned char c[4096];
};

struct compress_wrapper {
	const char *name;
	int type;
	int (*compress)(unsigned char *dst, int *dstlen,
                unsigned char *src, int srclen, int factor, void *extra);
	int (*decompress)(unsigned char *dst, int *dstlen,
                unsigned char *src, int srclen, void *extra);
};

struct compress_writer {
	union sscomp_header sscomp;

	unsigned long work_mem;
	unsigned long src;
	unsigned long dst;
	unsigned int src_buf_sz;
	unsigned int dst_buf_sz;
	unsigned int image_buf_sz;

	struct block_device *bdev;
	dev_t device;

	void *image;
	void *pagebuf;

	struct compress_wrapper *compressor;

	int (*compress)(struct compress_writer *, struct sscomp_block *,
			unsigned int *);
	int (*decompress)(struct compress_writer *, struct sscomp_block *,
			unsigned int *);
	int (*write)(struct compress_writer *, sector_t page_i,
			sector_t page_nr, const void *);
	int (*read)(struct compress_writer *, sector_t page_i,
			sector_t page_nr, void *buf);
	int (*finish)(struct compress_writer *);
};

struct compress_wrapper *hb_bdev_find_compressor_by_name(const char *name);

extern void put_susp_compress_writer(struct compress_writer *);
extern struct compress_writer *get_susp_compress_writer(const char *);

extern int hb_bdev_sha1_init(void);
extern int hb_bdev_sha1_update(const u8 *data, unsigned int len);
extern int hb_bdev_sha1_final(u8 *out);

#endif /* __KEREL_POWER_COMPRESS_H_ */

