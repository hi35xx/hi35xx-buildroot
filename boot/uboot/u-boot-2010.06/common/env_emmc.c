/*
 * (C) Copyright 2000-2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>
 *
 * (C) Copyright 2008 Atmel Corporation
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <environment.h>
#include <mmc.h>

DECLARE_GLOBAL_DATA_PTR;

/* references to names in env_common.c */
extern uchar default_environment[];

char *emmc_env_name_spec = "EMMC";
extern env_t *env_ptr;

uchar emmc_env_get_char_spec(int index)
{
	return *((uchar *)(gd->env_addr + index));
}

int emmc_saveenv(void)
{
	int ret;
	struct mmc *mmc = find_mmc_device(0);

	if (!mmc) {
		puts("Environment SPI flash not initialized\n");
		return 1;
	}

	mmc_init(mmc);

	ret = mmc->block_dev.block_write(0, CONFIG_ENV_OFFSET >> 9,
		 CONFIG_ENV_SIZE >> 9, env_ptr);

	if (ret != (CONFIG_ENV_SIZE >> 9))
		return 1;

	puts("done\n");
	return 0;
}

void emmc_env_relocate_spec(void)
{
	int ret;
	struct mmc *mmc = find_mmc_device(0);

	if (!mmc)
		goto error;

	mmc_init(mmc);

	ret = mmc->block_dev.block_read(0, CONFIG_ENV_OFFSET >> 9,
		 CONFIG_ENV_SIZE >> 9, env_ptr);

	/* flush cache after read */
	flush_cache((ulong)env_ptr, CONFIG_ENV_SIZE); /* FIXME */

	if (ret != (CONFIG_ENV_SIZE >> 9))
		goto error;

	if (crc32(0, env_ptr->data, ENV_SIZE) != env_ptr->crc)
		goto error;

	gd->env_valid = 1;

	return;

error:
	puts("*** Warning - bad CRC, using default environment\n\n");

	set_default_env();
}

int emmc_env_init(void)
{
	/* emmc isn't usable before relocation */
	gd->env_addr = (ulong)&default_environment[0];
	gd->env_valid = 1;

	return 0;
}
