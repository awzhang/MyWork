/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_OSDEX.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/03/10
  Description   : 
  History       :
  1.Date        : 2006/03/10
    Author      : z42136
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_OSDEX_H__
#define __HI_COMM_OSDEX_H__

#include "hi_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/*OSDEX API ERR*/
/* OSDEX common */
#define  HI_ERR_OSDEX_INVALID_PTR           HI_DEF_ERR(HI_ID_OSDEX, HI_LOG_LEVEL_ERROR, 1)
#define  HI_ERR_OSDEX_INVALID_PARAM         HI_DEF_ERR(HI_ID_OSDEX, HI_LOG_LEVEL_ERROR, 2)

/* OSDEX device */
#define  HI_ERR_OSDEX_VODEV_NOT_OPEN        HI_DEF_ERR(HI_ID_OSDEX, HI_LOG_LEVEL_ERROR, 11)
#define  HI_ERR_OSDEX_DEV_NOT_OPEN          HI_DEF_ERR(HI_ID_OSDEX, HI_LOG_LEVEL_ERROR, 12)
#define  HI_ERR_OSDEX_FAILED_SETATTR        HI_DEF_ERR(HI_ID_OSDEX, HI_LOG_LEVEL_ERROR, 13)    /*set osdex attr failed*/
#define  HI_ERR_OSDEX_FAILED_GETATTR        HI_DEF_ERR(HI_ID_OSDEX, HI_LOG_LEVEL_ERROR, 14)    /*get osdex attr failed*/

/* OSDEX region */
#define  HI_ERR_OSDEX_REGION_FAILED_SETATTR        HI_DEF_ERR(HI_ID_OSDEX, HI_LOG_LEVEL_ERROR, 13)    /*set osdex attr failed*/
#define  HI_ERR_OSDEX_REGION_FAILED_GETATTR        HI_DEF_ERR(HI_ID_OSDEX, HI_LOG_LEVEL_ERROR, 14)    /*get osdex attr failed*/

#define  HI_ERR_OSDEX_REGION_FAILED_CREATE         HI_DEF_ERR(HI_ID_OSDEX, HI_LOG_LEVEL_ERROR, 21)     /*create region failed*/
#define  HI_ERR_OSDEX_REGION_FAILED_DELETE         HI_DEF_ERR(HI_ID_OSDEX, HI_LOG_LEVEL_ERROR, 22)     /*delete region failed*/
#define  HI_ERR_OSDEX_REGION_FAILED_DELETEALL      HI_DEF_ERR(HI_ID_OSDEX, HI_LOG_LEVEL_ERROR, 23)     /*delete region failed*/

#define  HI_ERR_OSDEX_REGION_FAILED_FILL           HI_DEF_ERR(HI_ID_OSDEX, HI_LOG_LEVEL_ERROR, 26)    /*fill region failed*/
#define  HI_ERR_OSDEX_REGION_FAILED_CLEAR          HI_DEF_ERR(HI_ID_OSDEX, HI_LOG_LEVEL_ERROR, 27)    /*clear region failed*/

#define  HI_ERR_OSDEX_REGION_FAILED_UPDATE         HI_DEF_ERR(HI_ID_OSDEX, HI_LOG_LEVEL_ERROR, 28)    /*update region failed*/
#define  HI_ERR_OSDEX_REGION_FAILED_UPDATEALL      HI_DEF_ERR(HI_ID_OSDEX, HI_LOG_LEVEL_ERROR, 29)    /*update all region failed*/

typedef union hiOSDEX_COLOR_S
{
    struct
    {
        HI_U8 red       :8;
        HI_U8 green     :8;
        HI_U8 blue      :8;
        HI_U8 alpha     :8;
    }bits;

    HI_U32 ulColor;
}OSDEX_COLOR_S;

/* OSDEX device attribute */
typedef struct hiOSDEX_ATTR_S
{
    HI_BOOL        enable;
    HI_U32         diaphaneity;
    OSDEX_COLOR_S  color_key;
    PIXEL_FORMAT_E pixel_format;
    RECT_S         rectangle;
}OSDEX_ATTR_S;

typedef HI_U32 OSDEX_REGION_HANDLE;

typedef struct hiOSDEX_REGION_S
{
    HI_BOOL         enable;
    HI_U32          toplevel;
    RECT_S          rectangle;

    PIXEL_FORMAT_E  fmt;
    HI_U32          Bpp;
    HI_U32          mem_addr_phy;
    HI_U32          mem_addr_virt;
    HI_U32          mem_stride;
    HI_U32          mem_size;
} OSDEX_REGION_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __HI_COMM_OSDEX_H__ */

