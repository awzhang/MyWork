/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : dsu_ext.h
  Version       : Initial Draft
  Author        : l64467
  Created       : 2007/10/26
  Description   :
  History       :
  1.Date        : 2007/10/26
    Author      : l64467
    Modification: Created file

  2.Date         : 2008/08/20
    Author       : l64467
    Modification : Add DE_INTERLACE function

  3.Date         : 2009/01/18
    Author       : x00100808
    Modification : Add init function for VPP
******************************************************************************/
#ifndef __VPP_EXT_H__
#define __VPP_EXT_H__

#include "hi_comm_vpp.h"
#include "dsu_ext.h"

#ifdef __cplusplus
    #if __cplusplus
    extern "C"{
    #endif
#endif /* End of #ifdef __cplusplus */


#define VPP_MAX_MS_NUM 16



typedef struct hiVPP_ZMECFG_S
{
    HI_BOOL       bPixCutEn; 
    HI_BOOL       bHzMen;    
    HI_BOOL       bVzMen;    
    HI_U32        u32VhRint; 
    HI_U32        u32WhRint; 
}VPP_ZMECFG_S;

typedef struct hiVPP_OVERLAY_S
{
    PIXEL_FORMAT_E enPixelFmt;
    
    HI_U32 u32FgAlpha;
    HI_U32 u32BgAlpha;
    
    RECT_S stRect;
    HI_U32 u32PhyAddr;
	HI_S32 u32Stride;
}VPP_OVERLAY_S;


typedef struct hiVPP_SOVERLAY_S
{
    RECT_S stRect;
	PIXEL_FORMAT_E enPixelFormat;
	
    HI_U8  u8FgAlpha; 
	HI_U8  u8BgAlpha;
    HI_U8  u8Tcy;
    HI_U8  u8Tccb;
    HI_U8  u8Tccr;
	
    void * pdata; 
	HI_U32 u32PhyAddr;
}VPP_SOVERLAY_S;

typedef struct hiVPP_MOSAIC_S
{
    RECT_S stRect;
	PIXEL_FORMAT_E enPixelFormat;
	
    void * pdata;
	HI_U32 u32PhyAddr;
	HI_U32 u32Stride;
}VPP_MOSAIC_REGION_S;


typedef struct hiVPP_DEINTERLACE_S
{ 
	HI_BOOL            bDiTp;
	
	VIDEO_FRAME_INFO_S stPreImg;
    VIDEO_FRAME_INFO_S stCurImg;
    VIDEO_FRAME_INFO_S stNxtImg;
	
	HI_U32             u32MsStPhyAddr;
    HI_U32             u32MsStStride;

	HI_VOID            (*pCallBack)(struct hiVPP_DEINTERLACE_S *pstTask);
    VIDEO_FRAME_INFO_S stOutPic;
	
	HI_U32             privateData;
    HI_U32             reserved;  

}VPP_DEINTERLACE_S;




typedef struct hiVPP_TASK_DATA_S
{
    struct list_head    list;

    HI_BOOL	           bFieldMode;   
    HI_BOOL	           bTopField;    
	HI_BOOL            bReptEncInd;  
	HI_BOOL            bDiTp;        
    HI_BOOL            bDie;         
    HI_BOOL            bMs;          
    HI_BOOL            bTf;          
    HI_BOOL            bMed;         
    HI_BOOL            bEdge;        
	HI_BOOL            bTxtureRemove;
	HI_U32             u32TxThrd;		
    HI_U32             u32SadThrd;   
    HI_U32             u32TimeThrd;  
	
	VIDEO_FRAME_INFO_S stPreImg;     
    VIDEO_FRAME_INFO_S stCurImg;     
    VIDEO_FRAME_INFO_S stNxtImg;	 
	
    void               (*pCallBack)(struct hiVPP_TASK_DATA_S *pstTask);
	
	/************************************************************************/
	HI_U32             u32MsStPhyAddr;
    HI_U32             u32MsStStride;
	/*For OSD*/
    HI_U32             u32OsdRegNum;
    VPP_OVERLAY_S      stOsdReg[MAX_OVERLAY_NUM];
    VPP_ZMECFG_S       stZmeCfg;

	/************************************************************************/
	HI_U32             privateData;
    HI_U32             reserved;  

	/************************************************************************/
	HI_U32             u32MsResult[VPP_MAX_MS_NUM];
	HI_U32             u32GlbMotNum;


	/* For Debug */
	HI_BOOL            bImgOutaBig;
    VIDEO_FRAME_INFO_S stOutImgBig;
    HI_BOOL            bImgOutLittle;
    VIDEO_FRAME_INFO_S stOutImgLittle;
	void               (*pDeIntCallBack)(struct hiVPP_DEINTERLACE_S *pstTask);
	
}VPP_TASK_DATA_S;





/* following four functions is for VENC */
typedef HI_S32 FN_VPP_GetOverlay(VENC_GRP VeGroup,HI_U32 *pu32RegionNum, 
									VPP_OVERLAY_S astRegions[MAX_OVERLAY_NUM]);
typedef HI_S32 FN_VPP_PutOverlay(VENC_GRP VeGroup);
typedef HI_S32 FN_VPP_InitVppConf(VENC_GRP VeGroup);
typedef HI_S32 FN_VPP_QueryVppConf(VENC_GRP VeGroup, VIDEO_PREPROC_CONF_S *pstConf);

/* following three functions is for PCIV */
typedef HI_S32 FN_VPP_GetSftOverlay(VI_DEV ViDev,VI_CHN ViChn,HI_U32 *pu32RegionNum, 
									VPP_SOVERLAY_S astRegions[MAX_SOVERLAY_NUM]);
typedef HI_S32 FN_VPP_PutSftOverlay(VI_DEV ViDev,VI_CHN ViChn);


/* following three functions is for VI */
typedef HI_S32 FN_VPP_GetMosaicPic(VI_DEV ViDev,VI_CHN ViChn,HI_U32 *pu32RegionNum,
					VPP_MOSAIC_REGION_S astRegions[MAX_MOSAIC_REGION_NUM]);
typedef HI_S32 FN_VPP_PutMosaicPic(VI_DEV ViDev,VI_CHN ViChn);


/*for MPGE4 or ...*/
typedef HI_S32 FN_VPP_CreateTask(const VPP_TASK_DATA_S *pstTask);

/*for VENC_GROUP or VPP_TASK*/
typedef HI_VOID FN_VPP_ConfigRegs(VPP_TASK_DATA_S *pstTask);
typedef HI_S32 FN_VPP_IntHandle(VPP_TASK_DATA_S *pstTask);

/*for  ...*/
typedef HI_S32 FN_VPP_CreateDeInterlaceTask(const VPP_DEINTERLACE_S *pstTask);

typedef HI_S32 FN_VPP_SendPic(const VIDEO_FRAME_INFO_S *pstPic);

typedef struct hiVPP_EXPORT_FUNC_S
{
    FN_VPP_GetOverlay   *pfnVppGetOverlay;    
    FN_VPP_PutOverlay   *pfnVppPutOverlay;
    FN_VPP_InitVppConf  *pfnVppInitVppConf;
    FN_VPP_QueryVppConf *pfnVppQueryVppConf;

	FN_VPP_GetSftOverlay *pfnVppGetSftOverlay;
	FN_VPP_PutSftOverlay *pfnVppPutSftOverlay;

	FN_VPP_GetMosaicPic *pfnVppGetMosaicPic;
	FN_VPP_PutMosaicPic *pfnVppPutMosaicpic;
	
	FN_VPP_CreateTask   *pfnVppCreateTask;
	FN_VPP_ConfigRegs   *pfnVppConfigRegs;
	FN_VPP_IntHandle    *pfnVppIntHandle;
	FN_VPP_CreateDeInterlaceTask *pfnVppCreateDeInterlaceTask;
	FN_VPP_SendPic      *pfnVppSendPic;
	
}VPP_EXPORT_FUNC_S;


#ifdef __cplusplus
    #if __cplusplus
    }
    #endif
#endif /* End of #ifdef __cplusplus */


#endif /* __VPP_EXT_H__ */

