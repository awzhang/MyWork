/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name       	: 	hi_comm_adec.h
  Version        		: 	Initial Draft
  Author         		: 	Hisilicon multimedia software group
  Created       		: 	2006/01/10
  Last Modified		    :
  Description  		    : 
  Function List 		:
  History       		:
  1.Date        		: 	2006/01/10
    Author      		: 	F47391
    Modification   	    :	Created file

******************************************************************************/


#ifndef  __HI_COMM_ADEC_H__
#define  __HI_COMM_ADEC_H__

#include "hi_type.h"
#include "hi_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define  HI_ERR_ADEC_DECODER_CLOSE          HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,1) 
#define  HI_ERR_ADEC_DECODER_REOPEN         HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,2)   

#define  HI_ERR_ADEC_CHN_RECREAT            HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,3)   
#define  HI_ERR_ADEC_CHN_NOT_START          HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,4)   
#define  HI_ERR_ADEC_CHN_NOT_STOP           HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,5)
#define  HI_ERR_ADEC_CHN_RUNNING            HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,6)
#define  HI_ERR_ADEC_CHN_NOT_CREAT          HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,7)   

#define  HI_ERR_ADEC_INVALID_DECCHNNUM      HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,8)   
#define  HI_ERR_ADEC_INVALID_OUTCHNNUM      HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,9)   
#define  HI_ERR_ADEC_INVALID_PTR            HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,10)   
#define  HI_ERR_ADEC_INVALID_ENCTYPE        HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,11) 

#define  HI_ERR_ADEC_VOICE_BUF_FULL         HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,12)   
#define  HI_ERR_ADEC_DEV_OPEN_FAIL          HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,13)      

#define  HI_ERR_ADEC_INVALID_SAMPLERATE     HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,14)   
#define  HI_ERR_ADEC_INVALID_BITWIDTH       HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,15)   
#define  HI_ERR_ADEC_INVALID_SIOMODE        HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,16)   
#define  HI_ERR_ADEC_CHN_NOT_OWNER          HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,17)   
#define  HI_ERR_ADEC_INVALID_LENGTH         HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,18)   
#define  HI_ERR_ADEC_DENCODER_ERROR         HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,19)
#define  HI_ERR_ADEC_INVALID_AMRPACKAGE     HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,20)
#define  HI_ERR_ADEC_ATTRIB_READONLY        HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,21)
#define  HI_ERR_ADEC_INVALID_BUFFER_SIZE    HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,22)
#define  HI_ERR_ADEC_INVALID_VOICEPACKAGE   HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,23)
#define  HI_ERR_ADEC_BUFFER_NOTENOUGH       HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,24)
#define  HI_ERR_ADEC_INVALID_VOICEBPS       HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,25)
#define  HI_ERR_ADEC_INVALID_PAYLOADFMT     HI_DEF_ERR(HI_ID_ADEC,HI_LOG_LEVEL_ERROR,26)

typedef struct hiADEC_CH_ATTR_S
{
    HI_S32               ao_chn;    /* The audio out channel  */
    HI_BOOL              bFlag;     /* The flags for block    */
    AUDIO_CODEC_FORMAT_E enType;    /* The decoder type       */
    AMR_PACKAGE_TYPE_E   enFormat;  /* The AMR package format */
}ADEC_CH_ATTR_S;

/* The init struct for Gxx and ADPCM */
typedef struct hiADEC_VOICE_INIT_S
{
    HI_BOOL                 bFlag;		  /* the flag indicate block */
    HI_U32                  u32BufDepth;  /* The buffer max depth    */
    VOICE_PACAKGE_TYPE_E    enPackage;    /* The package lenght      */
    VOICE_PAYLOAD_FORMAT_E  enPayload;    /* The payload format      */
    VOICE_BITRATE_E         enVoicebps;   /* The bit rate of G726    */
} ADEC_VOICE_INIT_S;

/* AMR Init struct */
typedef struct hiADEC_AMR_INIT_S
{
    HI_BOOL              bFlag;		  /* the flag indicate block */
    HI_U32               u32BufDepth; /* The buffer max depth    */
    AMR_PACKAGE_TYPE_E   enFormat;    /* AMR package format      */
} ADEC_AMR_INIT_S;

/* The AAC init attribute */
typedef struct hiADEC_AAC_INIT_S
{
    HI_BOOL              bFlag;	      /* the flag indicate block */
    HI_U32               u32BufDepth; /* The buffer max depth    */
} ADEC_AAC_INIT_S;

typedef struct hiAO_CH_STATE_S
{
    HI_U32  u32InputFrame; /* Total frame number input */
    HI_U32  u32WaitNum;    /* The frame number which is waiting for play */
    HI_U32  u32Reserve1;    /* Not used */
    HI_U32  u32Reserve2;    /* Not used */
    HI_U32  u32Reserve3;    /* Not used */
} AO_CH_STATE_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __HI_COMM_ADEC_H__*/

