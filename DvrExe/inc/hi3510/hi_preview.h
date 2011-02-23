/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : Hi_Preview.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/01/19
  Description   : Hi_Preview.c header file
  History       :
  1.Date        : 2006/01/19
    Author      : cchao
    Modification: Created file

******************************************************************************/

#ifndef __HI_PREVIEW_H__
#define __HI_PREVIEW_H__

#include <hi_common.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


/* Create the preview channel */
HI_S32 HI_PREV_CreateCH( HI_S32 chnID,
                         VIDEO_PREVIEW_TYPE_E enType,
                         RECT_S *pRect);
                         
/* Destroy the preview channel */
HI_S32 HI_PREV_DestroyCH();

/* Start the preview channel */
HI_S32 HI_PREV_StartCH();

/* Stop the preview channel */
HI_S32 HI_PREV_StopCH();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __HI_PREVIEW_H__ */
