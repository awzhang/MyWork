/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : api_OSDEX.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/03/10
  Description   : 
  History       :
  1.Date        : 2006/03/10
    Author      : z42136
    Modification: Created file

******************************************************************************/

#ifndef __HI_OSDEXE_H__
#define __HI_OSDEXE_H__

#include <hi_common.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */



/* Open the OSDEX                                                          */
extern HI_S32 HI_OSDEX_Open(HI_VOID);

/* Close the OSDEX                                                         */
extern HI_S32 HI_OSDEX_Close(HI_VOID);

/* Set the attribute of OSDEX                                              */
extern HI_S32 HI_OSDEX_SetAttr(OSDEX_ATTR_S  *pAttr);

/* Get the attribute of OSDEX                                              */
extern HI_S32 HI_OSDEX_GetAttr(OSDEX_ATTR_S  *pAttr);

/* Create the osdex region                                                 */
extern HI_S32 HI_OSDEX_CreateRegion(
    OSDEX_REGION_S *pRegionAttr, 
    OSDEX_REGION_HANDLE *pHandle );

/* Delete the OSDEX region                                                 */
extern HI_S32 HI_OSDEX_DeleteRegion(OSDEX_REGION_HANDLE region_handle);
extern HI_S32 HI_OSDEX_DeleteAllRegion(HI_VOID);

/* Set the region attribute                                                */
extern HI_S32 HI_OSDEX_SetRegionAttr(OSDEX_REGION_HANDLE region_handle, OSDEX_REGION_S *pAttr);

/* Get the region attribute                                                */
extern HI_S32 HI_OSDEX_GetRegionAttr(OSDEX_REGION_HANDLE region_handle, OSDEX_REGION_S *pAttr);

/* Fill the OSDEX region                                                   */ 
extern HI_S32 HI_OSDEX_FillRegion(OSDEX_REGION_HANDLE region_handle, OSDEX_COLOR_S OSDEX_color);

/* Clear the OSDEX region                                                  */ 
extern HI_S32 HI_OSDEX_ClearRegion(OSDEX_REGION_HANDLE region_handle);

extern HI_S32 HI_OSDEX_UpdateRegion (OSDEX_REGION_HANDLE region_handle);
extern HI_S32 HI_OSDEX_UpdateAllRegion(HI_VOID);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __API_OSDEX_H__ */


