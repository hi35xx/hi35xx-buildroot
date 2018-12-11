/* gzip (GNU zip) -- compress files with zip algorithm and 'compress' interface
* Copyright (C) 1992-1993 Jean-loup Gailly
* The unzip code was written and put in the public domain by Mark Adler.
* Portions of the lzw code are derived from the public domain 'compress'
* written by Spencer Thomas, Joe Orost, James Woods, Jim McKie, Steve Davies,
* Ken Turkowski, Dave Mack and Peter Jannesen.
*
* See the license_msg below and the file COPYING for the software license.
* See the file algorithm.doc for the compression algorithms and file formats.
*/

/* Compress files with zip algorithm and 'compress' interface.
* See usage() and help() functions below for all options.
* Outputs:
* file.gz: compressed file with same mode, owner, and utimes
* or stdout with -c option or if stdin used as input.
* If the output file name had to be truncated, the original name is kept
* in the compressed file.
* On MSDOS, file.tmp -> file.tmz. On VMS, file.tmp -> file.tmp-gz.
*
* Using gz on MSDOS would create too many file name conflicts. For
* example, foo.txt -> foo.tgz (.tgz must be reserved as shorthand for
* tar.gz). Similarly, foo.dir and foo.doc would both be mapped to foo.dgz.
* I also considered 12345678.txt -> 12345txt.gz but this truncates the name
* too heavily. There is no ideal solution given the MSDOS 8+3 limitation.
*
* For the meaning of all compilation flags, see comments in Makefile.in.
*/
//#include <stdlib.h>
#include "zipmem.h"
#include "gzip.h"
#include "lzw.h"

/* global buffers */

DECLARE( uch, inbuf, INBUFSIZ + INBUF_EXTRA );
DECLARE( uch, outbuf, OUTBUFSIZ + OUTBUF_EXTRA );
DECLARE( ush, d_buf, DIST_BUFSIZE );
DECLARE( uch, window, 2L * WSIZE ); /* 声明 */
DECLARE( ush, tab_prefix, 1L << BITS );

/* local variables */
int quiet = 0; /* be very quiet (-q) */
int maxbits = BITS; /* max bits per code for LZW */
int method = DEFLATED;/* compression method */
int level = 6; /* compression level */
int exit_code = 0; /* program exit code */
int time_stamp; /* original time stamp (modification time) */

int bytes_in = 0; /* number of input bytes */
int bytes_out = 0; /* number of output bytes */
int total_in = 0; /* input bytes for all files */
int total_out = 0; /* output bytes for all files */
unsigned insize = 0; /* valid bytes in inbuf */
unsigned inptr = 0; /* index of next byte to be processed in inbuf */
unsigned outcnt = 0; /* bytes in output buffer */

//local int get_method(void);

int (* work )(void) = zip; /* function to call */

/* ========================================================================
* Compress
*/
int zipmem( char * mem_inptr, int mem_insize, char * mem_outptr )
{
	time_stamp = 0;
	ALLOC( uch, inbuf, INBUFSIZ + INBUF_EXTRA );
	ALLOC( uch, outbuf, OUTBUFSIZ + OUTBUF_EXTRA );
	ALLOC( ush, d_buf, DIST_BUFSIZE );
	ALLOC( uch, window, 2L * WSIZE ); /* 分配 */
	ALLOC( ush, tab_prefix, 1L << BITS );
	clear_bufs(); /* clear input and output buffers */
	zip_mem_outptr = mem_outptr; /* 输出缓存 */
	zip_mem_outlen = 0; /* 输出缓存大小 */
	zip_mem_inptr = mem_inptr; /* 输入指针 */
	zip_mem_insize = mem_insize; /* 输入缓存长度 */
	zip_mem_inpos = 0; /* 输入缓存当前位置 */
	zip();
	FREE( inbuf );
	FREE( outbuf );
	FREE( d_buf );
	FREE( window );
	FREE( tab_prefix );
	return zip_mem_outlen;
}

#if 0
/* ========================================================================
* deCompress
*/
int unzipmem( char * mem_inptr, int mem_insize, char * mem_outptr )
{
	time_stamp = 0;
	ALLOC( uch, inbuf, INBUFSIZ + INBUF_EXTRA );
	ALLOC( uch, outbuf, OUTBUFSIZ + OUTBUF_EXTRA );
	ALLOC( ush, d_buf, DIST_BUFSIZE );
	ALLOC( uch, window, 2L * WSIZE );/* 分配 */
	ALLOC( ush, tab_prefix, 1L << BITS );
	clear_bufs(); /* clear input and output buffers */
	zip_mem_outptr = mem_outptr; /* 输出缓存 */
	zip_mem_outlen = 0; /* 输出缓存大小 */
	/* 解压缩 */
	unzip_mem_inptr = mem_inptr; /* 输入指针 */
	unzip_mem_insize = mem_insize; /* 输入缓存长度 */
	unzip_mem_inpos = 0; /* 输入缓存当前位置 */
	get_method();
	unzip();
	FREE( inbuf );
	FREE( outbuf );
	FREE( d_buf );
	FREE( window );
	FREE( tab_prefix );
	return zip_mem_outlen;
}

/* ========================================================================
* Check the magic number of the input file and update ofname if an
* original name was given and to_stdout is not set.
* Return the compression method, -1 for error, -2 for warning.
* Set inptr to the offset of the next byte to be processed.
* Updates time_stamp if there is one and --no-time is not used.
* This function may be called repeatedly for an input file consisting
* of several contiguous gzip'ed members.
* IN assertions: there is at least one remaining compressed member.
* If the member is a zip file, it must be the only one.
*/
local int get_method()
{
	/* If --force and --stdout, zcat == cat, so do not complain about
	* premature end of file: use try_byte instead of get_byte.
	*/
	( char )get_byte();
	( char )get_byte();
	method = - 1; /* unknown yet */
	/* assume multiple members in gzip file except for record oriented I/O */

	method = ( int )get_byte();
	work = unzip;
	( uch )get_byte();
	( ulg )get_byte();
	(( ulg )get_byte());
	(( ulg )get_byte());
	(( ulg )get_byte());
	( void )get_byte(); /* Ignore extra flags for the moment */
	( void )get_byte(); /* Ignore OS type for the moment */

	return method;
}
#endif

int gzip_defalte(void *dst, unsigned int *dst_len, const void *src, unsigned int len)
{
	*dst_len = zipmem((char *)src, len, dst);
	return 0;
}
