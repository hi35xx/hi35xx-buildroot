#ifndef __HIGMAC_FEPHY_FIX_H
#define __HIGMAC_FEPHY_FIX_H

#define MII_MISC_CTL			0x16
#define MII_EXPMD			0x1D
#define MII_EXPMA			0x1E

#define XMODE_8bit_RW_NO_AUTO_INC	0x0
#define XMODE_8bit_RW_AUTO_INC		0x1
#define XMODE_16bit_RW_AUTO_INC		0x2
#define XMODE_8bit_WO_BUNDLED_ADDR	0x3

#define REALTEK_PHY_ID_8211EG		(0x001cc915)
#define REALTEK_PHY_MASK		(0xfffffff0)
#define HISILICON_PHY_ID_FESTAV200	(0x20669823)
#define HISILICON_PHY_MASK		(0xfffffff0)

/* the following two copied from phy_quirk()
 * in "./drivers/net/ethernet/hieth-sf/net.c"
 */
#define PHY_ID_KSZ8051MNL		(0x00221550)
#define PHY_ID_KSZ8081RNB		(0x00221560)
#define DEFAULT_PHY_MASK		(0xfffffff0)

void phy_register_fixups(void);
#endif
