/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name             :   hi_comm_aenc.h
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


#ifndef  __HI_COMM_AENC_H__
#define  __HI_COMM_AENC_H__

#include "hi_type.h"
#include "hi_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


#define  HI_ERR_AENC_ENCODER_CLOSE          HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,1) 
#define  HI_ERR_AENC_ENCODER_REOPEN         HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,2)   

#define  HI_ERR_AENC_CHN_RECREAT            HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,3)   
#define  HI_ERR_AENC_CHN_NOT_START          HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,4)
#define  HI_ERR_AENC_CHN_NOT_STOP           HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,5)
#define  HI_ERR_AENC_CHN_RUNNING            HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,6)
#define  HI_ERR_AENC_CHN_NOT_CREAT          HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,7)

#define  HI_ERR_AENC_INVALID_ENCCHNNUM      HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,8)   
#define  HI_ERR_AENC_INVALID_INCHNNUM       HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,9)   
#define  HI_ERR_AENC_INVALID_PTR            HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,10)   
#define  HI_ERR_AENC_INVALID_ENCTYPE        HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,11) 

#define  HI_ERR_AENC_VOICE_BUF_EMPTY        HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,12)   
#define  HI_ERR_AENC_DEV_OPEN_FAIL          HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,13)   

#define  HI_ERR_AENC_INVALID_SAMPLERATE     HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,14)   
#define  HI_ERR_AENC_INVALID_BITWIDTH       HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,15)   
#define  HI_ERR_AENC_INVALID_SIOMODE        HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,16)   
#define  HI_ERR_AENC_CHN_NOT_OWNER          HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,17)
#define  HI_ERR_AENC_ATTRIB_READONLY        HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,18)
#define  HI_ERR_AENC_INVALID_AMRMODE        HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,19)
#define  HI_ERR_AENC_AEC_AOUSED             HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,20)
#define  HI_ERR_AENC_AEC_INVALID_AO         HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,21)
#define  HI_ERR_AENC_ENCODER_ERROR          HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,22)
#define  HI_ERR_AENC_INVALID_AMRPACKAGE     HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,23)
#define  HI_ERR_AENC_INVALID_AACQUALITY     HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,24)
#define  HI_ERR_AENC_INVALID_AACCHANNEL     HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,25)
#define  HI_ERR_AENC_INVALID_AACBANDWIDTH   HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,26)
#define  HI_ERR_AENC_INVALID_AACBITRATE     HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,27)
#define  HI_ERR_AENC_INVALID_VOICEPACKAGE   HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,28)
#define  HI_ERR_AENC_INVALID_BUFFER_SIZE    HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,29)
#define  HI_ERR_AENC_INVALID_AECATTR        HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,30)
#define  HI_ERR_AENC_AEC_ENABLED            HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,31)
#define  HI_ERR_AENC_BUFFER_NOTENOUGH       HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,32)
#define  HI_ERR_AENC_INVALID_VOICEBPS       HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,33)
#define  HI_ERR_AENC_INVALID_PAYLOADFMT     HI_DEF_ERR(HI_ID_AENC,HI_LOG_LEVEL_ERROR,34)

#define AMR_MAGIC_NUMBER "#!AMR\n"

/* AENC stream data struct */
typedef struct hiAENC_STREAM_S 
{ 
    HI_U64  u64TimeStamp;   /* Time stamp                      */
    HI_U32  u32FrameNum;    /* Encode frame number             */
    HI_U32  u32Len;         /* Encode data length              */
    HI_S16 *ps16Addr;       /* Encode data buffer address      */
    HI_S32  s32TimeUnit;    /* The time spaned counted as (ms) */
} AENC_STREAM_S;

/* The init struct for Gxx and ADPCM */
typedef struct hiAENC_VOICE_INIT_S
{
    HI_BOOL                 bFlag;		  /* the flag indicate block */
    HI_U32                  u32BufDepth;  /* The buffer max depth    */
    VOICE_PACAKGE_TYPE_E    enPackage;    /* The package lenght      */
    VOICE_PAYLOAD_FORMAT_E  enPayload;    /* The payload format      */
    VOICE_BITRATE_E         enVoicebps;   /* The bit rate of voice   */
} AENC_VOICE_INIT_S;

/* AMR Init struct */
typedef struct hiAENC_AMR_INIT_S
{
    HI_BOOL              bFlag;		  /* the flag indicate block */
    HI_U32               u32BufDepth; /* The buffer max depth    */
    AMR_MODE_E           enAmrMode;   /* AMR bit rate mode       */
    AMR_PACKAGE_TYPE_E   enFormat;    /* AMR package format      */
} AENC_AMR_INIT_S;

/* The AAC init attribute */
typedef struct hiAENC_AAC_INIT_S
{
    HI_BOOL              bFlag;	      /* the flag indicate block */
    HI_U32               u32BufDepth; /* The buffer max depth    */
    AAC_CONFIG_S         aacCfg;      /* AAC config              */
} AENC_AAC_INIT_S;

/* All the AENC channel attribute */
typedef struct hiAENC_CH_ATTR_S
{ 
    HI_S32               ai_chn;     /* the context ai channel  */
    AUDIO_CODEC_FORMAT_E enType;     /* audio encode formate    */
    HI_BOOL              bFlag;		 /* the flag indicate block */
    AMR_MODE_E           enAmrMode;  /* AMR bit rate mode       */
    AMR_PACKAGE_TYPE_E   enFormat;   /* AMR package format      */
    AAC_CONFIG_S         aacCfg;     /* AAC config              */
} AENC_CH_ATTR_S;

/* The AEC attribute */
typedef struct hiAENC_AEC_ATTR_S
{
    HI_S32 s32Nlp;      /* Nonlinear Processor switch. 0 - Close, 1 - Open  */
    HI_S32 s32Erl;      /* Echo Return Loss. Scope is 0x0(-4.2dB)~0x9(-10dB)*/
    HI_S32 s32MaskSin;  /* Source signal waterlevel. Scope is 0x00~0x2F     */
    HI_S32 s32MaskRin;  /* reference signal waterlevel. Scope is 0x00~0x2F  */
} AENC_AEC_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif 
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __HI_COMM_AENC_H__*/

