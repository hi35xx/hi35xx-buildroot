#ifndef _HAL_EFUSE_H_
#define _HAL_EFUSE_H_

#include "hi_type.h"

HI_S32 HAL_Efuse_WriteKey(HI_U32 * p_key, HI_U32 opt_id);
HI_S32 HAL_Efuse_LoadCipherKey(HI_U32 chn_id, HI_U32 opt_id);
HI_S32 HAL_Efuse_LoadHashKey(HI_U32 opt_id);
HI_VOID HAL_Efuse_Init(HI_VOID);

#endif

