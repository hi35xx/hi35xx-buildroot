#ifndef __DRV_HDMI_COMPATIBILITY_H__
#define __DRV_HDMI_COMPATIBILITY_H__

#include "drv_hdmi_common.h"
#include "drv_hdmi_intf.h"
typedef struct
{
    HI_U8           u8IDManufactureName[4];     /**<Manufacture name*//**<CNcomment:设备厂商标识 */
    HI_U32          u32IDProductCode;           /**<Product code*//**<CNcomment:设备ID */
    HI_U32          u32DelayTimes;
    HI_U32          u32MuteDelay;
    HI_U8           u8ProductType[32];          /**<Product Type*//**<CNcomment:产品型号 */
}HDMI_DELAY_TIME_S;

typedef struct
{
    HI_U32  u32CrgDivMs;
    HI_U32  u32PhySrstMs;
}COMPAT_CRG_SRST_S;

HI_S32 FormatDelayGet(HDMI_DEVICE_S* pstHdmiDev, HI_U32 *pu32DelayTime);
HI_S32 AvMuteDelayGet(HDMI_DEVICE_S* pstHdmiDev, HI_U32 *pu32DelayTime);
HI_BOOL DRV_HDMI_CompatCecValidCheck(HDMI_DEVICE_S *pstHdmiDev);
HI_S32  DRV_HDMI_CompatHdcpMsGet(HDMI_DEVICE_ID_E enHdmiId,HI_U64 *pu64WaitMs);
HI_S32  DRV_HDMI_CompatAvmuteGet(HDMI_DEVICE_ID_E enHdmiId,HI_BOOL bAvmute,HDMI_AVMUTE_CFG_S *pstAvmuteCfg);
HI_S32  DRV_HDMI_CompatCrgMsGet(HDMI_DEVICE_ID_E enHdmiId,HI_U32 *pu32CrgDivMs,HI_U32 *pu32PhySrstMs);
HI_BOOL DRV_HDMI_CompatScdcInStartGet(HDMI_DEVICE_ID_E enHdmiId);

#endif //__DRV_HDMI_COMPATIBILITY_H__