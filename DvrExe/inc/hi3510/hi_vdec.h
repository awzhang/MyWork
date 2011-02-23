/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_vdec.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/01/22
  Description   : 
  History       :
  1.Date        : 2006/01/22
    Author      : 
    Modification: Created file

******************************************************************************/

#ifndef  __HI_VDEC_H__
#define  __HI_VDEC_H__

#include <hi_common.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/* open video decoder */
HI_S32 HI_VDEC_Open();

/* close video decoder */
HI_S32 HI_VDEC_Close();

/* get video decoder capability */
HI_S32 HI_VDEC_GetCapability(VDEC_CAPABILITY_S *pCap);

/* create video decode channel*/
HI_S32 HI_VDEC_CreateCH(VDEC_CHN *pChanID, 
                        VDEC_ATTR_TYPE_E enType, 
                        HI_VOID *pValue);

/* destroy video decode channel */
HI_S32 HI_VDEC_DestroyCH(VDEC_CHN ChanID);

/* reset video decode channel */
HI_S32 HI_VDEC_ResetCH(VDEC_CHN ChanID);

/* set video decode attribute */
HI_S32 HI_VDEC_SetAttr(VDEC_CHN ChanID,
                       VDEC_ATTR_TYPE_E enType,
                       HI_VOID *pValue);

/* get video decode attribute */
HI_S32 HI_VDEC_GetAttr(VDEC_CHN ChanID,
                       VDEC_ATTR_TYPE_E enType,
                       HI_VOID *pValue);

/* send stream to the particular video decoder channel */
HI_S32 HI_VDEC_SendStream(VDEC_CHN ChanID, const VDEC_STREAM_S *pstream,
    HI_BOOL bFlag);

/* get video (if ppridata is NULL,private data will be ignore) */
HI_S32 HI_VDEC_Receive(VDEC_CHN ChanID,
                       VDEC_FRAMEINFO_S *pframeinfo,
                       VDEC_PRIDATA_S *ppridata,
                       HI_BOOL bFlag);

/* release private data */
HI_S32 HI_VDEC_ReleasePrivate(VDEC_CHN ChanID, const VDEC_PRIDATA_S *ppridata);

/* release video data */
HI_S32 HI_VDEC_ReleaseVideoBuf(VDEC_CHN ChanID,
                               const VDEC_FRAMEINFO_S *pframeinfo);

/* get the bitrate infomation of video decode channel */
HI_S32 HI_VDEC_GetBps(VDEC_CHN ChanID, HI_U32 * pbps);

/* get the frame rate infomation of video decode channel */
HI_S32 HI_VDEC_GetFps(VDEC_CHN ChanID, HI_U32 * pfps);

/* Get the state of video decoder channel */
HI_S32 HI_VDEC_GetStatistic(VDEC_CHN ChanID, VDEC_STATISTIC_S *pStat);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __HI_VDEC_H__ */

