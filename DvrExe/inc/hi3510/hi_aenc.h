/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name             :   hi_adec.h
  Version               :   Initial Draft
  Author                :   Hisilicon multimedia software group
  Created               :   2006/01/10
  Last Modified         :
  Description           : 
  Function List         :
  History               :
  1.Date                :   2006/01/10
    Author              :   F47391
    Modification        :   Created file
******************************************************************************/


#ifndef  __HI_AENC_H__
#define  __HI_AENC_H__

#include <hi_common.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/* Set the attribute of audio input channel */
HI_S32 HI_AENC_SetAIAttr( AI_CHN ChanID, AUDIO_CH_ATTR_S *pAiAttr);

/* Get the attribute of audio input channel */
HI_S32 HI_AENC_GetAIAttr( AI_CHN ChanID, AUDIO_CH_ATTR_S *pAiAttr);

/* Set the attribute of audio encoder channel */
HI_S32 HI_AENC_SetCHAttr(AENC_CHN ChanID, AENC_CH_ATTR_S *pAattr);

/* Get the attribute of audio encoder channel */
HI_S32 HI_AENC_GetCHAttr(AENC_CHN ChanID, AENC_CH_ATTR_S *pAattr);

/* Open the audio encoder */
HI_S32  HI_AENC_Open(void);

/* Close the audio encoder */
HI_S32  HI_AENC_Close(void);

/* Start the audio encoder channel */
HI_S32 HI_AENC_StartCH(AENC_CHN ChanID, HI_S32 s32Priority);

/* Stop the audio encoder channel */
HI_S32 HI_AENC_StopCH( AENC_CHN ChanID);

/* Release the audio stream buffer */
HI_S32 HI_AENC_ReleaseStream(AENC_CHN ChanID,AENC_STREAM_S *pStream);

/* Get the audio raw data */
HI_S32 HI_AENC_GetRawData(AI_CHN ChanID, AUDIO_RAWDATA_S *pStream);

/* Set the AEC attribute */
HI_S32 HI_AENC_SetAECAttr(AI_CHN ai_ChanID, AENC_AEC_ATTR_S *pAttr);

/* Get AEC attribute */
HI_S32 HI_AENC_GetAECAttr(AI_CHN ai_ChanID, AENC_AEC_ATTR_S *pAttr);

/* Enable the AEC(Acoustic Echo Canceler) function */
HI_S32 HI_AENC_EnableAEC(AI_CHN ai_ChanID, AO_CHN ao_ChanID);

/* Disable the AEC(Acoustic Echo Canceler) function */
HI_S32 HI_AENC_DisableAEC(AI_CHN ai_ChanID, AO_CHN ao_ChanID);

/* The default ANR water level. */
#define ANR_WATERLEVEL_8BIT  0x05   /* For 8Bit sample */
#define ANR_WATERLEVEL_8TO16 0x550  /* For 16Bit sample which expend from 8Bit*/
#define ANR_WATERLEVEL_16BIT 0xA0   /* For 16Bit sample */
#define ANR_WATERLEVEL_ZERO  0x00   /* For most situation */

/*
** Enable the ANR function
** The default ANR_WATERLEVEL_ZERO can be used for most situation.
** And other values can be used allso.
*/
HI_S32 HI_AENC_EnableANR(AI_CHN ai_ChanID, HI_S32 s32WaterLevel);

/* Disable the ANR function */
HI_S32 HI_AENC_DisableANR(AI_CHN ai_ChanID);

/* Enable the AI to AO local loop function */
HI_S32 HI_AI_EnableAI2AO(AI_CHN ai_ChanID, AO_CHN ao_ChanID);

/* Disable the AI to AO local loop function */
HI_S32 HI_AI_DisableAI2AO(AI_CHN ai_ChanID, AO_CHN ao_ChanID);

/*
** --------------------- The common AENC APIs --------------------------
*/
/* Create one audio encoder channel */
HI_S32 HI_AENC_CreateCH( AI_CHN ChanID, 
                         AUDIO_CODEC_FORMAT_E enType, 
                         HI_BOOL bFlag,
                         AENC_CHN *pAencChn);

/*
** The exptend interface for audio encoder channel create.
** More attribute can setted than HI_AENC_CreateCH interface.
*/
HI_S32 HI_AENC_CreateCHEx(AI_CHN ChanID, AUDIO_CODEC_FORMAT_E enType, 
                          HI_VOID *pConfig, AENC_CHN *pAencChn);

/* Get the encoded audio stream. With the four bytes head. */
HI_S32 HI_AENC_GetStream(AENC_CHN ChanID,AENC_STREAM_S *pStream);

/* Get the encoded audio stream. Without the four bytes head.  */
HI_S32 HI_AENC_GetStreamEx(AENC_CHN ChanID,AENC_STREAM_S *pStream);

/* Destroy the audio encoder channel */
HI_S32 HI_AENC_DestroyCH(AENC_CHN ChanID, HI_BOOL bFlag);

/*
** --------------------- Just for G.711/G.726/ADPCM ----------------------
** If you just need G.711/G.726/ADPCM codec format then call the following 
** functions instead of common AENC APIs.
*/
HI_S32 HI_AENC_CreateVoiceCHEx(AI_CHN ChanID, AUDIO_CODEC_FORMAT_E enType, 
                               HI_VOID *pConfig, AENC_CHN *pAencChn);

/* Get the encoded audio stream. Without the four bytes head.  */
HI_S32 HI_AENC_GetVoiceStreamEx(AENC_CHN ChanID,AENC_STREAM_S *pStream);

/* Destroy the audio encoder channel */
HI_S32 HI_AENC_DestroyVoiceCHEx(AENC_CHN ChanID, HI_BOOL bFlag);


/*
** --------------------- Just for AMR-------------- --------
** If you just need AMR codec format then call the following 
** functions instead of common AENC APIs.
*/
HI_S32 HI_AENC_CreateAmrCHEx(AI_CHN ChanID, AUDIO_CODEC_FORMAT_E enType, 
                             HI_VOID *pConfig, AENC_CHN *pAencChn);

/* Get the encoded audio stream. Without the four bytes head.  */
HI_S32 HI_AENC_GetAmrStreamEx(AENC_CHN ChanID,AENC_STREAM_S *pStream);

/* Destroy the audio encoder channel */
HI_S32 HI_AENC_DestroyAmrCHEx(AENC_CHN ChanID, HI_BOOL bFlag);


#ifdef __cplusplus
#if __cplusplus
}
#endif 
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __HI_AENC_H__*/

