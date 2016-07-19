/*
 * (C) Copyright 2003
 * Kyle Harris, kharris@nexus-tech.net
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

#include <common.h>
#include <command.h>
#include <asm/arch/platform.h>
#include <environment.h>
#include <asm/io.h>

extern env_t *env_ptr;
char *slave_boot_start;

void slave_start(void)
{
	unsigned int slave_bootaddr, slave_kerneladdr, slave_initrdaddr;
	image_header_t hdr;
	int argc;
	char *argv[CONFIG_SYS_MAXARGS + 1];
	char *slave_env;
	char *slave_command;
	char slave_cmd[CONFIG_SYS_CBSIZE];
	unsigned int len, data;
	char cmd_buf[40];

	slave_command = getenv("slave_bootcmd");

	run_command_slave(slave_command, 0);

	slave_command = getenv("slave_cmd");

	strcpy(slave_cmd, slave_command);

	argc = parse_line(slave_cmd, argv);
	if ((argc <= 3) || (argc >= 5)) {
		slave_bootaddr = CONFIG_SLAVE_BOOT;
		slave_kerneladdr = CFG_SLAVE_KERNEL_ADDR;
		slave_initrdaddr = CFG_SLAVE_INITRD_ADDR;
	} else {
		slave_bootaddr = simple_strtoul(argv[1], NULL, 16);
		slave_kerneladdr = simple_strtoul(argv[2], NULL, 16);
		slave_initrdaddr = simple_strtoul(argv[3], NULL, 16);
	}

	/* copy kernel */
	memmove(&hdr, (char *)slave_kerneladdr, sizeof(image_header_t));

	if ((ntohl(hdr.ih_magic) != IH_MAGIC) ||
			(hdr.ih_type != IH_TYPE_KERNEL)) {
		printf("WARNING! unknown slave_kernel.img format!\n");
		goto out;
	}

	len  = ntohl(hdr.ih_size) + sizeof(image_header_t);
	memmove((void *)(ntohl(hdr.ih_load) - sizeof(image_header_t)),
					(uchar *)slave_kerneladdr, len);
	slave_kerneladdr =
		(unsigned int)(ntohl(hdr.ih_load) - sizeof(image_header_t));

	/* copy initrd */
	memmove(&hdr, (char *)slave_initrdaddr, sizeof(image_header_t));
	if (ntohl(hdr.ih_magic) != IH_MAGIC) {
		printf("WARNING! unknown slave_initrd.img format!\n");
		goto out;
	}

	len  = ntohl(hdr.ih_size)+sizeof(image_header_t);
	data = slave_initrdaddr;
	memmove((void *)ntohl(hdr.ih_load), (uchar *)data, len);
	slave_initrdaddr = (unsigned int)ntohl(hdr.ih_load);

	sprintf(cmd_buf, "setenv slave_cmd bootm %p %p",
			(void *)slave_kerneladdr, (void *)slave_initrdaddr);
	run_command(cmd_buf, 0);

	sprintf(cmd_buf, "setenv slaveboot %x", slave_bootaddr);
	run_command(cmd_buf, 0);

	env_ptr->crc = crc32(0, env_ptr->data, ENV_SIZE);

	slave_env = (void *)(slave_bootaddr + 0x80000);
	memcpy((void *)slave_env, (void *)env_ptr, sizeof(env_t));

	writel((unsigned int)slave_env, (SYS_CTRL_REG_BASE + REG_SC_GEN2));

	slave_boot_start = getenv("slaveboot");

	sprintf(cmd_buf, "setenv slave_cmd");
	run_command(cmd_buf, 0);

	sprintf(cmd_buf, "setenv slaveboot");
	run_command(cmd_buf, 0);

#if SLAVR_BOOT_DEBUG
	printf("start slave arm!\n");
#endif
out:
	return;
}

/*
int do_slavestart(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned int kernel_addr, initrd_addr;
	char cmd_buf[40];
	char *env;
#if SLAVR_BOOT_DEBUG
	printf("Enter do_slavestart!\n");
#endif
	if (argc < 2) {
		env = getenv("slave_kernel_addr");
		if (env)
			kernel_addr = simple_strtoul(env, NULL, 16);
		else
			kernel_addr = CFG_SLAVE_KERNEL_ADDR;

		env = getenv("slave_initrd_addr");
		if (env)
			initrd_addr = simple_strtoul(env, NULL, 16);
		else
			initrd_addr = CFG_SLAVE_INITRD_ADDR;
	} else {
		kernel_addr = simple_strtoul(argv[1], NULL, 16);
		initrd_addr = simple_strtoul(argv[2], NULL, 16);
	}

	sprintf(cmd_buf, "setenv slave_kernel_addr 0x%x", kernel_addr);
#if SLAVR_BOOT_DEBUG
	printf("cmd_buf=%s!\n", cmd_buf);
#endif
	run_command(cmd_buf, 0);
	sprintf(cmd_buf, "setenv slave_initrd_addr 0x%x", initrd_addr);
#if SLAVR_BOOT_DEBUG
	printf("cmd_buf=%s!\n", cmd_buf);
#endif
	run_command(cmd_buf, 0);
	slave_start();
}

U_BOOT_CMD(
	slavestart,	3,	0,	do_slavestart,
	"slavestart	- start slave arm!\n",
	"slave_kernel_addr	slave_initrd_addr \n"
);
*/


