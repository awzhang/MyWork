/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name       	: 	hi_adec.h
  Version        		: 	Initial Draft
  Author         		: 	Hisilicon multimedia software group
  Created       		: 	2006/01/10
  Last Modified		  :
  Description  		  : 
  Function List 		:
  History       		:
  1.Date        		: 	2006/01/10
    Author      		: 	F47391
    Modification   	:	Created file

******************************************************************************/


#ifndef  __HI_ADEC_H__
#define  __HI_ADEC_H__

#include <hi_common.h>
#include <hi_struct.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/* Set the attribute of audio output channel */
HI_S32 HI_ADEC_SetAOAttr(AO_CHN ChanID, AUDIO_CH_ATTR_S *pAoAttr);

/* Get the attribute of audio output channel */
HI_S32 HI_ADEC_GetAOAttr(AO_CHN ChanID, AUDIO_CH_ATTR_S *pAoAttr);

/* Set the attribute of audio decoder channel */
HI_S32 HI_ADEC_SetCHAttr(ADEC_CHN ChanID, ADEC_CH_ATTR_S *pAdecattr);

/* Get the attribute of audio decoder channel */
HI_S32 HI_ADEC_GetCHAttr(ADEC_CHN ChanID, ADEC_CH_ATTR_S *pAdecattr);

/* Get the state of audio decoder channel */
HI_S32 HI_ADEC_GetCHState(ADEC_CHN ChanID, AO_CH_STATE_S *pState);

/* Open the audio decoder */
HI_S32  HI_ADEC_Open(void);

/* Close the audio decoder */
HI_S32  HI_ADEC_Close(void);

/* Start the audio decoder channel */
HI_S32 HI_ADEC_StartCH (ADEC_CHN ChanID, HI_S32 s32Priority);

/* Stop the audio decoder channel */
HI_S32 HI_ADEC_StopCH (ADEC_CHN ChanID);

/*
** Get the decoded audio data.
** Attention: The buffer need the user application to prepare.
*/
HI_S32 HI_ADEC_GetStream(ADEC_CHN ChanID, AUDIO_RAWDATA_S *pRawData);

/* Play the audio raw data. */
HI_S32 HI_ADEC_SendRawData(AO_CHN ChanID, AUDIO_RAWDATA_S *pRawData);

/* Get the length of one AMR frame */
HI_S32 HI_ADEC_AmrGetFrameLength(AMR_PACKAGE_TYPE_E enType, HI_U8 byte);

/* Init the audio stream tranfer */
HI_S32 HI_ADEC_InitTransfer(HI_VOID **ppHandle, AUDIO_STREAM_TRANSFER_E enType);

/* Tranfer the pStream to new format. The result will be write back pStream */
HI_S32 HI_ADEC_DoTransfer(HI_VOID *pHandle, HI_S16 *pStream);

/* Deinit the audio stream tranfer */
HI_S32 HI_ADEC_DeInitTransfer(HI_VOID **ppHandle);

/*
** --------------------- The common AENC APIs --------------------------
*/

/* Create one audio decoder channel */
HI_S32 HI_ADEC_CreateCH(AO_CHN ChanID, 
                        AUDIO_CODEC_FORMAT_E enType,
                        HI_BOOL bFlag,
                        ADEC_CHN *pAdecChn);

/*
** The exptend interface for audio decoder channel create.
** More attribute can setted than HI_ADEC_CreateCH interface.
*/
HI_S32 HI_ADEC_CreateCHEx(AO_CHN ChanID, AUDIO_CODEC_FORMAT_E enType, 
                          HI_VOID *pConfig, ADEC_CHN *pAdecChn);
/*
** Send the audio stream into the decoder, and audio data will be played.
** The stream has four bytes head
*/
HI_S32 HI_ADEC_SendStream(ADEC_CHN ChanID, AENC_STREAM_S *pAencStream);

/*
** Send the audio stream into the decoder, and audio data will be played.
** The stream has no four bytes head
*/
HI_S32 HI_ADEC_SendStreamEx(ADEC_CHN ChanID, AENC_STREAM_S *pAencStream);

/* Destroy the audio decoder channel */
HI_S32 HI_ADEC_DestroyCH(ADEC_CHN ChanID, HI_BOOL bFlag);

/*
** --------------------- Just for G.711/G.726/ADPCM ----------------------
** If you just need G.711/G.726/ADPCM codec format then call the following 
** functions instead of common AENC APIs.
*/
HI_S32 HI_ADEC_CreateVoiceCHEx(AO_CHN ChanID, AUDIO_CODEC_FORMAT_E enType, 
                          HI_VOID *pConfig, ADEC_CHN *pAdecChn);
/* Send the audio stream into the decoder, and audio data will be played. */
HI_S32 HI_ADEC_SendVoiceStreamEx(ADEC_CHN ChanID, AENC_STREAM_S *pAencStream);

/* Destroy the audio decoder channel */
HI_S32 HI_ADEC_DestroyVoiceCHEx(ADEC_CHN ChanID, HI_BOOL bFlag);

/*
** --------------------- Just for AMR-------------- --------
** If you just need AMR codec format then call the following 
** functions instead of common AENC APIs.
*/
HI_S32 HI_ADEC_CreateAmrCHEx(AO_CHN ChanID, AUDIO_CODEC_FORMAT_E enType, 
                          HI_VOID *pConfig, ADEC_CHN *pAdecChn);

/* Send the audio stream into the decoder, and audio data will be played. */
HI_S32 HI_ADEC_SendAmrStreamEx(ADEC_CHN ChanID, AENC_STREAM_S *pAencStream);

/* Destroy the audio decoder channel */
HI_S32 HI_ADEC_DestroyAmrCHEx(ADEC_CHN ChanID, HI_BOOL bFlag);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __HI_ADEC_H__*/

