/* bits.c -- output variable-length bit strings
* Copyright (C) 1992-1993 Jean-loup Gailly
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License, see the file COPYING.
*/

/*
* PURPOSE
*
* Output variable-length bit strings. Compression can be done
* to a file or to memory. (The latter is not supported in this version.)
*
* DISCUSSION
*
* The PKZIP "deflate" file format interprets compressed file data
* as a sequence of bits. Multi-bit strings in the file may cross
* byte boundaries without restriction.
*
* The first bit of each byte is the low-order bit.
*
* The routines in this file allow a variable-length bit value to
* be output right-to-left (useful for literal values). For
* left-to-right output (useful for code strings from the tree routines),
* the bits must have been reversed first with bi_reverse().
*
* For in-memory compression, the compressed bit stream goes directly
* into the requested output buffer. The input data is read in blocks
* by the mem_read() function. The buffer is limited to 64K on 16 bit
* machines.
*
* INTERFACE
*
* void bi_init (FILE *zipfile)
* Initialize the bit string routines.
*
* void send_bits (int value, int length)
* Write out a bit string, taking the source bits right to
* left.
*
* int bi_reverse (int value, int length)
* Reverse the bits of a bit string, taking the source bits left to
* right and emitting them right to left.
*
* void bi_windup (void)
* Write out any remaining bits in an incomplete byte.
*
* void copy_block(char *buf, unsigned len, int header)
* Copy a stored block to the zip file, storing first the length and
* its one's complement if requested.
*
*/

#include "gzip.h"

/* ===========================================================================
* Local data used by the "bit string" routines.
*/

local unsigned short bi_buf = 0;
/* Output buffer. bits are inserted starting at the bottom (least significant
* bits).
*/

#define Buf_size (8 * 2*sizeof(char))
/* Number of bits used within bi_buf. (bi_buf might be implemented on
* more than 16 bits on some systems.)
*/

local int bi_valid = 0;
/* Number of valid bits in bi_buf. All bits above the last valid bit
* are always zero.
*/

int (* read_buf )( char * buf, unsigned size );
/* Current input function. Set to mem_read for in-memory compression */

/* ===========================================================================
* Initialize the bit string routines.
*/
void bi_init ( void )
{
	bi_buf = 0;
	bi_valid = 0;

	/* Set the defaults for file compression. They are set by memcompress
	* for in-memory compression.
	*/
	read_buf = mem_read;
}

/* ===========================================================================
* Send a value on a given number of bits.
* IN assertion: length <= 16 and value fits in length bits.
*/
void send_bits( value, length )
int value; /* value to send */
int length; /* number of bits */
{
	/* If not enough room in bi_buf, use (valid) bits from bi_buf and
	* (16 - bi_valid) bits from value, leaving (width - (16-bi_valid))
	* unused bits in value.
	*/
	if ( bi_valid > ( int )Buf_size - length ) {
		bi_buf |= ( value << bi_valid );
		put_short( bi_buf );
		bi_buf = ( ush )value >> ( Buf_size - bi_valid );
		bi_valid += length - Buf_size;
	} else {
		bi_buf |= value << bi_valid;
		bi_valid += length;
	}
}

/* ===========================================================================
* Reverse the first len bits of a code, using straightforward code (a faster
* method would use a table)
* IN assertion: 1 <= len <= 15
*/
unsigned bi_reverse( unsigned code, int len )
{
	register unsigned res = 0;
	do
	{
		res |= code & 1;
		code >>= 1, res <<= 1;
	} while (-- len > 0 );
	return res >> 1;
}

/* ===========================================================================
* Write out any remaining bits in an incomplete byte.
*/
void bi_windup()
{
	if ( bi_valid > 8 )/* 是否大于short长度 */
	{
		put_short( bi_buf );
	}
	else if ( bi_valid > 0 )
	{
		put_byte( bi_buf );
	}
	bi_buf = 0;
	bi_valid = 0;
}

/* ===========================================================================
* Copy a stored block to the zip file, storing first the length and its
* one's complement if requested.
*/
void copy_block( char * buf, unsigned len, int header )
{
	bi_windup(); /* align on byte boundary */

	if ( header )
	{
		put_short(( ush )len );
		put_short(( ush )~ len );
	}
	while ( len --)
	{
		put_byte(* buf ++);
	}
}
