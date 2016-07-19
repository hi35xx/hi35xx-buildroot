#include <linux/init.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/lzo.h>

#include <crypto/hash.h>

#include "compress.h"

/* I do not want to concern about {k|v}malloc something. */
#include <crypto/sha.h>
#include "../../crypto/internal.h"
#include <crypto/internal/hash.h>
#ifdef USE_SHA1
static union my_desc_s {
	struct shash_desc desc;
	struct {
		unsigned char c[sizeof(struct shash_desc)];
		struct sha1_state sha1_state;
	} p;
} my_desc;
static struct shash_alg *sha1_hash;

static void hb_bdev_find_hash_sha1(void)
{
	struct crypto_alg *hash_alg;
	hash_alg = crypto_find_alg("sha1", &crypto_ahash_type,
			CRYPTO_ALG_TYPE_HASH, CRYPTO_ALG_TYPE_AHASH_MASK);
	if (IS_ERR_OR_NULL(hash_alg)) {
		pr_err("%s: could not found sha1.\n", __func__);
		sha1_hash = NULL;
	}
	sha1_hash = container_of(hash_alg, struct shash_alg, base);
}

int hb_bdev_sha1_init(void)
{
	if (!sha1_hash) {
		pr_err("%s: could not found sha1.\n", __func__);
		return -1;
	}
	return sha1_hash->init(&my_desc.desc);
}

int hb_bdev_sha1_update(const u8 *data, unsigned int len)
{
	if (!sha1_hash) {
		pr_err("%s: could not found sha1.\n", __func__);
		return -1;
	}
	return sha1_hash->update(&my_desc.desc, data, len);
}

int hb_bdev_sha1_final(u8 *out)
{
	if (!sha1_hash) {
		pr_err("%s: could not found sha1.\n", __func__);
		return -1;
	}
	return sha1_hash->final(&my_desc.desc, out);
}
#else

static inline void hb_bdev_find_hash_sha1(void) {}

inline int hb_bdev_sha1_init(void){return 0; }

inline int hb_bdev_sha1_update(const u8 *data, unsigned int len) {return 0; }

inline int hb_bdev_sha1_final(u8 *out) {return 0; }

#endif


#if defined(CONFIG_LZO_COMPRESS) && defined(CONFIG_LZO_DECOMPRESS)
static int lzo_compress(unsigned char *dst, int *dstlen,
		unsigned char *src, int srclen, int unused, void *wrkmem)
{
	size_t tmp_slen, tmp_dlen;
	int ret;

	tmp_slen = srclen;
	tmp_dlen = *dstlen;
	ret = lzo1x_1_compress(src, tmp_slen, dst, &tmp_dlen, wrkmem);
	if (ret != LZO_E_OK) {
		pr_err("lzo1x_compress(): failed %d\n", ret);
		return -1;
	}
	*dstlen = tmp_dlen;
	return 0;
}

static int lzo_decompress(unsigned char *dst, int *dstlen,
		unsigned char *src, int srclen, void *unused)
{
	size_t tmp_slen, tmp_dlen;
	int ret;
	tmp_slen = srclen;
	tmp_dlen = *dstlen;
	ret = lzo1x_decompress_safe(src, tmp_slen, dst, &tmp_dlen);
	if (ret != LZO_E_OK) {
		pr_err("lzo1x_decompress(): failed %d\n", ret);
		return -1;
	}
	*dstlen = tmp_dlen;
	return 0;
}
#endif

#ifdef CONFIG_LZMA_COMPRESS
extern int Lzma_Encode(unsigned char *dest, unsigned int *destLen,
			const unsigned char *src, unsigned int srcLen);
extern int Lzma_Decode(unsigned char *dest, unsigned int *destLen,
			unsigned char *src, unsigned int srcLen);

static int lzma_compress(unsigned char *dst, int *dstlen,
		unsigned char *src, int srclen, int unused, void *wrkmem)
{
	return Lzma_Encode(dst, dstlen, src, srclen);
}

static int lzma_decompress(unsigned char *dst, int *dstlen,
		unsigned char *src, int srclen, void *unused)
{
	return Lzma_Decode(dst, dstlen, src, srclen);
}
#endif

static struct compress_wrapper compressor[] = {
#if defined(CONFIG_LZO_COMPRESS) && defined(CONFIG_LZO_DECOMPRESS)
	{
		.name = "lzo",
		.type = COMP_TYPE_LZO,
		.compress = lzo_compress,
		.decompress = lzo_decompress,
	},
#ifdef CONFIG_LZMA_COMPRESS
	{
		.name = "lzma",
		.type = COMP_TYPE_LZMA,
		.compress = lzma_compress,
		.decompress = lzma_decompress,
	},
#endif

#else
#error "there is no compressor"
#endif
};

struct compress_wrapper *hb_bdev_find_compressor_by_name(const char *name)
{
	int i;
	struct compress_wrapper *compress_wrapper;

	hb_bdev_find_hash_sha1();

	for (i = 0; i < ARRAY_SIZE(compressor); i++) {
		compress_wrapper = &compressor[i];
		if (strcmp(name, compress_wrapper->name) == 0)
			return compress_wrapper;
	}
	return NULL;
}
