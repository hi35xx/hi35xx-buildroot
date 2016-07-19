/*************************************************************************
* si_drv_videopath.c
*************************************************************************/

//#include "si_drv_videopath_cra.h"
#include "si_vidpath_regs.h"
#include "si_drv_tx_regs.h"
#include "si_drv_tx_regs_enums.h"
#include "si_drv_common.h"
#include "si_datatypes.h"
#include "si_lib_log_api.h"
#include "si_lib_obj_api.h"
#ifndef HDMI_BUILD_IN_BOOT
#include "si_lib_seq_api.h"
#endif
#include "si_mod_videopath_api.h"
#ifdef SI_DRV_DINO_CFG
#include "si_drv_dino_cfg.h"
#endif
#include "si_drv_tx_api.h"

/***** Register Module name **************************************************/
SII_LIB_OBJ_MODULE_DEF(videopath);

#define BIT_MASK__VP__INPUT_SYNC_ADJUST_CONFIG__SIZE		1 
#define BIT_MASK__VP__DEGEN_CONFIG__SIZE					1
#define BIT_MASK__VP__DEGEN_PIXEL_DELAY__SIZE				2
#define BIT_MASK__VP__DEGEN_LINE_DELAY__SIZE				2
#define BIT_MASK__VP__OUTPUT_MUTE__SIZE						1
#define BIT_MASK__VP__CMS_CSC1_444_422_CONFIG__SIZE  1
#define BIT_MASK__VP__CMS_CSC1_422_420_CONFIG__SIZE 1
#define BIT_MASK__VP__CMS_CSC0_420_422_CONFIG__SIZE 1
#define BIT_MASK__VP__CMS_CSC0_422_444_CONFIG__SIZE 1

#define BIT_MASK__VP__CMS_CSC0_MULTI_CSC_CONFIG__SIZE 2
#define BIT_MASK__VP__CMS_CSC0_444_422_CONFIG__SIZE 1
#define BIT_MASK__VP__CMS_CSC0_422_420_CONFIG__SIZE 1

#define BIT_MASK__VP__CMS_CSC1_MULTI_CSC_CONFIG__SIZE 2
#define BIT_MASK__VP__CMS_CSC1_420_422_CONFIG__SIZE 1
#define BIT_MASK__VP__CMS_CSC1_422_444_CONFIG__SIZE 1

#define BIT_MASK__VP__INPUT_FORMAT__SIZE 2

#define BIT_MASK__VP__INPUT_SYNC_ADJUST_CONFIG__SIZE 1

#define BIT_MASK__VP__DEGEN_CONFIG__SIZE 1
#define BIT_MASK__VP__DEGEN_PIXEL_DELAY__SIZE 2
#define BIT_MASK__VP__DEGEN_LINE_DELAY__SIZE 2

#define BIT_MASK__VP__OUTPUT_FORMAT__SIZE 2

#define BIT_MASK__VP__OUTPUT_MUTE__SIZE 1

#define BIT_MASK__VP__CMS__CSC1__DITHERING_CONFIG__SIZE 1

#define BIT_MASK__VP__FDET_STATUS__SIZE 1
#define BIT_MASK__VP__FDET_IRQ_STAT__SIZE 3
#define BIT_MASK__VP__OUTPUT_SYNC_CONFIG__SIZE				1
#define BIT_MASK__VP__FDET_IRQ_MASK__SIZE					3

#ifdef SI_DRV_DINO_CFG
extern bool_t isColorConv_en;
extern bool_t isOutBItDepthChng;
extern uint8_t outClrSpc;
extern uint8_t outClrConvStd;
extern uint8_t outClrDc;
#endif


typedef enum
{
	RGB=0,
	YCbCr422,
	YCbCr444,
	YCbCr420
}ColorSpace_t;

typedef enum
{
    QUANTIZATION_VIDEO_LEVELS, // 16 - 235
    QUANTIZATION_PC_LEVELS // 0 - 255
}QuantizationLevel_t;

typedef struct RateConvertCfg
{
	bit_fld_t     mux_420_enable;
}RateConvertCfg_t;

typedef struct EmbeddeSyncCfg
{
	uint8_t tmp;
}EmbeddeSyncCfg_t;

typedef struct DeGenCfg
{
	bit_fld_t enable;
	bit_fld16_t pixel_delay;
	bit_fld16_t line_delay;
}DeGenCfg_t;

typedef struct VideoBalnkCfg
{
	uint8_t tmp;
}VideoBalnkCfg_t;

typedef struct SamplerCfg
{
	ColorSpace_t inputClrSpc;
	ColorSpace_t outputClrSpc;
}SamplerCfg_t;

typedef struct CscConfig
{
	ColorSpace_t inputClrSpc;
	ColorSpace_t outputClrSpc;
	QuantizationLevel_t inQuantization;
	QuantizationLevel_t outQuantization;
	SiiDrvTxConvStd_t inConvStd;
	SiiDrvTxConvStd_t outConvStd;
}CscConfig_t;

typedef struct GammaCorrectionConfig
{
	uint8_t tmp;
}GammaCorrectionConfig_t;

typedef struct DataPathMuxCfg
{
	uint8_t tmp;
}DataPathMuxCfg_t;

typedef enum
{
	SYNCPOL_NEGATIVE,
	SYNCPOL_POSITIVE
}SyncPol_t;

typedef struct SyncPolAdjustCfg
{
	bit_fld_t             bAutoAdjust;
	SyncPol_t             HSYNC_Pol;
	SyncPol_t             VSYNC_Pol;
}SyncPolAdjustCfg_t;

typedef struct OuputClorInfo
{
	SiiDrvTxConvStd_t convStd;
	ColorSpace_t         clrSpc;
	SiiDrvBitDepth_t vidDcDepth;
	QuantizationLevel_t quntization;
}VideoColorInfo_t;



typedef struct CmsCfg
{
	bit_fld_t               bEnable_420422;
	bit_fld_t               bEnable_422444;
	bit_fld_t               bEnable_csc0;
	bit_fld_t               bIsGamaCorrEnable;
	bit_fld_t               bEnable_csc1;
	bit_fld_t               bEnable_444422;
	bit_fld_t               bEnable_422420;
	bit_fld_t               bEnable_Dithering;
	VideoColorInfo_t        inClrInfo;
	VideoColorInfo_t        outClrInfo;
}CmsCfg_t;

typedef struct VideoPathConfig
{
	bit_fld_t               bEnable_IDPMux;
	bit_fld_t               bEnable_IRC;
	bit_fld_t               bEnable_InSPA;
	bit_fld_t               bEnable_656D;
	bit_fld_t               bEnable_DEG;
	bit_fld_t               bEnable_CMS;
	bit_fld_t               bEnable_VB;
	bit_fld_t               bEnable_ORC;
	bit_fld_t               bEnable_ESE;
	bit_fld_t               bEnable_OutSPA;//hsync,vsync polarity control
	bit_fld_t               bEnable_ODPMux;
	bit_fld_t               bEnable_Dithering;

	VideoColorInfo_t        outClrInfo;
	VideoColorInfo_t        inClrInfo;
	SiiHvSyncPol_t			outputHVSyncPol;
	SiiInst_t				instTxCra;
}VideoPathConfig_t;

typedef struct videopathparam
{
	SiiHvSyncPol_t         hvSyncPol;
	bit_fld_t              bColorConv_en;
	bit_fld_t              bDithering_en;
	VideoColorInfo_t       inClrInfo;
	VideoColorInfo_t       outClrInfo;

	SiiInst_t			   instTxCra;

}VideoPathParam_t;



static void sVidPathOutputColorSpaceSet(VideoPathParam_t *pVidPathParam, SiiDrvTxClrSpc_t clrSpc);
static void sVidPathOutputBitDepthSet(VideoPathParam_t *pVidPathParam, SiiDrvBitDepth_t bitDepth);
static void sVideoPathColorInfoConfig(VideoPathParam_t *pVidPathParam, SiiDrvTxColorInfoCfg_t *pClrInfo);
static void sVidPathHvSyncPolaritySet(VideoPathParam_t *pVidPathParam, SiiHvSyncPol_t hvSyncPol);

static void sGetInputVideoColorInfo(VideoPathParam_t *pVidPathParam, VideoColorInfo_t* pVidClrInfo);
static void sSetVideoPathdefaults(VideoPathParam_t *pVidPathParam);

static void sVideoPathCoreConfig(VideoPathParam_t *pVidPathParam, VideoPathConfig_t* pConfig);
static void sInputDataPathMuxConfig(VideoPathParam_t *pVidPathParam, DataPathMuxCfg_t* pinMap);
static void sInputRateConverterConfig(VideoPathParam_t *pVidPathParam, RateConvertCfg_t* pCfg);
static void sInSyncPolarityAdjustmentConfig(VideoPathParam_t *pVidPathParam, SyncPolAdjustCfg_t *pCfg);
static void s656DecderConfig(VideoPathParam_t *pVidPathParam, EmbeddeSyncCfg_t* pCfg);
static void sDEGeneratorConfig(VideoPathParam_t *pVidPathParam, DeGenCfg_t* pCfg);
static void sVideoBlankingConfig(VideoPathParam_t *pVidPathParam, VideoBalnkCfg_t* pCfg);
static void sOutputRateConverterConfig(VideoPathParam_t *pVidPathParam, RateConvertCfg_t* pCfg);
static void sEmbeddedSyncEncoderConfig(VideoPathParam_t *pVidPathParam, EmbeddeSyncCfg_t* pCfg);
static void sOutSyncPolarityAdjustmentConfig(VideoPathParam_t *pVidPathParam, SyncPolAdjustCfg_t *pCfg);
static void sOutnputDataPathMuxConfig(VideoPathParam_t *pVidPathParam, DataPathMuxCfg_t* pinMap);

static void sVideoPathCmsConfig(VideoPathParam_t *pVidPathParam, CmsCfg_t* pCfg);
static void sUpSamplerConfig(VideoPathParam_t *pVidPathParam, SamplerCfg_t* pCfg); 
static void sVideoPathCsc0Config(VideoPathParam_t *pVidPathParam, CscConfig_t* pCfg);
static void sGammaCorrectionConfig(VideoPathParam_t *pVidPathParam, GammaCorrectionConfig_t* pCfg);
static void sVideoPathCsc1Config(VideoPathParam_t *pVidPathParam, CscConfig_t* pCfg);
static void sDownSamplerConfig(VideoPathParam_t *pVidPathParam, SamplerCfg_t* pCfg);
static void sDitheringConfig(VideoPathParam_t *pVidPathParam, SiiDitherCfg_t* dithConf);

static void sOutputMute(VideoPathParam_t *pVidPathParam, bool_t enMute);
static void sDecodeColorInfo(VideoPathParam_t *pVidPathParam, SiiDrvTxClrSpc_t clrSpc, VideoColorInfo_t* pClrInfo);
static void sSoftwareInit(VideoPathParam_t *pVidPathParam);

//static void sVideoPathIntrHandler(SiiInst_t);

//----------------------------------------------------------------------------
SiiInst_t SiiModVideoPathCreate(char *pNameStr, SiiDrvVideoPathCfg_t *pConfig)
{
	VideoPathParam_t*		pvidPathParam;
	//VideoPathConfig_t		vidPathCfg;
	//uint32_t				vp_fdet_irq_mask;
	pvidPathParam = (VideoPathParam_t *) SII_LIB_OBJ_CREATE(pNameStr, sizeof(VideoPathParam_t));
	SII_PLATFORM_DEBUG_ASSERT(pvidPathParam);
    sSoftwareInit(pvidPathParam);
    pvidPathParam->instTxCra = pConfig->instTxCra;
#if 0
	pvidPathParam->inClrInfo.clrSpc = RGB;//SII_DRV_TX_CLRSPC__RGB_FULL;
	pvidPathParam->inClrInfo.convStd = SII_DRV_TX_CONV_STD__BT_601;
	pvidPathParam->inClrInfo.vidDcDepth = SII_DRV_BIT_DEPTH__8_BIT;
	pvidPathParam->inClrInfo.quntization = QUANTIZATION_PC_LEVELS;
	pvidPathParam->bColorConv_en = 0;
	

	//Disabling Input datapath mux as it is required only if input tmsd lines are swapped
	vidPathCfg.bEnable_IDPMux = 0;
	vidPathCfg.bEnable_ODPMux = 0;

	//presently not using 656 decoder and embedded sync encoder
	vidPathCfg.bEnable_656D = 0;
	vidPathCfg.bEnable_ESE = 0;

	//automatic sync polairty adjustment
	vidPathCfg.bEnable_InSPA = 1;
	vidPathCfg.bEnable_OutSPA = 1;

	vidPathCfg.bEnable_DEG = 0;

	//video blanking shall be used when required
	vidPathCfg.bEnable_VB = 0;

	//temoprarily dsiabling below modules
	vidPathCfg.bEnable_ORC = 0;
	vidPathCfg.bEnable_IRC = 0;
	vidPathCfg.bEnable_CMS = 0;
	vidPathCfg.bEnable_Dithering = 0;
	
	vidPathCfg.instTxCra = pConfig->instTxCra;
	sVideoPathCoreConfig(pvidPathParam, &vidPathCfg);
	//Unmask VideoPath Interrupt
	//vp_fdet_irq_mask = 0x07FFFF;
	//VideoPath Intr mask in enabled by default. Clear it to avoid group Intr.
	vp_fdet_irq_mask = 0x0;
	SiiModVideoPathRegWrite(pvidPathParam->instTxCra, REG_ADDR__VP__FDET_IRQ_MASK, (uint8_t *) &vp_fdet_irq_mask, BIT_MASK__VP__FDET_IRQ_MASK__SIZE);
#endif
    return (SiiInst_t)SII_LIB_OBJ_INST(pvidPathParam);
}

//----------------------------------------------------------------------------
void SiiModVideoPathDelete( SiiInst_t inst)
{
    VideoPathParam_t *pVidPathParam = (VideoPathParam_t *)SII_LIB_OBJ_PNTR(inst);

    SII_LIB_OBJ_DELETE(pVidPathParam);
}


//output config
//********************************************************************************
void sVidPathOutputColorSpaceSet(VideoPathParam_t *pVidPathParam, SiiDrvTxClrSpc_t clrSpc)
{
	VideoPathConfig_t      vidPathCfg;
	

	SII_MEMSET(&vidPathCfg,0,sizeof(VideoPathConfig_t));

	pVidPathParam->bColorConv_en = 1;
	sDecodeColorInfo(pVidPathParam, clrSpc, & (vidPathCfg.outClrInfo));
	vidPathCfg.outClrInfo.vidDcDepth = pVidPathParam->outClrInfo.vidDcDepth;

	pVidPathParam->outClrInfo = vidPathCfg.outClrInfo;

	sGetInputVideoColorInfo(pVidPathParam, &vidPathCfg.inClrInfo);

    //set output rate converter block if required output clr space is YCBCR420
	if(vidPathCfg.outClrInfo.clrSpc == YCbCr420)
		vidPathCfg.bEnable_ORC = 1;
	else
		vidPathCfg.bEnable_ORC = 0;

	if(vidPathCfg.inClrInfo.clrSpc == YCbCr420)
		vidPathCfg.bEnable_IRC = 1;
	else
		vidPathCfg.bEnable_IRC = 0;

	vidPathCfg.bEnable_CMS = 1;
	vidPathCfg.instTxCra = pVidPathParam->instTxCra;

	sOutputMute(pVidPathParam, true);
	sSetVideoPathdefaults(pVidPathParam);
	sVideoPathCoreConfig(pVidPathParam, & vidPathCfg);
	sOutputMute(pVidPathParam, false);

#ifdef SI_DRV_DINO_CFG
	outClrSpc = (uint8_t)vidPathCfg.outClrInfo.clrSpc;
	outClrConvStd = (uint8_t)vidPathCfg.outClrInfo.convStd;
	outClrDc = (uint8_t)vidPathCfg.inClrInfo.vidDcDepth;
	isColorConv_en = true;
	DrvDinoUpdateInfoFrame(0);
#endif
}

//-----------------------------------------------------------------------------
void sVidPathOutputBitDepthSet(VideoPathParam_t *pVidPathParam, SiiDrvBitDepth_t bitDepth)
{
	VideoPathConfig_t      vidPathCfg;
	
	SII_MEMSET(&vidPathCfg,0,sizeof(VideoPathConfig_t));
	sGetInputVideoColorInfo(pVidPathParam, & vidPathCfg.inClrInfo );

	if((bitDepth != SII_DRV_BIT_DEPTH__PASSTHOUGH) && (bitDepth <= pVidPathParam->inClrInfo.vidDcDepth))
	{
		vidPathCfg.bEnable_Dithering = 1;
		if(vidPathCfg.inClrInfo.clrSpc == YCbCr420)
			vidPathCfg.bEnable_IRC = 1;
		else
			vidPathCfg.bEnable_IRC = 0;

		pVidPathParam->outClrInfo.vidDcDepth = bitDepth;
		vidPathCfg.outClrInfo = pVidPathParam->outClrInfo;

		if(vidPathCfg.outClrInfo.clrSpc == YCbCr420)
			vidPathCfg.bEnable_ORC = 1;
		else
		    vidPathCfg.bEnable_ORC = 0;

		pVidPathParam->bDithering_en = true;
		vidPathCfg.bEnable_CMS = 1;

		vidPathCfg.instTxCra = pVidPathParam->instTxCra;

		sOutputMute(pVidPathParam, true);
		sSetVideoPathdefaults(pVidPathParam);
		sVideoPathCoreConfig(pVidPathParam, &vidPathCfg);
		sOutputMute(pVidPathParam, false);
#ifdef SI_DRV_DINO_CFG
		outClrConvStd = (uint8_t)vidPathCfg.outClrInfo.convStd;
		outClrDc = (uint8_t)vidPathCfg.outClrInfo.vidDcDepth;
		outClrSpc = (uint8_t)vidPathCfg.outClrInfo.clrSpc;
		isColorConv_en = true;
		isOutBItDepthChng = true;
#endif
	}
    else
    {
        //need to recover when higher bit depth is set.
        pVidPathParam->outClrInfo.vidDcDepth = bitDepth;
        pVidPathParam->bDithering_en = true;
    }

}

//input config
void sVideoPathColorInfoConfig(VideoPathParam_t *pVidPathParam, SiiDrvTxColorInfoCfg_t *pClrInfo)
{
	VideoPathConfig_t      vidPathCfg;
	

	SII_MEMSET(&vidPathCfg,0,sizeof(VideoPathConfig_t));

	sDecodeColorInfo(pVidPathParam, pClrInfo->inputClrSpc, & pVidPathParam->inClrInfo);
	pVidPathParam->inClrInfo.vidDcDepth = pClrInfo->inputVidDcDepth;

	sGetInputVideoColorInfo(pVidPathParam, &vidPathCfg.inClrInfo);

	if(vidPathCfg.inClrInfo.clrSpc == YCbCr420)
		vidPathCfg.bEnable_IRC = 1;
	else
		vidPathCfg.bEnable_IRC = 0;

	if(pVidPathParam->bColorConv_en || pVidPathParam->bDithering_en)
	{
		vidPathCfg.bEnable_CMS = 1;
		vidPathCfg.outClrInfo = pVidPathParam->outClrInfo;
	}
	else
	{
		vidPathCfg.outClrInfo = vidPathCfg.inClrInfo;
		pVidPathParam->outClrInfo = vidPathCfg.outClrInfo;
	}

    vidPathCfg.bEnable_Dithering = pVidPathParam->bDithering_en;
    // below change is done considering the fact that for input colour depth has to be followed in following cases:
    // 1. when dithering is disabled
    // 2. Disthering is enabled and input bit depth is less than required dithering output.
    // In other cases colour depth has to be recovered to the desired colour depth
    /*if(pVidPathParam->bDithering_en && (vidPathCfg.inClrInfo.vidDcDepth >= pVidPathParam->outClrInfo.vidDcDepth))
    {
        vidPathCfg.bEnable_Dithering = 1;
    }
    else if(pVidPathParam->bDithering_en && (vidPathCfg.inClrInfo.vidDcDepth < pVidPathParam->outClrInfo.vidDcDepth))
    {
        vidPathCfg.outClrInfo.vidDcDepth = vidPathCfg.inClrInfo.vidDcDepth;
    }*/
   // else
    if(!(pVidPathParam->bDithering_en && (vidPathCfg.inClrInfo.vidDcDepth >= pVidPathParam->outClrInfo.vidDcDepth)))
    {
        vidPathCfg.outClrInfo.vidDcDepth = vidPathCfg.inClrInfo.vidDcDepth;
    }

	if(vidPathCfg.outClrInfo.clrSpc == YCbCr420)
		vidPathCfg.bEnable_ORC = 1;
	else
		vidPathCfg.bEnable_ORC = 0;
	// CMS is always enabled to make any incoming input videodepth to 12 bit.
	// For this if we wan to let the video in pas trough mode, dithering has to be enabled to match the input color depth.
	vidPathCfg.bEnable_CMS = 1;

	vidPathCfg.instTxCra = pVidPathParam->instTxCra;

	sOutputMute(pVidPathParam, true);
	sSetVideoPathdefaults(pVidPathParam );
	sVideoPathCoreConfig( pVidPathParam, &vidPathCfg);
	sOutputMute(pVidPathParam, false);

#ifdef SI_DRV_DINO_CFG
	outClrConvStd = (uint8_t)vidPathCfg.outClrInfo.convStd;

	if(pVidPathParam->bColorConv_en || (outClrSpc != vidPathCfg.outClrInfo.clrSpc))
		isColorConv_en = true;

	outClrSpc = (uint8_t)vidPathCfg.outClrInfo.clrSpc;

	if(outClrDc != vidPathCfg.outClrInfo.vidDcDepth)
	{
		outClrDc = (uint8_t)vidPathCfg.outClrInfo.vidDcDepth;
		isOutBItDepthChng = true;
	}
#endif
}


//-----------------------------------------------------------------------------
void sVidPathHvSyncPolaritySet(VideoPathParam_t *pVidPathParam, SiiHvSyncPol_t hvSyncPol)
{
	VideoPathConfig_t      vidPathCfg;
	

	SII_MEMSET(&vidPathCfg, 0, sizeof(VideoPathConfig_t));
	SII_LIB_LOG_DEBUG1(pVidPathParam, "Setting HV_SYNC Polarity to: %02X\n", hvSyncPol);
	vidPathCfg.outputHVSyncPol = hvSyncPol;
	vidPathCfg.bEnable_OutSPA = 1;
	vidPathCfg.instTxCra = pVidPathParam->instTxCra;
	sVideoPathCoreConfig(pVidPathParam, &vidPathCfg);	
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
static void sGetInputVideoColorInfo(VideoPathParam_t *pVidPathParam, VideoColorInfo_t* pVidClrInfo)
{
	//VideoPathParam_t *pVidPathParam = (VideoPathParam_t *) SII_LIB_OBJ_PNTR(inst);

	pVidClrInfo->clrSpc = pVidPathParam->inClrInfo.clrSpc;
	pVidClrInfo->convStd = pVidPathParam->inClrInfo.convStd;
	pVidClrInfo->vidDcDepth = pVidPathParam->inClrInfo.vidDcDepth;
	pVidClrInfo->quntization = pVidPathParam->inClrInfo.quntization;
}

//-----------------------------------------------------------------------------
static void sSetVideoPathdefaults(VideoPathParam_t *pVidPathParam)
{
    uint16_t vp__input_format;
    uint16_t vp__output_format;
	uint8_t vp__cms__csc0__c420_c422_config;
	uint8_t vp__cms__csc0__c422_c444_config;
	uint8_t vp__cms__csc1__c444_c422_config;
	uint8_t vp__cms__csc1__c422_c420_config;
	uint16_t vp__cms__csc0__multi_csc_config;
	uint16_t vp__cms__csc1__multi_csc_config;
    uint8_t vp__cms__csc1__dither_config;

	//SELECT_VIDEO_PATH_PAGE(PAGE_CORE);

	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__INPUT_FORMAT, (uint8_t *) &vp__input_format, BIT_MASK__VP__INPUT_FORMAT__SIZE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__OUTPUT_FORMAT,(uint8_t *) &vp__output_format, BIT_MASK__VP__OUTPUT_FORMAT__SIZE);

	vp__input_format &= ~BIT_MSK__VP__INPUT_FORMAT__MUX_420_ENABLE;
	vp__output_format &= ~BIT_MSK__VP__OUTPUT_FORMAT__DEMUX_420_ENABLE;
    //disable 4:2:0 Y mux
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__INPUT_FORMAT, (uint8_t *) &vp__input_format, BIT_MASK__VP__INPUT_FORMAT__SIZE);
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__OUTPUT_FORMAT, (uint8_t *) &vp__output_format, BIT_MASK__VP__OUTPUT_FORMAT__SIZE);

	//SELECT_VIDEO_PATH_PAGE(PAGE_CSC);

    //disable chroma vertical upsampler,set bypass
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__C420_C422_CONFIG, (uint8_t *) &vp__cms__csc0__c420_c422_config, BIT_MASK__VP__CMS_CSC0_420_422_CONFIG__SIZE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__C422_C444_CONFIG, (uint8_t *) &vp__cms__csc0__c422_c444_config, BIT_MASK__VP__CMS_CSC0_422_444_CONFIG__SIZE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__C444_C422_CONFIG, (uint8_t *) &vp__cms__csc1__c444_c422_config, BIT_MASK__VP__CMS_CSC1_444_422_CONFIG__SIZE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__C422_C420_CONFIG, (uint8_t *) &vp__cms__csc1__c422_c420_config, BIT_MASK__VP__CMS_CSC1_422_420_CONFIG__SIZE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__MULTI_CSC_CONFIG, (uint8_t *) &vp__cms__csc0__multi_csc_config, BIT_MASK__VP__CMS_CSC0_MULTI_CSC_CONFIG__SIZE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__MULTI_CSC_CONFIG, (uint8_t *) &vp__cms__csc1__multi_csc_config, BIT_MASK__VP__CMS_CSC1_MULTI_CSC_CONFIG__SIZE);
    //dither config
    SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__DITHER_CONFIG, (uint8_t *)&vp__cms__csc1__dither_config, BIT_MASK__VP__CMS__CSC1__DITHERING_CONFIG__SIZE);

    vp__cms__csc0__c420_c422_config |= BIT_MSK__VP__CMS__CSC0__C420_C422_CONFIG__BYPASS;
    vp__cms__csc0__c420_c422_config &= ~BIT_MSK__VP__CMS__CSC0__C420_C422_CONFIG__ENABLE;

    vp__cms__csc0__c422_c444_config &= ~BIT_MSK__VP__CMS__CSC0__C422_C444_CONFIG__ENABLE;

    vp__cms__csc1__c444_c422_config &= ~BIT_MSK__VP__CMS__CSC0__C444_C422_CONFIG__ENABLE;

    vp__cms__csc1__c422_c420_config |= BIT_MSK__VP__CMS__CSC1__C422_C420_CONFIG__BYPASS;
    vp__cms__csc1__c422_c420_config &= ~BIT_MSK__VP__CMS__CSC1__C422_C420_CONFIG__ENABLE;

    vp__cms__csc0__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC0__MULTI_CSC_CONFIG__ENABLE;

    vp__cms__csc1__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC1__MULTI_CSC_CONFIG__ENABLE;

    vp__cms__csc1__dither_config |= 0x03;


	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__C420_C422_CONFIG, (uint8_t *) &vp__cms__csc0__c420_c422_config, BIT_MASK__VP__CMS_CSC0_420_422_CONFIG__SIZE);
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__C422_C444_CONFIG, (uint8_t *) &vp__cms__csc0__c422_c444_config, BIT_MASK__VP__CMS_CSC0_422_444_CONFIG__SIZE);
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__C444_C422_CONFIG, (uint8_t *) &vp__cms__csc1__c444_c422_config, BIT_MASK__VP__CMS_CSC1_444_422_CONFIG__SIZE);
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__C422_C420_CONFIG, (uint8_t *) &vp__cms__csc1__c422_c420_config, BIT_MASK__VP__CMS_CSC1_422_420_CONFIG__SIZE);
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__MULTI_CSC_CONFIG, (uint8_t *) &vp__cms__csc0__multi_csc_config, BIT_MASK__VP__CMS_CSC0_MULTI_CSC_CONFIG__SIZE);
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__MULTI_CSC_CONFIG, (uint8_t *) &vp__cms__csc1__multi_csc_config, BIT_MASK__VP__CMS_CSC1_MULTI_CSC_CONFIG__SIZE);
 	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__DITHER_CONFIG, (uint8_t *)&vp__cms__csc1__dither_config, BIT_MASK__VP__CMS__CSC1__DITHERING_CONFIG__SIZE);

	SiiDrvCraWrReg8(pVidPathParam->instTxCra, REG_ADDR__CLKRATIO, (BIT_MSK__CLKRATIO__REG_CLKRATIO_SW_EN | BIT_MSK__CLKRATIO__REG_PCLKNX2PCLK_RATIO_DEFAULT | BIT_MSK__CLKRATIO__REG_IDCK2PCLK_RATIO_DEFAULT)); // setting default TMDS clock frequency values at the output....jagan
}

//-----------------------------------------------------------------------------
static void sDecodeColorInfo(VideoPathParam_t *pVidPathParam, SiiDrvTxClrSpc_t clrSpc, VideoColorInfo_t* pClrInfo)
{
	VideoColorInfo_t inClrInfo;
	

	sGetInputVideoColorInfo(pVidPathParam, &inClrInfo);

	switch(clrSpc)
	{
	case SII_DRV_TX_CLRSPC__PASSTHRU:
		pClrInfo->convStd = inClrInfo.convStd;
		pClrInfo->clrSpc = inClrInfo.clrSpc;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__RGB_FULL:
		pClrInfo->convStd = inClrInfo.convStd;
		pClrInfo->clrSpc = RGB;
		pClrInfo->quntization = QUANTIZATION_PC_LEVELS;//QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__RGB_LIMITED:
		pClrInfo->convStd = inClrInfo.convStd;
		pClrInfo->clrSpc = RGB;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__XVYCC420_601:
		pClrInfo->convStd = SII_DRV_TX_CONV_STD__BT_601;
		pClrInfo->clrSpc = YCbCr420;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__XVYCC420_709:
		pClrInfo->convStd = SII_DRV_TX_CONV_STD__BT_709;
		pClrInfo->clrSpc = YCbCr420;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__XVYCC422_601:
		pClrInfo->convStd = SII_DRV_TX_CONV_STD__BT_601;
		pClrInfo->clrSpc = YCbCr422;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__XVYCC422_709:
		pClrInfo->convStd = SII_DRV_TX_CONV_STD__BT_709;
		pClrInfo->clrSpc = YCbCr422;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__XVYCC444_601:
		pClrInfo->convStd = SII_DRV_TX_CONV_STD__BT_601;
		pClrInfo->clrSpc = YCbCr444;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__XVYCC444_709:
		pClrInfo->convStd = SII_DRV_TX_CONV_STD__BT_709;
		pClrInfo->clrSpc = YCbCr444;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__YC420_2020:
		pClrInfo->convStd = SII_DRV_TX_CONV_STD__BT_2020_const_luminous;
		pClrInfo->clrSpc = YCbCr420;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__YC420_601:
		pClrInfo->convStd = SII_DRV_TX_CONV_STD__BT_601;
		pClrInfo->clrSpc = YCbCr420;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__YC420_709:
		pClrInfo->convStd = SII_DRV_TX_CONV_STD__BT_709;
		pClrInfo->clrSpc = YCbCr420;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__YC422_2020:
		pClrInfo->convStd = SII_DRV_TX_CONV_STD__BT_2020_const_luminous;
		pClrInfo->clrSpc = YCbCr422;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__YC422_601:
		pClrInfo->convStd = SII_DRV_TX_CONV_STD__BT_601;
		pClrInfo->clrSpc = YCbCr422;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__YC422_709:
		pClrInfo->convStd = SII_DRV_TX_CONV_STD__BT_709;
		pClrInfo->clrSpc = YCbCr422;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__YC444_2020:
		pClrInfo->convStd = SII_DRV_TX_CONV_STD__BT_2020_const_luminous;
		pClrInfo->clrSpc = YCbCr444;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__YC444_601:
		pClrInfo->convStd = SII_DRV_TX_CONV_STD__BT_601;
		pClrInfo->clrSpc = YCbCr444;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	case SII_DRV_TX_CLRSPC__YC444_709:
		pClrInfo->convStd = SII_DRV_TX_CONV_STD__BT_709;
		pClrInfo->clrSpc = YCbCr444;
		pClrInfo->quntization = QUANTIZATION_VIDEO_LEVELS;
		break;
	}
}

//-----------------------------------------------------------------------------
static void sVideoPathCoreConfig(VideoPathParam_t *pVidPathParam, VideoPathConfig_t* pCfg)
{
	DataPathMuxCfg_t       dpmCfg;
	RateConvertCfg_t       rcCfg;
	SyncPolAdjustCfg_t     spaCfg = {0};
	EmbeddeSyncCfg_t       esdCfg;
	DeGenCfg_t             degCfg = {0};
	CmsCfg_t               cmsCfg;
	VideoBalnkCfg_t        vidBalnkCfg;
	EmbeddeSyncCfg_t       eseCfg;

    //no use
	if(pCfg->bEnable_IDPMux)
	{
		//set configuration params
		sInputDataPathMuxConfig(pVidPathParam, &dpmCfg);
	}

    //use when input color space is YCbCr420
    if(pCfg->bEnable_IRC)
	{
		//set configuration params
		rcCfg.mux_420_enable = 1;
		sInputRateConverterConfig(pVidPathParam, &rcCfg);
	}
    
    //default use
	if(pCfg->bEnable_InSPA)
	{
		spaCfg.bAutoAdjust = 1;
		sInSyncPolarityAdjustmentConfig(pVidPathParam, &spaCfg);
	}
    
    // no use
	if(pCfg->bEnable_656D)
	{
		//set configuration params
		s656DecderConfig(pVidPathParam, &esdCfg);
	}
    
    // no use
	if(pCfg->bEnable_DEG)
	{
		//set configuration params
		sDEGeneratorConfig(pVidPathParam, &degCfg);
	}
    
	if(pCfg->bEnable_CMS)
	{
	 #if 0
		if(pCfg->inClrInfo.clrSpc == YCbCr420)
		{
			cmsCfg.bEnable_420422 = 1;
			cmsCfg.bEnable_422444 = 1;
		}
		else if(pCfg->inClrInfo.clrSpc == YCbCr422)
		{
			cmsCfg.bEnable_420422 = 0;
			cmsCfg.bEnable_422444 = 1;
		}
		else
		{
			cmsCfg.bEnable_420422 = 0;
			cmsCfg.bEnable_422444 = 0;
		}
		cmsCfg.bEnable_csc0 = 1;
		cmsCfg.bIsGamaCorrEnable = 0;
		cmsCfg.bEnable_csc1 = 1;
		if(pCfg->outClrInfo.clrSpc == YCbCr420)
		{
			cmsCfg.bEnable_444422 = 1;
			cmsCfg.bEnable_422420 = 1;
		}
		else if(pCfg->outClrInfo.clrSpc == YCbCr422)
		{
			cmsCfg.bEnable_444422 = 1;
			cmsCfg.bEnable_422420 = 0;
		}
		else
		{
			cmsCfg.bEnable_444422 = 0;
			cmsCfg.bEnable_422420 = 0;
		}
    #else
		cmsCfg.bEnable_420422 = 0;
		cmsCfg.bEnable_422444 = 0;
		cmsCfg.bEnable_444422 = 0;
		cmsCfg.bEnable_422420 = 0;        
		cmsCfg.bEnable_csc0 = 0;
		cmsCfg.bEnable_csc1 = 0; 
		cmsCfg.bIsGamaCorrEnable = 0;
	    if(pCfg->inClrInfo.clrSpc == YCbCr444)
		{
		    switch (pCfg->outClrInfo.clrSpc)
            {
                case YCbCr444:                  
                    break;
                case YCbCr420:
                    cmsCfg.bEnable_422420 = 1;  
                case YCbCr422:
          			cmsCfg.bEnable_444422 = 1;         			                  
                    break; 
                case RGB :
                    cmsCfg.bEnable_csc0 = 1;
                    break; 
            }  
		}
		else if(pCfg->inClrInfo.clrSpc == YCbCr420)
		{
		    switch (pCfg->outClrInfo.clrSpc)
            {
                case YCbCr420:               
                    break;
                case RGB :
                    cmsCfg.bEnable_csc0 = 1;    
                case YCbCr444:
    			    cmsCfg.bEnable_422444 = 1;                                     
                case YCbCr422:
          			cmsCfg.bEnable_420422 = 1;                     
                    break;      
            }      
		}        
		else if(pCfg->inClrInfo.clrSpc == YCbCr422)
		{
		    switch (pCfg->outClrInfo.clrSpc)
            {
                case YCbCr422:                  
                    break;
                case YCbCr420:
                    cmsCfg.bEnable_422420 = 1;
                    break;
                case RGB :
                    cmsCfg.bEnable_csc0 = 1;                   
                case YCbCr444:
          			cmsCfg.bEnable_422444 = 1;         			                  
                    break;                    
            }  
		}   
        else if (pCfg->inClrInfo.clrSpc == RGB)
        {
		    switch (pCfg->outClrInfo.clrSpc)
            {
                case RGB:                  
                    break;
                case YCbCr420:
                    cmsCfg.bEnable_422420 = 1;
                case YCbCr422 :
                    cmsCfg.bEnable_444422 = 1;                   
                case YCbCr444:
          			cmsCfg.bEnable_csc1 = 1;          			                  
                    break;                    
            }  
        }
   
          
    #endif
		cmsCfg.outClrInfo = pCfg->outClrInfo;
		cmsCfg.inClrInfo =  pCfg->inClrInfo;

		if((pCfg->bEnable_Dithering) || (pCfg->outClrInfo.vidDcDepth != pCfg->inClrInfo.vidDcDepth))
		{
			cmsCfg.bEnable_Dithering = pCfg->bEnable_Dithering;
		}
		else
		{
			cmsCfg.bEnable_Dithering = 0;
		}

		sVideoPathCmsConfig(pVidPathParam, &cmsCfg);
	}
    
    // no use
	if(pCfg->bEnable_VB)
	{
		//set configuration params
		sVideoBlankingConfig(pVidPathParam, &vidBalnkCfg);
	}

    //use when output color space is YCbCr420
	if(pCfg->bEnable_ORC)
	{
		//set configuration params
		rcCfg.mux_420_enable = 1;
		sOutputRateConverterConfig(pVidPathParam, &rcCfg);
	}

    // no use
	if(pCfg->bEnable_ESE)
	{
		//set configuration params
		sEmbeddedSyncEncoderConfig(pVidPathParam, &eseCfg);
	}
    
    //default use
	if(pCfg->bEnable_OutSPA)
	{
		spaCfg.bAutoAdjust = 0;
		switch(pCfg->outputHVSyncPol)
		{
		case SII_HV_SYNC_POL__HPVP:
			spaCfg.HSYNC_Pol = SYNCPOL_POSITIVE;
			spaCfg.VSYNC_Pol = SYNCPOL_POSITIVE;
			break;
		case SII_HV_SYNC_POL__HPVN:
			spaCfg.HSYNC_Pol = SYNCPOL_POSITIVE;
			spaCfg.VSYNC_Pol = SYNCPOL_NEGATIVE;
			break;
		case SII_HV_SYNC_POL__HNVP:
			spaCfg.HSYNC_Pol = SYNCPOL_NEGATIVE;
			spaCfg.VSYNC_Pol = SYNCPOL_POSITIVE;
			break;
		case SII_HV_SYNC_POL__HNVN:
			spaCfg.HSYNC_Pol = SYNCPOL_NEGATIVE;
			spaCfg.VSYNC_Pol = SYNCPOL_NEGATIVE;
			break;
		}
		sOutSyncPolarityAdjustmentConfig(pVidPathParam, &spaCfg);
	}
    
    //no use
	if(pCfg->bEnable_ODPMux)
	{
		//set configuration params
		sOutnputDataPathMuxConfig(pVidPathParam, &dpmCfg);
	}
}

//-----------------------------------------------------------------------------
static void sVideoPathCmsConfig(VideoPathParam_t *pVidPathParam, CmsCfg_t* pCfg)
{
	SamplerCfg_t upSamplerCfg = {0};
	SamplerCfg_t downSamplerCfg = {0};
	GammaCorrectionConfig_t rCfg = {0};
	CscConfig_t csc0Cfg = {0};
	CscConfig_t csc1Cfg = {0};
    SiiDitherCfg_t dithCfg = {0};

	//SELECT_VIDEO_PATH_PAGE(PAGE_CMS);
    
	if(pCfg->bEnable_420422 || pCfg->bEnable_422444)
	{
		/**
		* Conversion from 420->422/422->444
		*/
		upSamplerCfg.inputClrSpc = pCfg->inClrInfo.clrSpc;
		//upSamplerCfg.outputClrSpc = YCbCr444;
		upSamplerCfg.outputClrSpc = pCfg->bEnable_csc0 ? YCbCr444 : pCfg->outClrInfo.clrSpc;
		sUpSamplerConfig(pVidPathParam, &upSamplerCfg);
        SII_LIB_LOG_DEBUG1(pVidPathParam, "upsampler enable\n");
	}
    
    //YUV->RGB
	if(pCfg->bEnable_csc0 && (pCfg->inClrInfo.clrSpc != RGB))
	{
		if(!pCfg->bIsGamaCorrEnable || (pCfg->bIsGamaCorrEnable && (pCfg->inClrInfo.clrSpc != RGB)))
		{
			/**
			* Conversion from YCbCr444->RGB
			*/
			csc0Cfg.inConvStd = pCfg->inClrInfo.convStd;
			csc0Cfg.inputClrSpc = pCfg->inClrInfo.clrSpc;
			csc0Cfg.outputClrSpc = RGB;
			csc0Cfg.inQuantization = pCfg->inClrInfo.quntization;
            csc0Cfg.outQuantization = pCfg->outClrInfo.quntization;
       
			if(pCfg->outClrInfo.clrSpc != RGB)
				pCfg->bEnable_csc1 = 1;
			else
				pCfg->bEnable_csc1 = 0;
      
			if(pCfg->bEnable_csc1)
			{
				csc0Cfg.outQuantization = pCfg->inClrInfo.quntization;
			}
	 	
			csc0Cfg.outConvStd = pCfg->bEnable_csc1 ? pCfg->inClrInfo.convStd: pCfg->outClrInfo.convStd;

			sVideoPathCsc0Config(pVidPathParam, &csc0Cfg);
            //SII_LIB_LOG_DEBUG1(pVidPathParam, "enable YUV->RGB\n");
		}
	}

	if(pCfg->bIsGamaCorrEnable)
	{
		/**
		* Gamma Correction
		*/
		sGammaCorrectionConfig(pVidPathParam, &rCfg);
	}
    
    //RGB->YUV
	if(pCfg->bEnable_csc1)
	{
		/**
		* Conversion from RGB->YCBCR444
		*/
		csc1Cfg.inConvStd = pCfg->inClrInfo.convStd;
		csc1Cfg.outConvStd = pCfg->outClrInfo.convStd;
		csc1Cfg.inputClrSpc = (pCfg->bEnable_csc0) ? RGB : pCfg->inClrInfo.clrSpc;
		csc1Cfg.outputClrSpc = (pCfg->outClrInfo.clrSpc == RGB) ? RGB : YCbCr444;
		csc1Cfg.inQuantization = pCfg->inClrInfo.quntization;
		csc1Cfg.outQuantization = pCfg->outClrInfo.quntization;
      
		sVideoPathCsc1Config(pVidPathParam, &csc1Cfg);
        //SII_LIB_LOG_DEBUG1(pVidPathParam, "enable RGB->YUV\n");
	}

	if(pCfg->bEnable_444422 || pCfg->bEnable_422420)
	{
		/**
		* Conversion from 444->422/422->420
		*/
		//downSamplerCfg.inputClrSpc = YCbCr444;
		downSamplerCfg.inputClrSpc = (pCfg->bEnable_csc1) ? YCbCr444 : pCfg->inClrInfo.clrSpc;
		downSamplerCfg.outputClrSpc = pCfg->outClrInfo.clrSpc;
		sDownSamplerConfig(pVidPathParam, &downSamplerCfg);
        SII_LIB_LOG_DEBUG1(pVidPathParam, "downsampler enable\n");
	}	
#if 0
	if(pCfg->bEnable_Dithering)
	{
		/**
		*  Conversion from 12->10, 12->8, 10->8 bit
        *  Only the registers is CSC1 have effect in dithering
		*/
        switch(pCfg->inClrInfo.vidDcDepth)
        {
        case SII_DRV_BIT_DEPTH__12_BIT:
            switch(pCfg->outClrInfo.vidDcDepth)
            {
            case SII_DRV_BIT_DEPTH__10_BIT:
                dithCfg = DITHER__12_TO_10;
                break;
            case SII_DRV_BIT_DEPTH__8_BIT:
                dithCfg = DITHER__12_TO_8;
                break;
            default:
                dithCfg = DITHER__NOCHNG; // video path core does not support any other scenarios
                break;
            }
            break;
        case SII_DRV_BIT_DEPTH__10_BIT:      
            switch(pCfg->outClrInfo.vidDcDepth)
            {
            //here only 8 bit conversion is allowed
            case SII_DRV_BIT_DEPTH__8_BIT:
                dithCfg = DITHER__10_TO_8;
                break;
            default:
                dithCfg = DITHER__NOCHNG; // video path core does not support any other scenarios
                break;
            }
            break;
        default:
           dithCfg = DITHER__NOCHNG; // upscaling is not available
           break;
       }
	}	
    else
    {
        switch(pCfg->inClrInfo.vidDcDepth)
        {
        case SII_DRV_BIT_DEPTH__12_BIT:
            dithCfg = DITHER__12_TO_8; // video path core does not support any other scenarios
            break;
        case SII_DRV_BIT_DEPTH__10_BIT:
            dithCfg = DITHER__10_TO_8;
            break;
        default:
           dithCfg = DITHER__NOCHNG; // upscaling is not available
           break;
       }
    }
#else
    dithCfg = DITHER__NOCHNG;//disable dither
#endif
    sDitheringConfig(pVidPathParam, &dithCfg);
}

//-----------------------------------------------------------------------------
static void sUpSamplerConfig(VideoPathParam_t *pVidPathParam, SamplerCfg_t* pCfg)
{
	uint8_t   vp__cms__csc0__c420_c422_config;
	uint8_t   vp__cms__csc0__c422_c444_config;

	//SELECT_VIDEO_PATH_PAGE(PAGE_CSC);

	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__C420_C422_CONFIG, (uint8_t *) &vp__cms__csc0__c420_c422_config, BIT_MASK__VP__CMS_CSC0_420_422_CONFIG__SIZE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__C422_C444_CONFIG, (uint8_t *) &vp__cms__csc0__c422_c444_config, BIT_MASK__VP__CMS_CSC0_422_444_CONFIG__SIZE);

	if(pCfg->inputClrSpc == YCbCr420)
	{
		switch(pCfg->outputClrSpc)
		{
		case YCbCr444:
            vp__cms__csc0__c420_c422_config &= ~BIT_MSK__VP__CMS__CSC0__C420_C422_CONFIG__BYPASS;
            vp__cms__csc0__c420_c422_config |= BIT_MSK__VP__CMS__CSC0__C420_C422_CONFIG__ENABLE;
			//fall through
		case YCbCr422:
            vp__cms__csc0__c422_c444_config |= BIT_MSK__VP__CMS__CSC0__C422_C444_CONFIG__ENABLE;
            vp__cms__csc0__c422_c444_config |= BIT_MSK__VP__CMS__CSC0__C422_C444_CONFIG__USE_CB_OR_CR;
			break;
		default:
			break;
		}
	}
	if(pCfg->inputClrSpc == YCbCr422)
	{
		switch(pCfg->outputClrSpc)
		{
		case YCbCr444:
            vp__cms__csc0__c422_c444_config |= BIT_MSK__VP__CMS__CSC0__C422_C444_CONFIG__ENABLE;
            vp__cms__csc0__c422_c444_config |= BIT_MSK__VP__CMS__CSC0__C422_C444_CONFIG__USE_CB_OR_CR;
			break;
		default:
			break;
		}
	}
	
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__C420_C422_CONFIG, (uint8_t *) &vp__cms__csc0__c420_c422_config, BIT_MASK__VP__CMS_CSC0_420_422_CONFIG__SIZE);
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__C422_C444_CONFIG, (uint8_t *) &vp__cms__csc0__c422_c444_config, BIT_MASK__VP__CMS_CSC0_422_444_CONFIG__SIZE);
}

//-----------------------------------------------------------------------------
static void sDownSamplerConfig(VideoPathParam_t *pVidPathParam, SamplerCfg_t* pCfg)
{
	uint8_t   vp__cms__csc1__c444_c422_config;
	uint8_t   vp__cms__csc1__c422_c420_config;
	//SELECT_VIDEO_PATH_PAGE(PAGE_CSC);

	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__C444_C422_CONFIG, (uint8_t *) &vp__cms__csc1__c444_c422_config, BIT_MASK__VP__CMS_CSC1_444_422_CONFIG__SIZE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__C422_C420_CONFIG, (uint8_t *) &vp__cms__csc1__c422_c420_config, BIT_MASK__VP__CMS_CSC1_422_420_CONFIG__SIZE);

	if(pCfg->inputClrSpc == YCbCr444)
	{
		switch(pCfg->outputClrSpc)
		{
		case YCbCr420:
            vp__cms__csc1__c422_c420_config |= BIT_MSK__VP__CMS__CSC1__C422_C420_CONFIG__ENABLE;
            vp__cms__csc1__c422_c420_config &= ~BIT_MSK__VP__CMS__CSC1__C422_C420_CONFIG__BYPASS;
			SiiDrvCraWrReg8(pVidPathParam->instTxCra, REG_ADDR__CLKRATIO, (BIT_MSK__CLKRATIO__REG_CLKRATIO_SW_EN | BIT_MSK__CLKRATIO__REG_IDCK2PCLK_RATIO_DEFAULT)); // setting TMDS clock frequency at the output....jagan
			//fall through
		case YCbCr422:
            vp__cms__csc1__c444_c422_config |= BIT_MSK__VP__CMS__CSC1__C444_C422_CONFIG__ENABLE;
			break;
		default:
			break;
		}
	}
	if(pCfg->inputClrSpc == YCbCr422)
	{
		switch(pCfg->outputClrSpc)
		{
		case YCbCr420://change bug YCbCr444:
            vp__cms__csc1__c422_c420_config |= BIT_MSK__VP__CMS__CSC1__C422_C420_CONFIG__ENABLE;
            vp__cms__csc1__c422_c420_config &= ~BIT_MSK__VP__CMS__CSC1__C422_C420_CONFIG__BYPASS;
			break;
		default:
			break;
		}
	}

	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__C444_C422_CONFIG, (uint8_t *) &vp__cms__csc1__c444_c422_config, BIT_MASK__VP__CMS_CSC1_444_422_CONFIG__SIZE);
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__C422_C420_CONFIG, (uint8_t *) &vp__cms__csc1__c422_c420_config, BIT_MASK__VP__CMS_CSC1_422_420_CONFIG__SIZE);
}

//-----------------------------------------------------------------------------
static void sVideoPathCsc0Config(VideoPathParam_t *pVidPathParam, CscConfig_t* pCfg)
{
	uint16_t  vp__cms__csc0__multi_csc_config;
	uint8_t   vp__cms__csc0__c444_c422_config;
	uint8_t   vp__cms__csc0__c422_c420_config;

 
	//SELECT_VIDEO_PATH_PAGE(PAGE_CSC);


	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__MULTI_CSC_CONFIG, (uint8_t *)&vp__cms__csc0__multi_csc_config, BIT_MASK__VP__CMS_CSC0_MULTI_CSC_CONFIG__SIZE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__C444_C422_CONFIG, (uint8_t *)&vp__cms__csc0__c444_c422_config, BIT_MASK__VP__CMS_CSC0_444_422_CONFIG__SIZE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__C422_C420_CONFIG, (uint8_t *)&vp__cms__csc0__c422_c420_config, BIT_MASK__VP__CMS_CSC0_422_420_CONFIG__SIZE);
	
	//-----------configure csc0----------------
	/**
	* In CSC0 we convert to RGB irrespective of input as gamma correction requires RGB input
	*/
	vp__cms__csc0__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC0__MULTI_CSC_CONFIG__IN_STD;
	vp__cms__csc0__multi_csc_config |= (pCfg->inConvStd << 6);
	vp__cms__csc0__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC0__MULTI_CSC_CONFIG__IN_RGB;
	vp__cms__csc0__multi_csc_config |= (pCfg->inputClrSpc == RGB)? BIT_MSK__VP__CMS__CSC0__MULTI_CSC_CONFIG__IN_RGB:0;
	vp__cms__csc0__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC0__MULTI_CSC_CONFIG__IN_PC;
    if(pCfg->inQuantization == QUANTIZATION_PC_LEVELS)
        vp__cms__csc0__multi_csc_config |= BIT_MSK__VP__CMS__CSC0__MULTI_CSC_CONFIG__IN_PC;

	vp__cms__csc0__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC0__MULTI_CSC_CONFIG__OUT_STD;
	vp__cms__csc0__multi_csc_config |= (pCfg->outConvStd << 2);
	vp__cms__csc0__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC0__MULTI_CSC_CONFIG__OUT_RGB;
	vp__cms__csc0__multi_csc_config |= (pCfg->outputClrSpc == RGB)? BIT_MSK__VP__CMS__CSC0__MULTI_CSC_CONFIG__OUT_RGB:0;
	vp__cms__csc0__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC0__MULTI_CSC_CONFIG__OUT_PC;
    if(pCfg->outQuantization == QUANTIZATION_PC_LEVELS)
        vp__cms__csc0__multi_csc_config |= BIT_MSK__VP__CMS__CSC0__MULTI_CSC_CONFIG__OUT_PC;

	vp__cms__csc0__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC0__MULTI_CSC_CONFIG__DITHER_ENABLE;

	vp__cms__csc0__multi_csc_config |= (BIT_MSK__VP__CMS__CSC0__MULTI_CSC_CONFIG__ENABLE & 0x0001);

	vp__cms__csc0__c444_c422_config &= ~BIT_MSK__VP__CMS__CSC0__C444_C422_CONFIG__ENABLE;
    vp__cms__csc0__c422_c420_config |= BIT_MSK__VP__CMS__CSC0__C422_C420_CONFIG__BYPASS;
    vp__cms__csc0__c422_c420_config &= ~BIT_MSK__VP__CMS__CSC0__C422_C420_CONFIG__ENABLE;

	
	//--------------------------------------------------------
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__MULTI_CSC_CONFIG, (uint8_t *)&vp__cms__csc0__multi_csc_config, BIT_MASK__VP__CMS_CSC0_MULTI_CSC_CONFIG__SIZE);
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__C444_C422_CONFIG, (uint8_t *)&vp__cms__csc0__c444_c422_config, BIT_MASK__VP__CMS_CSC0_444_422_CONFIG__SIZE);
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__C422_C420_CONFIG, (uint8_t *)&vp__cms__csc0__c422_c420_config, BIT_MASK__VP__CMS_CSC0_422_420_CONFIG__SIZE);
}

//-----------------------------------------------------------------------------
static void sVideoPathCsc1Config(VideoPathParam_t *pVidPathParam, CscConfig_t* pCfg)
{
	uint16_t   vp__cms__csc1__multi_csc_config;
	uint8_t   vp__cms__csc1__c420_c422_config;
	uint8_t   vp__cms__csc1__c422_c444_config;


	//SELECT_VIDEO_PATH_PAGE(PAGE_CSC);

	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__MULTI_CSC_CONFIG, (uint8_t *) &vp__cms__csc1__multi_csc_config, BIT_MASK__VP__CMS_CSC1_MULTI_CSC_CONFIG__SIZE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__C420_C422_CONFIG, (uint8_t *) &vp__cms__csc1__c420_c422_config, BIT_MASK__VP__CMS_CSC1_420_422_CONFIG__SIZE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__C422_C444_CONFIG, (uint8_t *) &vp__cms__csc1__c422_c444_config, BIT_MASK__VP__CMS_CSC1_422_444_CONFIG__SIZE);

	//-----------configure cs1----------------
	/**
	* Input to this block is RGB
	*/
	vp__cms__csc1__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC1__MULTI_CSC_CONFIG__IN_STD;
	vp__cms__csc1__multi_csc_config |= (pCfg->inConvStd << 6);
	vp__cms__csc1__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC1__MULTI_CSC_CONFIG__IN_RGB;
	vp__cms__csc1__multi_csc_config |= (pCfg->inputClrSpc == RGB)? BIT_MSK__VP__CMS__CSC1__MULTI_CSC_CONFIG__IN_RGB:0;
	vp__cms__csc1__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC1__MULTI_CSC_CONFIG__IN_PC;
    if(pCfg->inQuantization == QUANTIZATION_PC_LEVELS)
        vp__cms__csc1__multi_csc_config |= BIT_MSK__VP__CMS__CSC1__MULTI_CSC_CONFIG__IN_PC;

	vp__cms__csc1__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC1__MULTI_CSC_CONFIG__OUT_STD;
	vp__cms__csc1__multi_csc_config |= (pCfg->outConvStd << 2);
	vp__cms__csc1__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC1__MULTI_CSC_CONFIG__OUT_RGB;
	vp__cms__csc1__multi_csc_config |= (pCfg->outputClrSpc == RGB)? BIT_MSK__VP__CMS__CSC1__MULTI_CSC_CONFIG__OUT_RGB:0;
	vp__cms__csc1__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC1__MULTI_CSC_CONFIG__OUT_PC;
	vp__cms__csc1__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC1__MULTI_CSC_CONFIG__OUT_PC;
    if(pCfg->outQuantization == QUANTIZATION_PC_LEVELS)
        vp__cms__csc1__multi_csc_config |= BIT_MSK__VP__CMS__CSC1__MULTI_CSC_CONFIG__OUT_PC;

	vp__cms__csc1__multi_csc_config &= ~BIT_MSK__VP__CMS__CSC1__MULTI_CSC_CONFIG__DITHER_ENABLE;

	vp__cms__csc1__multi_csc_config |= (BIT_MSK__VP__CMS__CSC1__MULTI_CSC_CONFIG__ENABLE & 0x0001);

	vp__cms__csc1__c420_c422_config &= ~BIT_MSK__VP__CMS__CSC1__C420_C422_CONFIG__ENABLE;
    vp__cms__csc1__c420_c422_config |= BIT_MSK__VP__CMS__CSC1__C420_C422_CONFIG__BYPASS;
    vp__cms__csc1__c422_c444_config &= ~BIT_MSK__VP__CMS__CSC1__C422_C444_CONFIG__ENABLE;

	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__MULTI_CSC_CONFIG, (uint8_t *) &vp__cms__csc1__multi_csc_config, BIT_MASK__VP__CMS_CSC1_MULTI_CSC_CONFIG__SIZE);
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__C420_C422_CONFIG, (uint8_t *) &vp__cms__csc1__c420_c422_config, BIT_MASK__VP__CMS_CSC1_420_422_CONFIG__SIZE);
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__C422_C444_CONFIG, (uint8_t *) &vp__cms__csc1__c422_c444_config, BIT_MASK__VP__CMS_CSC1_422_444_CONFIG__SIZE);
}

//-----------------------------------------------------------------------------
static void sInputDataPathMuxConfig(VideoPathParam_t *pVidPathParam, DataPathMuxCfg_t* pinMap)
{

}

//-----------------------------------------------------------------------------
static void sInputRateConverterConfig(VideoPathParam_t *pVidPathParam, RateConvertCfg_t* pCfg)
{
	uint16_t vp_input_format;

	//SELECT_VIDEO_PATH_PAGE(PAGE_CORE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__INPUT_FORMAT, (uint8_t *)&vp_input_format, BIT_MASK__VP__INPUT_FORMAT__SIZE);

    vp_input_format &= ~BIT_MSK__VP__INPUT_FORMAT__MUX_420_ENABLE;
	vp_input_format |= pCfg->mux_420_enable? BIT_MSK__VP__INPUT_FORMAT__MUX_420_ENABLE:0;

	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__INPUT_FORMAT,(uint8_t *) &vp_input_format, BIT_MASK__VP__INPUT_FORMAT__SIZE);
}

//-----------------------------------------------------------------------------
static void sInSyncPolarityAdjustmentConfig(VideoPathParam_t *pVidPathParam, SyncPolAdjustCfg_t *pCfg)
{
	uint8_t vp__input_sync_adjust_config;

	//SELECT_VIDEO_PATH_PAGE(PAGE_CORE);

	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__INPUT_SYNC_ADJUST_CONFIG, (uint8_t *)&vp__input_sync_adjust_config, BIT_MASK__VP__INPUT_SYNC_ADJUST_CONFIG__SIZE);

    vp__input_sync_adjust_config &= ~BIT_MSK__VP__INPUT_SYNC_ADJUST_CONFIG__AUTO_DISABLE;
    vp__input_sync_adjust_config |= pCfg->bAutoAdjust? 0 : BIT_MSK__VP__INPUT_SYNC_ADJUST_CONFIG__AUTO_DISABLE;
     
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__INPUT_SYNC_ADJUST_CONFIG, (uint8_t *)&vp__input_sync_adjust_config, BIT_MASK__VP__INPUT_SYNC_ADJUST_CONFIG__SIZE);
}

static void s656DecderConfig(VideoPathParam_t *pVidPathParam, EmbeddeSyncCfg_t* pCfg)
{
}

static void sDEGeneratorConfig(VideoPathParam_t *pVidPathParam, DeGenCfg_t* pCfg)
{
	uint8_t       vp__degen_config;
	uint16_t      vp__degen_pixel_delay;
	uint16_t      vp__degen_line_delay;

	//SELECT_VIDEO_PATH_PAGE(PAGE_CORE);

	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__DEGEN_CONFIG, (uint8_t *)&vp__degen_config, BIT_MASK__VP__DEGEN_CONFIG__SIZE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__DEGEN_PIXEL_DELAY, (uint8_t *)&vp__degen_pixel_delay, BIT_MASK__VP__DEGEN_PIXEL_DELAY__SIZE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__DEGEN_LINE_DELAY, (uint8_t *)&vp__degen_line_delay, BIT_MASK__VP__DEGEN_LINE_DELAY__SIZE);

    vp__degen_config &= ~BIT_MSK__VP__DEGEN_CONFIG__ENABLE;
    vp__degen_config |= pCfg->enable? BIT_MSK__VP__DEGEN_CONFIG__ENABLE:0;

	vp__degen_pixel_delay = pCfg->pixel_delay;
	vp__degen_line_delay = pCfg->line_delay;

	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__DEGEN_CONFIG, (uint8_t *)&vp__degen_config, BIT_MASK__VP__DEGEN_CONFIG__SIZE);
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__DEGEN_PIXEL_DELAY, (uint8_t *)&vp__degen_pixel_delay, BIT_MASK__VP__DEGEN_PIXEL_DELAY__SIZE);
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__DEGEN_LINE_DELAY, (uint8_t *)&vp__degen_line_delay, BIT_MASK__VP__DEGEN_LINE_DELAY__SIZE);
}

static void sGammaCorrectionConfig(VideoPathParam_t *pVidPathParam, GammaCorrectionConfig_t* pCfg)
{
}


static void sVideoBlankingConfig(VideoPathParam_t *pVidPathParam, VideoBalnkCfg_t* pCfg)
{
}

static void sOutputRateConverterConfig(VideoPathParam_t *pVidPathParam, RateConvertCfg_t* pCfg)
{
	uint16_t vp__output_format;

	//SELECT_VIDEO_PATH_PAGE(PAGE_CORE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__OUTPUT_FORMAT, (uint8_t *)&vp__output_format, BIT_MASK__VP__OUTPUT_FORMAT__SIZE);

    vp__output_format &= ~BIT_MSK__VP__OUTPUT_FORMAT__DEMUX_420_ENABLE;
    vp__output_format |= pCfg->mux_420_enable ? BIT_MSK__VP__OUTPUT_FORMAT__DEMUX_420_ENABLE : 0;

	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__OUTPUT_FORMAT,(uint8_t *) &vp__output_format, BIT_MASK__VP__OUTPUT_FORMAT__SIZE);
}

static void sEmbeddedSyncEncoderConfig(VideoPathParam_t *pVidPathParam, EmbeddeSyncCfg_t* pCfg)
{
}

//set hsync ,vsync polarity
static void sOutSyncPolarityAdjustmentConfig(VideoPathParam_t *pVidPathParam, SyncPolAdjustCfg_t *pCfg)
{
	uint8_t vp__output_sync_config;
	//SELECT_VIDEO_PATH_PAGE(PAGE_CORE);
	
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__OUTPUT_SYNC_CONFIG, (uint8_t *)&vp__output_sync_config, BIT_MASK__VP__OUTPUT_SYNC_CONFIG__SIZE);
	vp__output_sync_config &= ~(BIT_MSK__VP__OUTPUT_SYNC_CONFIG__VSYNC_POLARITY | BIT_MSK__VP__OUTPUT_SYNC_CONFIG__HSYNC_POLARITY);
	vp__output_sync_config |= pCfg->VSYNC_Pol ? 0 : BIT_MSK__VP__OUTPUT_SYNC_CONFIG__VSYNC_POLARITY;
	vp__output_sync_config |= pCfg->HSYNC_Pol ? 0 : BIT_MSK__VP__OUTPUT_SYNC_CONFIG__HSYNC_POLARITY;

	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__OUTPUT_SYNC_CONFIG, &vp__output_sync_config, BIT_MASK__VP__OUTPUT_SYNC_CONFIG__SIZE);
}

static void sOutnputDataPathMuxConfig(VideoPathParam_t *pVidPathParam, DataPathMuxCfg_t* pinMap)
{
}

static void sOutputMute(VideoPathParam_t *pVidPathParam, bool_t enMute)
{
	uint8_t vp__output_mute;

	//SELECT_VIDEO_PATH_PAGE(PAGE_CORE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__OUTPUT_MUTE, (uint8_t *)&vp__output_mute, BIT_MASK__VP__OUTPUT_MUTE__SIZE);

    vp__output_mute = enMute? 0xFF:0x00;

	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__OUTPUT_MUTE, (uint8_t *)&vp__output_mute, BIT_MASK__VP__OUTPUT_MUTE__SIZE);
}

static void sDitheringConfig(VideoPathParam_t *pVidPathParam, SiiDitherCfg_t* dithConf)
{
    uint8_t vp__cms__csc1__dither_config;

    //SELECT_VIDEO_PATH_PAGE(PAGE_CSC);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__DITHER_CONFIG, (uint8_t *)&vp__cms__csc1__dither_config, BIT_MASK__VP__CMS__CSC1__DITHERING_CONFIG__SIZE);

    vp__cms__csc1__dither_config &= ~BIT_MSK__VP__CMS__CSC1__DITHER_CONFIG__MODE;

    switch(*dithConf)
    {
    case DITHER__12_TO_10:
        vp__cms__csc1__dither_config |= 0x00;
        break;
    case DITHER__12_TO_8:
        vp__cms__csc1__dither_config |= 0x01;
        break;
    case DITHER__10_TO_8:
        vp__cms__csc1__dither_config |= 0x02;
        break;
    case DITHER__NOCHNG:
         vp__cms__csc1__dither_config |= 0x03;
       break;
    }

	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__DITHER_CONFIG, (uint8_t *)&vp__cms__csc1__dither_config, BIT_MASK__VP__CMS__CSC1__DITHERING_CONFIG__SIZE);
}
#ifndef HDMI_BUILD_IN_BOOT
static void sUpdateHVSyncPol(VideoPathParam_t *pVidPathParam)
{
    uint8_t vp__fdet_status;

    //SELECT_VIDEO_PATH_PAGE(pVidPathParam->pConfig->instCra, PAGE_CORE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__FDET_STATUS, (uint8_t *)&vp__fdet_status, BIT_MASK__VP__FDET_STATUS__SIZE);

    if(vp__fdet_status & BIT_MSK__VP__FDET_STATUS__HSYNC_POLARITY)
    {
        pVidPathParam->hvSyncPol = (vp__fdet_status & BIT_MSK__VP__FDET_STATUS__VSYNC_POLARITY)? SII_HV_SYNC_POL__HPVP:SII_HV_SYNC_POL__HPVN;
    }
    else
    {
        pVidPathParam->hvSyncPol = (vp__fdet_status & BIT_MSK__VP__FDET_STATUS__VSYNC_POLARITY)? SII_HV_SYNC_POL__HNVP:SII_HV_SYNC_POL__HNVN;
    }
}
#endif

static void sSoftwareInit(VideoPathParam_t *pVidPathParam)
{
	pVidPathParam->inClrInfo.clrSpc = RGB;//SII_DRV_TX_CLRSPC__RGB_FULL;
	pVidPathParam->inClrInfo.convStd = SII_DRV_TX_CONV_STD__BT_601;
	pVidPathParam->inClrInfo.vidDcDepth = SII_DRV_BIT_DEPTH__10_BIT;
	pVidPathParam->inClrInfo.quntization = QUANTIZATION_PC_LEVELS;

	pVidPathParam->bColorConv_en = 0;
}

static void sHardwareInit(VideoPathParam_t *pVidPathParam)
{
    uint32_t				vp_fdet_irq_mask;
	VideoPathConfig_t		vidPathCfg;

	//Disabling Input datapath mux as it is required only if input tmsd lines are swapped
	vidPathCfg.bEnable_IDPMux = 0;
	vidPathCfg.bEnable_ODPMux = 0;

	//presently not using 656 decoder and embedded sync encoder
	vidPathCfg.bEnable_656D = 0;
	vidPathCfg.bEnable_ESE = 0;

	//automatic sync polairty adjustment
	vidPathCfg.bEnable_InSPA = 1;
	vidPathCfg.bEnable_OutSPA = 1;

	vidPathCfg.bEnable_DEG = 0;

	//video blanking shall be used when required
	vidPathCfg.bEnable_VB = 0;

	//temoprarily dsiabling below modules
	vidPathCfg.bEnable_ORC = 0;
	vidPathCfg.bEnable_IRC = 0;
	vidPathCfg.bEnable_CMS = 0;
	vidPathCfg.bEnable_Dithering = 0;
	
	vidPathCfg.instTxCra = pVidPathParam->instTxCra;
	sVideoPathCoreConfig(pVidPathParam, &vidPathCfg);
	//Unmask VideoPath Interrupt
	//vp_fdet_irq_mask = 0x07FFFF;
	//VideoPath Intr mask in enabled by default. Clear it to avoid group Intr.
	vp_fdet_irq_mask = 0x0;
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__FDET_IRQ_MASK, (uint8_t *) &vp_fdet_irq_mask, BIT_MASK__VP__FDET_IRQ_MASK__SIZE);

}

bool_t SiiModVidpathSet(SiiInst_t inst, SiiModVidPathInternalOpcode_t opcode, void *inData)
{
	VideoPathParam_t*      pVidPathParam = (VideoPathParam_t*)SII_LIB_OBJ_PNTR(inst);
	switch(opcode)
	{
	case SII_MOD_VIDPATH__OUTPUT_COLORSPACE:
		{
			SiiDrvTxClrSpc_t clrSpc = *(SiiDrvTxClrSpc_t *)inData;
			sVidPathOutputColorSpaceSet(pVidPathParam, clrSpc);
			break;
		}
	case SII_MOD_VIDPATH__COLOR_INFO_CONFIG:
		{
			SiiDrvTxColorInfoCfg_t *infoCfg = (SiiDrvTxColorInfoCfg_t *)inData;
			sVideoPathColorInfoConfig(pVidPathParam,infoCfg);
			break;
		}
	case SII_MOD_VIDPATH__BIT_DEPTH:
		{
			SiiDrvBitDepth_t bitDepth = *(SiiDrvBitDepth_t *)inData;
			sVidPathOutputBitDepthSet(pVidPathParam,bitDepth);
			break;
		}
	case SII_MOD_VIDPATH__HV_SYNC_POLARITY:
		{
			SiiHvSyncPol_t hvSyncPol = *(SiiHvSyncPol_t *)inData;
			sVidPathHvSyncPolaritySet(pVidPathParam,hvSyncPol);
			break;
		}
    case SII_MOD_VIDPATH__MUTE:
		{
			bool_t mute = *(bool_t *)inData;
			sOutputMute(pVidPathParam, mute);
			break;
		}
    case SII_MOD_VIDPATH__HW_INIT:
		{
			sHardwareInit(pVidPathParam);
			break;
		}
    case SII_MOD_VIDPATH__SW_INIT:
		{
			sSoftwareInit(pVidPathParam);
			break;
		}
    default:
        break;
	}
	return true;
}

//add by h00180450
static void sVideoHardwareStatusGet(SiiInst_t inst, SiiVideoStatus_t *videoStatus)
{
	VideoPathParam_t*      pVidPathParam = (VideoPathParam_t*)SII_LIB_OBJ_PNTR(inst);
	uint8_t vp__input_format 				= 0;
	uint8_t vp__output_format 				= 0;
	uint8_t vp__fdet_status 				= 0;
	//uint8_t vp__output_sync_config 			= 0;
	uint8_t vp__cms__csc1__dither_config 	= 0;
	uint8_t vp__cms__csc0__multi_csc_config	= 0;
	uint8_t vp__cms__csc1__multi_csc_config = 0;
	uint8_t vp__cms__csc0__c420_c422_config = 0;
	uint8_t vp__cms__csc0__c422_c444_config = 0;
	uint8_t vp__cms__csc1__c444_c422_config = 0;
	uint8_t vp__cms__csc1__c422_c420_config = 0;	


	videoStatus->in420Ydemux	= 0 ;
	videoStatus->out420Ydemux	= 0 ;
	videoStatus->YUV2RGB  		= 0 ;
	videoStatus->RGB2YUV  		= 0 ;
	videoStatus->YUV420_422		= 0 ;
	videoStatus->YUV422_444  	= 0 ;
	videoStatus->YUV444_422  	= 0 ;
	videoStatus->YUV422_420  	= 0 ;
	videoStatus->hvSyncPol  	= 0 ;
	videoStatus->OutDitherMode  = 0 ;

#if 0
	/* sync polarity , see sInSyncPolarityAdjustmentConfig*/
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__OUTPUT_SYNC_CONFIG, (uint8_t *)&vp__output_sync_config, BIT_MASK__VP__OUTPUT_SYNC_CONFIG__SIZE);
	vp__output_sync_config &= (BIT_MSK__VP__OUTPUT_SYNC_CONFIG__VSYNC_POLARITY | BIT_MSK__VP__OUTPUT_SYNC_CONFIG__HSYNC_POLARITY);
	
	switch (vp__output_sync_config)
	{
		case 0 :
			videoStatus->hvSyncPol = SII_HV_SYNC_POL__HNVN;
			break;
			
		case 1 :
			videoStatus->hvSyncPol = SII_HV_SYNC_POL__HNVP;
			break;
			
		case 2 :
			videoStatus->hvSyncPol = SII_HV_SYNC_POL__HPVN;
			break;
			
		case 3 :
			videoStatus->hvSyncPol =  SII_HV_SYNC_POL__HPVP;
			break;

		/*default:*/
	}
#endif
	/* sync polarity , see sUpdateHVSyncPol*/
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__FDET_STATUS, (uint8_t *)&vp__fdet_status, BIT_MASK__VP__FDET_STATUS__SIZE);
	if(vp__fdet_status & BIT_MSK__VP__FDET_STATUS__HSYNC_POLARITY)
	{
		videoStatus->hvSyncPol = (vp__fdet_status & BIT_MSK__VP__FDET_STATUS__VSYNC_POLARITY)? SII_HV_SYNC_POL__HPVP : SII_HV_SYNC_POL__HPVN;
	}
	else
	{
		videoStatus->hvSyncPol = (vp__fdet_status & BIT_MSK__VP__FDET_STATUS__VSYNC_POLARITY)? SII_HV_SYNC_POL__HNVP : SII_HV_SYNC_POL__HNVN;
	}

	/* csc0 & csc1 */
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__MULTI_CSC_CONFIG, (uint8_t *)&vp__cms__csc0__multi_csc_config, BIT_MASK__VP__CMS_CSC0_MULTI_CSC_CONFIG__SIZE);
	videoStatus->YUV2RGB =	( vp__cms__csc0__multi_csc_config & (BIT_MSK__VP__CMS__CSC0__MULTI_CSC_CONFIG__ENABLE & 0x0001) ) ? true : false;
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__MULTI_CSC_CONFIG, (uint8_t *)&vp__cms__csc1__multi_csc_config, BIT_MASK__VP__CMS_CSC1_MULTI_CSC_CONFIG__SIZE);
	videoStatus->RGB2YUV =  ( vp__cms__csc1__multi_csc_config & (BIT_MSK__VP__CMS__CSC1__MULTI_CSC_CONFIG__ENABLE & 0x0001) ) ? true : false;

 	/* up sampler */
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__C420_C422_CONFIG, (uint8_t *) &vp__cms__csc0__c420_c422_config, BIT_MASK__VP__CMS_CSC0_420_422_CONFIG__SIZE);
	videoStatus->YUV420_422 = (vp__cms__csc0__c420_c422_config & BIT_MSK__VP__CMS__CSC0__C420_C422_CONFIG__ENABLE) ? true : false;
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC0__C422_C444_CONFIG, (uint8_t *) &vp__cms__csc0__c422_c444_config, BIT_MASK__VP__CMS_CSC0_422_444_CONFIG__SIZE);
	videoStatus->YUV422_444 = (vp__cms__csc0__c422_c444_config & BIT_MSK__VP__CMS__CSC0__C422_C444_CONFIG__ENABLE)? true :false;	

	/* down sampler */
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__C444_C422_CONFIG, (uint8_t *) &vp__cms__csc1__c444_c422_config, BIT_MASK__VP__CMS_CSC1_444_422_CONFIG__SIZE);
	videoStatus->YUV444_422 = (vp__cms__csc1__c444_c422_config & BIT_MSK__VP__CMS__CSC1__C444_C422_CONFIG__ENABLE)? true : false;
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__C422_C420_CONFIG, (uint8_t *) &vp__cms__csc1__c422_c420_config, BIT_MASK__VP__CMS_CSC1_422_420_CONFIG__SIZE);
	videoStatus->YUV422_420 = (vp__cms__csc1__c422_c420_config & BIT_MSK__VP__CMS__CSC1__C422_C420_CONFIG__ENABLE)? true : false;

	/* Y420 demux */
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__INPUT_FORMAT, (uint8_t *)&vp__input_format, BIT_MASK__VP__INPUT_FORMAT__SIZE);
	videoStatus->in420Ydemux = (vp__input_format & BIT_MSK__VP__INPUT_FORMAT__MUX_420_ENABLE) ? true : false;
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__OUTPUT_FORMAT, (uint8_t *)&vp__output_format, BIT_MASK__VP__OUTPUT_FORMAT__SIZE);
	videoStatus->out420Ydemux = (vp__output_format & BIT_MSK__VP__OUTPUT_FORMAT__DEMUX_420_ENABLE) ? true : false;

	/* dither conver */
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__CMS__CSC1__DITHER_CONFIG, (uint8_t *)&vp__cms__csc1__dither_config, BIT_MASK__VP__CMS__CSC1__DITHERING_CONFIG__SIZE);
	videoStatus->OutDitherMode = (vp__cms__csc1__dither_config & BIT_MSK__VP__CMS__CSC1__DITHER_CONFIG__MODE) >> 0;
    videoStatus->dither = (videoStatus->OutDitherMode == DITHER__NOCHNG) ? false : true;
}



bool_t SiiModVidpathGet(SiiInst_t inst, SiiModVidPathInternalOpcode_t opcode, void *outData)
{
	VideoPathParam_t*      pVidPathParam = (VideoPathParam_t*)SII_LIB_OBJ_PNTR(inst);
    SiiVideoStatus_t*      videoStatus = (SiiVideoStatus_t*)outData;
    switch(opcode)
    {
	case SII_MOD_VIDPATH__HW_STATUS:
        sVideoHardwareStatusGet(pVidPathParam, videoStatus);
        break;
    default:
        break;

    }
    
    return true;
}

#ifndef HDMI_BUILD_IN_BOOT
void SiiModVidPathInterruptHandler(SiiInst_t inst)
{
    uint32_t vp__fdet_irq_stat;
    VideoPathParam_t *pVidPathParam = (VideoPathParam_t *)SII_LIB_OBJ_PNTR(inst);

    //pVidPathParam->eventsFalg = 0;

    //SELECT_VIDEO_PATH_PAGE(pVidPathParam->pConfig->instCra, PAGE_CORE);
	SiiModVideoPathRegRead(pVidPathParam->instTxCra, REG_ADDR__VP__FDET_IRQ_STATUS, (uint8_t *)&vp__fdet_irq_stat, BIT_MASK__VP__FDET_IRQ_STAT__SIZE);
	SiiModVideoPathRegWrite(pVidPathParam->instTxCra, REG_ADDR__VP__FDET_IRQ_STATUS, (uint8_t *)&vp__fdet_irq_stat, BIT_MASK__VP__FDET_IRQ_STAT__SIZE);

    if(vp__fdet_irq_stat & (BIT_MSK__VP__FDET_IRQ_STATUS__HSYNC_POLARITY | BIT_MSK__VP__FDET_IRQ_STATUS__VSYNC_POLARITY))
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "HSYNC/VSYNC changed");
        sUpdateHVSyncPol(pVidPathParam);
        //pVidPathParam->eventsFlag |= SII_DRV_VIDPATH_EVENT__SYNC_CHANGE;
    }
    if(vp__fdet_irq_stat  &BIT_MSK__VP__FDET_IRQ_STATUS__FRAME_RATE)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "Frame Rate changed");
    }
    if(vp__fdet_irq_stat & BIT_MSK__VP__FDET_IRQ_STATUS__HBACK_COUNT)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "HBACK Count changed");
    }
    if(vp__fdet_irq_stat & BIT_MSK__VP__FDET_IRQ_STATUS__HFRONT_COUNT)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "HFRONT Count changed");
    }
    if(vp__fdet_irq_stat & BIT_MSK__VP__FDET_IRQ_STATUS__HSYNC_HIGH_COUNT)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "HSYNC HIGH COUNT changed");
    }
    if(vp__fdet_irq_stat & BIT_MSK__VP__FDET_IRQ_STATUS__HSYNC_LOW_COUNT)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "HSYNC LOW Count changed");
    }
    if(vp__fdet_irq_stat & BIT_MSK__VP__FDET_IRQ_STATUS__INTERLACED)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "Interlaced changed");
    }
    if(vp__fdet_irq_stat & BIT_MSK__VP__FDET_IRQ_STATUS__LINE_COUNT)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "LINE Count changed");
    }
    if(vp__fdet_irq_stat & BIT_MSK__VP__FDET_IRQ_STATUS__PIXEL_COUNT)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "PIXEL Count changed");
    }
    if(vp__fdet_irq_stat & BIT_MSK__VP__FDET_IRQ_STATUS__VBACK_COUNT_EVEN)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "VBACK Count EVEN changed");
    }
    if(vp__fdet_irq_stat & BIT_MSK__VP__FDET_IRQ_STATUS__VBACK_COUNT_ODD)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "VFRONT Count ODD changed");
    }
    if(vp__fdet_irq_stat & BIT_MSK__VP__FDET_IRQ_STATUS__VFRONT_COUNT_EVEN)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "VFRONT Count EVEN changed");
    }
    if(vp__fdet_irq_stat & BIT_MSK__VP__FDET_IRQ_STATUS__VFRONT_COUNT_ODD)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "VFRONT Count ODD changed");
    }
    if(vp__fdet_irq_stat & BIT_MSK__VP__FDET_IRQ_STATUS__VIDEO656)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "VIDEO656 changed");
    }
    if(vp__fdet_irq_stat & BIT_MSK__VP__FDET_IRQ_STATUS__VSYNC_HIGH_COUNT_EVEN)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "VSYNC HIGH Count EVEN changed");
    }
    if(vp__fdet_irq_stat & BIT_MSK__VP__FDET_IRQ_STATUS__VSYNC_HIGH_COUNT_ODD)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "VSYNC HIGH Count ODD changed");
    }
    if(vp__fdet_irq_stat & BIT_MSK__VP__FDET_IRQ_STATUS__VSYNC_LOW_COUNT_EVEN)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "VSYNC LOW Count EVEN changed");
    }
    if(vp__fdet_irq_stat & BIT_MSK__VP__FDET_IRQ_STATUS__VSYNC_LOW_COUNT_ODD)
    {
        SII_LIB_LOG_DEBUG1(pVidPathParam, "VSYNC LOW Count ODD changed");
    }

    //if( pVidPathParam->appNotifycbFunc && pVidPathParam->eventsFalg)
    {
        //pVidPathParam->appNotifycbFunc(pVidPathParam->eventsFalg);
    }
}
#endif
