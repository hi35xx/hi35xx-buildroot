#ifndef __HI_IRQS_H__
#define __HI_IRQS_H__

#define GODCUBE_IRQ_START	(0)

#define TIMER01_IRQ		(GODCUBE_IRQ_START + 3)
#define TIMER23_IRQ		(GODCUBE_IRQ_START + 4)
#define UART0_IRQ		(GODCUBE_IRQ_START + 5)

#define NR_IRQS			(GODCUBE_IRQ_START + 32)

#endif
