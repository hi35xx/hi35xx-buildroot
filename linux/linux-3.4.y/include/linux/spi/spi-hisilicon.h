#ifndef SPI_HI_H
#define SPI_HI_H

#ifdef CONFIG_HI_SPI_DEBUG
#define hi_msg(x...) \
	do { \
		printk(KERN_NOTICE "%s->%d: ", __func__, __LINE__); \
		printk(KERN_NOTICE x); \
	} while (0)
#else
#define hi_msg(x...) do { } while (0)
#endif

/*
 * struct hi_spi_platform_data - SPI Controller defining structure
 * @clk_rate: spi input clk rate.
 * @num_cs: Number of CS this controller emulates.
 * @fifo_size: size of tx rx fifo.
 */
struct hi_spi_platform_data {
	unsigned long	clk_rate;
	unsigned char	num_cs;
	unsigned int	fifo_size;
	int		(*cfg_cs)(s16 bus_num, u8 csn);
	int		(*hw_init_cfg)(s16 bus_num);
	int		(*hw_exit_cfg)(s16 bus_num);

};

int hi_spi_set_platdata(struct hi_spi_platform_data *spd,
		struct platform_device *pdev);
#endif
