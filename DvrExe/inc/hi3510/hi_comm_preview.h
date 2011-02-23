/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : Hi_comm_Preview.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/01/19
  Description   : Hi_Preview.c header file
  History       :
  1.Date        : 2006/01/19
    Author      : cchao
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_PREVIEW_H__
#define __HI_COMM_PREVIEW_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum hiVIDEO_PREVIEW_TYPE_E
{
    VIDEO_PREVIEW_TYPE_VI   = 0,   /* Show the video input data            */
    VIDEO_PREVIEW_TYPE_VENC = 1,   /* Show the video encoder's reconstruct */
    VIDEO_PREVIEW_TYPE_VDEC = 2,   /* Show the video decoder image         */
    VIDEO_PREVIEW_TYPE_BUTT
} VIDEO_PREVIEW_TYPE_E;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __HI_COMM_PREVIEW_H__ */
