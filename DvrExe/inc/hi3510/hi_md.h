/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_md.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/03/13
  Description   : 
  History       :
  1.Date         : 2006/3/13
    Author       : w60000787
    Modification : Created function

******************************************************************************/

#ifndef __HI_MD_H__
#define __HI_MD_H__

#include "hi_comm_md.h"
#include "hi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/* Set the MD(Motion Detect) type */
HI_S32 HI_MD_SetType (VENC_CHN chnId, MD_TYPE_E MDType);

/* Set the MD(Motion Detect) type */
HI_S32 HI_MD_GetType (VENC_CHN chnId, MD_TYPE_E *pMDType);
    
/* Set the MD(Motion Detect) area */
HI_S32 HI_MD_SetArea(VENC_CHN ChnID, HI_U8 u8Mask[]);

/* Get the MD area */
HI_S32 HI_MD_GetArea(VENC_CHN ChnID, HI_U8 u8Mask[]);

/* Enable the MD function */
HI_S32 HI_MD_Enable(VENC_CHN ChnID);

/* Disable the MD function */
HI_S32 HI_MD_Disable(VENC_CHN ChnID);

/* Get the VENC channel's MD enable status */
HI_S32 HI_MD_GetEnable(VENC_CHN ChnID, HI_BOOL *pbEnable);

/* Get the MD status */
HI_S32 HI_MD_GetStatus (VENC_CHN ChnID, MD_STATUS_S *pStatus);

/* Get the MD status address */
HI_S32 HI_MD_GetStatusAdr( VENC_CHN ChnID, MD_STATUS_S **ppStatus);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __HI_MD_H__ */
