/*************************
 Create By Lgpeng
*************************/

void dcache_enable(unsigned int ddrsize);
void dcache_disable(void);
void enable_mmu(void);
void stop_mmu(void);

extern unsigned char input_data[];
extern unsigned char input_data_end[];
extern unsigned int _armboot_start;

