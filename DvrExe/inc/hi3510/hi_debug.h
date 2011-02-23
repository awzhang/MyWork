/******************************************************************************
Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_debug.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2005/4/23
Last Modified :
Description   : The common debug macro defination 
Function List :
History       :
******************************************************************************/
#ifndef __HI_DEBUG_H__
#define __HI_DEBUG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#ifdef __KERNEL__
    #define HI_PRINT printk
#else
    #define HI_PRINT printf
#endif

#define RET_VOID  /* Used in void return functions */

/* Report the current position */
#define POS_REPORT()                                    \
    do{                                                 \
        HI_PRINT("  >File     : "__FILE__"\n"           \
                 "  >Function : %s\n"                   \
                 "  >Line     : %d\n"                   \
                 ,__FUNCTION__, __LINE__);              \
    }while(0)


#ifdef HI_DEBUG

#include <hi_type.h>

/* Used for HI_TRACE */
extern HI_BOOL HI_CheckLevel(HI_U32 level, HI_U32 module_id);

#define HI_INFO_LEVEL(subtype)     (0xffff& (subtype))
#define HI_WARN_LEVEL(subtype)     (0x10000 | (0xffff & subtype))
#define HI_ERR_LEVEL(subtype)     (0x20000 | (0xffff & subtype))
#define HI_FATAL_LEVEL(subtype)     (0x40000 | (0xffff & subtype))

#define HI_MOD_API       "API"
#define HI_MOD_MPI       "MPI"
#define HI_MOD_CMPI      "CMPI"
#define HI_MOD_VIO       "VIO"
#define HI_MOD_VIU       "VIU"
#define HI_MOD_DSU       "DSU"
#define HI_MOD_VOU       "VOU"
#define HI_MOD_ZSP       "ZSP"
#define HI_MOD_VENC      "VENC"
#define HI_MOD_VDEC      "VDEC"
#define HI_MOD_DRV3510   "DRV3510"
#define HI_MOD_AENC      "AENC"
#define HI_MOD_ADEC      "ADEC"
#define HI_MOD_AVENC     "AVENC"
#define HI_MOD_AVDEC     "AVDEC"

/* Using samples:   HI_ASSERT(x>y); */
#define HI_ASSERT(expr)                                     \
    do{                                                     \
        if (!(expr)) {                                      \
            HI_PRINT("\nASSERT Failure{" #expr "}\n");      \
            POS_REPORT();                                   \
        }                                                   \
    }while(0)

/* Using samples: 
 * HI_TRACE(HI_INFO_LEVEL(255), HI_ID_DRV3510, "Test %d, %s\n", 12, "Test");
**/
#define HI_TRACE(level, module_id, fmt...)                  \
    do{                                                     \
        if (HI_CheckLevel(level, module_id)){               \
            HI_PRINT("[M:%d L:%d]: ",module_id, level);     \
            HI_PRINT(fmt);                                  \
        }                                                   \
    }while(0)

/* Using samples: 
 * ASSERT_RETURN((x>y), HI_FAILURE); // For int return functions 
 * ASSERT_RETURN((x>y), RET_VOID);   // For void return functions
**/
#define ASSERT_RETURN(exp, err)                             \
    do{                                                     \
        if (!(exp)) {                                       \
            HI_PRINT("\nASSERT Failure{" #exp "}\n");       \
            HI_PRINT("  >Error    : 0x%08x\n", err);        \
            POS_REPORT();                                   \
            return err;                                     \
        }                                                   \
    }while(0)

/* Using samples:  DBG_PRINT("This is a test!\n");*/
#define DBG_PRINT(args...)                                  \
    do{                                                     \
        HI_PRINT("\nDebug Information\n");                  \
        POS_REPORT();                                       \
        HI_PRINT("  >Info     : ");                         \
        HI_PRINT(args);                                     \
    }while(0)

#else /* #ifndef HI_DEBUG*/

#define HI_ASSERT(expr)
#define HI_TRACE(level, modname, X, args...)
#define ASSERT_RETURN(exp, err) 
#define DBG_PRINT(args...)

#endif /* End of #ifdef HI_DEBUG */

/* Using samples: 
 * CHECK_RETURN((pData != NULL), HI_FAILURE, "Pointer is NULL\n");
 * This macro will be used for parament check. It's useble forever.
**/
#define CHECK_RETURN(exp, err, args...)     \
    if(!(exp))                              \
    {                                       \
        DBG_PRINT(args);                    \
        return err;                         \
    }


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_DEBUG_H__ */


