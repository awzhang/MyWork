/******************************************************************************

Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

******************************************************************************
File Name     : hi_vi.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2005/12/27
Last Modified :
Description   : dvs hight level api .h file
Function List :
History       :
1.Date        : 2005/12/27
Author        : QuYaxin
Modification  : Created file

******************************************************************************/

#ifndef __HI_VI_H__
#define __HI_VI_H__

#include <hi_common.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/* set VI public attribute */
HI_S32 HI_VI_SetPublicAttr(const VI_ATTR_S *pAttr);

/* get VI public attribute */
HI_S32 HI_VI_GetPublicAttr(VI_ATTR_S *pAttr);

/* set VI channel public attribute */
HI_S32 HI_VI_SetCHAttr(VI_CHN chnId, const VI_CH_ATTR_S *pAttr);

/* get VI channel public attribute */
HI_S32 HI_VI_GetCHAttr(VI_CHN chnId, VI_CH_ATTR_S *pAttr);

/* set VBI attribute */
HI_S32 HI_VI_SetVBI(VBI_CFG_S *pVbiCfg);

/* get VBI attribute */
HI_S32 HI_VI_GetVBI(VBI_CFG_S *pVbiCfg);

/* get luminance information */
HI_S32 HI_VI_GetCHLum(VI_CHN chnID, VI_CHN_LUM_S *pLum);

/* start VI  */
HI_S32 HI_VI_EnableVI(HI_VOID);

/* stop VI  */
HI_S32 HI_VI_DisableVI(HI_VOID);

/* start VI channel */
HI_S32 HI_VI_EnableVIChn(VI_CHN chnId);

/* stop VI */
HI_S32 HI_VI_DisableVIChn(VI_CHN chnId);

/* start VI overlay */
HI_S32 HI_VI_EnableOverlay(VI_CHN ViChn);

/* Expand interface of HI_VI_EnableOverlay. Support multi overlay */
HI_S32 HI_VI_EnableOverlayEx(VI_CHN ViChn, HI_U32 u32Index);

/* stop VI overlay */
HI_S32 HI_VI_DisableOverlay(VI_CHN ViChn);

/* Expand interface of HI_VI_DisableOverlay. Support multi overlay */
HI_S32 HI_VI_DisableOverlayEx(VI_CHN ViChn, HI_U32 u32Index);

/* Clear all the overlay */
HI_S32 HI_VI_ClearOverlay();

/* get VI overlay attribute */
HI_S32 HI_VI_GetOverlay(VI_CHN chnID,VI_OVERLAY_CONF_S *pViOverlay);

/* Expand interface of HI_VI_GetOverlay. Support multi overlay */
HI_S32 HI_VI_GetOverlayEx(VI_CHN chnID,VI_OVERLAY_CONF_S *pViOverlay, HI_U32 u32Index);

/* set VI overlay attribute */
HI_S32 HI_VI_SetOverlay(VI_CHN chnID,VI_OVERLAY_CONF_S *pViOverlay);

/* Expand interface of HI_VI_GetOverlay. Support multi overlay */
HI_S32 HI_VI_SetOverlayEx(VI_CHN chnID,VI_OVERLAY_CONF_S *pViOverlay, HI_U32 u32Index);

/* start VI to VO */
HI_S32 HI_VI_EnVi2Vo(VI_CHN ViChnID, VO_CHN VoChnID, HI_S32 u32Flag);

/* stop VI to VO */
HI_S32 HI_VI_DisVi2Vo(VI_CHN ViChnID, VO_CHN VoChnID, HI_S32 u32Flag);

/* get VI buffer, including : buffer information¡¢Y address, C address */
HI_S32 HI_VI_GetBuf(VIDEO_BUFFER_S *pBufAttr, HI_VOID **pUserAddrY, HI_VOID **pUserAddrC);

/* release VI buffer */
HI_S32 HI_VI_PutBuf(HI_U32 bfield2);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* End of #ifndef __HI_VI_H__ */


