/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_vo.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/01/19
  Description   : hi_video_out.c header file
  History       :
  1.Date        : 2006/01/19
    Author      : x45704
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_VO_H__
#define __HI_COMM_VO_H__

#include "hi_type.h"
#include "hi_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define VO_MAX_CHN_NUM  4

#define VO_CHN_MAX_IMG_NUM  30  /* The max channel buffer length*/

#define PIP_BUF_NUM_MAX      10 /* The max VO PIP buffer lenght */

#define MAX_FILENAME_LENGTH  20 /* the max length of filename */

#define HI_ERR_VO_DEV_NOT_OPEN      HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 1)
#define HI_ERR_VO_INVALID_CHN       HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 2)
#define HI_ERR_VO_NULL_PTR          HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 3)
#define HI_ERR_VO_INVALID_PARA      HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 4)
#define HI_ERR_VO_NOT_ENABLE        HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 5)
#define HI_ERR_VO_BUF_FULL          HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 6)
#define HI_ERR_VO_BUF_EMPTY         HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 7)
#define HI_ERR_VO_BUF_NOT_EXIST     HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 8)
#define HI_ERR_VO_IMAGE_MEM_FAILED  HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 9)
#define HI_ERR_VO_GET_IMAGEINFO_FAILED HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 10)
#define HI_ERR_VO_LOADBMP_FAILED    HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 11)
#define HI_ERR_VO_BMPSIZE_ERROR     HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 12)
#define HI_ERR_VO_OPENMEMDEVICE_FAILED HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 13)
#define HI_ERR_VO_SET_IMAGEATTR_FAILED HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 14)
#define HI_ERR_VO_NOT_ENABLE_VOCHN  HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 15)
#define HI_ERR_VO_FILENAME_OUTOFLENGTH HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 16)
#define HI_ERR_VO_GET_IMAGEATTR_FAILED HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 17)
#define HI_ERR_VO_NOT_ENOUGH_MEMORY  HI_DEF_ERR(HI_ID_VOU, HI_LOG_LEVEL_ERROR, 18)
typedef enum hiVO_MODE_E
{
    VO_MODE_BT656=0, /* ITU-R BT.656 YUV4:2:2 */
    VO_MODE_BT601,   /* ITU-R BT.601 YUV4:2:2 */
    VO_MODE_DIGITAL, /* LCD output */
    VO_MODE_BUTT
}VO_MODE_E;

typedef enum hiVO_SCALE_TYPE_E
{
    VO_SCALE_FRAME        = 0, /* Scale the image as the whole              */
    VO_SCALE_TOP_FIELD    = 1, /* Only select the top field to scale        */
    VO_SCALE_BOTTOM_FIELD = 2, /* Only select the bottom field to scale     */
    VO_SCALE_BOTH_FIELD   = 3, /* Scale the top and bottom field separately */
    VO_SCALE_TYPE_BUTT
} VO_SCALE_TYPE_E;


/* config parameter for TV mode */
typedef struct hiTV_CFG_S
{ 
	
	VIDEO_CONTROL_MODE_E	enMaster; /* if display_mode is BT601, this field can be slave or master mode, else this field has no sense */

	VIDEO_NORM_E			compose_mode;  /* output compose mode:  PAL NTSC SCREAM      */	
	
    HI_BOOL		            seriatim_mode; /* seriatim mode    */	  
	
} TV_CFG_S;

/* config paramter for LCD mode */
typedef struct hiLCD_CFG_S
{
	HI_U32 u32Lpp;	  /* active Line per panel.                 */
	HI_U32 u32Vsw;	  /* Vertical Synchronization Pulse width.  */
	HI_U32 u32Vbp;	  /* Vertical Back Porch.                   */
	HI_U32 u32Vfp;	  /* Vertical Front Porch.                  */
	
	HI_U32 u32Ppl;	  /* Pixel per line.                        */
	HI_U32 u32Hsw;	  /* Horizontal Synchronization Pulse width.*/
	HI_U32 u32Hbp;	  /* Horizontal Back Porch.                 */
	HI_U32 u32Hfp;	  /* Horizontal Front Porch.                */	
} LCD_CFG_S;

typedef enum hiVO_IMAGE_TYPE_E
{
    VO_TYPE_BMP24=0,  /* BMP IMAGE*/
    VO_TYPE_YUV420,   /* YUV4:2:0 IMAGE */
    VO_TYPE_BUTT
}VO_IMAGE_TYPE_E;

typedef struct hiVO_CHN_IMAGE_S
{
    VO_IMAGE_TYPE_E imageType;  /* the format of image, BMP can be supported now only */
    HI_CHAR  *pFilename;        /* the the filename of BMP */
    HI_U32   u32Count;          /* how long the image will be shown */
    HI_BOOL  enable;		    /* enable image-show HI_TRUE: enable, HI_FALSE: disable */
    HI_U32   u32Reserved;       /* reserved paramter */
}VO_CHN_IMAGE_S;

typedef struct hiVO_CHN_STATE_S
{
    HI_U32  u32InputFrame; /* Total frame number */
    HI_U32  u32CurFps;     /* Curent frame rate  */
    HI_U32  u32TimeSpan;   /* The secends from channel started           */
    HI_U32  u32WaitNum;    /* The frame number which is waiting for play */
} VO_CHN_STATE_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __HI_COMM_VO_H__ */

