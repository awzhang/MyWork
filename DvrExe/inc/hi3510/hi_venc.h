/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_venc.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/1/13
  Last Modified :
  Description   : hi_dvs_venc.c header file
  Function List :
  History       :
  1.Date        : 2006/1/13
    Author      : qushen
    Modification: Created file

******************************************************************************/

#ifndef __HI_VENC_H__
#define __HI_VENC_H__

#include <hi_common.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define MAX_JPEG_BUFFER 0x120000

typedef enum hiFRAME_TYPE_E
{
    FRAME_I = 0,   /* I frame */
    FRAME_P,       /* P frame */
    FRAME_BUTT
} FRAME_TYPE_E;

/* open video encoder */
HI_S32 HI_VENC_Open(VENC_INIT_S *pVencCfg);

/* close video encoder */
HI_S32 HI_VENC_Close();

/* create video encode channel */
HI_S32 HI_VENC_CreateCH( VI_CHN vichn, VENC_CHN *pVencChn);

/* destroy video encode channel */
HI_S32 HI_VENC_DestroyCH( VENC_CHN ChanID, HI_BOOL bFlag );

/* set a particular attribut of video encoder */
HI_S32 HI_VENC_SetSingleAttr(VENC_CHN ChanID, 
                             VENC_CONFIG_TYPE_E enType, 
                             HI_U32 u32Value);

/* get a particular attribut of video encoder */                            
HI_S32 HI_VENC_GetSingleAttr(VENC_CHN ChanID, 
                             VENC_CONFIG_TYPE_E enType, 
                             HI_U32 *pu32Value);

/* set all the attribut of video encoder */                             
HI_S32 HI_VENC_SetAttr(VENC_CHN ChanID, HI_VOID *pAttr);

/* get all the attribut of video encoder */ 
HI_S32 HI_VENC_GetAttr(VENC_CHN ChanID, HI_VOID *pAttr);

/* start the particular video encode channel */
HI_S32 HI_VENC_StartCH(VENC_CHN ChanID, HI_S32 s32Priority);

/* stop the particular video encode channel */
HI_S32 HI_VENC_StopCH(VENC_CHN ChanID);

/* get stream from the particular video channel  */
HI_S32 HI_VENC_GetStream(VENC_CHN ChanID, VENC_STREAM_S *pStream, HI_BOOL u32Flag);

/* release stream for the particular video channel  */
HI_S32 HI_VENC_ReleaseStream(VENC_CHN ChanID, VENC_STREAM_S *pStream);

/* get orginal video data form the  particular video channel*/
HI_S32 HI_VENC_GetVideo(VENC_CHN ChanID, VIDEO_BUFFER_S *pVideo);

/* release orginal video data form the  particular video channel*/
HI_S32 HI_VENC_ReleaseVideo(VENC_CHN ChanID);

/* request I frame */
HI_S32 HI_VENC_RequestIFrame(VENC_CHN ChanID);

/* insert user data to stream */
HI_S32 HI_VENC_InsertUserInfo(VENC_CHN ChanID, HI_U8 *pu8Data,
                              HI_U32 u32Len);

/* snapshot for the special video channel */
HI_S32 HI_JPEG_GetSnapShot(VENC_CHN ChnID, HI_U32 u32Qval,
                           HI_U8 *pu8Buf, HI_U32 *pu32Len);

/* set the mode of getting stream (slice or frame) */
HI_S32 HI_VENC_SetStreamMode( HI_U32 streamType );

/* get the mode of getting stream (slice or frame) */
HI_S32 HI_VENC_GetStreamMode( HI_U32 *pStreamMode );

/* start filter of DSU */
HI_S32 HI_VENC_EnablePreProcess (VENC_CHN venc_chn);

/* stop filter of DSU */
HI_S32 HI_VENC_DisPreProcess (VENC_CHN venc_chn);

/* start corlor to grey */
HI_S32 HI_VENC_EnColor2Grey(VENC_CHN venc_chn);

/* stop corlor to grey */
HI_S32 HI_VENC_DisColor2Grey(VENC_CHN venc_chn);

/* set water mark attribute */
HI_S32 HI_VENC_SetWaterMarkAttr(VENC_CHN ChanID, VENC_WATERMARK_S VencWaterMark);

/* get water mark attribute */
HI_S32 HI_VENC_GetWaterMarkAttr(VENC_CHN ChanID, VENC_WATERMARK_S *pVencWaterMark);

/* start water mark */
HI_S32 HI_VENC_StartWaterMark(VENC_CHN ChanID);

/* set the length of stream cache */
HI_S32 HI_VENC_SetBuffLevel(VENC_CHN ChanID, HI_U32 u32Length);

/* get the length of stream cache */
HI_S32 HI_VENC_GetBuffLevel(VENC_CHN ChanID, HI_U32 *pLength);

/* set the stream buffer length */
HI_S32 HI_VENC_SetStreamBufLen(HI_U32 u32StreamLen);

/* get the stream buffer length */
HI_S32 HI_VENC_GetStreamBufLen( HI_U32 *pStreamLen );

/* Get the stream statistic */
HI_S32 HI_VENC_GetStatistic(VENC_CHN ChanID, VENC_STATISTIC_S *pStat);

/* Set the extend attribute for encoder*/
HI_S32 HI_VENC_SetSingleAttrEx(VENC_CHN ChanID, VENC_CONFIGEX_TYPE_E enType, HI_U32 u32Value);

/* Get the extend attribute for encoder*/
HI_S32 HI_VENC_GetSingleAttrEx(VENC_CHN ChanID, VENC_CONFIGEX_TYPE_E enType, HI_U32 *pValue);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HI_VENC_H__ */
