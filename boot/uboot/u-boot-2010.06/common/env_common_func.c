#include <common.h>
#include <config.h>
#include <environment.h>
#include <asm/arch/platform.h>

struct env_common_func_t {
	int (* env_init)(void);
	unsigned char (* env_get_char_spec)(int);
	int (* saveenv)(void);
	void (* env_relocate_spec)(void);
	char *env_name_spec;
};

#ifdef CONFIG_ENV_IS_IN_NAND
extern int nand_env_init(void);
extern unsigned char nand_env_get_char_spec(int);
extern int nand_saveenv(void);
extern void nand_env_relocate_spec(void);
static struct env_common_func_t nand_env_cmn_func = {
	.env_init = nand_env_init,
	.env_get_char_spec = nand_env_get_char_spec,
	.saveenv = nand_saveenv,
	.env_relocate_spec = nand_env_relocate_spec,
	.env_name_spec = "NAND",
};
#else
static struct env_common_func_t nand_env_cmn_func = {0};
#endif /* CONFIG_ENV_IS_IN_NAND */

#ifdef CONFIG_ENV_IS_IN_SPI_FLASH
extern int sf_env_init(void);
extern unsigned char sf_env_get_char_spec(int);
extern int sf_saveenv(void);
extern void sf_env_relocate_spec(void);
static struct env_common_func_t sf_env_cmn_func = {
	.env_init = sf_env_init,
	.env_get_char_spec = sf_env_get_char_spec,
	.saveenv = sf_saveenv,
	.env_relocate_spec = sf_env_relocate_spec,
	.env_name_spec = "SPI Flash",
};
#else
static struct env_common_func_t sf_env_cmn_func = {0};
#endif /* CONFIG_ENV_IS_IN_SPI_FLASH */

#ifdef CONFIG_ENV_IS_IN_EMMC
extern int emmc_env_init(void);
extern unsigned char emmc_env_get_char_spec(int);
extern int emmc_saveenv(void);
extern void emmc_env_relocate_spec(void);
static struct env_common_func_t emmc_env_cmn_func = {
	.env_init = emmc_env_init,
	.env_get_char_spec = emmc_env_get_char_spec,
	.saveenv = emmc_saveenv,
	.env_relocate_spec = emmc_env_relocate_spec,
	.env_name_spec = "eMMC Flash",
};
#else
static struct env_common_func_t emmc_env_cmn_func = {0};
#endif /* CONFIG_ENV_IS_IN_EMMC_FLASH */

char *env_name_spec;
env_t *env_ptr = 0;
struct env_common_func_t *env_cmn_func = NULL;

unsigned char env_get_char_spec(int index)
{
	return env_cmn_func ? env_cmn_func->env_get_char_spec(index) : -1;
}

int saveenv(void)
{
	return env_cmn_func ? env_cmn_func->saveenv() : -1;
}

void env_relocate_spec(void)
{
	if (env_cmn_func)
		env_cmn_func->env_relocate_spec();
}

int env_init(void)
{
	switch (get_boot_media()) {
	default:
		env_cmn_func = NULL;
		break;
	case BOOT_MEDIA_NAND:
		env_cmn_func = &nand_env_cmn_func;
		break;
	case BOOT_MEDIA_SPIFLASH:
		env_cmn_func = &sf_env_cmn_func;
		break;
	case BOOT_MEDIA_EMMC:
		env_cmn_func = &emmc_env_cmn_func;
		break;
	case BOOT_MEDIA_DDR:
		env_cmn_func = &sf_env_cmn_func;
		break;
	}

	if (env_cmn_func && !env_cmn_func->env_name_spec)
		env_cmn_func = NULL;

	/* unknow start media */
	if (!env_cmn_func)
		return -1;

	env_cmn_func->env_init();
	env_name_spec = env_cmn_func->env_name_spec;

	return 0;
}

