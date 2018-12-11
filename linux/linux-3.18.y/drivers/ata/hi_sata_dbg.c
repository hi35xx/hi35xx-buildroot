#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/libata.h>
#include <mach/io.h>
#include "ahci.h"
#include "hi_sata_dbg.h"

void hi_sata_mem_dump(unsigned int *addr, unsigned int size)
{
	int ix;

	for (ix = 0; ix < size; ix += 0x04, addr++) {
		if (!(ix & 0x0F))
			pr_debug("\n0x%08X: ",
				(unsigned int)virt_to_phys(addr));
		pr_debug("%08X ", *addr);
	}
}
EXPORT_SYMBOL(hi_sata_mem_dump);

void hi_sata_phys_mem_dump(unsigned int addr, unsigned int size)
{
	hi_sata_mem_dump(phys_to_virt(addr), size);
}
EXPORT_SYMBOL(hi_sata_phys_mem_dump);

void hi_ahci_reg_dump(void)
{
	int ix;
	unsigned int regbase;

	regbase = CONFIG_HI_SATA_IOBASE;
	pr_debug("AHCI GHC Register dump:");
	for (ix = 0; ix <= 0x28; ix += 0x04) {
		if (!(ix & 0x0F))
			pr_debug("\n0x%08X: ", (regbase + ix));
		pr_debug("%08X ", readl(__io_address(regbase + ix)));
	}
	pr_debug("\n");

	regbase = CONFIG_HI_SATA_IOBASE + 0x0100;
	pr_debug("AHCI PORT 0 Register dump:");
	for (ix = 0; ix <= 0x7F; ix += 0x04) {
		if (!(ix & 0x0F))
			pr_debug("\n0x%08X: ", (regbase + ix));
		pr_debug("%08X ", readl(__io_address(regbase + ix)));
	}
	pr_debug("\n");
}
EXPORT_SYMBOL(hi_ahci_reg_dump);

void hi_ahci_rx_fis_dump(struct ata_link *link, int pmp_port_num)
{
	struct ahci_port_priv *pp = NULL;

	pp = link->ap->private_data;
	if (NULL == pp) {
		pr_debug("Error: pp=NULL\n");
		return;
	}
	pr_debug("ACHI Received FIS:");
	hi_sata_phys_mem_dump((unsigned int)(pp->rx_fis_dma),
				AHCI_RX_FIS_SZ * pmp_port_num);
	pr_debug("\n");
}
EXPORT_SYMBOL_GPL(hi_ahci_rx_fis_dump);

void hi_ata_taskfile_dump(struct ata_taskfile *tf)
{
	if (NULL == tf) {
		pr_debug("Error: tf=NULL\n");
		return;
	}

	pr_debug("Taskfile dump:\n");
	pr_debug("flags:0x%08lX, protocol:0x%02X, command:0x%02X, device:0x%02X, ctl:0x%02X\n",
		tf->flags, tf->protocol, tf->command, tf->device, tf->ctl);
	pr_debug("feature:0x%08X, nsect:0x%02X, lbal:0x%02X, lbam:0x%02X, lbah:0x%02X\n",
		tf->feature, tf->nsect, tf->lbal, tf->lbam, tf->lbah);
	pr_debug("hob_feature:0x%08X, hob_nsect:0x%02X, hob_lbal:0x%02X, hob_lbam:0x%02X, hob_lbah:0x%02X\n",
		tf->hob_feature, tf->hob_nsect, tf->hob_lbal,
		tf->hob_lbam, tf->hob_lbah);
}
EXPORT_SYMBOL_GPL(hi_ata_taskfile_dump);

static void __hi_ahci_st_md(void __iomem *addr)
{
	unsigned int *addr_v;
	unsigned int *tmp;
	unsigned int i;

	addr_v = (unsigned int *)addr;

	pr_debug("\n\n");
	for (i = 0; i < 16; i++) {
		tmp = addr_v + i * 4;
		pr_debug("%8x: %8x %8x %8x %8x\n",
			(unsigned int)(addr + i * 16),
			*tmp, *(tmp + 1), *(tmp + 2), *(tmp + 3));
	}

	pr_debug("\n");
}

void hi_ahci_st_dump(void __iomem *port_base)
{
	unsigned int tmp;

	pr_debug("\n**********Dmac status**********\n");
	tmp = readl(port_base + 0x58);
	pr_debug("txdmac_curr_st:0x%2x\n", (tmp>>24) & 0xf);
	tmp = readl(port_base + 0x64);
	pr_debug("rxdmac_curr_st:0x%2x\n", (tmp>>24) & 0xf);
	tmp = readl(port_base + 0x70);
	pr_debug("dmac tx fifo:count-0x%x-empty-%x-ful-%x\n",
			(tmp>>0) & 0xff,
			(tmp>>16) & 0x1, (tmp>>17) & 0x1);
	pr_debug("dmac rx fifo:count-0x%x-empty-%x-ful-%x\n",
			(tmp>>8) & 0xff,
			(tmp>>18) & 0x1, (tmp>>19) & 0x1);

	pr_debug("\n");
	pr_debug("**********HBA status**********\n");
	tmp = readl(port_base + 0x50);
	pr_debug("pxxx_curr_st:0x%2x      ndrx_curr_st:0x%2x\n",
			(tmp>>24) & 0xf,
			(tmp>>16) & 0xff);
	pr_debug("cfis_curr_st:0x%2x      piox_curr_st:0x%2x\n",
			(tmp>>12) & 0xf,
			(tmp>>8) & 0xf);
	pr_debug("pmxx_curr_st:0x%2x      errx_curr_st:0x%2x\n",
			(tmp>>4) & 0xf,
			(tmp>>0) & 0xf);

	pr_debug("\n");
	pr_debug("**********Link status**********\n");
	tmp = readl(port_base + 0x54);
	pr_debug("link_curr_st:0x%2x\n", (tmp>>24) & 0x1f);
	pr_debug("link tx fifo:count-0x%x-empty-%x-ful-%x\n",
			(tmp>>0) & 0x1f,
			(tmp>>5) & 0x1, (tmp>>6) & 0x1);
	pr_debug("link rx fifo:count-0x%x-empty-%x-ful-%x\n",
			(tmp>>8) & 0x1f,
			(tmp>>13) & 0x1, (tmp>>14) & 0x1);
	pr_debug("link df fifo:count-0x%x-empty-%x-ful-%x\n\n",
			(tmp>>16) & 0x1f,
			(tmp>>21) & 0x1, (tmp>>22) & 0x1);

	pr_debug("**********CMD header**********\n");
	tmp = readl(port_base + 0x0);
	__hi_ahci_st_md(phys_to_virt(tmp));
	__hi_ahci_st_md(phys_to_virt(tmp+0x100));
	__hi_ahci_st_md(phys_to_virt(tmp+0x200));
	__hi_ahci_st_md(phys_to_virt(tmp+0x300));
}
EXPORT_SYMBOL_GPL(hi_ahci_st_dump);

