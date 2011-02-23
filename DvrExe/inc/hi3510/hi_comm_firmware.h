/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_firmware.h
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

#ifndef __HI_COMM_FIRMWARE_H__
#define __HI_COMM_FIRMWARE_H__

#include <hi_common.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define PRODUCT_MAJOR_VERSION  1
#define PRODUCT_MINOR_VERSION  3
#define PRODUCT_MODIFY_VERSION 6
#define PRODUCT_BUILD_VERSION  0

#define PRODUCT_NUMBER_VERSION \
({                             \
    HI_U32 u32Tmp;             \
    u32Tmp  = (PRODUCT_BUILD_VERSION) & 0xFF;               \
    u32Tmp |= (((PRODUCT_MODIFY_VERSION) << 8)& 0xFF00);    \
    u32Tmp |= (((PRODUCT_MINOR_VERSION) << 16)& 0xFF0000);  \
    u32Tmp |= (((PRODUCT_MAJOR_VERSION) << 24)& 0xFF000000);\
    u32Tmp; \
})

#define MKSTR(exp) # exp
#define MKMARCOTOSTR(exp) MKSTR(exp)
#define PRODUCT_STRING_VERSION             \
    MKMARCOTOSTR(PRODUCT_MAJOR_VERSION) "."\
    MKMARCOTOSTR(PRODUCT_MINOR_VERSION) "."\
    MKMARCOTOSTR(PRODUCT_MODIFY_VERSION)"."\
    MKMARCOTOSTR(PRODUCT_BUILD_VERSION)
    
#define PARSE_MAJOR_VERSION(ver)  (((ver)&0xFF000000)>>24)
#define PARSE_MINOR_VERSION(ver)  (((ver)&0x00FF0000)>>16) 
#define PARSE_MODIFY_VERSION(ver) (((ver)&0x0000FF00)>>8) 
#define PARSE_BUILD_VERSION(ver)  ((ver)&0x0000000FF) 

typedef struct hiFIRMWARE_INFO_S
{
    HI_S32 s32HW_version;             /* The hardware version information */
    HI_S32 s32SW_version;             /* The software version information */

    /* The capbility of video encoder */
    HI_U32 u32VencAbility[VENC_FORMAT_BUTT];

    /* The capbility of video decoder */
    HI_U32 u32VdecAbility[VENC_FORMAT_BUTT];

    /* The capbility of audio encoder */
    HI_U32 u32AencAbility[AUDIO_CODEC_FORMAT_BUTT]; 

    /* The capbility of audio decoder */
    HI_U32 u32AdecAbiliby[AUDIO_CODEC_FORMAT_BUTT];
} FIRMWARE_INFO_S;

typedef struct hiVERSION_INFO_S
{
    HI_U32 u32ProductVer;       /* The product version  */
    HI_U32 u32RootBoxVer;       /* RootBox version      */
    HI_U32 u32LinuxVer;         /* Linux kernel version */
    HI_U32 u32MspVer;           /* The MSP version      */
    HI_U32 u32AlgVer;           /* The codec alg version*/
    HI_U32 u32ChipVer;          /* The chip version     */
} VERSION_INFO_S;

typedef struct hiVERSION_STRING_S
{
    HI_CHAR szProductVer[80];   /* The product version  */
    HI_CHAR szRootBoxVer[80];   /* RootBox version      */
    HI_CHAR szLinuxVer[80];     /* Linux kernel version */
    HI_CHAR szMspVer[80];       /* The MSP version      */
    HI_CHAR szAlgVer[80];       /* The codec alg version*/
    HI_CHAR szChipVer[80];      /* The chip version     */
} VERSION_STRING_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HI_COMM_FIRMWARE_H__ */
