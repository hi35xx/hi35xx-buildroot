/*
 * ARM Cortex-A7 save/restore for suspend to disk (Hibernation)
 */
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

typedef struct ns_saved_context {
	unsigned long cp15_misc_regs[2];     /* cp15 miscellaneous registers */
	unsigned long cp15_ctrl_regs[20];    /* cp15 control registers */
	unsigned long cp15_mmu_regs[16];     /* cp15 mmu registers */
	cp15_fault_regs cp15_fault_regs; /* cp15 fault status registers */
	gen_timer_context gen_timer_regs;
	vfp_context vfp_regs;
} saved_context;


/*restore helper functions*/
extern void restore_cp15(void *pointer);
extern void restore_mmu(void *pointer);
extern void restore_fault_status(void *pointer);
extern void restore_control_registers(void *pointer, int is_hyp);
extern void restore_generic_timer(void *pointer, int is_hyp);
extern void invalidate_unified_tlb_inner_shareable(void);
extern void call_swsusp_resume(unsigned long, unsigned long);


/*variable define here*/
void restore_processor_state_ext(void *pointer, unsigned long resume_addr)
{
	unsigned long sctl_register;

	saved_context *ctxt = (saved_context *)pointer;

	restore_fault_status(&ctxt->cp15_fault_regs);

	restore_mmu(ctxt->cp15_mmu_regs);

	invalidate_unified_tlb_inner_shareable();

	restore_control_registers(ctxt->cp15_ctrl_regs, 0x0);

	restore_cp15(ctxt->cp15_misc_regs);

	/* restore_generic_timer(&ctxt->gen_timer_regs, 0x0); */

	asm volatile ("dmb" : : : "memory");
	asm volatile ("dsb" : : : "memory");
	asm volatile ("isb" : : : "memory");

	/* now turning MMU on*/
	sctl_register = ctxt->cp15_ctrl_regs[1];

	call_swsusp_resume(sctl_register, resume_addr);
}

