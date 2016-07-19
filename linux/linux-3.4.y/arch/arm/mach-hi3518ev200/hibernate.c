/*
 * ARM Cortex-A7 save/restore for suspend to disk (Hibernation)
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/poll.h>
#include <linux/delay.h>
#include <linux/sysrq.h>
#include <linux/proc_fs.h>
#include <linux/pm.h>
#include <linux/device.h>
#include <linux/suspend.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/tlbflush.h>
#include <asm/suspend.h>
#include <asm/cacheflush.h>

typedef struct fault_regs {
	unsigned long dfar;
	unsigned long ifar;
	unsigned long ifsr;
	unsigned long dfsr;
	unsigned long adfsr;
	unsigned long aifsr;
} cp15_fault_regs;

typedef struct vfp_context {
	unsigned long fpexc;
	unsigned long fpscr;
	/*we just used low 32bit of d0-d15*/
	unsigned long d0_15[16];
} vfp_context;

typedef struct generic_timer_context {
	unsigned long cntfrq;
	unsigned long long cntvoff;
	unsigned long cnthctl;
	unsigned long cntkctl;
	unsigned long long cntp_cval;
	unsigned long cntp_tval;
	unsigned long cntp_ctl;
	unsigned long long cntv_cval;
	unsigned long cntv_tval;
	unsigned long cntv_ctl;
	unsigned long long cnthp_cval;
	unsigned long cnthp_tval;
	unsigned long cnthp_ctl;
} gen_timer_context;
#if 0
typedef struct ns_saved_context {
	unsigned long cp15_misc_regs[2];     /* cp15 miscellaneous registers */
	unsigned long cp15_ctrl_regs[20];    /* cp15 control registers */
	unsigned long cp15_mmu_regs[16];     /* cp15 mmu registers */
	cp15_fault_regs cp15_fault_regs; /* cp15 fault status registers */
	gen_timer_context gen_timer_regs;
	vfp_context vfp_regs;
} saved_context;
#endif
/* image of the saved processor state */
typedef struct ns_saved_context {
	/*
	 * Structure saved_context would be used to hold processor state
	 * except caller and callee registers, just before suspending.
	 */

	__u32 CR;
	__u32 TTBR;
	__u32 DACR;
	__u32 D_FSR;
	__u32 I_FSR;
	__u32 FAR;
	__u32 D_CLR;
	__u32 I_CLR;
	__u32 D_TCMRR;
	__u32 I_TCMRR;
	__u32 TLBLR;
	__u32 FCSE;
	__u32 CID;
} saved_context;
#ifdef CONFIG_DEBUG_LL
extern void printascii(const char *);
#endif

extern const void __nosave_begin, __nosave_end;

/*save&restore helper functions*/
extern asmlinkage void save_cp15(void *pointer);
extern asmlinkage void save_control_registers(void *pointer, int is_secure);
extern asmlinkage void save_mmu(void *pointer);
extern asmlinkage void save_fault_status(void *pointer);
extern asmlinkage void save_vfp(void *pointer);
extern asmlinkage void save_generic_timer(void *pointer, int is_hyp);
extern asmlinkage void restore_control_registers(void *pointer, int is_secure);

extern asmlinkage void restore_vfp(void *pointer);
extern asmlinkage void restore_cp15(void *pointer);
extern asmlinkage void restore_mmu(void *pointer);
extern asmlinkage void restore_fault_status(void *pointer);
extern asmlinkage void restore_generic_timer(void *pointer, int is_hyp);
extern asmlinkage void invalidate_unified_TLB_inner_shareable(void);

/*variable define here*/
saved_context saved_ctx __attribute__((aligned(PAGE_SIZE)));

const void *saved_processor_context = &saved_ctx;

int pfn_is_nosave(unsigned long pfn)
{
	unsigned long nosave_begin_pfn = __pa_symbol(&__nosave_begin)
		>> PAGE_SHIFT;
	unsigned long nosave_end_pfn = PAGE_ALIGN(__pa_symbol(&__nosave_end))
		>> PAGE_SHIFT;

	return (pfn >= nosave_begin_pfn) && (pfn < nosave_end_pfn);
}

void __save_processor_state(saved_context *ctxt)
{
#ifdef CONFIG_DEBUG_LL
	printascii("__save_processor_state(): enter\n");
#endif
#if 0
	save_cp15(ctxt->cp15_misc_regs);
	save_control_registers(ctxt->cp15_ctrl_regs, 0x0);
	save_mmu(ctxt->cp15_mmu_regs);

	save_fault_status(&ctxt->cp15_fault_regs);

	save_vfp(&ctxt->vfp_regs);
#endif
	/* save coprocessor 15 registers */
	asm volatile ("mrc p15, 0, %0, c1, c0, 0" : "=r" (ctxt->CR));
	asm volatile ("mrc p15, 0, %0, c3, c0, 0" : "=r" (ctxt->DACR));
	asm volatile ("mrc p15, 0, %0, c5, c0, 0" : "=r" (ctxt->D_FSR));
	asm volatile ("mrc p15, 0, %0, c5, c0, 1" : "=r" (ctxt->I_FSR));
	asm volatile ("mrc p15, 0, %0, c6, c0, 0" : "=r" (ctxt->FAR));
	asm volatile ("mrc p15, 0, %0, c9, c0, 0" : "=r" (ctxt->D_CLR));
	asm volatile ("mrc p15, 0, %0, c9, c0, 1" : "=r" (ctxt->I_CLR));
	asm volatile ("mrc p15, 0, %0, c9, c1, 0" : "=r" (ctxt->D_TCMRR));
	asm volatile ("mrc p15, 0, %0, c9, c1, 1" : "=r" (ctxt->I_TCMRR));
	asm volatile ("mrc p15, 0, %0, c10, c0, 0" : "=r" (ctxt->TLBLR));
	asm volatile ("mrc p15, 0, %0, c13, c0, 0" : "=r" (ctxt->FCSE));
	asm volatile ("mrc p15, 0, %0, c13, c0, 1" : "=r" (ctxt->CID));
	asm volatile ("mrc p15, 0, %0, c2, c0, 0" : "=r" (ctxt->TTBR));

#ifdef CONFIG_DEBUG_LL
	printascii("__save_processor_state(): exit\n");
#endif
}

void __restore_processor_state(saved_context *ctxt)
{
#ifdef CONFIG_DEBUG_LL
	printascii("__restore_processor_state(): enter\n");
#endif
#if 0
	restore_vfp(&ctxt->vfp_regs);

	restore_fault_status(&ctxt->cp15_fault_regs);

	restore_mmu(ctxt->cp15_mmu_regs);

	invalidate_unified_TLB_inner_shareable();

	restore_control_registers(ctxt->cp15_ctrl_regs, 0x0);
	isb();
	dsb();

	restore_cp15(ctxt->cp15_misc_regs);
#endif
	/* restore coprocessor 15 registers */
	asm volatile ("mcr p15, 0, %0, c2, c0, 0" : : "r" (ctxt->TTBR));
	asm volatile ("mcr p15, 0, %0, c13, c0, 1" : : "r" (ctxt->CID));
	asm volatile ("mcr p15, 0, %0, c13, c0, 0" : : "r" (ctxt->FCSE));
	asm volatile ("mcr p15, 0, %0, c10, c0, 0" : : "r" (ctxt->TLBLR));
	asm volatile ("mcr p15, 0, %0, c9, c1, 1" : : "r" (ctxt->I_TCMRR));
	asm volatile ("mcr p15, 0, %0, c9, c1, 0" : : "r" (ctxt->D_TCMRR));
	asm volatile ("mcr p15, 0, %0, c9, c0, 1" : : "r" (ctxt->I_CLR));
	asm volatile ("mcr p15, 0, %0, c9, c0, 0" : : "r" (ctxt->D_CLR));
	asm volatile ("mcr p15, 0, %0, c6, c0, 0" : : "r" (ctxt->FAR));
	asm volatile ("mcr p15, 0, %0, c5, c0, 1" : : "r" (ctxt->I_FSR));
	asm volatile ("mcr p15, 0, %0, c5, c0, 0" : : "r" (ctxt->D_FSR));
	asm volatile ("mcr p15, 0, %0, c3, c0, 0" : : "r" (ctxt->DACR));
	asm volatile ("mcr p15, 0, %0, c1, c0, 0" : : "r" (ctxt->CR));

#ifdef CONFIG_DEBUG_LL
	printascii("__restore_processor_state(): done\n");
#endif
}

void save_processor_state(void)
{
	preempt_disable();
	__save_processor_state(&saved_ctx);
}
EXPORT_SYMBOL(save_processor_state);

void restore_processor_state(void)
{
	__restore_processor_state(&saved_ctx);
	preempt_enable();
}
EXPORT_SYMBOL(restore_processor_state);
