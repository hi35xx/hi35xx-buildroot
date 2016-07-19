#ifndef __HISI_CLOCK_H__
#define __HISI_CLOCK_H__

#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/clkdev.h>
#include <asm/clkdev.h>

struct clk;

/**
 * struct clksel_rate - register bitfield values corresponding to clk divisors
 * @val: register bitfield value (shifted to bit 0)
 * @div: clock divisor corresponding to @val
 * @flags: (see "struct clksel_rate.flags possibilities" above)
 *
 * @val should match the value of a read from struct clk.clksel_reg
 * AND'ed with struct clk.clksel_mask, shifted right to bit 0.
 *
 * @div is the divisor that should be applied to the parent clock's rate
 * to produce the current clock's rate.
 *
 * XXX @flags probably should be replaced with an struct omap_chip.
 */
struct clksel_rate {
	u32			val;
	u8			div;
	u8			flags;
};

/**
 * struct clksel - available parent clocks, and a pointer to their divisors
 * @parent: struct clk * to a possible parent clock
 * @rates: available divisors for this parent clock
 *
 * A struct clksel is always associated with one or more struct clks
 * and one or more struct clksel_rates.
 */
struct clksel {
	struct clk		 *parent;
	const struct clksel_rate *rates;
	unsigned int value;
};

enum clk_state {
	UNINITIALIZED = 0,
	ON,
	OFF,
};

struct clk_ops {
	void	(*init)(struct clk *);
	int	(*enable)(struct clk *clk);
	void	(*disable)(struct clk *clk);
	long	(*round_rate)(struct clk *clk, long rate);
	int	(*set_rate)(struct clk *clk, unsigned rate);
	unsigned  int (*get_rate)(struct clk *clk);
	int	(*set_parent)(struct clk *clk, struct clk *parent);
	int	(*is_enabled)(struct clk *clk);
	void	(*reset)(struct clk *, bool);
};

struct clk {
	char	*name;
	struct clk	*parent;
	unsigned int	refcnt;
	unsigned long	rate;
	unsigned int	min_rate;
	unsigned int	max_rate;
	struct clk	*child;
	struct clk	*friend;
	enum clk_state	state;
	bool	set;
	u32	flags;
	struct clk_ops	*ops;
	struct clksel	*clksel;
	bool	is_enabled;
	struct mutex	mutex;
	spinlock_t	spinlock;
	struct list_head	node;
};

void  mpu_init_clocks(void);
void clk_init(struct clk *c);
void clk_exit(struct clk *c);

struct clk *hi_get_clock_by_name(const char *name);
unsigned long clk_get_rate(struct clk *c);
int clk_set_rate(struct clk *c, unsigned long rate);
int clk_enable(struct clk *c);
void clk_disable(struct clk *c);

#define OSC_FREQ 24000000

#endif
