#include <config.h>

/* image of the saved processor state */
typedef struct ns_saved_context {
	/*
	 * Structure saved_context would be used to hold processor state
	 * except caller and callee registers, just before suspending.
	 */

	/* coprocessor 15 registers */
	/* unsigned int ID_code; *//* read only reg */
	/* unsigned int cache_type; *//* read only reg */
	/* unsigned int TCM_stat; *//* read only reg */
	unsigned int CR;
	unsigned int TTBR;
	unsigned int DACR;
	unsigned int D_FSR;
	unsigned int I_FSR;
	unsigned int FAR;
	/* unsigned int COR; *//*write only reg */
	/* unsigned int TLBOR; *//*write only reg */
	unsigned int D_CLR;
	unsigned int I_CLR;
	unsigned int D_TCMRR;
	unsigned int I_TCMRR;
	unsigned int TLBLR;
	unsigned int FCSE;
	unsigned int CID;
} saved_context;

extern void call_swsusp_resume(unsigned long, unsigned long);

/*variable define here*/
void restore_processor_state_ext(void *pointer, unsigned long resume_addr)
{
	unsigned long sctl_register = 0;

	saved_context *ctxt = (saved_context *)pointer;

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
	/* asm volatile ("mcr p15, 0, %0, c1, c0, 0" : : "r" (ctxt->CR)); */

	call_swsusp_resume(sctl_register, resume_addr);
}

