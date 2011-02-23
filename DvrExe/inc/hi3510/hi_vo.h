/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_vo.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/01/19
  Description   : hi_video_out.c header file
  History       :
  1.Date        : 2006/01/19
    Author      : x45704
    Modification: Created file

******************************************************************************/

#ifndef __HI_VIDEO_OUT_H__
#define __HI_VIDEO_OUT_H__

#include <hi_common.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define VO_ERR_RETURN(err_no) return (err_no)

/*VO public attribute*/
typedef struct hiVO_PUBLIC_ATTR_S
{
    HI_BOOL enable;             /* enable VO switch HI_TRUE: enable, HI_FALSE: disable */	
    
    HI_BOOL   bOutputScale;     /* scale flag, HI_TRUE: need scale(used for single channel), HI_FALSE: not need scale */
    VO_MODE_E display_mode;     /* display mode*/

    TV_CFG_S  tv_config;        /* if display_mode is VO_MODE_BT656 or VO_MODE_BT601, this field make effect */
    LCD_CFG_S lcd_config;       /* if display_mode is VO_MODE_DIGITAL, this field make effect */

    HI_U32    background_color; /* backgrount color RGB(8BIT R  8BIT G  8BIT  B)	the low 24 bit make effect */
    HI_U32    vo_chn_img_num;   /* The channel buffer length */
    HI_U32    pip_buf_num;      /* The PIP buffer length */
}VO_PUBLIC_ATTR_S;


/*VO channel attribute*/
typedef struct hiVO_CHN_ATTR_S
{
    HI_BOOL enable;			/* enable VO channel switch HI_TRUE: enable, HI_FALSE: disable */

    RECT_S  rectangle;		/* dislay region */
    HI_BOOL zoom_enable;    /* zoom switch, HI_TRUE: zoom£¬HI_FALSE: not zoom */
}VO_CHN_ATTR_S;

/* init VO device */
HI_S32 HI_VO_Init (HI_VOID);

/* get VO public attribute*/
HI_S32 HI_VO_GetPublicAttr (VO_PUBLIC_ATTR_S *pAttr);

/* set VO public attribute */
HI_S32 HI_VO_SetPublicAttr(VO_PUBLIC_ATTR_S *pAttr);

/* get VO public attribute */ 
HI_S32 HI_VO_GetChnAttr (VO_CHN  ChanID, VO_CHN_ATTR_S *pAttr);

/* set VO channel attribute */
HI_S32 HI_VO_SetChnAttr (VO_CHN  ChanID, VO_CHN_ATTR_S *pAttr);

/* set the image attribute*/
HI_S32 HI_VO_SetImageAttr (VO_CHN VoChn, VO_CHN_IMAGE_S *pImageAttr);

/* get the image attribute*/
HI_S32 HI_VO_GetImageAttr (VO_CHN VoChn, VO_CHN_IMAGE_S *pImageAttr);

/* send image data to VO */
HI_S32 HI_VO_SendData(VO_CHN ChanID, VDEC_FRAMEINFO_S *pframe);

/*
** Set the clip window. If the width or height is 0, means not clip.
** You should be sure that the window is within the image. 
*/
HI_S32 HI_VO_SetClipWindow(VO_CHN ChanID, RECT_S *pRect);

/* Get the clip window */
HI_S32 HI_VO_GetClipWindow(VO_CHN ChanID, RECT_S *pRect);

/* Set the scale type. The default type is VO_SCALE_FRAME. */
HI_S32 HI_VO_SetScaleType(VO_CHN ChanID, VO_SCALE_TYPE_E enType);

/* Get the scale type */
HI_S32 HI_VO_GetScaleType(VO_CHN ChanID, VO_SCALE_TYPE_E *penType);

/* Set the lost repair function state */
HI_S32 HI_VO_SetLostRepair(VO_CHN ChanID, HI_BOOL bRepair);

/* Get the lost repair function state */
HI_S32 HI_VO_GetLostRepair(VO_CHN ChanID, HI_BOOL *pRepair);

/* Set the frame rate. 0 means no limited */
HI_S32 HI_VO_SetFrameRate(VO_CHN ChanID, HI_U32 u32FrameRate);

/* Get the frame rate. 0 means no limited */
HI_S32 HI_VO_GetFrameRate(VO_CHN ChanID, HI_U32 *pu32FrameRate);

/* Get the channel state, such as the frame number which is waiting for show */
HI_S32 HI_VO_GetChnState(VO_CHN ChanID, VO_CHN_STATE_S *pState);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __HI_VIDEO_OUT_H__ */

