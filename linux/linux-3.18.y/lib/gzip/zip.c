/* zip.c -- compress files to the gzip or pkzip format
* Copyright (C) 1992-1993 Jean-loup Gailly
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License, see the file COPYING.
*/
#include "gzip.h"
local ulg crc = 0; /* crc on uncompressed file data */
/* ===========================================================================
* Deflate in to out.
* IN assertions: the input and output buffers are cleared.
* The variables time_stamp and save_orig_name are initialized.
*/
int zip( void )
{
	uch flags = 0; /* general purpose bit flags */
	ush attr = 0; /* ascii/binary flag */
	ush deflate_flags = 0; /* pkzip -es, -en or -ex equivalent */
	outcnt = 0; /* Write the header to the gzip file. See algorithm.doc for the format */
	method = DEFLATED;
	put_byte( GZIP_MAGIC[0] ); /* magic header */
	put_byte( GZIP_MAGIC[1] );
	put_byte( DEFLATED ); /* compression method */
	put_byte( flags ); /* general flags */
	put_long( time_stamp ); /* Write deflated file to zip file */
	crc = updcrc( 0, 0 );
	bi_init();
	ct_init(& attr, & method );
	lm_init( level, & deflate_flags );
	put_byte(( uch )deflate_flags ); /* extra flags */
	put_byte( 0 ); /* OS identifier */
	( void )deflate(); /* Write the crc and uncompressed size */
	put_long( crc );
	put_long( isize );
	flush_outbuf();

	return 0;
}

char * zip_mem_inptr = NULL;
int zip_mem_insize = 0;
int zip_mem_inpos = 0;
int mem_read( char * buf, unsigned size )
{
	int len;
#define min_1( a, b ) ((( int )( a ) < ( int )( b )) ? ( a ) : ( b ))
	len = min_1( zip_mem_insize - zip_mem_inpos, size );
	if( len > 0 )
	{
		memcpy( buf, & zip_mem_inptr[zip_mem_inpos], len );
		crc = updcrc(( uch *)buf, len );
		isize += ( ulg )len;
		zip_mem_inpos += len;
	}
	else
		len = - 1;
	return ( int )len;
}
