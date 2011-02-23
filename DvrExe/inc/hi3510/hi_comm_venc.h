/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_venc.h
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

#ifndef __HI_COMM_VENC_H__
#define __HI_COMM_VENC_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_struct.h"

#define VENC_MAX_NALU_NUM_IN_FRAME     256   /*max number of slice in one frame*/
#define VENC_STREAM_OFFSET             12    /*stream offset                   */

#define MAX_IMG_WIDTH           720
#define MAX_IMG_HEIGHT          288
#define MAX_IMG_STRIDE          (MAX_IMG_WIDTH + (64 - MAX_IMG_WIDTH%64))
#define MAX_IMG_SIZE            (MAX_IMG_STRIDE*MAX_IMG_HEIGHT*2)

#define MAX_STREAMBUFLEN   4 * 1024 * 1024
#define MIN_STREAMBUFLEN   100 * 1024

#define HI_ERR_VENC_NOT_OPEN                    HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 1)  
#define HI_ERR_VENC_REOPEN                      HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 2)  
#define HI_ERR_VENC_INVALID_VICHN               HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 3)  
#define HI_ERR_VENC_INVALID_VENCCHN             HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 4)  
#define HI_ERR_VENC_OPEN_DEVICE                 HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 5)  
#define HI_ERR_VENC_CHN_NOT_CREATED             HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 6)  
#define HI_ERR_VENC_MAX_CHN                     HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 7)  
#define HI_ERR_VENC_DESTROY_RUNCHN              HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 8)  
#define HI_ERR_VENC_CHN_START_ENCODE            HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 9)
#define HI_ERR_VENC_SPREPARE_ENCDATA            HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 10)
#define HI_ERR_VENC_CHN_NOT_START               HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 11)
#define HI_ERR_VENC_STOP_ENCDATA                HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 12)
#define HI_ERR_VENC_CHN_STOP_ENCODE             HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 13)
#define HI_ERR_VENC_INVALID_VENC_CONFIG         HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 14)
#define HI_ERR_VENC_MAX_STREAM_NUM              HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 15)
#define HI_ERR_VENC_MODE_NOT_MATCH              HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 16)
#define HI_ERR_VENC_CHN_ALREADY_PLAY            HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 17)    
#define HI_ERR_VENC_STATIC_PARAM                HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 18)    

#define HI_ERR_VENC_UNSUPPORT_CODEC_TYPE        HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 19)    

/*start by v, cannot stop by av, vice versa*/
#define HI_ERR_VENC_CHN_STOP_PRIVILEGE          HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 21)    
#define HI_ERR_VENC_CHN_GETSTREAM_PRIVILEGE     HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 22)    
#define HI_ERR_VENC_CHN_GETSTREAM               HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 23)    
#define HI_ERR_VENC_CHN_RELEASESTREAM           HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 24)    
#define HI_ERR_VENC_CHN_REQUEST_I_FRAME         HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 25)
#define HI_ERR_VENC_CHN_RELEASEBUFS             HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 26)
#define HI_ERR_VENC_CHN_CLOSEFD                 HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 27)
#define HI_ERR_VENC_CHN_ATTR_SET                HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 28)
#define HI_ERR_VENC_CHN_ATTR_GET                HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 29)
#define HI_ERR_VENC_CHN_BRC_SET                 HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 30)
#define HI_ERR_VENC_CHN_BRC_GET                 HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 31)
#define HI_ERR_VENC_CHN_PREPAREBUFS             HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 32)


#define HI_ERR_JPEG_INVALID_PTR                 HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 33)  
#define HI_ERR_JPEG_INVALID_QVALUE              HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 34)  
#define HI_ERR_JPEG_INVALID_CHNNUM              HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 35)  
#define HI_ERR_JPEG_CHN_FAILED_GETVIDEO         HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 36)  
#define HI_ERR_VENC_CHN_FAILED_GETVIDEO         HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 37)  
#define HI_ERR_VENC_CHN_NOT_GETVIDEO            HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 38)  

#define HI_ERR_VENC_INVALID_LEN                 HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 40)  
#define HI_ERR_VENC_INVALID_FRAMETYPE           HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 41)  
#define HI_ERR_VENC_INVALID_PTR                 HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 43)  

#define HI_ERR_VENC_CHN_FAILED_INSERTUSERINFO   HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 44)  
#define HI_ERR_VENC_VIMASK                      HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 45)
#define HI_ERR_VENC_INVALID_CONFIG_TYPE         HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 46)
#define HI_ERR_VENC_MAX_CAP                     HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 47)  

#define HI_ERR_VENC_INVALID_SIZE                HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 48)  
#define HI_ERR_VENC_INVALID_BPS                 HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 49)  
#define HI_ERR_VENC_INVALID_FPS                 HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 50)  
#define HI_ERR_VENC_INVALID_GOP                 HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 51)  
#define HI_ERR_VENC_INVALID_MAXQP               HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 52)  
#define HI_ERR_VENC_INVALID_MODE                HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 53)  

#define HI_ERR_VENC_CHN_ATTR_WIDTHFORBID        HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 54) 
#define HI_ERR_VENC_CHN_ATTR_HEIGHTFORBID       HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 55) 
#define HI_ERR_VENC_CHN_ATTR_MODEFORBID         HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 56) 
#define HI_ERR_VENC_INVALID_BASEQP              HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 57) 
#define HI_ERR_VENC_INVALID_MAXBPS              HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 58) 
#define HI_ERR_VENC_INVALID_GRAPHIC_FRAMERATE   HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 59) 
#define HI_ERR_VENC_CHN_ATTR_FORBIDEN           HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 60) 


#define HI_ERR_VENC_CHN_BIND                    HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 61) 
#define HI_ERR_VENC_CODEC_TYPE                  HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 62) 
#define HI_ERR_VENC_INVALID_PARA                HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 63) 
#define HI_ERR_VENC_CHN_OVERFLOW                HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 64) 
#define HI_ERR_VENC_BUF_EMPTY                   HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 65) 

#define HI_ERR_VENC_CHN_WMSETATTR_FORBIDDEN     HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 66) 
#define HI_ERR_VENC_CHN_WMSETATTR_FAILED        HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 67) 
#define HI_ERR_VENC_CHN_WMSTART_FORBIDDEN       HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 68) 
#define HI_ERR_VENC_CHN_WMSTART_FAILED          HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 69) 
#define HI_ERR_VENC_CHN_WMGETATTR_FAILED        HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 70) 
#define HI_ERR_VENC_CHN_INVALID_WMDENSITY       HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 71) 
#define HI_ERR_VENC_INVALID_STRATEGY            HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 72) 
#define HI_ERR_VENC_CHN_SETBUFFLEVEL_FAILED     HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 73) 
#define HI_ERR_VENC_CHN_GETBUFFLEVEL_FAILED     HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 74) 

#define HI_ERR_VENC_INVALID_CAPSELMODE          HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 75)
#define HI_ERR_VENC_INVALID_STATICPARAM         HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 76)

#define HI_ERR_VENC_INVALID_STREAMBUFFLEN       HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 77)
#define HI_ERR_VENC_INVALID_VENCEXTYPE          HI_DEF_ERR(HI_ID_VENC, HI_LOG_LEVEL_ERROR, 78)
typedef enum hiVENC_FORMAT_E
{
    VENC_FORMAT_H261  = 0,  /*H261  */
    VENC_FORMAT_H263  = 1,  /*H263  */
    VENC_FORMAT_MPEG2 = 2,  /*MPEG2 */
    VENC_FORMAT_MPEG4 = 3,  /*MPEG4 */
    VENC_FORMAT_H264  = 4,  /*H264  */
    VENC_FORMAT_MJPEG = 5,  /*MOTION_JPEG*/
    VENC_FORMAT_BUTT
} VENC_FORMAT_E;

typedef struct hiVENC_INIT_S
{
    VENC_FORMAT_E enVencType;   /*encode type*/
    HI_U32        reserved;     /*reserved*/
} VENC_INIT_S;

typedef enum hiVENC_CONFIG_TYPE_E
{
    VENC_PICWIDTH = 0,
    VENC_PICHEIGHT,
    VENC_CAPSEL,
    VENC_STATICPARAM,
    VENC_TARGET_BITRATE,
    VENC_TARGET_FRAMERATE,
    VENC_GOP,
    VENC_MAX_QP,
    VENC_PREFRAMERATE,
    VENC_STRATEGY,
    VENC_MODE,
    VENC_BASEQP,
    VENC_MAXBPS,
    VENC_UMV,
    VENC_DF,
    VENC_MQ,
    VENC_UPDATETIME,
    VENC_CONFIG_ALL_H264,
    VENC_CONFIG_ALL_H263,
    VENC_CONFIG_BUTT
} VENC_CONFIG_TYPE_E;

typedef struct hiVENC_CONFIG_TYPE_S
{
    VENC_FORMAT_E enVencType;   /*encode type*/
    VENC_CONFIG_TYPE_E attrType;
} VENC_CONFIG_TYPE_S;
typedef enum hiVENC_BITRATE_MODE_E
{
    VENC_CBR_MODE = 0,
    VENC_VBR_MODE,
    VENC_BITRATE_MODE_BUTT                 
}VENC_BITRATE_MODE_E;

typedef enum hiVENC_GRAPHIC_FRAMERATE_E
{
    VENC_PREFERGRAPHIC_MODE = 0,  /* graphic preference    */
    VENC_PREFERFRAMERATE_MODE,    /* frame rate preference */
    VENC_GRAPHIC_FRAMERATE_BUTT                 
}VENC_GRAPHIC_FRAMERATE_E;

typedef enum hiVENC_STREAM_TYPE_E
{
    VENC_STREAM_SLICE = 0,    /*slice*/
    VENC_STREAM_FRAME,        /*frame*/
    VENC_STREAM_PRIVATEDATA,  /*private data*/
    VENC_STREAM_BUTT
} VENC_STREAM_TYPE_E;

typedef enum hiVENC_WMDENSITY_E
{
    VENC_WMDENSITY_LOW = 0, /*water mark density low */
    VENC_WMDENSITY_MIDDLE,  /*water mark density middle*/
    VENC_WMDENSITY_HIGH,    /*water mark density high*/
    VENC_WMDENSITY_BUTT     
} VENC_WMDENSITY_E;

typedef enum hiVENC_CONFIGEX_TYPE_E
{
    VENC_EXATTR_BPS_CTL = 0,
    VENC_EXATTR_FIELD_SHAM,
    VENC_EXATTR_CHROMA_REDUCE,
    VENC_EXATTR_REORDER,
    VENC_EXATTR_FPS_FIX,
    VENC_EXATTR_ALLPIC_MOTION_ANALYSIS,
    VENC_EXATTR_LOSEFIELD_BITS,
    VENC_EXATTR_LOSEFIELD_MOTION,
    VENC_EXATTR_LOSEFIELD_VIBUF,
    VENC_EXATTR_DENOISE_CHOICE,
    VENC_EXATTR_MOTION_ANALYSIS_VERT,
    VENC_EXATTR_BUTT
}VENC_CONFIGEX_TYPE_E;

typedef struct hiIO_VECTOR_S{
    HI_VOID *pAddr;
    HI_S32   u32Len;
}IO_VECTOR_S;

typedef struct hiVENC_DATA_S{
    HI_VOID *pAddr;
    HI_U32   u32Len;
}VENC_DATA_S;

typedef struct hiVENC_STREAM_INFO_S
{
    HI_U64   u64PTS;         /*time stamp*/
    HI_U16   u16IFrameFlag;  /*the flag of I frame  */
    HI_U16   reserved;       /*reserved*/

    HI_U32   u32Seq;         /*the serial number of stream*/
    VENC_CHN ChnID;          /*venc channel number*/
} VENC_STREAM_INFO_S;

typedef struct hiUSER_BUFFER_S{
    /* bit stream is put in the front, following is MD data */
    IO_VECTOR_S *pstruVector; 

     /* slice count that memory pointed by "pstruVector" contains */
    HI_U32       u32Count;
    HI_U32       u32BSBufCount;
    HI_U32       u32MDBufCount;
    HI_U32       u32Index;
    HI_BOOL      bIsField;
}USER_BUFFER_S;

typedef struct hiPACK_PROP_S
{
    HI_U16 FrameLastPacketFlag :1;     /* Last stream packet flag for one frame.        0: Normal stream packet 
                                                                                        1: The last stream packet*/
    HI_U16 LastByteOverLapFlag :1;     /* Last byte valid flag when saving pure stream  0: Last byte is not valid, data length should minus one
                                                                                        1: Last byte is valid, data length should not be changed    */
    HI_U16 RTPModeAFlag        :1;     /* Flag for 2190 PayLoad Mode A                  0: Mode B               1: Mode A */
    HI_U16 CheckSumEnableFlag  :1;     /* Flag for Check Sum Mode Enable                Not Used */
    HI_U16 ResvFlags           :12;    /* Reserve flags                        */
}PACK_PROP_S;

typedef struct hiVENC_STREAM_S
{ 
    HI_U32              u32DataNum;    /* slice number in one frame */
    HI_U32              u32DataLen;    /* all the stream length in one frame */
    VENC_DATA_S         struData[VENC_MAX_NALU_NUM_IN_FRAME];/*stream data address*/
    PACK_PROP_S         struProp[VENC_MAX_NALU_NUM_IN_FRAME];/*field for H.263 protocol*/  
    VENC_STREAM_INFO_S  struDataInfo;  /*stream infomation               */
    USER_BUFFER_S       struUbuff;     
} VENC_STREAM_S;

#ifndef __KERNEL__
struct usr_list_head {
    struct usr_list_head *next, *prev;
};
#else
#include <linux/list.h>
#endif

typedef struct hiVIDEO_BUFFER_S
{ 
    HI_U32 ChanID;                  /* video encode channel id */

    PIXEL_FORMAT_E  enPixelFormat;  /* the format of video input */
    HI_U32          u32Width;       /* the width of video input */
    HI_U32          u32Height;      /* the height of video input */

    #define VIDEO_FIELD_TOP     0x01    /* even field */
    #define VIDEO_FIELD_BOTTOM  0x02    /* odd  field */
    #define VIDEO_FIELD_FRAME   0x04    /* frame field */
    HI_U32          u32Field;
    HI_U32          u32DownScaled;  /* the flag down scaling */

    /* 
    ** if format is RGB or PackageYUV, u32PhyAddr[0] make effect

    ** if format is Semi-Planar YUV , u32PhyAddr[0~1] make effect£¬
    
    ** if format is Planar , u32PhyAddr[0~2] make effect£¬

    */
    HI_U32   u32PhyAddr[3];        /* physical address */
    HI_VOID *pVirAddr[3];          /* vitual address */
    HI_U32   u32Stride[3];         /* stride of line */

    HI_U64  pts;
    HI_U32  timeRef;

    HI_U32  privateData;    
    HI_U32  owner;   

    /* ATTENTION: The defination of list must be consistent. */
    #ifndef __KERNEL__
        struct usr_list_head list;
    #else
        struct list_head list;
    #endif
} VIDEO_BUFFER_S;


#define VENC_MAX_BITRATE   4000000
#define VENC_MIN_BITRATE   16000
#define VENC_MAX_PAL_FRAMERATE 25
#define VENC_MAX_NTSC_FRAMERATE 30
#define VENC_MIN_FRAMERATE 1

#define VENC_MIN_DELAY 10
#define VENC_MAX_DELAY 4000

#define VENC_MAX_MAXQP 51
#define VENC_MIN_MAXQP 10

#define VENC_MAX_GOP 0x7FFF
#define VENC_MIN_GOP 0

#define VENC_CAP_DEFAULT      0x07    /* capture all the data from VI */
#define VENC_CAP_TOP          0x01    /* capture top field            */
#define VENC_CAP_BOTTOM       0x02    /* capture bottom field         */
#define VENC_CAP_BOTH         0x03    /* capture top and bottom field */
#define VENC_CAP_FRAME        0x04    /* capture frame                */

#define VENC_STRATEGY_DEFAULT 0
#define VENC_STRATEGY_ONE 1
#define VENC_STRATEGY_TWO 2
typedef struct hiVENC_CONFIG_H264_S
{
    HI_U32  u32PicWidth;         /* the width of stream£¬align 16 pixel , larger than 64         */ 
    HI_U32  u32PicHeight;        /* the width of stream, align 16 pixel , larger than 64         */
    HI_U32  u32CapSel;            /* the way that Video encoder how to deal with the data from VI, 
                                    the default value:  VENC_CAP_DEFAULT                         */
    HI_U32  u32StaticParam;      /* set the graphic quality of image with static mode (0..4095, default:1800)*/
        
    HI_U32  u32TargetBitrate;    /* target bitrate(106,000-4000,000), change freedom             */
    HI_U32  u32TargetFramerate;  /* target frame rate (1-30), change freedom                     */
    HI_U32  u32Gop;              /* interval of I frame(0-32767), change freedom                 */
    HI_U32  u32MaxQP;            /* max QP(10£­51), change freedom                               */
    HI_U32  u32PreferFrameRate;  /* mode switch, 1: frame rate preference£¬
                                                 0: quality preference change freedom            */
    HI_U32  u32Strategy;         /* strategy for codeing,
                                               VENC_STRATEGY_DEFAULT : encoder select the strategy
                                 */
    HI_U32  u32VencMode;         /* encode mode, 0 - CBR, 1 - VBR                                */

    HI_U32  u32BaseQP;           /* 0- maxQP£¬change freedom                                     */
    HI_U32  u32MaxBitRate;       /* max bitrate(16,000-4000,000) >= targetBitRate£¬change freedom*/

} VENC_CONFIG_H264_S;

typedef struct hiVENC_CONFIG_H263_S
{
    HI_U32  u32PicWidth;         /* the width of stream£¬align 16 pixel */ 
    HI_U32  u32PicHeight;        /* the width of stream, align 16 pixel */
    HI_U32  u32CapSel;            /* the way that Video encoder how to deal with the data from VI */
    
    HI_U32  u32TargetBitrate;    /* target bitrate              */
    HI_U32  u32TargetFramerate;  /* target frame rate     */
    HI_U32  u32MaxQP;            /* max Quantization parameter */

    /* Fields for H.263 protocol particular configuration*/    
    HI_U32  u32UMV;              /* Unrestricted Motion Vector (UMV) mode enable flag        */
    HI_U32  u32DF;               /* Deblocking Filter          (DF ) mode enable flag        */
    HI_U32  u32MQ;               /* Modified Quantization      (MQ ) mode enable flag        */
    HI_U32  u32PreferFrameRate;  /* mode switch£º£±frame rate preference£¬0: quality preference*/
    HI_U32  UpdateTime;          /* update time (£±¡«£´)*/
    HI_U32  reserved;            /* reserved                        */
    HI_U32  reserved1;           /* reserved1                       */
} VENC_CONFIG_H263_S;

typedef struct hiVENC_CONFIG_MJPEG_S
{
    HI_U32  u32PicWidth;         /*the width of stream £¬ align 16 pixel*/ 
    HI_U32  u32PicHeight;        /*the height of stream £¬align 16 pixel*/

    HI_U32  u32MAXQP;            /* max Quantization parameter          */
    HI_U32  u32TargetFramerate;  /* target frame rate                   */
} VENC_CONFIG_MJPEG_S;



typedef union  hiVENC_CONFIG_U
{
    VENC_CONFIG_H263_S h263;
    VENC_CONFIG_H264_S h264; 
    VENC_CONFIG_MJPEG_S mjpeg;
} VENC_CONFIG_U;

typedef struct VENC_CONFIG_S
{
    VENC_FORMAT_E enVencType; /*encoding format */
    HI_VOID * pValue;         /*point to the structure of  attribute*/
}VENC_CONFIG_S;

typedef struct hiVENC_VALUE_S
{
    HI_U32 type;
    HI_S32 value;
} VENC_VALUE_S;

typedef struct hiVENC_USRINFO_CONFIG_S
{
    HI_U8 *pUserData;/*the address of user data                   */
    HI_U32 len;      /*the user data length£¬max length is 256 byte*/
}VENC_USRINFO_CONFIG_S;
typedef struct hiVENC_STREAM_BUFFER_CONFIG_S
{    
    HI_U32      u32BufSize;
    HI_BOOL     bIsFrame;    
} VENC_STREAM_BUFFER_CONFIG_S;

typedef struct venc_capability_struct
{
    HI_U32 u32MaxSupportChnNum;   /* the max number that the encoder can support */
    HI_U32 u32VencVersion;        /* encode version information  */
    VENC_FORMAT_E enVencType;     /* video encode type           */                 
}VENC_CAPABILITY_S;

typedef struct hiVENC_WATERMARK_S
{
    HI_CHAR u8Key[8];          /*water mark key, length is 8*/
    HI_CHAR u8Symbol[16];      /*water mark symbol, lenghth is 16*/
    VENC_WMDENSITY_E Density;  /*water mark density*/
} VENC_WATERMARK_S;

typedef struct hiVENC_STATISTIC_S
{
    HI_U32              u32TotalFrame; /* Total frame number          */
    HI_U64              u64TotalByte;  /* Total byte of stream(Byte)  */
    HI_U32              u32CurFps;     /* Curent frame rate           */
    HI_U32              u32CurBps;     /* Curent bitrate(Kbps)        */
    HI_U32              u32TimeSpan;   /* The secends from channel started */
} VENC_STATISTIC_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HI_COMM_VENC_H__ */
