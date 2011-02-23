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

#ifndef __HI_COMM_VI_H__
#define __HI_COMM_VI_H__

#include "hi_type.h"
#include "hi_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */


#define VI_INVALID_CHN        (-1UL)
#define VI_MAX_CHN_NUM        4
#define VI_OVERLAY_NUM        16

#define HI_ERR_VI_INVALID_NORM          HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 2) 
#define HI_ERR_VI_INVALID_MODE          HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 3) 
#define HI_ERR_VI_INVALID_MASTER        HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 4) 
#define HI_ERR_VI_INVALID_VISIZE        HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 5) 
#define HI_ERR_VI_INVALID_FPS           HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 6) 
#define HI_ERR_VI_INVALID_PARA          HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 7) 
#define HI_ERR_VI_INVALID_CHNMASK       HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 8)
#define HI_ERR_VI_INVALID_CHNID         HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 9)

#define HI_ERR_VI_FAILED_SETNORM        HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 11) 
#define HI_ERR_VI_FAILED_SETMODE        HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 12) 
#define HI_ERR_VI_FAILED_SETMASTER      HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 13) 
#define HI_ERR_VI_FAILED_SETFPS         HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 14) 
#define HI_ERR_VI_FAILED_SETSIZE        HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 15)
#define HI_ERR_VI_FAILED_SETCHNMASK     HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 16)
#define HI_ERR_VI_FAILED_SETVBI         HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 17)

#define HI_ERR_VI_FAILED_GETNORM        HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 21) 
#define HI_ERR_VI_FAILED_GETMODE        HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 22) 
#define HI_ERR_VI_FAILED_GETMASTER      HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 23) 
#define HI_ERR_VI_FAILED_GETFPS         HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 24)
#define HI_ERR_VI_FAILED_GETSIZE        HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 25)
#define HI_ERR_VI_FAILED_GETCHNMASK     HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 26)
#define HI_ERR_VI_FAILED_GETVBI         HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 27)

#if 0
#define HI_ERR_VI_CHN_INVALID_SIZE      HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 100) 
#define HI_ERR_VI_CHN_FAILED_SETSIZE    HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 110)
#define HI_ERR_VI_CHN_FAILED_GETSIZE    HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 120)
#define HI_ERR_VI_FAILED_GET_MAX_CHN_NUM HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 121)
#endif

#define HI_ERR_VI_INVALID_OPTION HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 30)
#define HI_ERR_VI_BUF_2SMALL     HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 31)
#define HI_ERR_VI_BUF_NOMEM      HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 32)
#define HI_ERR_VI_BUSY           HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 33)
#define HI_ERR_VI_NEED_CONF      HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 34)
#define HI_ERR_VI_NEED_SYNC      HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 35)
#define HI_ERR_VI_INVALID_CAPSEL HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 36)
#define HI_ERR_VI_NOT_OPEN       HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 37)
#define HI_ERR_VI_NOT_REGOSD     HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 38)
#define HI_ERR_VI_ALREADY_REGOSD HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 39)
#define HI_ERR_VI_NOT_ENCHN      HI_DEF_ERR(HI_ID_VIO, HI_LOG_LEVEL_ERROR, 40)

typedef enum hiVIDEO_SIZE_E
{
    VIDEO_SIZE_QCIF = 0,
    VIDEO_SIZE_CIF,
    VIDEO_SIZE_HD1,   
    VIDEO_SIZE_D1,
    VIDEO_SIZE_2CIF,  
    VIDEO_SIZE_4CIF,  
    VIDEO_SIZE_QVGA,
    VIDEO_SIZE_VGA,
    VIDEO_SIZE_SXGA,
    VIDEO_SIZE_BUTT
} VIDEO_SIZE_E;

#define IMG_QCIF_PAL_WIDTH      176
#define IMG_QCIF_PAL_HEIGHT     144
#define IMG_QCIF_PAL_STRIDE_Y   256
#define IMG_QCIF_PAL_STRIDE_C   256
#define IMG_CIF_PAL_WIDTH       352
#define IMG_CIF_PAL_HEIGHT      288
#define IMG_CIF_PAL_STRIDE_Y    512
#define IMG_CIF_PAL_STRIDE_C    512
#define IMG_HD1_PAL_WIDTH       720
#define IMG_HD1_PAL_HEIGHT      288
#define IMG_HD1_PAL_STRIDE_Y    1024
#define IMG_HD1_PAL_STRIDE_C    1024
#define IMG_D1_PAL_WIDTH        720
#define IMG_D1_PAL_HEIGHT       576
#define IMG_D1_PAL_STRIDE_Y     1024
#define IMG_D1_PAL_STRIDE_C     1024
#define IMG_2CIF_PAL_WIDTH      352  /* cif@50fps */
#define IMG_2CIF_PAL_HEIGHT     288
#define IMG_2CIF_PAL_STRIDE_Y   512
#define IMG_2CIF_PAL_STRIDE_C   512
#define IMG_4CIF_PAL_WIDTH      704
#define IMG_4CIF_PAL_HEIGHT     576
#define IMG_4CIF_PAL_STRIDE_Y   1024
#define IMG_4CIF_PAL_STRIDE_C   1024


#define IMG_QCIF_NTSC_WIDTH     176
#define IMG_QCIF_NTSC_HEIGHT    120
#define IMG_QCIF_NTSC_STRIDE_Y  256
#define IMG_QCIF_NTSC_STRIDE_C  256
#define IMG_CIF_NTSC_WIDTH      352
#define IMG_CIF_NTSC_HEIGHT     240
#define IMG_CIF_NTSC_STRIDE_Y   512
#define IMG_CIF_NTSC_STRIDE_C   512
#define IMG_HD1_NTSC_WIDTH      720
#define IMG_HD1_NTSC_HEIGHT     240
#define IMG_HD1_NTSC_STRIDE_Y   1024
#define IMG_HD1_NTSC_STRIDE_C   1024
#define IMG_D1_NTSC_WIDTH       720
#define IMG_D1_NTSC_HEIGHT      480
#define IMG_D1_NTSC_STRIDE_Y    1024
#define IMG_D1_NTSC_STRIDE_C    1024
#define IMG_2CIF_NTSC_WIDTH     352   /* cif@60fps */
#define IMG_2CIF_NTSC_HEIGHT    240
#define IMG_2CIF_NTSC_STRIDE_Y  512
#define IMG_2CIF_NTSC_STRIDE_C  512
#define IMG_4CIF_NTSC_WIDTH     704
#define IMG_4CIF_NTSC_HEIGHT    480
#define IMG_4CIF_NTSC_STRIDE_Y  1024
#define IMG_4CIF_NTSC_STRIDE_C  1024

#define IMG_DC_MAX_WIDTH        1280  
#define IMG_DC_MAX_HEIGHT       1024

#define IMG_DC_QVGA_WIDTH       320
#define IMG_DC_QVGA_HEIGHT      240
#define IMG_DC_QVGA_STRIDE_Y    512    
#define IMG_DC_QVGA_STRIDE_C    512

#define IMG_DC_VGA_WIDTH        640
#define IMG_DC_VGA_HEIGHT       480
#define IMG_DC_VGA_STRIDE_Y     1024
#define IMG_DC_VGA_STRIDE_C     1024

#define IMG_DC_SXGA_WIDTH       1280
#define IMG_DC_SXGA_HEIGHT      1024
#define IMG_DC_SXGA_STRIDE_Y    1536    
#define IMG_DC_SXGA_STRIDE_C    1536

typedef enum hiVI_MODE_E
{
    VI_MODE_BT656=0,   /* ITU-R BT.656 YUV4:2:2 */
    VI_MODE_BT601,     /* ITU-R BT.601 YUV4:2:2 */
  
    VI_MODE_DIGITAL_CAMERA,/*digatal camera*/
  
    VI_MODE_RAW_DATA,  /* CVBS and other */
    VI_MODE_BUTT
} VI_MODE_E;


#define VI_CAP_TOP          0x01    /* capture top field            */
#define VI_CAP_BOTTOM       0x02    /* capture bottom field         */
#define VI_CAP_BOTH         0x03    /* capture top and bottom field */
#define VI_CAP_FRAME        0x04    /* capture frame                */

typedef struct hiVI_CH_ATTR_S
{
    HI_U32      u32CapSel;  /* capture mode, such as  VI_CAP_TOP ,and so on       */

    HI_U32      u32StartX;  /* start position of video input*/
    HI_U32      u32StartY;
    HI_U32      u32Width;   /* size of video input         */
    HI_U32      u32Height;
} VI_CH_ATTR_S;


/* this structure define the way how to read the VI channel information from VBI , when 4 D1 is inputed */
typedef struct hiVBI_CFG_S
{
    HI_U32 u32Position;     /* reserved                            */
    HI_U32 u32Interval;     /* the lines from the begining of blank interval to VBI data line  */
    HI_U32 u32Excursion;    /*the  pixel offset from begining of horizontal valid data to VBI data*/
    HI_U32 u32Start;        /* the bits from the begining of valid VBI data to channel ID */
    HI_U32 u32Size;         /* the bits a channel ID will take                          */
    HI_U32 u32Count;        /* reserved                             */ 
    HI_U32 Reserved;
} VBI_CFG_S;


/* VI channel distributing
 * How are the macros named ?
 * VI_(I)nput_(A)nalog/(D)igital_(L)ive/(S)with
 * _(chns per field)(toatl chns ) 
 */
#define VI_IA_L11   0x0011
#define VI_IA_L12   0x0012
#define VI_IA_L22   0x0022
#define VI_IA_L24   0x0024
#define VI_IA_S12   0x0112
#define VI_IA_S14   0x0114

#define VI_ID_L11   0x1011 /* QVGA */
#define VI_ID_L12   0x2011 /* VGA  */
#define VI_ID_L13   0x3011 /* XGA  */
#define VI_ID_L14   0x4011 /* D1P   */
#define VI_ID_L15   0x5011 /* D1N   */

typedef struct hiVI_ATTR_S
{
    VIDEO_CONTROL_MODE_E    enMaster; /*if enInputMode is BT601,this field can be slave and master */

    VI_MODE_E   enInputMode; /*video input mode*/

    VIDEO_NORM_E    enInputNorm; /* video input normal */

    HI_U32 u32InputOption; /* VI channel distribute */
    
    HI_U32 u32CapSel; /* VI capture mode */
    
    HI_BOOL   bDownScaling; /*downscaling switch(1/2) in horizon*/

    HI_U32  u32Bufsize;/* the size of frame buffer */
    
} VI_ATTR_S;

typedef struct hiVI_CHN_LUM_S
{
    HI_U32 u32FramId;
    HI_U32 u32Lum;    
}VI_CHN_LUM_S;

#define VI_RGB(r,g,b) ((((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff))

#define VI_RED(c)   ((HI_U8)(((c) & 0xff0000) >> 16))
#define VI_GREEN(c) ((HI_U8)(((c) & 0xff00) >> 8))
#define VI_BLUE(c)  ((HI_U8)((c) & 0xff))

#define VI_RGB2Y(r,g,b)  ((HI_U8)((( (r)*66 +(g)*129+(b)*25) >> 8) + 16))
#define VI_RGB2U(r,g,b)  ((HI_U8)(((((b)*112-(r)*38)-(g)*74) >> 8) + 128))
#define VI_RGB2V(r,g,b)  ((HI_U8)(((((r)*112-(g)*94)-(b)*18) >> 8) + 128))

#define VI_YUV(y,u,v)  (((y) & 0xff) | (((u) & 0xff) << 8) | (((v) & 0xff) << 16))
#define VI_Y(c) ((HI_U8)((c) & 0xff))
#define VI_U(c) ((HI_U8)(((c) & 0xff00) >> 8))
#define VI_V(c) ((HI_U8)(((c) & 0xff0000) >> 16))

#define VI_OVERLAY_GREEN 0x0000FF00
#define VI_OVERLAY_BLACK 0x00000000
#define VI_OVERLAY_RED   0x00FF0000

typedef struct hiVI_OVERLAY_CONF_S
{
    VI_CHN chnId;
    HI_U32 u32Color;
    HI_U32 u32StartX;
    HI_U32 u32StartY;
    HI_U32 u32Width;
    HI_U32 u32Height;
    HI_U32 u32Reserve;
    HI_U32 u32Index;
}VI_OVERLAY_CONF_S;

typedef struct hiVI_VI2VO_S
{
   VI_CHN ViChnId;
   VO_CHN VoChnId;
}VI_VI2VO_S;

typedef int (*VI_OSD_Fun)(unsigned int , unsigned int, unsigned int reserved);

typedef struct hiVI_CHN_MKPATTR_S
{
	unsigned int u32Width;
	unsigned int u32Height;
	unsigned int u32StrdeY;
	unsigned int u32StrdeC;
}VI_CHN_MKPATTR_S;

extern int HI_MKP_VI_RegOSD(VI_CHN chn, VI_OSD_Fun pFun);
extern int HI_MKP_VI_UnRegOSD (VI_CHN chn);

extern int HI_MKP_VI_GetChnAttr (VI_CHN chn, VI_CHN_MKPATTR_S *pAttr);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* End of #ifndef__HI_COMM_VIDEO_IN_H__ */


