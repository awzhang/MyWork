/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_osd.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/03/10
  Description   : OSD common head file
  History       :
  1.Date        : 2006/03/10
    Author      : z42136
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_OSD_H__
#define __HI_COMM_OSD_H__

#include "hi_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/*OSD API ERR*/
#define  HI_ERR_OSD_INVALID_HANDLE              HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 2)     /* Invalid region handle      */
#define  HI_ERR_OSD_INVALID_PTR                 HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 3)     /* The pointer may be NULL    */
#define  HI_ERR_OSD_INVALID_LEN                 HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 4)     /* Invalid buffer length      */
#define  HI_ERR_OSD_INVALID_CHNNUM              HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 5)     /* Invalid channel NO.        */
#define  HI_ERR_OSD_INVALID_PARAM               HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 6)     /* Invalid input parament     */

/*OSD MPI ERR*/
#define  HI_ERR_OSD_FAILED_CREATE               HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 7)     /* Create region failed       */
#define  HI_ERR_OSD_FAILED_DELETE               HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 8)     /* Delete region failed       */
#define  HI_ERR_OSD_FAILED_SHOW                 HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 9)     /* Show region failed         */
#define  HI_ERR_OSD_FAILED_HIDE                 HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 10)    /* Hide region failed         */
#define  HI_ERR_OSD_FAILED_FILL                 HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 11)    /* Fill region failed         */
#define  HI_ERR_OSD_FAILED_CLEAR                HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 12)    /* Clear region failed        */
#define  HI_ERR_OSD_FAILED_SETALPHA             HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 13)    /* Set region' alpha failed   */
#define  HI_ERR_OSD_FAILED_GETALPHA             HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 14)    /* Get region' alpha failed   */
#define  HI_ERR_OSD_FAILED_SETTOPLEVEL          HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 15)    /* Set region' toplevel failed*/
#define  HI_ERR_OSD_FAILED_GETTOPLEVEL          HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 16)    /* Get region' toplevel failed*/
#define  HI_ERR_OSD_FAILED_SETPOSITION          HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 17)    /* Set region' position failed*/
#define  HI_ERR_OSD_FAILED_GETPOSITION          HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 18)    /* Get region' position failed*/
#define  HI_ERR_OSD_FAILED_PUTBITMAPTOREGION    HI_DEF_ERR(HI_ID_OSD, HI_LOG_LEVEL_ERROR, 19)    /* Put bitmap to region failed*/

#define HI_OSD_MAX_WIDTH_END      720
#define HI_OSD_MAX_HEIGHT_END     576
#define HI_OSD_MAX_WIDTH_FRONT    720
#define HI_OSD_MAX_HEIGHT_FRONT   576

typedef HI_U32 OSD_REGION_HANDLE;
typedef HI_S32 OSD_CHN;

#define OSD_ALLREGION_FREE ((OSD_REGION_HANDLE)-1)

typedef enum hiOSD_TYPE_E
{
    OSD_FRONT = 0,    /* Set OSD before encode   */
    OSD_END,          /* Set OSD when show video */
    OSD_BUTT
} OSD_TYPE_E;

typedef struct hiOSD_REGION_S
{
    /* The available rect region */
    RECT_S struValidRect;

    /*
    ** Transparence: The value 0-100 can be used
    ** 0   means all OSD image will be invisible, the background will be show whole.
    ** 100 means OSD image will cover the background whole
    */
    HI_U32 u32Alpha;

    /*
    ** The transparent color will be treated as invisible color.COLORSPACECOEF
    ** RGB format should be used for this parament.
    */
    HI_U32 u32TransparentColor; 

    /*
    ** The top level when more than one OSD region is showing.
    ** The smaller value will be show more backgroud.
    */
    HI_U32 u32TopLevel;
} OSD_REGION_S;

typedef struct hiOSD_BITMAP_S
{
    HI_U32         u32Width;       /* Bitmap's width */
    HI_U32         u32Height;      /* Bitmap's height */

    /* Bitmap's pixel format. Only PIXEL_FORMAT_RGB_888 is available */
    PIXEL_FORMAT_E enPixelFormat;  
    HI_VOID       *pData;          /* Bitmap data  */
} OSD_BITMAP_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __HI_COMM_OSD_H__ */
