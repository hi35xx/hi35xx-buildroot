//#include <stdio.h>
#include <linux/string.h>
#include <linux/module.h>

#include "Types.h"

#include "Lzma.h"
#include "crc32.h"
#include "Alloc.h"
#include "LzmaEnc.h"
#include "LzmaDec.h"

#define SZE_OUT_OVERFLOW SZE_DATA_ERROR
#define FILE_MODE

//#define PURE_DATA

#ifdef PURE_DATA
#define BLOCK_SIZE (1024*1024*1024)
#else
#define BLOCK_SIZE (512*1024)
#endif

static void *SzAlloc(void *p, size_t size) { p = p; return MyAlloc(size); }
static void SzFree(void *p, void *address) { p = p; MyFree(address); }

int _Lzma_Encode(unsigned char *dest, unsigned int *destLen, const unsigned char *src, unsigned int srcLen)
{
	ISzAlloc g_Alloc = { SzAlloc, SzFree };
	int res;
	CLzmaEncProps props;
	SizeT outPropsSize = 5;
	LzmaEncProps_Init(&props);
	props.level = 5;
	props.dictSize = 1 << 12;
	props.algo = 0;  

	res = LzmaEncode(dest, destLen, src, srcLen,
			&props, dest, &outPropsSize, 0,
			NULL, &g_Alloc, &g_Alloc);

	return res;
}

#if 0
int add_end(unsigned char *dest, unsigned int *destLen)
{
	UInt32 packet, packet_bits, move_len, dest_offset;
	UInt64 packet_move = 0;

	//add end
	//11 1111 1111 1111 1111 1101
	//  packet = 0x00FCFFBF;  //从左到又
	packet = 0x003FFFFD;  //从右到左

	packet_bits = 22;
	move_len = (*destLen % 8);
	dest_offset = (*destLen / 8);
	packet_move = ((UInt64)packet << move_len);
	*(UInt64 *)(dest + dest_offset) |= packet_move;
	*destLen += packet_bits;

	return 0;
}

int add_align(unsigned char *dest, unsigned int *destLen)
{
	UInt32 packet, move_len, dest_offset, align_len;
	UInt64 packet_move = 0;

	//add align
	align_len = (128 - *destLen % 128);
	if (align_len == 128)
	{
		//printf("don't need align\n");
		return 0;
	}
	move_len = (*destLen % 8);
	dest_offset = (*destLen / 8);
	packet = *(char *)(dest + dest_offset);
	memset((dest + dest_offset), 0, (align_len + move_len)/8);
	*(char *)(dest + dest_offset) = packet;
	*destLen += align_len;

	return 0;
}
#endif

unsigned long crc32(unsigned long crc, const unsigned char *buf, unsigned int len)
{
	if (buf == NULL) return 0UL;

	crc = crc ^ 0xffffffffUL;
	/*相当于1Byte计算一个crc值，调用DO1*/
	while (len >= 8) {
		DO8;
		len -= 8;
	}
	if (len) do {
		DO1;
	} while (--len);
	return crc ^ 0xffffffffUL;
}

int Lzma_Encode(unsigned char *dest, unsigned int *destLen, const unsigned char *src, unsigned int srcLen)
{
	int res = _Lzma_Encode(dest, destLen, src, srcLen);
	return res;
}
EXPORT_SYMBOL_GPL(Lzma_Encode);

int Lzma_Decode(unsigned char *dest, unsigned int *destLen, unsigned char *src, unsigned int srcLen)
{
	int res = gzip_inflate(dest, destLen, src, srcLen);

	return res;
}
EXPORT_SYMBOL_GPL(Lzma_Decode);

/*int main(void)
  {
  UInt32 i = 0;
  FILE *fp_in  = NULL;
  FILE *fp_out = NULL;
  UInt32 srcLen = 0;
  UInt32 destLen = 0;
  UInt32 decode_destLen = 0;
  Byte *src, *dest, *decode_dest, *src_tmp;

  char *src_name = "d:/3516d/data_4.txt";
  char *dest_name = "d:/3516d/4.txt";
  char *decode_name = "d:/3516d/decode.txt";

  srcLen = 512*1024;

#ifdef FILE_MODE
fp_in = fopen(src_name, "rb");
fseek(fp_in, 0L, SEEK_END);
srcLen = ftell(fp_in);
fclose(fp_in);
#endif

src = (Byte *)malloc(srcLen);
dest = (Byte *)malloc(srcLen * 2);
decode_dest = (Byte *)malloc(srcLen);

#ifdef FILE_MODE
fp_in = fopen(src_name, "rb");
fread(src, srcLen, 1, fp_in);
fclose(fp_in);
#else
src_tmp = src;
for (i = 0; i < srcLen; i++)
{
 *src_tmp++ = 1;
 }
#endif

printf("srcLen: %u\n", srcLen);
memset(dest, 0 , srcLen * 2);
Lzma_Encode(dest, &destLen, src, srcLen);
printf("destLen: %u\n", destLen);

fp_out = fopen(dest_name, "wb");
fwrite(dest, destLen, 1, fp_out);
fclose(fp_out);

memset(decode_dest, 0 , srcLen);
//Lzma_Decode(decode_dest, &decode_destLen, dest, destLen);
printf("decode_destLen: %u\n", decode_destLen);
fp_out = fopen(decode_name, "wb");
fwrite(decode_dest, decode_destLen, 1, fp_out);
fclose(fp_out);

return 0;
}*/
