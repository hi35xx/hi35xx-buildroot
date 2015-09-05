@/******************************************************************************
@ *    Copyright (c) 2009-2012 by Hisi.
@ *    All rights reserved.
@ * ***
@ *    Create by Czyong. 2012-09-07
@ *
@******************************************************************************/

	.section .image,#alloc
	.globl	input_data
input_data:
	.incbin   "image_data.lzma"
	.globl    input_data_end
input_data_end:
