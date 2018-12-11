#include <linux/platform_device.h>
#include <linux/reset.h>
#include "hieth.h"
#include "mdio.h"
#include "phy.h"

static int KSZ8051MNL_phy_fix(struct phy_device *phy_dev)
{
	u32 v;

	if (phy_dev->interface != PHY_INTERFACE_MODE_RMII)
		return 0;

	v = phy_read(phy_dev, 0x1F);
	v |= (1 << 7);       /* set phy RMII 50MHz clk; */
	phy_write(phy_dev, 0x1F, v);

	v = phy_read(phy_dev, 0x16);
	v |= (1 << 1);       /* set phy RMII override; */
	phy_write(phy_dev, 0x16, v);

	return 0;
}

static int KSZ8081RNB_phy_fix(struct phy_device *phy_dev)
{
	u32 v;

	if (phy_dev->interface != PHY_INTERFACE_MODE_RMII)
		return 0;

	v = phy_read(phy_dev, 0x1F);
	v |= (1 << 7);       /* set phy RMII 50MHz clk; */
	phy_write(phy_dev, 0x1F, v);

	return 0;
}

void hieth_phy_register_fixups(void)
{
	phy_register_fixup_for_uid(PHY_ID_KSZ8051MNL,
				   DEFAULT_PHY_MASK, KSZ8051MNL_phy_fix);
	phy_register_fixup_for_uid(PHY_ID_KSZ8081RNB,
				   DEFAULT_PHY_MASK, KSZ8081RNB_phy_fix);
}

static void hieth_internal_phy_reset(struct hieth_phy_param_s *phy_param)
{
}

static void hieth_external_phy_reset(struct hieth_phy_param_s *phy_param)
{
	if (phy_param->phy_rst) {
		/* write 0 to cancel reset */
		reset_control_deassert(phy_param->phy_rst);
		msleep(50);

		/* RST_BIT, write 1 to reset phy, write 0 to cancel reset */
		reset_control_assert(phy_param->phy_rst);

		/* delay some time to ensure reset ok,
		 * this depends on PHY hardware feature
		 */
		msleep(50);

		/* write 0 to cancel reset */
		reset_control_deassert(phy_param->phy_rst);
		/* delay some time to ensure later MDIO access */
		msleep(50);
	}
}

void hieth_phy_reset(void)
{
	int i;
	struct hieth_phy_param_s *phy_param;

	for (i = 0; i < HIETH_MAX_PORT; i++) {
		phy_param = &hieth_phy_param[i];

		if (!phy_param->isvalid)
			continue;

		if (phy_param->isinternal)
			hieth_internal_phy_reset(phy_param);
		else
			hieth_external_phy_reset(phy_param);
	}
}
