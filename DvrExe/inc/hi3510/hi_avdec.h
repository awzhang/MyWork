/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_avdec.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/01/19
  Description   : 
  History       :
  1.Date        : 2006/01/19
    Author      : Z44949
    Modification: Created file

******************************************************************************/

#ifndef __HI_AVDEC_H__
#define __HI_AVDEC_H__

#include <hi_avenc.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef HI_S32 AVDEC_CHN;      /* The AVDEC channel data type */

/* The AVDEC stream */
typedef struct avdec_stream_struct
{
    HI_U32 u32Length;          /* Data length  */
    HI_U8  *pAddr;             /* Data address */
} AVDEC_STREAM_T;

/* Create the AVDEC channel */
HI_S32 HI_AVDEC_CreateCH(VDEC_CHN VChnId,ADEC_CHN AChnId,AVDEC_CHN *pAVChnId);
                                       
/* Destroy the AVDEC channel */
HI_S32 HI_AVDEC_DestroyCH(AVDEC_CHN ChanID);

/* Start the AVDEC channel */
HI_S32 HI_AVDEC_StartCH(AVDEC_CHN ChanID, HI_S32 s32Priority);

/* Stop the AVDEC channel */
HI_S32 HI_AVDEC_StopCH(AVDEC_CHN ChanID);

/* Send the AVENC stream into decoder */
HI_S32 HI_AVDEC_SendStream(AVDEC_CHN ChanID, AVENC_STREAM_S *pAVEncStream);
                                      
/* Get the decoded stream */
HI_S32 HI_AVDEC_GetStream(AVDEC_CHN ChanID, AVDEC_STREAM_T *pAVDecStream);

/* Release the stream buffers */
HI_S32 HI_AVDEC_ReleaseStream(AVDEC_CHN ChanID, AVDEC_STREAM_T *pAVDecStream);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __HI_AVDEC_H__ */
