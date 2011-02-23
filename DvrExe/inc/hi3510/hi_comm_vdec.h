/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_vdec.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/05/17
  Description   : 
  History       :
  1.Date        : 2006/05/17
    Author      : h48078
    Modification: Created file

******************************************************************************/

#ifndef  __HI_COMM_VDEC_H__
#define  __HI_COMM_VDEC_H__

#include "hi_type.h"
#include "hi_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

//#define UT_TEST   /* Just for test */

#define ADDR HI_U32

#define PADDR HI_U32*

#define VDEC_PRIDATA_MAX_SIZE 256 /* max length of private data             */

#define VDEC_MAX_CHNUM  4         /* the channel number the video decoder can support           */

#define VDEC_CAPLENGTH 100        

#define VDEC_VERSIONLENGTH 20     

typedef struct hiVDEC_H264CAPABILITY_S{
    HI_U8 profile;              /*0:baseline 1:mainprofile  */
    HI_U8 level;                /*4:4. High 4 bits indicate the integer part, and low 4 bits indicate the decimal part. For example, 0x22 represents level2.2.*/
    HI_U8 subqcif;              /*SubQCIF£º1 support, 0 not support   */
    HI_U8 qcif;                 /*QCIF£º1 support, 0 not support    */
    HI_U8 cif;                  /*CIF£º1 support, 0 not support    */
    HI_U8 fourcif;              /*4CIF£º1 support, 0 not support    */
    HI_U8 sixteencif;           /*16CIF£º1 support, 0 not support    */
    HI_U8 lostpacket;           /*lose packet:£º1 support, 0 not support    */
    HI_U8 upperbandwidth;       /*bandwidth upper limit(kbps)         */
    HI_U8 lowerbandwidth;       /*bandwidth lower limit(kbps)         */
    HI_U8 palfps;               /*frame rate in PAL norm(fps)      */
    HI_U8 ntscfps;              /*frame rate in NTSC norm(fps)     */
}VDEC_H264CAPABILITY_S;         /*Description structure of the H264 private capacity    */

typedef struct hiVDEC_MPEG2CAPABILITY_S{
    HI_U8 profile;              /*0:baseline 1:mainprofile  */
    HI_U8 level;                /*LEVEL                     */
    HI_U8 subqcif;              /*SubQCIF£º1 support, 0 not support  */
    HI_U8 qcif;                 /*QCIF£º1 support, 0 not support  */
    HI_U8 cif;                  /*CIF£º1 support, 0 not support  */
    HI_U8 fourcif;              /*4CIF£º1 support, 0 not support  */
    HI_U8 sixteencif;           /*16CIF£º1 support, 0 not support  */
    HI_U8 lostpacket;           /*lose packet£º1 support, 0 not support  */
    HI_U8 upperbandwidth;       /*bandwidth upper limit(kbps)        */
    HI_U8 lowerbandwidth;       /*bandwidth upper limit(kbps)        */
    HI_U8 palfps;               /*frame rate in PAL norm(fps)      */
    HI_U8 ntscfps;              /*frame rate in PAL norm(fps)      */
}VDEC_MPEG2CAPABILITY_S;        /*Description structure of the MPEG2 private capacity    */

typedef struct hiVDEC_H261CAPABILITY_S{
    HI_U8 subqcif;              /*SubQCIF£º1 support, 0 not support  */
    HI_U8 qcif;                 /*QCIF£º1 support, 0 not support  */
    HI_U8 cif;                  /*CIF£º1 support, 0 not support  */
    HI_U8 fourcif;              /*4CIF£º1 support, 0 not support  */
    HI_U8 sixteencif;           /*16CIF£º1 support, 0 not support  */
    HI_U8 lostpacket;           /*lose packet£º1 support, 0 not support  */
    HI_U8 upperbandwidth;       /*bandwidth upper limit(kbps)        */
    HI_U8 lowerbandwidth;       /*bandwidth upper limit(kbps)        */
    HI_U8 palfps;               /*frame rate in PAL norm(fps)      */
    HI_U8 ntscfps;              /*frame rate in NTSC norm(fps)      */
}VDEC_H261CAPABILITY_S;         /*Description structure of the H261 private capacity    */

typedef struct hiVDEC_H263CAPABILITY_S{
    HI_U8 mq_mode;         /*1 support ,0 not support:Optional Modified Quantization (MQ) mode (see Annex T)*/
    HI_U8 umv_mode;        /*1 support ,0 not support:Optional Unrestricted Motion Vector (UMV) mode (see Annex D)*/
    HI_U8 df_mode;         /*1 support ,0 not support:Optional Deblocking Filter (DF) mode (see Annex J)*/
    HI_U8 pb_frame_mode;   /*1 support ,0 not support:Optional PB-frames mode (see Annex G)*/
    HI_U8 sac_mode;        /*1 support ,0 not support:Optional Syntax-based Arithmetic Coding (SAC) mode (see Annex E)*/
    HI_U8 ap_mode;         /*1 support ,0 not support:Optional Advanced Prediction (AP) mode (see Annex F)*/
    HI_U8 aic_mode;        /*1 support ,0 not support:Optional Advanced INTRA Coding (AIC) mode (see Annex I)*/
    HI_U8 ss_mode;         /*1 support ,0 not support:Optional Slice Structured (SS) mode (see Annex K)*/
    HI_U8 isd_mode;        /*1 support ,0 not support:Optional Independent Segment Decoding (ISD) mode (see Annex R)*/
    HI_U8 aiv_mode;        /*1 support ,0 not support:Optional Alternative INTER VLC (AIV) mode (see Annex S)*/
    HI_U8 rpr_mode;        /*1 support ,0 not support:Optional Reference Picture Resampling (RPR) mode (see Annex P)*/
    HI_U8 rru_mode;        /*1 support ,0 not support:Optional Reduced-Resolution Update (RRU) mode (see Annex Q)*/
    HI_U8 cpfmt;           /*1 support ,0 not support:Custom Picture Format (CPFMT)*/
    HI_U8 uui;             /*1 support ,0 not support:Unlimited Unrestricted Motion Vectors Indicator (UUI)*/
    HI_U8 sss;             /*1 support ,0 not support:Slice Structured Submode bits (SSS)*/
    HI_U8 subqcif;         /*SubQCIF:1 support ,0 not support */
    HI_U8 qcif;            /*QCIF:1 support ,0 not support     */
    HI_U8 cif;             /*CIF:1 support ,0 not support     */
    HI_U8 fourcif;         /*4CIF:1 support ,0 not support    */
    HI_U8 sixteencif;      /*16CIF:1 support ,0 not support    */
    HI_U8 lostpacket;      /*lose packet£º1 support, 0 not support  */
    HI_U16 upperbandwidth; /*bandwidth upper limit(kbps)        */
    HI_U16 lowerbandwidth; /*bandwidth upper limit(kbps)        */
    HI_U8 palfps;          /*frame rate in PAL norm(fps)      */
    HI_U8 ntscfps;         /*frame rate in NTSC norm(fps)      */
}VDEC_H263CAPABILITY_S;    /*Description structure of the H263 private capacity    */


typedef struct hiVDEC_DECODERCAPABILITY_S{
        HI_S32 support;                /*Whether to support the relevant decode type. 1 for yes, and 0 for no.*/
        HI_CHAR value[VDEC_CAPLENGTH]; /*Private capacity of the decoder. Different protocols correspond to different capacity description structures.*/
}VDEC_DECODERCAPABILITY_S;

typedef enum hiVDEC_DECODERTYPE_E{
    VDEC_H264 = 0,
    VDEC_MPEG2,
    VDEC_H263,
    VDEC_H261,
    VDEC_MPEG4,
    VDEC_VC1,
    VDEC_TYPE
}VDEC_DECODERTYPE_E;  /*the picture mode that decoder can support*/

typedef struct hiVDEC_CAPABILITY_S{
    HI_S32 channels;                                 /* the channel number decoed can support */
    HI_CHAR version[VDEC_VERSIONLENGTH];             /* hardware version information  */
    VDEC_DECODERCAPABILITY_S vdec_ability[VDEC_TYPE];/* Different protocols correspond to different capacity description structures. */
}VDEC_CAPABILITY_S, *PTR_VDEC_CAPABILITY_S;

/* pictoture mode, it  is used for calculatiing the size of memory ,
 * if the user cann't make sure the picture mode, then use VDEC_DEFAULT_PICTUREFORMAT, 
 * decoder will malloc the memory in D1 format 
**/
typedef enum hiVDEC_PICTUREFORMAT_E{
    VDEC_DEFAULT_PICTUREFORMAT = 0, /*default is VDEC_D1*/
    VDEC_QCIF = 1,
    VDEC_CIF = 4,
    VDEC_HD1 = 8,
    VDEC_D1= 16,
    VDEC_QVGA,
    VDEC_VGA
}VDEC_PICTUREFORMAT_E;

typedef enum hiVDEC_ATTR_TYPE_E{
    VDEC_DECODERTYPE,        /* config the decode type                   */
    VDEC_FRAMENUM,           /* config decode frame number              */
    VDEC_PICTUREFORMAT,      /* config picture format ,VDEC_PICTUREFORMAT_E  */

    VDEC_ATTR_ALL            /* config all the attribute for decoder ,VDEC_ChannelPara_S   */
}VDEC_ATTR_TYPE_E;           

typedef struct hiVDEC_ChannelPara_S{
    VDEC_DECODERTYPE_E type;        /* decode format            */
    VDEC_PICTUREFORMAT_E picformat; /* stream mode             */
    HI_U32 framenumber;             /* number of frame buffer  */
}VDEC_ChannelPara_S,*PTR_VDEC_ChannelPara_S;



typedef struct hiVDEC_FRAMEINFO_S{
    HI_BOOL    value;               /* the flag of video, HI_TRUE: have data£¬HI_FALSE: no data*/
    VDEC_CHN ChanID;                /* video decode channel id            */
    PIXEL_FORMAT_E  enPixelFormat;  /* data format                        */
    HI_U32      width;              /* image width                        */
    HI_U32      height;             /* image height                       */
    HI_U64      pts;                /* time stamp                         */
    /* 
    ** In RGB or PackageYUV format, only phy_addr[0] is valid.

    ** In the Semi-Planar YUV data format, only u32PhyAddr [0~1] are valid,
    ** respectively representing the storage address for Y and C components.

    ** In the Planar YUV data format, only u32PhyAddr[[0~2] are valid,
    ** respectively representing the storage address for Y U and V components.
    */
    HI_U32   u32PhyAddr[3];        /* Physical addresses for the A/B/C components */
    HI_U32   u32Stride[3];         /* Stride for the A/B/C components  */
    HI_U32   u32MaskHandle;        /* For buffer manager use, Don't touch it!  */
}VDEC_FRAMEINFO_S, *PTR_VDEC_FRAMEINFO_S;


typedef struct hiVDEC_PRIDATA_S{
    ADDR    addr;   /*private data address                      */
    HI_U32  len;    /*private data length, if len is 0, there is no private data*/
}VDEC_PRIDATA_S, *PTR_VDEC_PRIDATA_S;


typedef struct hiVDEC_STREAM_S{
    ADDR   Dataaddr;
    HI_U32 Datalen;
    HI_U64 pts; 
}VDEC_STREAM_S,*PTR_VDEC_STREAM_S;

typedef struct hiVDEC_STATISTIC_S
{
    HI_U32  u32ReadyFrame; /* The frame number which is ready */
    HI_U32  u32TotalFrame; /* Total decode frame number until now */
    HI_U32  u32CurFps;     /* Curent frame rate           */
    HI_U32  u32CurBps;     /* Curent bitrate(Kbps)        */
    HI_U32  u32SliceNum;   /* The slice number which is waiting for decode */
} VDEC_STATISTIC_S;

/*IOCTL error code*/
#define HI_ERR_VDEC_IOCTL_NOSUPPORT_DECODE_TYPE  1
#define HI_ERR_VDEC_IOCTL_UNSUPPORT_DECODE_TYPE  2
#define HI_ERR_VDEC_IOCTL_CREATECH_FAILED        3
#define HI_ERR_VDEC_IOCTL_DESTROYCH_FAILED       4
#define HI_ERR_VDEC_IOCTL_INVALID_CHANID         5
#define HI_ERR_VDEC_IOCTL_RESETCH_FAILED         6
#define HI_ERR_VDEC_IOCTL_SETATTR_FAILED         7
#define HI_ERR_VDEC_IOCTL_GETATTR_FAILED         8
#define HI_ERR_VDEC_IOCTL_SENDSTREAM_FAILED      9
#define HI_ERR_VDEC_IOCTL_RECEIVE_FAILED         10
#define HI_ERR_VDEC_IOCTL_DISCARD_PRIDATA        11
#define HI_ERR_VDEC_IOCTL_RELEASEFRAME_FAILED    12
#define HI_ERR_VDEC_IOCTL_NO_PAYLOAD             13
#define HI_ERR_VDEC_IOCTL_NO_INPUTBUF            14
#define HI_ERR_VDEC_IOCTL_NO_MSGBUF              15


/*MPI error code*/
#define HI_ERR_VDEC_OPEN_FAILED       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 41)
#define HI_ERR_VDEC_CLOSE_FAILED       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 42)
#define HI_ERR_VDEC_INVALID_PARA       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 43)
#define HI_ERR_VDEC_MALLOC_FAILED       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 44)
#define HI_ERR_VDEC_NO_SUPPORT_DECODE_TYPE       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 45)
#define HI_ERR_VDEC_UNSUPPORT_DECODE_TYPE       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 46)
#define HI_ERR_VDEC_CREATECH_FAILED       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 47)
#define HI_ERR_VDEC_DESTROYCH_FAILED       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 48)
#define HI_ERR_VDEC_INVALID_CHANID       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 49)
#define HI_ERR_VDEC_RESETCH_FAILED       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 50)
#define HI_ERR_VDEC_SETATTR_FAILED       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 51)
#define HI_ERR_VDEC_GETATTR_FAILED       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 52)
#define HI_ERR_VDEC_SENDSTREAM_FAILED       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 53)
#define HI_ERR_VDEC_RECEIVE_FAILED       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 54)
#define HI_ERR_VDEC_DISCARD_PRIDATA       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 55)
#define HI_ERR_VDEC_RELEASEFRAME_FAILED       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 56)
#define HI_ERR_VDEC_RELEASEPRI_FAILED       HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 57)
#define HI_ERR_VDEC_NOT_OPEN      HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 58)
#define HI_ERR_VDEC_NO_PAYLOAD      HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 59)
#define HI_ERR_VDEC_NO_INPUTBUF      HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 60)
#define HI_ERR_VDEC_NO_MSGBUF      HI_DEF_ERR(HI_ID_VDEC, HI_LOG_LEVEL_ERROR, 61)



//#define HI_VDEC_DEBUG
#ifdef HI_VDEC_DEBUG
#define HI_VDEC_PRINT(arg...) printk(arg)
#else
#define HI_VDEC_PRINT(arg...)
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __HI_COMM_VDEC_H__ */
