#include <config.h>

.section .image,#alloc

#ifdef CONFIG_DDR_TRAINING_HI3535
.globl hi3535_ddr_training_data_start
hi3535_ddr_training_data_start:
.incbin "../arch/arm/cpu/hi3535/ddr_training/hi3535/ddr_training.bin"
.globl hi3535_ddr_training_data_end
hi3535_ddr_training_data_end:
#endif /* CONFIG_DDR_TRAINING_HI3535 */
