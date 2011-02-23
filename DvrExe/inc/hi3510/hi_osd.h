/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_osd.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/03/10
  Description   : 
  History       :
  1.Date        : 2006/03/10
    Author      : z42136
    Modification: Created file

******************************************************************************/

#ifndef __HI_OSD_H__
#define __HI_OSD_H__

#include <hi_common.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/* Create one OSD Region */
HI_S32 HI_OSD_CreateRegion(OSD_TYPE_E enType,
                           OSD_CHN ChnId,
                           const OSD_REGION_S * pRegionParam, 
                           OSD_REGION_HANDLE * pHandle);

HI_S32 HI_OSD_CreateRegionEx(OSD_TYPE_E enType,
                             OSD_CHN ChnId,
                             const OSD_REGION_S * pRegionParam, 
                             OSD_REGION_HANDLE * pHandle);
                             
/* Delete the OSD Region */                           
HI_S32 HI_OSD_DeleteRegion(OSD_REGION_HANDLE Handle);

/* Show the OSD Region */
HI_S32 HI_OSD_ShowRegion(OSD_REGION_HANDLE Handle);

/* Hide the OSD Region */
HI_S32 HI_OSD_HideRegion(OSD_REGION_HANDLE Handle);

/* Fill the OSD Region with the u32FillColor(RGB format). */
HI_S32 HI_OSD_FillRegion(OSD_REGION_HANDLE Handle,
                         HI_U32 u32FillColor);

/* Cleaer OSD Region. The region will be transparent */
HI_S32 HI_OSD_ClearRegion(OSD_REGION_HANDLE Handle);

/* Set fill bitmap for the OSD Region */
HI_S32 HI_OSD_PutBitmapToRegion(OSD_REGION_HANDLE Handle,
                                const OSD_BITMAP_S * pBitmap,
                                HI_U32 u32StartX,
                                HI_U32 u32StartY);

/* Set the OSD Region's alpha value */
HI_S32 HI_OSD_SetRegionAlpha(OSD_REGION_HANDLE Handle, 
                             HI_U32 u32Alpha);

/* Get the OSD Region's alpha value */
HI_S32 HI_OSD_GetRegionAlpha(OSD_REGION_HANDLE Handle,
                             HI_U32 *pu32Alpha);

/* Set the OSD Region's position */
HI_S32 HI_OSD_SetRegionPosition(OSD_REGION_HANDLE Handle,
                                HI_U32 u32PosX, 
                                HI_U32 u32PosY);

/* Get the OSD Region's position */
HI_S32 HI_OSD_GetRegionPosition(OSD_REGION_HANDLE Handle,
                                HI_U32 *pu32PosX,
                                HI_U32 *pu32PosY);

/* Set the OSD Region's toplevel */
HI_S32 HI_OSD_SetRegionTopLevel(OSD_REGION_HANDLE Handle,
                                HI_U32 u32TopLevel);

/* Get the OSD Region's toplevel */
HI_S32 HI_OSD_GetRegionTopLevel(OSD_REGION_HANDLE Handle,
                                HI_U32 *pu32TopLevel);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __HI_OSD_H__ */

