/******************************************************************************
*    Copyright (c) 2009-2012 by Hisi.
*    All rights reserved.
* ***
*    Create by Czyong. 2012-04-27
*
******************************************************************************/

#include <config.h>

/******************************************************************************/
extern void enable_mmu(void);
extern void stop_mmu(void);
extern void dcache_enable(unsigned int ddrsize);
extern void dcache_disable(void);
extern void uart_early_init(void);
extern void uart_early_puts(const char *ss);
extern void reset_cpu(unsigned long addr);
/******************************************************************************/
const unsigned int IMAGE_ENTRY = (TEXT_BASE + 0x100000);

extern unsigned char input_data[];
extern unsigned char input_data_end[];
extern unsigned int _armboot_start;

typedef void (*ENTRY)(void);
/******************************************************************************/
#define error(_s)               uart_early_puts(_s)
#define putstr(_s)              uart_early_puts(_s)

#define large_malloc(_n)        malloc(_n)
#define large_free(_n)          free(_n)

/******************************************************************************/
/*
 * this value will be (_armboot_start - CONFIG_SYS_MALLOC_LEN)
 */
static char *malloc_start;

/******************************************************************************/
static void *malloc(unsigned int size)
{
	void *ptr = malloc_start;
	malloc_start += size;
	return ptr;
}
/******************************************************************************/
static void free(void *ptr)
{
}
/******************************************************************************/

#include "unlzma.c"

/******************************************************************************/
void start_armboot(void)
{
	ENTRY entry = (ENTRY)IMAGE_ENTRY;
	unsigned char *pdst = (unsigned char *)IMAGE_ENTRY;
	unsigned int image_data_len = input_data_end - input_data;

	malloc_start = (char *)(_armboot_start - CONFIG_SYS_MALLOC_LEN);
	uart_early_init();
	/* DDR should larger than 16M */
	enable_mmu();

	dcache_enable(0);

	if (input_data[0] == 0x5D) {
		uart_early_puts("\nUncompress");
		decompress(input_data, image_data_len, pdst);
		uart_early_puts("Ok\r\n");
	} else {
		int *s = (int *)input_data;
		int *d = (int *)pdst;
		unsigned int len = ((image_data_len + 3) >> 2);
		while (len--)
			*d++ = *s++;
	}
	dcache_disable();

	stop_mmu();
	entry();
}

/******************************************************************************/
void hang(void)
{
	uart_early_puts("### ERROR ### Please RESET the board ###\n");
	for (;;)
		;
}

void do_undefined_instruction(void)
{
	uart_early_puts("undefined instruction\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	reset_cpu(0);
}

void do_software_interrupt(void)
{
	uart_early_puts("software interrupt\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	reset_cpu(0);
}

void do_prefetch_abort(void)
{
	uart_early_puts("prefetch abort\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	reset_cpu(0);
}

void do_data_abort(void)
{
	uart_early_puts("data abort\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	reset_cpu(0);
}

void do_not_used(void)
{
	uart_early_puts("not used\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	reset_cpu(0);
}

void do_fiq(void)
{
	uart_early_puts("fast interrupt request\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	reset_cpu(0);
}

void do_irq(void)
{
	uart_early_puts("interrupt request\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	reset_cpu(0);
}
