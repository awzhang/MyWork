/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_firmware.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/1/13
  Last Modified :
  Description   : hi_dvs_venc.c header file
  Function List :
  History       :
  1.Date        : 2006/1/13
    Author      : qushen
    Modification: Created file

******************************************************************************/

#ifndef __HI_FIRMWARE_H__
#define __HI_FIRMWARE_H__

#include <hi_common.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/* Get the firmware information */
HI_S32 HI_SYS_GetFirmwareInfo(FIRMWARE_INFO_S *pInfo);

/* Load the firmware */
HI_S32 HI_SYS_LoadFirmware();

/* Init the firmware */
HI_S32 HI_SYS_InitFirmware();

/* Firmware self test */
HI_S32 HI_SYS_TestFirmware();

/* Get the product version information */
HI_S32 HI_SYS_GetVersionInfo(VERSION_INFO_S *pVersion);

/* Get the product version information */
HI_S32 HI_SYS_GetVersionString(VERSION_STRING_S *pVersion);

/* Get the msp library build time */
const HI_CHAR *HI_SYS_GetMspBuildTime();

/* Get the KO version and build time */
HI_S32 HI_SYS_GetKOInfo(char ver[80], char cTime[80]);

/* set/get register: 1bit a time */
HI_S32 HI_SYS_SetRegBit(HI_U32 addr, HI_U32 bit, HI_U32 value);
HI_S32 HI_SYS_GetRegBit(HI_U32 addr, HI_U32 bit, HI_U32 *pValue);

/* set/get register: 32bits a time */
HI_S32 HI_SYS_SetReg(HI_U32 addr, HI_U32 value);
HI_S32 HI_SYS_GetReg(HI_U32 addr, HI_U32 *pValue);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HI_COMM_FIRMWARE_H__ */
