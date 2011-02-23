/******************************************************************************
Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_type.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2005/4/23
Last Modified :
Description   : the common data type defination 
Function List :
History       :
******************************************************************************/
#ifndef __HI_TYPE_H__
#define __HI_TYPE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*----------------------------------------------*
 * The basic data type defination               *
 *----------------------------------------------*/

typedef unsigned char           HI_U8;
typedef unsigned char           HI_UCHAR;
typedef unsigned short          HI_U16;
typedef unsigned int            HI_U32;
typedef unsigned long long      HI_U64;

typedef char                    HI_S8;
typedef short                   HI_S16;
typedef int                     HI_S32;
typedef long long               HI_S64;


typedef char                    HI_CHAR;
typedef char*                   HI_PCHAR;

typedef float                   HI_FLOAT;
typedef double                  HI_DOUBLE;
typedef void                    HI_VOID;

typedef unsigned long long      HI_PTS_TIME;
typedef unsigned long           HI_SIZE_T;
typedef unsigned long           HI_LENGTH_T;
/*----------------------------------------------*
 * The channel type defination                  *
 *----------------------------------------------*/
typedef HI_S32 VENC_CHN;

typedef HI_S32 VDEC_CHN;

typedef HI_U32  VI_CHN;

typedef HI_S32 AI_CHN;

typedef HI_S32 AENC_CHN;


typedef HI_S32 VO_CHN;
typedef HI_S32 AO_CHN;

typedef HI_S32 ADEC_CHN;

/*----------------------------------------------*
 * The common constant                          *
 *----------------------------------------------*/
typedef enum {
    HI_FALSE    = 0,
    HI_TRUE     = 1,
} HI_BOOL;

#ifndef NULL
#define NULL             0L
#endif
#define HI_NULL          0L
#define HI_NULL_PTR      0L

#define HI_SUCCESS          0 
#define HI_FAILURE          (-1)

#define  HI_LITTLE_ENDIAN   1234       /* The little endian */
#define  HI_BIG_ENDIAN      4321       /* The big endian    */

typedef enum hiMOD_ID_E
{
	HI_ID_API = 0,
	HI_ID_MPI = 1,
	HI_ID_CMPI = 2,
	HI_ID_VIO = 3,
	HI_ID_VIU = 4,
	HI_ID_DSU = 5,
	HI_ID_VOU = 6,
	HI_ID_ZSP = 7,
	HI_ID_VENC = 8,
	HI_ID_VDEC = 9,
	HI_ID_DRV3510 = 10,
	HI_ID_AENC  = 11,
	HI_ID_ADEC  = 12,
	HI_ID_MD    = 13,
	HI_ID_OSD   = 14,
	HI_ID_AVENC = 15,
	HI_ID_AVDEC = 16,
    HI_ID_AI    = 17,
	HI_ID_AO    = 18,
    HI_ID_OSDEX = 19
} MOD_ID_E;


/* 1010 0000b
 * VTOP use APPID from 0~31
 * so, hisilicon use APPID based on 32
 */
#define HI_ERR_APPID  (0x80000000L + 0x20000000L)

/* The debug level defination */
typedef enum hiLOG_ERRLEVEL_E
{
    HI_LOG_LEVEL_DEBUG = 0,  /* debug-level                                  */
    HI_LOG_LEVEL_INFO,       /* informational                                */
    HI_LOG_LEVEL_NOTICE,     /* normal but significant condition             */
    HI_LOG_LEVEL_WARNING,    /* warning conditions                           */
    HI_LOG_LEVEL_ERROR,      /* error conditions                             */
    HI_LOG_LEVEL_CRIT,       /* critical conditions                          */
    HI_LOG_LEVEL_ALERT,      /* action must be taken immediately             */
    HI_LOG_LEVEL_FATAL,      /* just for compatibility with previous version */
    HI_LOG_LEVEL_BUTT
} LOG_ERRLEVEL_E;

/******************************************************************************
|----------------------------------------------------------------| 
| 1 |   APP_ID   |   MOD_ID    | ERR_LEVEL |   ERR_ID            | 
|----------------------------------------------------------------| 
|<--><--7bits----><----8bits---><--3bits---><------13bits------->|
******************************************************************************/

#define HI_DEF_ERR( module, level, errid) \
    ((HI_S32)( (HI_ERR_APPID) | ((module) << 16 ) | ((level)<<13) | (errid) ))

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_TYPE_H__ */


