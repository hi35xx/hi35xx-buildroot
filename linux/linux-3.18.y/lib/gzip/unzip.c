/* unzip.c -- decompress files in gzip or pkzip format.
* Copyright (C) 1992-1993 Jean-loup Gailly
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License, see the file COPYING.
*
* The code in this file is derived from the file funzip.c written
* and put in the public domain by Mark Adler.
*/

#include "gzip.h"

/* ===========================================================================
* Unzip in to out. This routine works on both gzip and pkzip files.
*
* IN assertions: the buffer inbuf contains already the beginning of
* the compressed data, from offsets inptr to insize-1 included.
* The magic header has already been checked. The output buffer is cleared.
*/

int unzip( void )
{
	ulg orig_crc = 0; /* original crc */
	ulg orig_len = 0; /* original uncompressed length */
	int n; uch buf[8]; /* extended local header */

	updcrc( NULL, 0 ); /* initialize crc */

	/* Decompress */
	inflate();

	for ( n = 0; n < 8; n ++) /* Get the crc and original length */
	{
		buf[n] = ( uch )get_byte(); /* may cause an error if EOF */
	}

	orig_crc = LG( buf );
	orig_len = LG( buf + 4 ); /* Validate decompression */

	if ( orig_crc != updcrc( outbuf, 0 ))
	{
		error( "invalid compressed data--crc error" );
	}

	if ( orig_len != ( ulg )bytes_out )
	{
		error( "invalid compressed data--length error" );
	}

	return 0;
}
