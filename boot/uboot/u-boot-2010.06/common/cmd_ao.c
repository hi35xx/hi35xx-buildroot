/*
 * (C) Copyright 2009-2015
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * audio output Support
 */
#include <common.h>
#include <command.h>

#if defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556)
#include "hi3559_ao.h"
#elif defined(CONFIG_ARCH_HI3519)
#include "hi3519_ao.h"
#else
#include "hi3516a_ao.h"
#endif

extern int acodec_i2s_set(AUDIO_SAMPLE_RATE_E enSample);
extern int acodec_device_init(void);
extern int acodec_device_exit(void);
extern int start_ao(unsigned int u32Addr, unsigned int u32Size, 
					AUDIO_SAMPLE_RATE_E enSample, unsigned int u32Vol);
extern int stop_ao(void);


int do_startao(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    unsigned int addr, size, vol;
	AUDIO_SAMPLE_RATE_E enSample = AUDIO_SAMPLE_RATE_8000;
    if (argc < 4)
    {
        printf("Insufficient parameter!\n");
        printf ("Usage:\n%s\n", cmdtp->usage);
        return -1;
    }
    //printf("ao dev starting!\n");

    addr = (unsigned int)simple_strtoul(argv[1], NULL, 10);
    size = (unsigned int)simple_strtoul(argv[2], NULL, 10);
	vol  = (unsigned int)simple_strtoul(argv[3], NULL, 10);
	if (addr%32)
	{
        printf("[Error] Invalid addr parameter:0x%0x, address should be aligned by 32Byte!\n", addr);
        return -1;
	}
	if (size%32)
	{
		printf("[Warning] Invalid size parameter:0x%0x, size should be aligned by 32Byte!\n", size);
		size = (size/32)*32;
	}

    if (vol > 6)
    {
        printf("[Warning] Invalid volume parameter, range:0 ~ 6dB!, vol default value:0.\n");
        vol = 0;
    }

	acodec_device_init();
	acodec_i2s_set(enSample);
    start_ao(addr, size, enSample, vol);

    printf("ao dev start ok!\n");

    return 0;
}

int do_stopao(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    stop_ao();
	//acodec_device_exit();

    printf("ao dev closed!\n");

    return 0;
}

U_BOOT_CMD(
	startao,    10,	1,  do_startao,
	"startao   - open interface of ao device.\n"
	"\t- startao [addr size volume]",
	"\nargs: [addr, size volume]\n"
	"\t-<addr> : ao address,align by 32Byte\n"
	"\t-<size>: ao size,align by 32Byte\n"
	"\t-<volume>: audio output volume default:0dB, range:0 ~ 6dB\n"
	);

U_BOOT_CMD(
	stopao,    1,	0,  do_stopao,
	"stopao   - close interface of ao device.\n",
	""
	);




