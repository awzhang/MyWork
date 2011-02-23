/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : Hi_comm_md.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/01/19
  Description   : Hi_MotionDetect.c header file
  History       :
  1.Date        : 2006/01/19
    Author      : cchao
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_MD_H__
#define __HI_COMM_MD_H__

#include "hi_type.h"
#include "hi_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


#define HI_DEF_ERR_MD(errid) \
        HI_DEF_ERR(HI_ID_MD,HI_LOG_LEVEL_ERROR,errid)


#define HI_ERR_MD_INVALID_PARAM           HI_DEF_ERR_MD(0x01)  /* Invalid input parament  */
#define HI_ERR_MD_INVALID_CHNNUM          HI_DEF_ERR_MD(0x02)  /* Invalid channel         */
#define HI_ERR_MD_INVALID_PTR             HI_DEF_ERR_MD(0x03)  /* Pointer is NULL         */

#define HI_ERR_MD_CHN_FAILED_GETRESULT    HI_DEF_ERR_MD(0x04)  /* Get MD value failed     */
#define HI_ERR_MD_CHN_FAILED_GETTYPE      HI_DEF_ERR_MD(0x05)  /* Get MD type failed      */
#define HI_ERR_MD_CHN_FAILED_SETTYPE      HI_DEF_ERR_MD(0x06)  /* Set MD type failed      */
#define HI_ERR_MD_CHN_FAILED_ENABLE       HI_DEF_ERR_MD(0x07)  /* Enable MD failed        */
#define HI_ERR_MD_CHN_FAILED_DISABLE      HI_DEF_ERR_MD(0x08)  /* Disable MD failed       */
#define HI_ERR_MD_CHN_FAILED_GETSTATUS    HI_DEF_ERR_MD(0x09)  /* Get MD status failed    */

#define HI_ERR_MD_CHN_NOT_UPDATE          HI_DEF_ERR_MD(0x0A)  /* No MD data available    */
#define HI_ERR_MD_CHN_NOT_NEEDGETMDINFO   HI_DEF_ERR_MD(0x0B)  /* MD data no need update  */
#define HI_ERR_MD_CHN_NOT_STARTMD         HI_DEF_ERR_MD(0x0C)  /* MD not enabled          */
#define HI_ERR_MD_CHN_NOT_STARTVENC       HI_DEF_ERR_MD(0x0D)  /* VENC channel not start  */
#define HI_ERR_MD_CHN_NOT_CREATE          HI_DEF_ERR_MD(0x0E)  /* VENC channel not create */
#define HI_ERR_MD_CHN_ALREADY_ENABLE      HI_DEF_ERR_MD(0x0F)  /* MD alread enable        */
#define HI_ERR_MD_INVALID_TYPE            HI_DEF_ERR_MD(0x10)  /* MD alread enable        */
typedef enum
{
    MD_TYPE_NULL = 0x00,
    MD_TYPE_SAD = 0x01,	/* Now, only SAD is available */
    MD_TYPE_MV  = 0x02,
    MD_TYPE_ALL = 0x03
}MD_TYPE_E;

typedef struct
{
    /* 
    ** 每个字段的高6位为整数部分，后两位为小数部分,此数除以4转换为浮点数就是
    ** 我们所需的带小数的有符号宏块运动值。
    */
	HI_S8 MVy3;    //该宏块中的第4个子宏块(大小:8*8)y方向运动矢量值 
	HI_S8 MVx3;    //该宏块中的第4个子宏块(大小:8*8)x方向运动矢量值
	HI_S8 MVy2;    //该宏块中的第3个子宏块(大小:8*8)y方向运动矢量值
	HI_S8 MVx2;    //该宏块中的第3个子宏块(大小:8*8)x方向运动矢量值
	HI_S8 MVy1;    //该宏块中的第2个子宏块(大小:8*8)y方向运动矢量值
	HI_S8 MVx1;    //该宏块中的第2个子宏块(大小:8*8)x方向运动矢量值
	HI_S8 MVy0;    //该宏块中的第1个子宏块(大小:8*8)y方向运动矢量值
	HI_S8 MVx0;    //该宏块中的第1个子宏块(大小:8*8)x方向运动矢量值
}MD_MV_T;

typedef HI_U8 MD_SAD;

#define MAX_MACROCELL_NUM   1620          /* (720*576)/(16*16)=1620 */
typedef struct hiMD_STATUS_S
{
    HI_U8   u8Mask[MAX_MACROCELL_NUM];     /* The macrocell to get MD             */
    HI_U8   u8MDValue[MAX_MACROCELL_NUM];  /* The SAD result value. Scope is 0~100 */
    MD_MV_T MV[MAX_MACROCELL_NUM];        /* the MV  value */
} MD_STATUS_S;

typedef struct hiMD_PTRSTATUS_S
{
    HI_U8   *pU8Mask;     /* pointer to area   */
    HI_U8   *pU8MDValue;  /* poniter to SAD result value. Scope is 0~100 */
    MD_MV_T *pMV;         /* pointer MV  value */
} MD_PTRSTATUS_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __HI_COMM_MD_H__ */
