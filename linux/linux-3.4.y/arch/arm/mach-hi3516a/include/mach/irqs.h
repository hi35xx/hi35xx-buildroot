#ifndef __HI_IRQS_H__
#define __HI_IRQS_H__

#define HISI_GIC_IRQ_START      (32)
#define IRQ_LOCALTIMER          (29)

#define INTNR_COMMTX0           (HISI_GIC_IRQ_START + 0)
#define INTNR_COMMRX0           (HISI_GIC_IRQ_START + 1)

#define INTNR_WATCHDOG          (HISI_GIC_IRQ_START + 2)

#define INTNR_TIMER_0         (HISI_GIC_IRQ_START + 3)
#define INTNR_TIMER_1         (HISI_GIC_IRQ_START + 4)
#define INTNR_TIMER_2         (HISI_GIC_IRQ_START + 5)
#define INTNR_TIMER_3         (HISI_GIC_IRQ_START + 6)

#define INTNR_UART0             (HISI_GIC_IRQ_START + 8)
#define INTNR_UART1             (HISI_GIC_IRQ_START + 9)
#define INTNR_UART2             (HISI_GIC_IRQ_START + 10)
#define INTNR_UART3             (HISI_GIC_IRQ_START + 11)

#define NR_IRQS                 (HISI_GIC_IRQ_START + 96)

#define MAX_GIC_NR              1

#endif
