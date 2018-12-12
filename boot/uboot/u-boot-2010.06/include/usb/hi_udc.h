#ifndef __HI_UDC_H__
#define __HI_UDC_H__

int udc_init(void);
int udc_connect(void);
void udc_disconnect(void);
void udc_irq_poll(void);
void udc_puts(const char *s);

#endif /* __HI_UDC_H__ */
